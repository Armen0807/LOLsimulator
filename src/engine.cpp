//
// Created by armen on 12/11/2026.
//
#include "engine.h"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <array>
#include <limits>

constexpr std::uint64_t FNV_OFFSET = 14695981039346656037ULL;
constexpr std::uint64_t FNV_PRIME = 1099511628211ULL;
constexpr std::array all_actions = {
    Action::None, Action::AutoAttack, Action::CastQ,
    Action::CastW, Action::CastE, Action::CastR
};


float damage_after_resist(const float raw, const float resist) {
    return raw * (100 / (100 + resist));
}

float spell_damage(const ChampionDef &caster, const ChampionDef &target, const int x) {
    float their_resist = target.physic_resist;

    const float ad_damage = caster.spells[x].ad_ratio * caster.ad;
    const float ap_damage = caster.spells[x].ap_ratio * caster.ap;

    if (caster.spells[x].type == DamageType::Magic) {
        their_resist = target.magic_resist;
    }

    const float full_damage = caster.spells[x].base_damage +
                              ad_damage + ap_damage;

    return damage_after_resist(full_damage, their_resist);
}


int attack_interval_ticks(const float attack_speed) {
    return static_cast<int>(TICKS_PER_SECOND / attack_speed);
}

bool is_legal(const State &s, const ChampionDef *defs, int i, Action a) {
    if (a == Action::None) return true;

    if (s.tick < s.units[i].busy_until_tick) {
        return false;
    }
    switch (a) {
        case Action::None:
            break;

        case Action::AutoAttack:
            if (s.tick < s.units[i].next_attack_tick) {
                return false;
            }
            break;

        case Action::CastQ:
        case Action::CastW:
        case Action::CastE:
        case Action::CastR: {
            int x = static_cast<int>(a) - static_cast<int>(Action::CastQ);

            if (s.tick < s.units[i].spell_ready_tick[x]) {
                return false;
            }
            if (defs[s.units[i].def_id].spells[x].cooldown_ticks == 0) {
                return false;
            }
        }
    }

    return true;
}

float score_action(const State &s, const ChampionDef *defs, int i, Action a) {
    if (!is_legal(s, defs, i, a)) {
        return -std::numeric_limits<float>::infinity();
    }

    const int other = 1 - i;

    const UnitState &me = s.units[i];
    const ChampionDef &my_def = defs[me.def_id];
    const ChampionDef &their_def = defs[s.units[other].def_id];

    const float DAA = damage_after_resist(my_def.ad, their_def.physic_resist);

    switch (a) {
        case Action::None:
            return 0.f;
        case Action::AutoAttack:
            return DAA;

        case Action::CastQ:
        case Action::CastW:
        case Action::CastE:
        case Action::CastR: {
            const int x = static_cast<int>(a) - static_cast<int>(Action::CastQ);
            const float lambda = 1.00 / attack_interval_ticks(my_def.attack_speed);

            const int cast_ticks = my_def.spells[x].cast_ticks;

            const float cost = cast_ticks * DAA * lambda;
            const float Dx = spell_damage(my_def, their_def, x);

            return Dx - cost;
        }
    }

    return 0.f;
}

Action best_action(const State &s, const ChampionDef *defs, int i) {
    float highest_score = 0;
    Action action_wmaxscore = Action::None;
    constexpr std::array all_actions = {
        Action::None, Action::AutoAttack, Action::CastQ,
        Action::CastW, Action::CastE, Action::CastR
    };

    for (Action a: all_actions) {
        float current_score = score_action(s, defs, i, a);
        if (current_score > highest_score) {
            highest_score = current_score;
            action_wmaxscore = a;
        }
    }
    return action_wmaxscore;
}

TickActions scripted_actions(const State &s, const ChampionDef *defs) {
    TickActions ta{};

    for (int i = 0; i < 2; i++) {
        ta.act[i] = best_action(s, defs, i);
    }
    return ta;
}

State step(const State &s, const ChampionDef *defs, const TickActions &action) {
    State n = s;
    for (int i = 0; i < 2; i++) {
        int other = 1 - i;

        const UnitState &me = s.units[i];
        const ChampionDef &my_def = defs[me.def_id];
        const ChampionDef &their_def = defs[s.units[other].def_id];

        if (s.tick < me.busy_until_tick) {
            continue;
        }
        switch (Action a = action.act[i]) {
            case Action::None:
                break;
            case Action::AutoAttack:
                if (is_legal(s, defs, i, a)) {
                    n.units[other].hp -= damage_after_resist(my_def.ad, their_def.physic_resist);
                    n.units[i].next_attack_tick = s.tick + attack_interval_ticks(my_def.attack_speed);
                }
                break;

            case Action::CastQ:
            case Action::CastW:
            case Action::CastE:
            case Action::CastR: {
                if (!is_legal(s, defs, i, a)) {
                    break;
                }

                const int x = static_cast<int>(a) - static_cast<int>(Action::CastQ);

                const float full_damage = spell_damage(my_def, their_def, x);

                n.units[other].hp -= full_damage;
                n.units[i].spell_ready_tick[x] = s.tick + my_def.spells[x].cooldown_ticks;
                n.units[i].busy_until_tick = s.tick + my_def.spells[x].cast_ticks;
                break;
            }
        }
    }
    n.tick = s.tick + 1;
    return n;
}

float normhp(const UnitState &u, const ChampionDef &champ) {
    return u.hp / champ.max_hp;
}

float evaluate(const State &s, const ChampionDef *defs) {
    const ChampionDef &A = defs[s.units[0].def_id];
    const ChampionDef &B = defs[s.units[1].def_id];

    if (s.units[0].hp > 0 && s.units[1].hp > 0) {
        float normhpA = normhp(s.units[0], A);
        float normhpB = normhp(s.units[1], B);

        return normhpA - normhpB;
    }
    if (s.units[0].hp <= 0 && s.units[1].hp <= 0) {
        return 0.f;
    }
    if (s.units[0].hp < 0) {
        return -1.f;
    }
    if (s.units[1].hp < 0) {
        return 1.f;
    }
    return 0.f;
}

float rollout(const State &s, const ChampionDef *defs, int depth) {
    State n = s;
    for (int k = 0; k < depth && n.units[0].hp > 0 && n.units[1].hp > 0; k++) {
        TickActions action = scripted_actions(n, defs);
        n = step(n, defs, action);
    }
    return evaluate(n, defs);
}

float q_value(const State &s, const ChampionDef *defs, int depth, Action a) {
    TickActions ta{};
    ta.act[0] = a;
    ta.act[1] = best_action(s, defs, 1);
    State next = step(s, defs, ta);
    return search(next, defs, depth - 1);
}


float search(const State &s, const ChampionDef* defs, const int depth) {
    float max_value = -2.f;

    if (s.units[0].hp <= 0 || s.units[1].hp <= 0 || depth == 0) {
        return evaluate(s, defs);
    }

    for (const Action action: all_actions) {
        if (is_legal(s, defs, 0, action)) {
            max_value = std::max(max_value, q_value(s, defs, depth, action));
        }
    }
    return max_value;
}

float regret(const State& s, const ChampionDef* defs, const int depth, const Action a) {
    return search(s,defs,depth) - q_value(s,defs,depth,a);
}

std::uint32_t float_bits(const float x) {
    std::uint32_t bits;
    std::memcpy(&bits, &x, sizeof(bits));
    return bits;
}

std::uint64_t mix(std::uint64_t h, std::uint32_t value) {
    for (int i = 0; i < 4; i++) {
        std::uint32_t octet = (value >> (8 * i)) & 0xFF;
        h = h ^ octet;
        h = h * FNV_PRIME;
    }
    return h;
}

uint64_t hash_state(const State &s) {
    std::uint64_t h = FNV_OFFSET;
    h = mix(h, s.tick);

    for (const auto &unit: s.units) {
        h = mix(h, unit.def_id);
        h = mix(h, unit.next_attack_tick);
        h = mix(h, float_bits(unit.hp));
    }
    return h;
}


State run_fight(const State &initial, const ChampionDef *defs) {
    State s = initial;
    while (s.units[0].hp > 0 && s.units[1].hp > 0) {
        TickActions ta = scripted_actions(s, defs);
        s = step(s, defs, ta);
    }
    return s;
}

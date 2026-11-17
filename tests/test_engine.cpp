#include <cassert>
#include <cmath>
#include <iostream>
#include "engine.h"


SpellDef spellQ = {
    .cooldown_ticks = 15, .cast_ticks = 2, .base_damage = 150.f, .ad_ratio = 1.10, .ap_ratio = 1.10 , .type = DamageType::Magic
};

ChampionDef defs[2] = {
    { .max_hp = 1000.f, .ad = 100.f, .attack_speed = 1.0f, .ap = 100.f,
        .physic_resist = 0.f, .magic_resist = 50, .spells = {spellQ}  },
    { .max_hp = 1000.f, .ad =  80.f, .attack_speed = 1.0f, .ap = 100.f,
        .physic_resist = 100.f,.magic_resist = 50 }
};

State initialstate() {
    State s {};
    s.units[0] = {0, 1000.f, 0};
    s.units[1] = {1, 1000.f, 0};
    return s;
}

// comparaison de floats avec tolérance (jamais == sur des float en général)
bool approx(float a, float b, float eps = 1e-3f) { return std::fabs(a - b) < eps; }

void test_damage_after_armor() {
    assert(approx(damage_after_resist(100.f, 0.f), 100.f));
    assert(approx(damage_after_resist(100.f, 100.f), 50.f));
}

void test_attack_interval() {
    assert(attack_interval_ticks(1.0f) == 30);
    assert(attack_interval_ticks(2.0f) == 15);
}


void test_hash_state() {
    State other_state {};
    other_state.units[0] = {0, 1000.f, 0};
    other_state.units[1] = {1, 1000.1f, 0};

    assert(hash_state(initialstate()) != hash_state(other_state));
}
void test_hash_reproducible() {
    State r1 = run_fight(initialstate(),defs);
    State r2 = run_fight(initialstate(),defs);
    assert(hash_state(r1) == hash_state(r2));
}
void test_run_fight_V0() {

    State final_state = initialstate();
    final_state = run_fight(final_state,defs);
    assert(final_state.tick ==  46);
    assert(final_state.units[0].hp == 840);
    assert(approx(final_state.units[1].hp, -86.6667f));
    assert(hash_state(initialstate()) != hash_state(final_state));
}

void test_full_fight_AA() {
    State s = initialstate();
    while (s.units[0].hp > 0 && s.units[1].hp > 0) {
        TickActions ta{};
        ta.act[0] = Action::AutoAttack;
        ta.act[1] = Action::AutoAttack;
        s = step(s, defs,ta);
    }
    assert(s.tick == 361);
    assert(s.units[0].hp <= 0);   // A est mort
    assert(s.units[1].hp > 0);    // B est vivant
}

void test_full_fight_Q() {
    State s = initialstate();
    while (s.units[0].hp > 0 && s.units[1].hp > 0) {

        TickActions ta{};
        ta.act[0] = Action::CastQ;
        ta.act[1] = Action::AutoAttack;

        s = step(s, defs,ta);
    }

    assert(s.tick == 61);
    assert(s.units[0].hp  ==  760);
    const float expected = 1000.f - 5.f * (370.f * 100.f / 150.f);
    assert(approx(s.units[1].hp, expected));
}

void test_tick_cooldown() {
    State s = initialstate();
    assert(s.units[1].hp == defs[s.units[1].def_id].max_hp);

    while (s.tick <= 15) {

        TickActions ta{};
        ta.act[0] = Action::CastQ;
        ta.act[1] = Action::None;

        s = step(s, defs,ta);

        if (s.tick <= 15) {
            assert(static_cast<int>(s.units[1].hp) == 753);
        }
    }
    assert(static_cast<int>(s.units[1].hp) == 506);
}

void test_is_legal() {
    State s = initialstate();
    assert(is_legal(s, defs, 0, Action::CastQ));
    assert(!is_legal(s, defs, 1, Action::CastQ));      // B n'a pas de sort

    TickActions ta{};
    ta.act[0] = Action::CastQ;
    s = step(s, defs, ta);                             // tick 1
    assert(!is_legal(s, defs, 0, Action::CastQ));      // A occupé

    while (s.tick < 14) s = step(s, defs, TickActions{});
    assert(!is_legal(s, defs, 0, Action::CastQ));      // tick 14 : cooldown
    s = step(s, defs, TickActions{});
    assert(is_legal(s, defs, 0, Action::CastQ));       // tick 15 : frontière
}

void test_score_action() {
    State s = initialstate();
    const float inf = std::numeric_limits<float>::infinity();
    assert(approx(score_action(s, defs, 0, Action::AutoAttack), 50.f));
    assert(approx(score_action(s, defs, 0, Action::CastQ), 243.333f, 0.01f));
    assert(score_action(s, defs, 0, Action::CastW) == -inf);
    assert(score_action(s, defs, 0, Action::CastE) == -inf);
}

void test_best_action() {
    State s = initialstate();
    while (s.tick < 3) {
        if (s.tick == 0) {
            assert(best_action(s,defs,0) == Action::CastQ);
            assert(best_action(s,defs,1) == Action::AutoAttack);
        }
        TickActions ta{};
        ta = scripted_actions(s,defs);
        s = step(s, defs,ta);
        if (s.tick == 1) {
            assert(best_action(s,defs,0) == Action::None);
        }
        if (s.tick == 2) {
            assert(best_action(s,defs,0) == Action::AutoAttack);
        }
    }
}

int main() {
    test_damage_after_armor();
    test_attack_interval();
    test_hash_reproducible();
    test_hash_state();
    test_run_fight_V0();
    test_full_fight_AA();
    test_full_fight_Q();
    test_tick_cooldown();
    test_is_legal();
    test_score_action();
    test_best_action();

    std::cout << "tous les tests passent\n";
}
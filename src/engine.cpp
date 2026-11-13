//
// Created by armen on 12/11/2026.
//
#include "engine.h"
#include <cstdint>
#include <cstring>
#include <iostream>

constexpr std::uint64_t FNV_OFFSET = 14695981039346656037ULL;
constexpr std::uint64_t FNV_PRIME  = 1099511628211ULL;

float damage_after_armor(const float raw,const float armor) {
    return raw * (100 / (100+armor));
}

int attack_interval_ticks(const float attack_speed) {
    return static_cast<int>(TICKS_PER_SECOND / attack_speed);
}

State step(const State& s, const ChampionDef* defs, const TickActions) {
    State n = s;
    for (int i = 0; i < 2; i++) {
        int other = 1 - i;

        const UnitState& me = s.units[i];
        const ChampionDef& my_def = defs[me.def_id];
        const ChampionDef& their_def = defs[s.units[other].def_id];

        if (s.tick >= me.busy_until_tick) {
            if (s.tick >= spell_ready_tick[x])
            if (s.tick >= me.next_attack_tick) {
                n.units[other].hp -= damage_after_resist(my_def.ad,their_def.physic_resist);
                n.units[i].next_attack_tick = s.tick + attack_interval_ticks(my_def.attack_speed);
            }
        }

        n.tick = s.tick + 1;
    }
    return n;}

std::uint32_t float_bits(const float x) {
    std::uint32_t bits;
    std::memcpy(&bits,&x,sizeof(bits));
    return bits;
}
std::uint64_t mix(std::uint64_t h, std::uint32_t value) {
    for (int i = 0; i<4 ;i++) {
        std::uint32_t octet = (value >> (8 * i)) & 0xFF;
        h = h ^ octet;
        h = h * FNV_PRIME;
    }
return h;}

uint64_t hash_state(const State& s) {
    std::uint64_t h = FNV_OFFSET;
    h = mix(h,s.tick);

    for (const auto & unit : s.units) {
        h = mix(h,unit.def_id);
        h = mix(h,unit.next_attack_tick);
        h = mix(h,float_bits(unit.hp));
    }
return h;}

State run_fight(const State& initial, const ChampionDef* defs) {
    State s = initial;
    while (s.units[0].hp > 0 && s.units[1].hp > 0) {
        s = step(s,defs);
    }
    return s;
}
#pragma once
#include<cstdint>

constexpr int TICKS_PER_SECOND = 30;


struct ChampionDef {float max_hp, ad, armor, attack_speed;};
struct UnitState {std::uint8_t def_id; float hp; std::int32_t next_attack_tick;};
struct State    { std::int32_t tick; UnitState units[2];};

float damage_after_armor(float raw, float armor);
int attack_interval_ticks(float attack_speed);
State step(const State& s, const ChampionDef* defs);

std::uint32_t float_bits(const float x);
std::uint64_t mix(std::uint64_t h, std::uint32_t value);
uint64_t hash_state(const State& s);
State run_fight(const State& initial, const ChampionDef* defs);
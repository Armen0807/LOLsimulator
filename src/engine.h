#pragma once
#include<cstdint>

// overall constants

constexpr int TICKS_PER_SECOND = 30;


// different classes

enum class Action {None, AutoAttack, CastQ, CastW, CastE, CastR };
enum class DamageType {Physical, Magic};

// Structures over champions and global properties
struct SpellDef     {int cooldown_ticks, cast_ticks;
    float base_damage, ad_ratio, ap_ratio; DamageType type;};

struct ChampionDef  {float max_hp, ad, attack_speed, ap,
    physic_resist, magic_resist; SpellDef spells[4];};

struct UnitState    {std::uint8_t def_id; float hp; std::int32_t next_attack_tick,
    spell_ready_tick[4], busy_until_tick;};

struct State        { std::int32_t tick; UnitState units[2];};


struct TickActions  {Action act[2];}; // done -> to be verified




// Core functions : attacks, cooldowns...
float damage_after_resist(float raw,float resist); // done -> to be verified
int attack_interval_ticks(float attack_speed); // done
bool is_legal(const State& s, const ChampionDef* defs, int i, Action a); // to be verified
float score_action(const State& s, const ChampionDef* defs, int i, Action a); // to be verified


State step(const State& s, const ChampionDef* defs, const TickActions& actions); // done -> to be verified
TickActions scripted_actions(const State& s, const ChampionDef* defs); // done -> to be upgraded/verified
Action best_action(const State& s, const ChampionDef* defs,int i);


// Identification
std::uint32_t float_bits(float x);
std::uint64_t mix(std::uint64_t h, std::uint32_t value);
uint64_t hash_state(const State& s);

// Statistics && results
State run_fight(const State& initial, const ChampionDef* defs);
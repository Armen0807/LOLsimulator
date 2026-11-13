#pragma once
#include<cstdint>

// overall constants

constexpr int TICKS_PER_SECOND = 30;


// different classes

enum class Action {None, AutoAttack, CastQ, CastW, CastE, CastR };
enum class DamageType {Physical, Magic};

// Structures over champions and global properties

struct ChampionDef  {float max_hp, ad, attack_speed, ap,
    physic_resist, magic_resist; int32_t spellDef_spells[4];};

struct UnitState    {std::uint8_t def_id; float hp; std::uint32_t next_attack_tick;
    std::uint32_t spell_ready_tick[4]; std::uint32_t busy_until_tick;};

struct State        { std::int32_t tick; UnitState units[2];};

struct SpellDef     {const int cooldown_ticks, cast_ticks;
    float base_damage, ad_ratio, ap_ratio; DamageType type;};

struct TickActions  {Action None,AutoAttack, CastQ, CastW, CastR;}; // done -> to be verified




// Core functions : attacks, cooldowns...
float damage_after_resist(float raw,float resist); // done -> to be verified
int attack_interval_ticks(float attack_speed); // done

State step(const State& s, const ChampionDef* defs, TickActions); // done -> to be verified
void scripted_actions(const State&); // TODO


// Identification
std::uint32_t float_bits(float x);
std::uint64_t mix(std::uint64_t h, std::uint32_t value);
uint64_t hash_state(const State& s);

// Statistics && results
State run_fight(const State& initial, const ChampionDef* defs);
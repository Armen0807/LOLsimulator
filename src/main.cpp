#include <iostream>
#include "engine.h"
#include <chrono>


int main() {
    SpellDef spellQ = {
        .cooldown_ticks = 15, .cast_ticks = 2, .base_damage = 150.f, .ad_ratio = 1.10, .ap_ratio = 1.10 , .type = DamageType::Magic
    };
    SpellDef spellE = {
        .cooldown_ticks = 20, .cast_ticks = 5, .base_damage = 250.f, .ad_ratio = 1.20, .ap_ratio = 1.10 , .type = DamageType::Magic
    };

    ChampionDef defs[2] = {
        { .max_hp = 1000.f, .ad = 100.f, .attack_speed = 1.0f, .ap = 100.f,
            .physic_resist = 0.f, .magic_resist = 50, .spells = {spellQ,spellE}  },
        { .max_hp = 1000.f, .ad =  80.f, .attack_speed = 1.0f, .ap = 100.f,
            .physic_resist = 100.f,.magic_resist = 50 }
    };

    State s{};
    s.tick = 0;
    s.units[0] = {.def_id = 0,.hp = 1000.f,.next_attack_tick = 0};
    s.units[1] = {.def_id = 1, .hp = 1000.f,.next_attack_tick = 0};

    State final_state = run_fight(s,defs);
    const int D = 26;

    auto t0 = std::chrono::steady_clock::now();
    float best = search(s, defs, D);
    auto t1 = std::chrono::steady_clock::now();

    float script = rollout(s, defs, D);

    std::cout << "search  : " << best << "\n";
    std::cout << "rollout : " << script << "\n";
    std::cout << "ecart   : " << best - script << "\n";
    std::cout << "temps   : " << std::chrono::duration<double>(t1 - t0).count() << " s\n";

}
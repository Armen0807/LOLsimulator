#include <iostream>
#include "engine.h"


int main() {
    SpellDef spellQ = {
        .cooldown_ticks = 15, .cast_ticks = 2, .base_damage = 150.f, .ad_ratio = 1.10, .ap_ratio = 1.10 , .type = DamageType::Magic
    };

    ChampionDef defs[2] = {
        { .max_hp = 1000.f, .ad = 100.f, .attack_speed = 1.0f, .ap = 100.f,
            .physic_resist = 0.f, .magic_resist = 50, .spells = {spellQ}  },
        { .max_hp = 1000.f, .ad =  80.f, .attack_speed = 1.0f, .ap = 100.f,
            .physic_resist = 100.f,.magic_resist = 50 }
    };

    State s{};
    s.tick = 0;
    s.units[0] = {0,1000.f,0};
    s.units[1] = {1, 1000.f,0};

    State final_state = run_fight(s,defs);
    std::cout << "tick : " << final_state.tick << "\n";
    std::cout << "hp A : " << final_state.units[0].hp << "\n";
    std::cout << "hp B : " << final_state.units[1].hp << "\n";
    std::cout << "hash : " << hash_state(final_state) << "\n";

}
#include <iostream>
#include "engine.h"


int main() {
    ChampionDef defs[2] = {
        {1000.f,100.f,0.f,1.0f},
        {1000.f, 80.f,100.f,1.0f}
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
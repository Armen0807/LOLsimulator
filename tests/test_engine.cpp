#include <cassert>
#include <cmath>
#include <iostream>
#include "engine.h"


const ChampionDef defs[2] = {
    {1000.f, 100.f, 0.f,   1.0f},
    {1000.f,  80.f, 100.f, 1.0f}
};
State initialstate() {
    State s {};
    s.units[0] = {0, 1000.f, 0};
    s.units[1] = {1, 1000.f, 0};
    return s;
}

// comparaison de floats avec tolérance (jamais == sur des float en général)
bool approx(float a, float b) { return std::fabs(a - b) < 1e-4f; }

void test_damage_after_armor() {
    assert(approx(damage_after_armor(100.f, 0.f), 100.f));
    assert(approx(damage_after_armor(100.f, 100.f), 50.f));
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
void test_run_fight() {

    State final_state = initialstate();
    final_state = run_fight(final_state,defs);
    assert(final_state.tick ==  361);
    assert(final_state.units[0].hp == -40);
    assert(final_state.units[1].hp ==  350);
    assert(hash_state(initialstate()) != hash_state(final_state));
}

void test_full_fight() {
    State s = initialstate();
    while (s.units[0].hp > 0 && s.units[1].hp > 0) {
        s = step(s, defs);
    }
    assert(s.tick == 361);
    assert(s.units[0].hp <= 0);   // A est mort
    assert(s.units[1].hp > 0);    // B est vivant
}

int main() {
    test_damage_after_armor();
    test_attack_interval();
    test_hash_reproducible();
    test_hash_state();
    test_run_fight();
    test_full_fight();

    std::cout << "tous les tests passent\n";
}
#include "simplerandom.hpp"


const uint_least64_t SimpleRandom::MAX_NEXT = UINT64_C(0xffffffff);
const float SimpleRandom::MAX_NEXT_FLOAT = (float)SimpleRandom::MAX_NEXT;

SimpleRandom::SimpleRandom(unsigned long seed) {
    this->seed = seed;
}

void SimpleRandom::setSeed(unsigned long seed) {
    this->seed = seed;
}

float SimpleRandom::float_uniform() {
    return (float)next() / SimpleRandom::MAX_NEXT_FLOAT;
}

uint_least64_t SimpleRandom::next() {
    seed = UINT64_C(6364136223846793005) * seed + 1;
    return seed >> 32;
}
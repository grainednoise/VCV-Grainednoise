#include "simplerandom.hpp"

/*
  A very simple (to implement) random number generator based on the Linear congruential generator algorithm
  See: https://en.wikipedia.org/wiki/Linear_congruential_generator. While this article warns not just to
  pick some of the number in the table therein (i.e. potentially repeating other peaple's mistake),
  I did so anyway, as none of it is critical in any way.
  
  Better choices may be had from:
  https://www.ams.org/journals/mcom/1999-68-225/S0025-5718-99-00996-5/S0025-5718-99-00996-5.pdf

*/

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
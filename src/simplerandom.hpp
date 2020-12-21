#pragma once

#include <cstdint>

class SimpleRandom {
    public:
        SimpleRandom(unsigned long seed);
        void setSeed(unsigned long seed);
        float float_uniform();  // Will generate a uniform value between 0.0 and 1.0
    
    private:
        static const uint_least64_t MAX_NEXT;
        static const float MAX_NEXT_FLOAT;

        uint_least64_t seed;
        uint_least64_t next();
};

#pragma once

#include "plugin.hpp"
#include "simplerandom.hpp"
// #include <memory>


class Noise2D {
    public:
        static const int MIN_SIZE;
        static const int MAX_SIZE;

        Noise2D();

        int getXSize();
        int getYSize();
        void setXSize(int xSize); 
        void setYSize(int ySize); 
        void setSeed(unsigned long seed, bool forceRecalc=false);

        float getValue(float x, float y);
        float getLinearValue(float x, float y);
        float getLanczos2Value(float x, float y);

    private:
        int xSize;
        int ySize;
        unsigned long seed;
        SimpleRandom randgen;
        float noise[16][16];

        void recalculate();
        float getArrayValue(int x, int y);
};


class NoiseDisplay : public OpenGlWidget {
    public:
        // void setNoiseGen(Noise2D *noiseGen);
        void setNoiseGen(std::shared_ptr<Noise2D> noiseGen);
        

    private:
        // Noise2D *noiseGen;
        std::shared_ptr<Noise2D> noiseGen;
    
        void drawFramebuffer() override;
};
#include <math.h>

#include "plugin.hpp"
#include "berlincode.hpp"

const int Noise2D::MIN_SIZE = 1;
const int Noise2D::MAX_SIZE = 16;


 Noise2D::Noise2D() : randgen(0) {
     setXSize(2);
     setYSize(2);
     setSeed(0);
     recalculate();
 }

template<class T>
const T& clamp(const T& x, const T& upper, const T& lower) {
    return min(upper, max(x, lower));
}

int Noise2D::getXSize() {
    return xSize;
}
int Noise2D::getYSize() {
    return ySize;
}

void Noise2D::setXSize(int xSize) {
    this->xSize = clamp(xSize, Noise2D::MIN_SIZE, Noise2D::MAX_SIZE);
}

void Noise2D::setYSize(int ySize) {
    this->ySize = clamp(ySize, Noise2D::MIN_SIZE, Noise2D::MAX_SIZE);
}

void Noise2D::setSeed(unsigned long seed) {
    if (this->seed != seed) {
        this->seed = seed;
        recalculate();
    }
}

void Noise2D::recalculate() {
    randgen.setSeed(seed);
    INFO("Recalculating noise samples");

    for (int x = 0; x < Noise2D::MAX_SIZE; x++) {
        for (int y = 0; y < Noise2D::MAX_SIZE; y++) {
            noise[x][y] = randgen.float_uniform();
        }
    }

}

 inline std::tuple<double,int> interpAddress(float value, float size) {
    //Scale to 0..1 range
    float fracFalue = value - floor(value);

    float scaled = fracFalue * size;
    float intPart;
    float float_part = modf(scaled, &intPart);

    int index = (int)round(fmod(intPart, size));

    return std::make_tuple(float_part, index);
}

const float PI = 3.141592653589793;
const float PI_SQUARED = PI * PI;

float lanczos2Kernel(const float value) {
    if (value == 0.0) {
        return 1.0;
    }

    if (value > 2.0 || value < -2.0) {
        return 0.0;
    }

    float piV = value * PI;

    return 2.0 * sin(piV) * sin(piV / 2) / PI_SQUARED / value / value;
}

/*
    This is an appoximation of the Lancszos function, as the actual function
    runs much too slowly, presumably because of the two sine() fuctions.

    This implementation is fast, but not terribly accurate. It uses a piecewise
    order 3 polynomial approximation of the squared input value.


*/
inline float fastLanczsos2Kernel(const float value) {
    float squared = value * value;

    if (squared < 0.51) {
        float x1 = squared;
        float x2 = x1 * x1;
        float x3 = x2 * x1;
        return 1.0 - 2.0561 * x1 + 1.59 * x2 -0.63 * x3;
    }

    if (squared < 1.890) {
        float x1 = squared - 1.6;
        float x2 = x1 * x1;
        float x3 = x2 * x1;
        return -0.08325 + 0.139 * x2 - 0.154 * x3;
    }

    if (squared < 4.0) {
        float x1 = squared - 4.0;
        float x2 = x1 * x1;
        float x3 = x2 * x1;
        return -0.028 * x2 - 0.0053 * x3;
    }

    return 0.0;
}


inline float Noise2D::getArrayValue(int x, int y) {
    int realX = (x + 16 * xSize) % xSize;
    if (realX < 0) {
        return 0.0;
    }

    int realY = (y + 16 * ySize) % ySize;
    if (realY < 0) {
        return 0.0;
    }

    return noise[realX][realY];
}

float Noise2D::getValue(float x, float y) {
    // return getLinearValue(x, y);
    return getLanczos2Value(x, y);
}

float Noise2D::getLinearValue(float x, float y) {
    double fracX;
    int indexX;

    std::tie (fracX, indexX) = interpAddress(x, (double)xSize);

    double fracY;
    int indexY;

    std::tie (fracY, indexY) = interpAddress(y, (double)ySize);

    return getArrayValue(indexX, indexY) * (1 - fracX) * (1 - fracY) + 
           getArrayValue(indexX + 1, indexY) * fracX * (1 - fracY) + 
           getArrayValue(indexX, indexY + 1) * (1 - fracX) * fracY +
           getArrayValue(indexX + 1, indexY + 1) * fracX * fracY;
}

float Noise2D::getLanczos2Value(float x, float y) {
    double fracX;
    int indexX;

    std::tie (fracX, indexX) = interpAddress(x, (double)xSize);

    double fracY;
    int indexY;

    std::tie (fracY, indexY) = interpAddress(y, (double)ySize);

    float accu = 0.0;
    float yKernel4[4];
     for (int dy = -1; dy <= 2; dy++) {
        yKernel4[dy + 1] = fastLanczsos2Kernel(fracY - dy);
    }


    for (int dx = -1; dx <= 2; dx++) {
        float kernX = fastLanczsos2Kernel(fracX - dx);
        for (int dy = -1; dy <= 2; dy++) {
            // accu += getArrayValue(indexX + dx, indexY + dy) * lanczos2Kernel(fracX - dx) * lanczos2Kernel(fracY - dy);
            // accu += getArrayValue(indexX + dx, indexY + dy) * kernX * fastLanczsos2Kernel(fracY - dy);
            accu += getArrayValue(indexX + dx, indexY + dy) * kernX * yKernel4[dy + 1];
        }
    }

    return accu;
}


// void NoiseDisplay::setNoiseGen(std::shared_ptr<Noise2D> noiseGen) {
//     this->noiseGen = noiseGen;
// }

void NoiseDisplay::setNoiseGen(Noise2D *noiseGen) {
    this->noiseGen = noiseGen;
}

void NoiseDisplay::drawFramebuffer() {
    try {
        glViewport(0.0, 0.0, fbSize.x, fbSize.y);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, fbSize.x, 0.0, fbSize.y, -1.0, 1.0);

        if (!noiseGen) {
            return;
        }

        for (float yCo = 0.0; yCo <= fbSize.y; yCo += 1.0) {
            glBegin(GL_LINE_STRIP);
            for (float xCo = 0.0; xCo < fbSize.x; xCo += 1.0) {
                float xAddr = xCo / fbSize.x;
                float yAddr = yCo / fbSize.y;

                float colVal = noiseGen->getValue(xAddr, yAddr);
                glColor3f(colVal, colVal, colVal);
                glVertex3f(xCo, yCo, 0);

            }
            glEnd();
        }
    } catch (const std::string& ex) {
		WARN("Caught exception: %s", ex);
	} catch (...) {
		WARN("Caught unknown exception");
	}
}

NoiseDisplay::~NoiseDisplay() {

}
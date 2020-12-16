#include <math.h>

#include "plugin.hpp"
#include "berlincode.hpp"

const int Noise2D::MIN_SIZE = 1;
const int Noise2D::MAX_SIZE = 16;


 Noise2D::Noise2D() : randgen(0) {
     setXSize(2);
     setYSize(2);
     setSeed(0, true);
 }


int Noise2D::getXSize() {
    return xSize;
}
int Noise2D::getYSize() {
    return ySize;
}

void Noise2D::setXSize(int xSize) {
    this->xSize = std::min(std::max(xSize, Noise2D::MIN_SIZE), Noise2D::MAX_SIZE);
}

void Noise2D::setYSize(int ySize) {
    this->ySize = std::min(std::max(ySize, Noise2D::MIN_SIZE), Noise2D::MAX_SIZE);
}

void Noise2D::setSeed(unsigned long seed, bool forceRecalc) {
    bool recalc = forceRecalc || this->seed != seed;
    this->seed = seed;
    if (recalc) {
        recalculate();
    }
}

void Noise2D::recalculate() {
    randgen.setSeed(seed);

    for (int x = 0; x < Noise2D::MAX_SIZE; x++) {
        for (int y = 0; y < Noise2D::MAX_SIZE; y++) {
            noise[x][y] = randgen.float_uniform();
            INFO("noise[%d][%d] = %g", x, y, noise[x][y]);
        }
    }

}

 std::tuple<double,int> interpAddress(double value, double size) {
    //Scale to 0..1 range
    double fracFalue = value - floor(value);

    double scaled = fracFalue * size;
    double intPart;
    double float_part = modf(scaled, &intPart);

    int index = (int)round(fmod(intPart, size));

    return std::make_tuple(float_part, index);
}

float Noise2D::getArrayValue(int x, int y) {
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


void NoiseDisplay::setNoiseGen(std::shared_ptr<Noise2D> noiseGen) {
    this->noiseGen = noiseGen;
}

void NoiseDisplay::drawFramebuffer() {
	glViewport(0.0, 0.0, fbSize.x, fbSize.y);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, fbSize.x, 0.0, fbSize.y, -1.0, 1.0);

   if (noiseGen == nullptr) {
        return;
    }

	glBegin(GL_QUADS);

    int xSize = noiseGen->getXSize();
    int ySize = noiseGen->getYSize();
    
    INFO("Doing NoiseDisplay %d x %d", xSize, ySize);

    for (int x = 0; x <= xSize; x++) {
        float addrX = (float)x / (float)xSize;
        float addrXNext = (float)(x + 1) / (float)xSize;

        float xCo0 = addrX * fbSize.x;
        float xCo1 = addrXNext * fbSize.x;

        for (int y = 0; y <= ySize; y++) {
            float addrY = (float)y / (float)ySize;
            float addrYNext = (float)(y + 1) / (float)ySize;

            float yCo0 = addrY * fbSize.y;
            float yCo1 = addrYNext * fbSize.y;

            if (x < xSize && y < ySize) {
                float bwVal00 = noiseGen->getValue(addrX, addrY);
                INFO("Calc X %d=%g, Y %d=%g = V %g", x, addrX, y, addrY, bwVal00);
                glColor3f(bwVal00, bwVal00, bwVal00);
                glVertex3f(xCo0, yCo0, 0);

                float bwVal10 = noiseGen->getValue(addrXNext, addrY);
                glColor3f(bwVal10, bwVal10, bwVal10);
                glVertex3f(xCo1, yCo0, 0);

                float bwVal11 = noiseGen->getValue(addrXNext, addrYNext);
                glColor3f(bwVal11, bwVal11, bwVal11);
                glVertex3f(xCo1, yCo1, 0);

                float bwVal01 = noiseGen->getValue(addrX, addrYNext);
                glColor3f(bwVal01, bwVal01, bwVal01);
                glVertex3f(xCo0, yCo1, 0);


            }
        }
    }
 

	// glColor3f(0.5, 0.5, 0.5);
	// glVertex3f(0, 0, 0);
	// glColor3f(0, 1, 0);
	// glVertex3f(fbSize.x, 0, 0);
	// glColor3f(0, 0, 1);
	// glVertex3f(0, fbSize.y, 0);
	glEnd();
}
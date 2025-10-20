#include "camera.h"

struct exposure_iso {
    int exposure;
    int ISO;
};

exposure_iso testPoints[] = {
    {10000, 3200},
    {15000, 2500},
    {22000, 2000},
    {33000, 1600},
    {50000, 1250},
    {75000, 1000},
    {110000, 800},
    {165000, 640},
    {250000, 500},
    {375000, 400},
    {560000, 320},
    {840000, 250},
    {1250000, 200},
    {1850000, 160},
    {2750000, 125},
    {4100000, 100},
    {6100000, 80},
    {7500000, 64},
    {9000000, 50},
    {10000000, 50}
};


int main() {
    StarCamera camera(1, 1, MAX_WIDTH, MAX_HEIGHT, "out.png");
    
    int numTests = sizeof(testPoints) / sizeof(testPoints[0]);

    for (int i = 0; i < numTests; ++i) {
        int exposure = testPoints[i].exposure;
        int ISO = testPoints[i].ISO;
        camera.set_exposure(exposure);
        camera.set_iso(ISO);
        camera.set_outputFilename("testExposure" + std::to_string(exposure / 1000000) + "_ISO" + std::to_string(ISO) + ".png");

        camera.take_picture();
    }
}
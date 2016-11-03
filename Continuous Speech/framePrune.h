#ifndef FRAME_PRUNE
#define FRAME_PRUNE

#include <iostream>
#include <math.h>
using namespace std;

typedef short SAMPLE;
#define SAMPLE_PER_FRAME        (882)    // in order to make each frame 20ms
#define FORGET_FACTOR   (1)
#define ADJUSTMENT      (0.05)
#define THRESHOLD_F           (2)
#define SPEAKTHRESHOLD      (10)
#define SILENCETHRESHOLD    (1)
#define FRAME_IGNORE_S        (1)
#define FRAME_IGNORE_E        (15)
#define FRAME_TO_BACKGROUND (1)

double EnergyPerSample(SAMPLE *audioframe, long framesToCalc);
bool classifyFrame(SAMPLE *audioframe, long framesToCalc, double& backgroundOf10f, double& levelStart);
int pruneFrameFromStart(short* dataWave, int& numSamples);
int pruneFrameFromEnd(short* dataWave, int& numSamples);










#endif


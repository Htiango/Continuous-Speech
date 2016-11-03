#ifndef _FEATUREEXTRACTION_H_
#define _FEATUREEXTRACTION_H_

#include <iostream>
#include <fstream>
#include "readwave.h"
#include "fft.h"
#include "framePrune.h"
#include <vector>

using namespace std;


//----------
#define SAMPLE_RATE             (44100)  // The sample rate that mac allows only

#define PREEMPHASIZED_FACTOR    (0.95)   // s[n] = s[n] - alpha * s[n-1]

#define SAMPLE_PER_FRAME        (882)    // in order to make each frame 20ms

#define FRAME_INTERVAL_TIME     (10)      // set the interval time of each frame to be 10ms, so have 10ms overlapping

#define ACTUAL_SAMPLE_PER_FRAME (1024)    // in order to do the fft

#define MEL_POINT               (40)

#define DCT_DIMENSION           (13)

#define fre_MinMel              (133.33)

#define fre_MaxMel              (6855.4976)

// ---------
#define NUM_FILTER              (40)     // set the num of filters to be 40

#define COMPARE_SCALE (15)
#define PI (3.141592653589793238460)
typedef short SAMPLE;
#define FORGET_FACTOR   (1)
#define ADJUSTMENT      (0.05)
#define THRESHOLD_F           (10)
#define SPEAKTHRESHOLD      (5)
#define SILENCETHRESHOLD_F    (5)
#define FRAME_IGNORE        (4)
#define FRAME_TO_BACKGROUND (5)


//prune frame from start and end at different time, then get feature
void featureExtractionTwo(vector<vector<double>>& normDCT, string& wav, string& filePath);
//prune frame from start and end at same time, then get feature
void featureExtraction(vector<vector<double>>& normDCT, string& wav, string& filePath);

#endif

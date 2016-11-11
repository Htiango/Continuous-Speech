#ifndef _FEATUREEXTRACTION_NEW_H_
#define _FEATUREEXTRACTION_NEW_H_

#include <iostream>
#include <fstream>
#include "readwave.h"
#include "fft.h"
#include "framePrune.h"
#include <vector>

using namespace std;


//----------
#define SAMPLE_RATE_N             (8000)  // The sample rate that mac allows only

#define PREEMPHASIZED_FACTOR_N    (0.95)   // s[n] = s[n] - alpha * s[n-1]

#define SAMPLE_PER_FRAME_N        (160)    // in order to make each frame 20ms

#define FRAME_INTERVAL_TIME_N    (10)      // set the interval time of each frame to be 10ms, so have 10ms overlapping

#define ACTUAL_SAMPLE_PER_FRAME_N (256)    // in order to do the fft

#define MEL_POINT_N               (40)

#define DCT_DIMENSION_N           (13)

#define fre_MinMel_N              (133.33)

#define fre_MaxMel_N              (6855.4976)

// ---------
#define NUM_FILTER_N              (40)     // set the num of filters to be 40

#define COMPARE_SCALE_N (15)
#define PI_N (3.141592653589793238460)
typedef short SAMPLE_N;
#define FORGET_FACTOR_N   (1)
#define ADJUSTMENT_N      (0.05)
#define THRESHOLD_F_N           (10)
#define SPEAKTHRESHOLD_N      (5)
#define SILENCETHRESHOLD_F_N    (5)
#define FRAME_IGNORE_N        (0)
#define FRAME_TO_BACKGROUND_N (4)


//prune frame from start and end at different time, then get feature
void featureExtractionTwoNew(vector<vector<double>>& normDCT, string& wav, string& filePath);
//prune frame from start and end at same time, then get feature
void featureExtractionNew(vector<vector<double>>& normDCT, string& wav, string& filePath);

#endif

#ifndef _FEATUREEXTRACTION_OLD_H_
#define _FEATUREEXTRACTION_OLD_H_

#include <iostream>
#include <fstream>
#include "readwave.h"
#include "fft.h"
#include "framePrune.h"
#include <vector>

using namespace std;


//----------
#define SAMPLE_RATE_O             (44100)  // The sample rate that mac allows only

#define PREEMPHASIZED_FACTOR_O    (0.95)   // s[n] = s[n] - alpha * s[n-1]

#define SAMPLE_PER_FRAME_O        (882)    // in order to make each frame 20ms

#define FRAME_INTERVAL_TIME_O    (10)      // set the interval time of each frame to be 10ms, so have 10ms overlapping

#define ACTUAL_SAMPLE_PER_FRAME_O (1024)    // in order to do the fft

#define MEL_POINT_O               (40)

#define DCT_DIMENSION_O           (13)

#define fre_MinMel_O              (133.33)

#define fre_MaxMel_O              (6855.4976)

// ---------
#define NUM_FILTER_O              (40)     // set the num of filters to be 40

#define COMPARE_SCALE_O (15)
#define PI (3.141592653589793238460)
typedef short SAMPLE_O;
#define FORGET_FACTOR_O   (1)
#define ADJUSTMENT_O      (0.05)
#define THRESHOLD_F_O           (10)
#define SPEAKTHRESHOLD_O      (5)
#define SILENCETHRESHOLD_F_O    (5)
#define FRAME_IGNORE_O        (4)
#define FRAME_TO_BACKGROUND_O (5)


//prune frame from start and end at different time, then get feature
void featureExtractionTwoOld(vector<vector<double>>& normDCT, string& wav, string& filePath);
//prune frame from start and end at same time, then get feature
void featureExtractionOld(vector<vector<double>>& normDCT, string& wav, string& filePath);

#endif

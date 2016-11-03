//
//  seg_K_mean.h
//  DTW
//
//  Created by hty on 01/10/2016.
//  Copyright © 2016 hty. All rights reserved.
//

#ifndef seg_K_mean_h
#define seg_K_mean_h

#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <algorithm>

using namespace std;

#define SEG_NUM     (5)
#define DIMENSION   (39)
#define PI          (3.1416)

vector<vector<double>> dtw2hmm(vector<vector<vector<double>>>& temGroup);

#endif /* seg_K_mean_h */

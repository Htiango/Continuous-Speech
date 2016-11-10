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


double nodeCost(vector<double>& frame, vector<double>& segTem, vector<double>& varianceTerm);
double edgeCost(int nextIndex, vector<int>& countTransfer);
vector<vector<double>> dtw2hmm(vector<vector<vector<double>>>& temGroup, vector<vector<double>>& varianceTerm, vector<vector<int>>& countTransfer);
double segmentalDtw(vector<vector<double>>& inputAduio, vector<vector<double>>& temAduio, vector<vector<double>> varianceTerm, vector<vector<int>> countTransfer);
vector<vector<vector<vector<int>>>> conDtw2hmm(vector<vector<vector<vector<double>>>> temGroup, vector<vector<vector<vector<int>>>> initSegIndex, vector<vector<vector<double>>>& varianceTerm, vector<vector<vector<int>>>& countTransfer);
#endif /* seg_K_mean_h */


//#endif /* seg_K_mean_h */

#ifndef _DTW_H_
#define _DTW_H_

#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <iomanip>


using namespace std;

#define DTW_BEAM (10)


#define TABLE_GAP (10)

double dtw(vector<vector<double>>& inputAduio, vector<vector<double>>& temAduio);
double beamDTW(vector<vector<double>>& inputAudio, vector<vector<double>>& temAudio);
vector<double> beamSynchronousDTW(vector<vector<double>>& inputAudio, vector<vector<vector<double>>>& temAudio);
double printDTWPath(vector<vector<double>>& s1, vector<vector<double>>& s2, ofstream& out);

#endif

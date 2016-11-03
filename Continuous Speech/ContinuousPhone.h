#ifndef ContinuousPhone
#define ContinuousPhone

#include <math.h>
#include <stack>
#include "Trie.h"
#include "dtw.h"
#include "featureExtraction.h"
#include "seg_K_mean.h"

#define PENALTY  (1)
#define TYPE_NUM    (10)
#define TEM_NUM     (10)
#define INPUT_NUM   (10)

double Dis(vector<double>& matrix1, vector<double>& matrix2);
double costUtil(vector<double>& vec, double c, int& pos);
void RestrictPhone(Trie& trie, vector<vector<double>>& input);

#endif


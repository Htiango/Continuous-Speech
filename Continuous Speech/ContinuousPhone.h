#ifndef ContinuousPhone
#define ContinuousPhone

#include <math.h>
#include <stack>
#include "Trie.h"
#include "dtw.h"
#include "featureExtraction.h"
#include "seg_K_mean.h"

#define PENALTY  (18)
#define TYPE_NUM    (10)
#define TEM_NUM     (10)
#define INPUT_NUM   (10)
#define DIGIT_NUM7  (7)
#define DIGIT_NUM4  (4)


double Dis(vector<double>& matrix1, vector<double>& matrix2);
double costUtil(vector<double>& vec, double c, int& pos);
void RestrictPhone(Trie& trie, vector<vector<double>>& input);
void DigitRecognition(int digit_num, vector<vector<double>>& input, vector<vector<vector<double>>> segTemGroup);

#endif


#ifndef ContinuousPhone
#define ContinuousPhone

#include <math.h>
#include <stack>
#include "Trie.h"
#include "dtw.h"
#include "featureExtraction.h"
#include "seg_K_mean.h"

#define PENALTY  (80)
#define TYPE_NUM    (10)
#define TEM_NUM     (5)
#define INPUT_NUM   (10)
#define DIGIT_NUM10  (10)
#define DIGIT_NUM7 (7)
#define DIGIT_NUM4 (4)

//one template ten digits
#define DIGIT_NUM (10)
//six kinds of train data
#define TRAIN_TYPE (6)
//five same train data
#define TRAIN_NUM (5)

double Dis(vector<double>& matrix1, vector<double>& matrix2);
double costUtil(vector<double>& vec, double c, int& pos);

stack<int> RestrictPhone(Trie& trie, vector<vector<double>>& input, vector<vector<vector<double>>>& varianceTerm, vector<vector<vector<int>>>& countTransfer);
stack<int> RestrictPhoneGuassian(Trie& trie, vector<vector<double>>& input, vector<vector<vector<double>>>& varianceTerm, vector<vector<vector<int>>>&countTransfer);

stack<int> backTrace(vector<vector<double>>& input, vector<vector<vector<int>>>& backTable, vector<double>& last);

vector<vector<vector<double>>> getContinuousSeg(int digit_num, vector<vector<vector<double>>>& segTemGroup, vector<vector<double>>& input, vector<vector<vector<double>>>& varianceTerm, vector<vector<vector<int>>>& countTransfer);

vector<vector<int>> getStateIndex(int digit_num, vector<vector<vector<double>>>& segTemGroup, vector<vector<double>>& input, vector<vector<vector<double>>>& varianceTerm, vector<vector<vector<int>>>& countTransfer);

vector<vector<vector<vector<int>>>> getAllStateIndex(int digit_num, vector<vector<vector<double>>>& segTemGroup, vector<vector<vector<vector<double>>>>& input, vector<vector<vector<double>>>& varianceTerm, vector<vector<vector<int>>>& countTransfer);

vector<vector<vector<double>>> getSegFrame(vector<vector<vector<vector<int>>>>& allState, vector<vector<vector<vector<double>>>>& input, vector<vector<vector<double>>>& variance, vector<vector<vector<int>>>& transfer);

stack<int> DigitRecognition(int digit_num, vector<vector<double>>& input, vector<vector<vector<double>>>& segTemGroup, vector<vector<vector<double>>>& varianceTerm, vector<vector<vector<int>>>& countTransfer);

stack<int> DigitRecognitionGussian(int digit_num, vector<vector<double>>& input, vector<vector<vector<double>>>& segTemGroup, vector<vector<vector<double>>>&varianceTerm, vector<vector<vector<int>>>& countTransfer);

stack<int> backTrace_T(vector<vector<double>>& input, vector<vector<vector<int>>>& backTable, vector<double>& last);

stack<int> RestrictPhoneGuassian_T(Trie& trie, vector<vector<double>>& input, vector<vector<vector<double>>>& varianceTerm, vector<vector<vector<int>>>& countTransfer);

unsigned int pureLevenshteinDistance(const vector<int>& s1, const vector<int>& s2);

stack<int> DigitRecognitionGussian_T(int digit_num, vector<vector<double>>& input, vector<vector<vector<double>>>& segTemGroup, vector<vector<vector<double>>> &varianceTerm, vector<vector<vector<int>>>& countTransfer);

#endif


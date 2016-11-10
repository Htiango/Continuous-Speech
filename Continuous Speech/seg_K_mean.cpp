//
//  seg_K_mean.cpp
//  DTW
//
//  Created by hty on 01/10/2016.
//  Copyright © 2016 hty. All rights reserved.
//

#include "seg_K_mean.h"


// return to you which segment the frame currently belonging to
int currentState(int index, vector<vector<int>>& segIndexPrev){
    int current_state = 0;
    for (int i = 0; i < SEG_NUM; i++) {
        // compare the end with the index
        if (index < segIndexPrev[i][1]) {
            current_state = i;
        }
    }
    return current_state;
}

// return the node cost according to the negative log Gaussian formula
double nodeCost(vector<double>& frame, vector<double>& segTem, vector<double>& varianceTerm){
    double nodeCost = 0;
    
    for (int i = 0; i < DIMENSION; i++) {
        //        double x = log(2 * PI * varianceTerm[i]);
//        double variance = varianceTerm[i];
//        double distance = pow((frame[i] - segTem[i]), 2);
//        double gauLog = log(2 * PI * varianceTerm[i]);
        nodeCost += log(2 * PI * varianceTerm[i]) + pow((frame[i] - segTem[i]), 2) / varianceTerm[i];
    }
    
    nodeCost *= 0.5;
    
    return nodeCost;
}


// return the edgecost according to the count/sum
double edgeCost(int nextIndex, vector<int>& countTransfer){
    
    int sum = 0;
    
    int state_num = (int) countTransfer.size();
    
    for (int i = 0; i < state_num; i++) {
        sum += countTransfer[i];
    }
    
    if (countTransfer[nextIndex] == 0) {
        countTransfer[nextIndex] = 1;
    }
    
    double prob = 1.0 * countTransfer[nextIndex] / sum;
    double edgeCost;
    edgeCost = -1 * log(prob);
    return edgeCost;
}


/*
//// return the edgecost according to the count/sum
//double edgeCost(int nextIndex, vector<int>& countTransfer){
//
//    int sum = 0;
//
//    for (int i = 0; i < SEG_NUM; i++) {
//        sum += countTransfer[i];
//    }
//
//    double prob = 1.0 * countTransfer[nextIndex] / sum;
//    double edgeCost;
//    if (prob == 0) {
//        edgeCost = 0;
//    }
//    else{
//        edgeCost = -1 * log(prob);
//    }
//    return edgeCost;
//}
*/
 
 
// compute how to segment one template, return one best way of segment based on the best path and the best value
vector<vector<int>> getOneSegIndex(vector<vector<double>>& temp1, vector<vector<double>>& segmentTem, vector<vector<double>>& varianceTerm, vector<vector<int>>& countTransfer, vector<vector<int>>& segIndexPrev, int state_num){
    
    int frameNum = int(temp1.size());   // the frame number
    vector<vector<int>> temSegIndex;    // store the segment information
    vector<int> oneTemSegIndex(2);      // each segment information has 2 things, first is starting index, the second is ending index
    
    //    // initialize the segment information vector
    //    for (int i = 0; i < SEG_NUM; i++) {
    //        temSegIndex.push_back(oneTemSegIndex);
    //    }
    
    // use the method of two column to compute the best cost
    vector<double> col(state_num), prevCol(state_num), colMax(state_num);
    vector<vector<double>> costMap;
    
    // initialize the colMap vector
    for (int i = 0; i < state_num; i++) {
        colMax[i] = UINT_MAX / 2;
    }
    
    // initialize the costMap vector vector, temporary use col to size the vector
    for (int i = 0; i < frameNum; i++) {
        costMap.push_back(col);
    }
    
    prevCol = colMax;
    prevCol[0] = nodeCost(temp1[0], segmentTem[0], varianceTerm[0]) + edgeCost(0, countTransfer[0]);
    prevCol[1] = nodeCost(temp1[0], segmentTem[1], varianceTerm[1]) + edgeCost(1, countTransfer[0]);
    costMap[0] = prevCol;
    
    
    // get the best cost and the full map of the cost value
    for (int i = 1; i < frameNum; i++) {
        int top = min({ 2 * ( i + 1), state_num});
        col = colMax;
        
        for (int j = 0; j < top; j++) {
            if (j == 0) {
                col[j] = prevCol[j] + nodeCost(temp1[i], segmentTem[j], varianceTerm[j]) + edgeCost(j, countTransfer[j + 1]);
            }
            else if (j == 1) {
                double cost1 = prevCol[j - 1] + edgeCost(j, countTransfer[j]);
                double cost2 = prevCol[j] + edgeCost(j, countTransfer[j + 1]);
                col[j] = min({cost1, cost2}) + nodeCost(temp1[i], segmentTem[j], varianceTerm[j]);
            }
            else{
                double cost1 = prevCol[j - 2] + edgeCost(j, countTransfer[j - 1]);
                double cost2 = prevCol[j - 1] + edgeCost(j, countTransfer[j]);
                double cost3 = prevCol[j] + edgeCost(j, countTransfer[j + 1]);
                col[j] = min({cost1, cost2, cost3}) + nodeCost(temp1[i], segmentTem[j], varianceTerm[j]);
            }
        }
        costMap[i] = col;
        col.swap(prevCol);
    }
    
    
    int dummy_index = state_num - 1;
    int dummy_end = frameNum - 1;
    vector<vector<int>> dummy_segIndex;  // a reverse one
    
    // do the backtracing
    for (int i = frameNum - 2; i >= 0; i--) {
        double cost1, cost2, cost3;
        double minCost;
        if (dummy_index > 1) {
            cost1 = costMap[i][dummy_index];
            cost2 = costMap[i][dummy_index - 1];
            cost3 = costMap[i][dummy_index - 2];
            minCost = min({cost1,cost2,cost3});
            if (minCost == cost2) {
                dummy_index -= 1;
                dummy_segIndex.push_back({i + 1 , dummy_end});
                dummy_end = i;
            }
            else if (minCost == cost3){
                dummy_index -= 2;
                dummy_segIndex.push_back({i + 1, dummy_end});
                dummy_segIndex.push_back({-1 , -1});
                dummy_end = i;
            }
        }
        else if (dummy_index == 1){
            cost1 = costMap[i][dummy_index];
            cost2 = costMap[i][dummy_index - 1];
            minCost = min({cost1,cost2});
            if (minCost == cost2) {
                dummy_index -= 1;
                dummy_segIndex.push_back({i + 1 , dummy_end});
                dummy_end = i;
            }
        }
    }
    
    if (dummy_index != 0) {
        dummy_segIndex.push_back({0, dummy_end});
        dummy_segIndex.push_back({-1,-1});
    }
    else{
        dummy_segIndex.push_back({0, dummy_end});
    }
    
    
    for (int i = 0; i < state_num; i++) {
        temSegIndex.push_back(dummy_segIndex[state_num - 1 - i]) ;
    }
    
    return temSegIndex;
}


// vector<vector<vector<int>>> segIndex : first vector means each template,  second means each segment, third is 2 length [seg starting index, seg ending index], if == -1, means that there is no such segment in this template.
// Here we get the segment template
vector<vector<double>> getSegTem(vector<vector<vector<int>>>& segIndex, vector<vector<vector<double>>>& temGroup, int state_num){
    
    int temNum = int(segIndex.size());
    
    vector<double> oneSeg(DIMENSION);   // each segment
    vector<vector<double>> segmentTem;  // the whole segment template
    vector<int> countVector(state_num);   // count the number of frames in each segment, in order to divide
    
    // initialize the segment vector
    for (int i = 0; i < state_num; i++) {
        segmentTem.push_back(oneSeg);
    }
    
    for (int i = 0 ; i < temNum; i++) {
        for (int j = 0; j < state_num; j++) {
            // get the starting and ending frame's index
            int segStart = segIndex[i][j][0];
            
            // make sure there is jth segment in this template, if not, jump to next segment
            if(segStart >= 0){
                
                int segEnd = segIndex[i][j][1];
                
                // now the jth segment start from segStart and end before segEnd
                for (int k = segStart; k < segEnd + 1; k++) {
                    for (int m = 0; m < DIMENSION; m++) {
                        segmentTem[j][m] += temGroup[i][k][m];      // add to jth segment mth dimension
                    }
                    countVector[j] += 1;     // the number of frames in jth segment add one
                }
            }
        }
    }
    
    // compute the average of the segmental templates.
    for (int i = 0; i < state_num; i++) {
        for (int j = 0; j < DIMENSION; j++) {
            segmentTem[i][j] /= countVector[i];
        }
    }
    
    return segmentTem;
}


// get the segemnt index of each input template. Note that in each segIndex each int is the index of frames meaning the start of one segment. To make it easy, the last one is always the number of frames.
vector<vector<vector<int>>> getSegIndex(vector<vector<double>>& segmentTem, vector<vector<vector<double>>>& temGroup, vector<vector<vector<int>>>& segIndexPrev, vector<vector<double>>& varianceTerm, vector<vector<int>>& countTransfer, int state_num){
    int temNum = int (temGroup.size());
    
    vector<vector<vector<int>>> segIndex;     // store the segment information
    
    //    vector<vector<double>> varianceTerm;   // store the individual variance terms
    //    vector<double> oneSeg(DIMENSION);   // each segment
    
    vector<int> countVector(state_num);   // count the number of frames in each segment, in order to divide
    //    vector<vector<int>> countTransfer;  // first vector is segment part + 1, the second vector is about the number of the next static [(0-1)](index-1 refer to the static).
    //    vector<int> tempTransfer(SEG_NUM);
    
    // initialize the variance term vector and the count transfer vector
    //    for (int i = 0; i < SEG_NUM; i++) {
    //        varianceTerm.push_back(oneSeg);
    //        countTransfer.push_back(tempTransfer);
    //    }
    //    countTransfer.push_back(tempTransfer);
    
    vector<vector<double>> varianceTermEmpty(state_num, vector<double>(DIMENSION));
    varianceTerm = varianceTermEmpty;
    
    vector<vector<int>> countTransferEmpty(state_num + 1, vector<int>(state_num));
    countTransfer = countTransferEmpty;
    
    // count the number of frames in each segment and count the transfer number.
    for (int i = 0; i < temNum; i++) {
        int segGap = 1;
        for (int j = 0; j < state_num; j++) {
            int segStart = segIndexPrev[i][j][0];
            if (segStart >= 0) {
                int segEnd = segIndexPrev[i][j][1];
                // now the jth segment start from segStart and end at segEnd
                for (int k = segStart; k < segEnd + 1; k++) {
                    countVector[j] += 1;     // the number of frames in jth segment add one
                    if (k != segEnd) {
                        countTransfer[j + 1][j] += 1;
                    }
                }
                countTransfer[j - segGap + 1][j] += 1;
                segGap = 1;
            }
            else{
                segGap = 2;
            }
        }
    }
    
    
    // now we compute the sum of individual variance terms
    for (int k = 0; k < temNum; k++) {
        for (int j = 0; j < state_num; j++) {
            int segStart = segIndexPrev[k][j][0];
            if (segStart >= 0 ) {
                int segEnd = segIndexPrev[k][j][1];
                for (int i = segStart; i < segEnd; i++) {
                    for (int l = 0; l < DIMENSION; l++) {
                        varianceTerm[j][l] += pow(temGroup[k][i][l] - segmentTem[j][l], 2);
                    }
                }
            }
        }
    }
    
    // divide by the number of frames in this segment, get the real individual varicance terms.
    for (int j = 0; j < state_num; j++) {
        for (int l = 0; l < DIMENSION; l++) {
            varianceTerm[j][l] /= countVector[j];
        }
    }
    
    
    for (int k = 0; k < temNum; k++) {
        //vector<vector<int>> getOneSegIndex(vector<vector<double>>& temp1, vector<vector<double>>& segmentTem, vector<vector<double>>& varianceTerm, vector<vector<int>>& countTransfer, vector<vector<int>>& segIndexPrev)
        vector<vector<int>> segIndexOf1;
        segIndexOf1 = getOneSegIndex(temGroup[k], segmentTem, varianceTerm, countTransfer, segIndexPrev[k], state_num);
        segIndex.push_back(segIndexOf1);
    }
    
    return segIndex;
}


vector<vector<double>> dtw2hmm(vector<vector<vector<double>>>& temGroup, vector<vector<double>>& varianceTerm, vector<vector<int>>& countTransfer){
    int temNum = int(temGroup.size());
    vector<vector<vector<int>>> segIndex(temNum);
    vector<vector<vector<int>>> segIndexPrev(temNum);
    
    // set the initialize index vector, based on the average of frames
    for (int i = 0; i < temNum; i++) {
        int frameNum = int (temGroup[i].size());
        int gap = frameNum / SEG_NUM;
        for (int j = 0; j < SEG_NUM; j++) {
            if (j == SEG_NUM -1){
                segIndex[i].push_back({j * gap, frameNum - 1});
                segIndexPrev[i].push_back({j * gap, frameNum - 1});
            }
            else{
                segIndex[i].push_back({j * gap, (j + 1) * gap - 1});
                segIndexPrev[i].push_back({j * gap, (j + 1) * gap - 1});
            }
        }
    }
    
    vector<vector<double>> segmentTem;
    vector<double> oneSeg(DIMENSION);   // each segment
    
    // initialize the segment vector
    for (int i = 0; i < SEG_NUM; i++) {
        segmentTem.push_back(oneSeg);
    }
    
    //    vector<vector<double>> varianceTerm(SEG_NUM, vector<double>(DIMENSION));
    //    vector<vector<int>> countTransfer(SEG_NUM + 1, vector<int>(SEG_NUM));
    
    // get the first segment template
    segmentTem = getSegTem(segIndex, temGroup, SEG_NUM);
//    getSegIndex(segmentTem, temGroup, segIndexPrev, varianceTerm, countTransfer,SEG_NUM);
    segIndex = getSegIndex(segmentTem, temGroup, segIndexPrev, varianceTerm, countTransfer,SEG_NUM);
    
    while (segIndex != segIndexPrev) {
        segIndexPrev = segIndex;
        segmentTem = getSegTem(segIndex, temGroup, SEG_NUM);
        segIndex = getSegIndex(segmentTem, temGroup, segIndexPrev, varianceTerm, countTransfer, SEG_NUM);
    }
    
    return segmentTem;
}

/**
 *  use segmental dtw to compute the cost
 *
 *  @param inputAduio input audio feature, seperate into different frames and each frame with a 39-D features
 *  @param temAduio   pre-input template audio feature, the same as input
 *
 *  @return the minimun cost from the best path.
 */
double segmentalDtw(vector<vector<double>>& inputAduio, vector<vector<double>>& temAduio, vector<vector<double>> varianceTerm, vector<vector<int>> countTransfer){
    
    unsigned int inputFrameNum = (unsigned int)inputAduio.size();
    unsigned int temFrameNum = (unsigned int)temAduio.size();
    
    vector<double> col(temFrameNum), prevCol(temFrameNum), colMax(temFrameNum);
    
    for (unsigned i = 0; i < temFrameNum; i++) {
        colMax[i] = UINT_MAX / 2;
    }
    
    prevCol = colMax;
    
    prevCol[0] = nodeCost(inputAduio[0], temAduio[0], varianceTerm[0]) + edgeCost(0, countTransfer[0]);
    prevCol[1] = nodeCost(inputAduio[0], temAduio[1], varianceTerm[0]) + edgeCost(1, countTransfer[0]);
    
    for (unsigned int i = 1; i < inputFrameNum; i++) {
        
        unsigned int temp = min({2 * (i + 1), temFrameNum});
        col = colMax;
        
        for (unsigned int j = 0; j < temp; j++) {
            
            if (j == 0) {
                col[j] = prevCol[j] + nodeCost(inputAduio[i], temAduio[j], varianceTerm[j]) + edgeCost(j, countTransfer[j + 1]);
            }
            
            else if (j == 1){
                double cost1 = prevCol[j - 1] + edgeCost(j, countTransfer[j]);
                double cost2 = prevCol[j] + edgeCost(j, countTransfer[j + 1]);
                col[j] = min({cost1, cost2}) + nodeCost(inputAduio[i], temAduio[j], varianceTerm[j]);
            }
            else{
                double cost1 = prevCol[j - 2] + edgeCost(j, countTransfer[j - 1]);
                double cost2 = prevCol[j - 1] + edgeCost(j, countTransfer[j]);
                double cost3 = prevCol[j] + edgeCost(j, countTransfer[j + 1]);
                col[j] = min({cost1, cost2, cost3}) + nodeCost(inputAduio[i], temAduio[j], varianceTerm[j]);
            }
        }
        
        col.swap(prevCol);
    }
    
    return prevCol[temFrameNum - 1];
}



vector<vector<vector<vector<int>>>> conDtw2hmm(vector<vector<vector<vector<double>>>> temGroup, vector<vector<vector<vector<int>>>> initSegIndex, vector<vector<vector<double>>>& varianceTerm, vector<vector<vector<int>>>& countTransfer)
{
    int tem_type = (int) initSegIndex.size();
    
    // seg-index: seg method, vector: tem_type -> tem_num -> states_num -> start & end
    vector<vector<vector<vector<int>>>> segIndex = initSegIndex;
    vector<vector<vector<vector<int>>>> segIndexPrev = initSegIndex;
    vector<vector<vector<double>>> segmentFullTems;
    
    
    for (int i = 0; i < tem_type; i ++)
    {
        int temp_num = 0;
        int state_num = (int) initSegIndex[i][0].size();
        vector<vector<double>> segmentTem;
        segmentTem = getSegTem(segIndex[i], temGroup[i], state_num);
//        getSegIndex(segmentTem, temGroup[i], segIndexPrev[i], varianceTerm[i], countTransfer[i], state_num);
        segIndex[i] = getSegIndex(segmentTem, temGroup[i], segIndexPrev[i], varianceTerm[i], countTransfer[i], state_num);
        while (segIndex[i] != segIndexPrev[i]) {
            segIndexPrev[i] = segIndex[i];
            segmentTem = getSegTem(segIndex[i], temGroup[i], state_num);
            segIndex[i] = getSegIndex(segmentTem, temGroup[i], segIndexPrev[i], varianceTerm[i], countTransfer[i], state_num);
            temp_num += 1;
        }
//        segmentFullTems.push_back(segmentTem);
    }
    
    return segIndex;
}








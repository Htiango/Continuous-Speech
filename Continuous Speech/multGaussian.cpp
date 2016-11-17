//
//  multGaussian.cpp
//  Continuous Speech
//
//  Created by hty on 11/11/2016.
//  Copyright © 2016 hty. All rights reserved.
//

#include "multGaussian.h"
//#include "seg_K_mean.h"


/**
 * return the euclidean distance of two 39 dimension matrixs
 */
double E_dis(vector<double> input1, vector<double> input2){
    int size = (int)input1.size();
    double distance = 0;
    for (int i = 0; i < size; i++) {
        distance += pow(input1[i] - input2[i], 2);
    }
    return distance;
}


/**
 * get the node cost based on the mixed variance, first assign it to one gauss and then get the node cost
 * @param frame one frame of the input (39 dimension)
 * @param kernel the gauss kernel of one state (gauss_num -> 39 dimension)
 * @return the node cost
 */
double gaussian_node_cost(vector<double>& frame, vector<vector<double>>& kernel, vector<vector<double>>& variance)
{
    int gauss_num = (int) kernel.size();
    int dimension = (int) kernel[0].size();
    double node_cost = 0;
    double minDis = E_dis(frame, kernel[0]);
    int minIndex = 0;
    for (int i = 1; i < gauss_num; i++) {
        double distance = E_dis(frame, kernel[i]);
        if (distance < minDis) {
            minDis = distance;
            minIndex = i;
        }
    }
    for (int i = 0; i < dimension; i++) {
        if (variance[minIndex][i] != 0) {
            node_cost += log(2 * PI_GAUSS * variance[minIndex][i]) + pow((frame[i] - kernel[minIndex][i]), 2) / variance[minIndex][i];
        }
        else{
            node_cost += log(2 * PI_GAUSS * MIN_VARIANCE) + pow((frame[i] - kernel[minIndex][i]), 2) / MIN_VARIANCE;
        }
    }
    return node_cost;
}

/**
 * get the edge cost based on the transfer matrix
 * @param nextIndex the state index that going to enter
 * @param countTransfer the transfer matrix (state_num)
 *
 * @return the edge cost
 */
double gaussian_edge_cost(int nextIndex, vector<int>& countTransfer)
{
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


/**
 * do the k-mean and change the kernel
 * @param stateFrames all frames of this state length -> 39 dimension
 * @param kernel gaussian number -> 39 dimension, basically the average segment frames
 * @return the new kernel (average of the Gauss)
 *
 */
vector<vector<double>> kMean(vector<vector<double>>& stateFrames, vector<vector<double>>& kernel)
{
    int gauss_num = (int) kernel.size();
    int dimension = (int) kernel[0].size();
    int frame_num = (int) stateFrames.size();
    
    vector<vector<double>> kernel_new(gauss_num, vector<double>(dimension));
    
    while(kernel_new != kernel){
        vector<int> frames_per_kernel(gauss_num);
        for (int i = 0; i < frame_num; i++) {
            double minDis = E_dis(stateFrames[i], kernel[0]);
            int minIndex = 0;
            for (int j = 1; j < gauss_num; j++) {
                double distance = E_dis(stateFrames[i], kernel[j]);
                if (distance < minDis) {
                    minDis = distance;
                    minIndex = j;
                }
            }
            
            frames_per_kernel[minIndex] += 1;
            
            for (int k = 0; k < dimension; k++) {
                kernel_new[minIndex][k] += stateFrames[i][k];
            }
        }
        for (int j = 0; j < gauss_num; j++) {
            for (int k = 0; k < dimension; k++) {
                kernel_new[j][k] /= frames_per_kernel[j];
            }
        }
    }
    return kernel_new;
}

/**
 * Split into seperate two gauss from origianl one
 * @param allState (tem_num -> state_num -> start & end), the seg method
 * @param input (temp -> input_length -> 39 dimension)
 * @param gauss_num the number of gauss
 *
 * @return (state_num -> gaussian_num -> 39 dimension)
 *
 */
vector<vector<vector<double>>> splitGaussInit(vector<vector<vector<int>>>& allState, vector<vector<vector<double>>>& input, int gauss_num)
{
    int tem_num = (int)allState.size();
    int state_num = (int)allState[0].size();
    int dimension = (int)input[0][0].size();
    vector<vector<vector<double>>> kernel(state_num, vector<vector<double>>(gauss_num, vector<double>(dimension)));
    vector<vector<vector<double>>> averageFrame(tem_num,vector<vector<double>>(state_num, vector<double>(dimension)));
    vector<vector<double>> count(state_num);
    for(int p = 0; p < tem_num; p++){
        for (int i = 0; i < state_num; i++) {
            int start = allState[p][i][0];
            int end = allState[p][i][1];
            for (int frame_index = start; frame_index <= end; frame_index++){
                for (int j = 0; j < dimension; j++) {
                    averageFrame[p][i][j] += input[p][frame_index][j];
                }
                count[p][i] += 1;
            }
        }
    }
    for(int p = 0; p < tem_num; p++){
        for (int i = 0; i < state_num; i++) {
            for (int j = 0; j < dimension; j++) {
                averageFrame[p][i][j] /= count[p][i];
                kernel[i][0][j] = averageFrame[p][i][j] - TEM_BIAS;
                kernel[i][1][j] = averageFrame[p][i][j] + TEM_BIAS;
            }
        }
    }
    return kernel;
}


/**
 * split twice the num of previous kernel
 * @param gauss_num the number of gauss
 * @param pre_kernel the previous kernel(state_num -> 1/2 gaussian_num -> 39 dimension)
 * @return the new kernel (state_num -> gaussian_num -> 39 dimension)
 */
vector<vector<vector<double>>> splitGaussAfter(int gauss_num, vector<vector<vector<double>>>& pre_kernel)
{
    int state_num = (int) pre_kernel.size();
    int pre_gauss_num = (int) pre_kernel[0].size();
    int dimension = (int) pre_kernel[0][0].size();
    
    vector<vector<vector<double>>> kernel(state_num, vector<vector<double>>(gauss_num, vector<double>(dimension)));
    
    for (int i = 0; i < state_num; i++) {
        for (int j = 0; j < pre_gauss_num; j++) {
            for (int k = 0 ; k < dimension; k++) {
                kernel[i][j * 2][k] = pre_kernel[i][j][k] + TEM_BIAS;
                kernel[i][j * 2 + 1][k] = pre_kernel[i][j][k] - TEM_BIAS;
            }
        }
    }
    return kernel;
}

/**
 * Using the seg method to get each state's frame
 * @param allState (tem_num -> state_num -> start & end), the seg method
 * @param input (tem_num -> input_length -> 39 dimension)
 * @param gauss_num the number of gauss
 * @param kernel_pre the previous gaussian kernel (state_num -> gauss_num -> 39 dimension)
 * 
 * @return (state_num -> gaussian_num -> 39 dimension)
 *
 */
vector<vector<vector<double>>> getStateFrames(vector<vector<vector<int>>>& allState, vector<vector<vector<double>>>& input, vector<vector<vector<double>>>& kernel_pre, int gauss_num)
{
    
    int tem_num = (int)allState.size();
    int state_num = (int)allState[0].size();
    int dimension = (int)input[0][0].size();
    
    vector<vector<vector<double>>> kernel(state_num,vector<vector<double>>(gauss_num, vector<double>(dimension)));
    
    vector<vector<vector<double>>> stateFrame(state_num, vector<vector<double>>());
    
    // get all the frames in each state
    for (int i = 0; i < tem_num; i++) {
        for (int k = 0; k < state_num; k++) {
            int start = allState[i][k][0];
            int end = allState[i][k][1];
            for (int frame_index = start; frame_index <= end; frame_index++) {
                stateFrame[k].push_back(input[i][frame_index]);
            }
        }
    }
    
    // do the k-mean
    for (int k = 0; k < state_num; k++) {
        kernel[k] = kMean(stateFrame[k], kernel_pre[k]);
    }
    return kernel;
}

/**
 * get the variance for each gauss in each state of one template
 * @param input_one one input template (tem_num -> input_length -> 39 dimension)
 * @param kernel the average of the frame assigned to one gauss (state_num -> gauss_num -> 39 dimension)
 * @param segIndex the seg method (tem_num -> state_num -> start & end)
 *
 * @return variance (state_num -> gauss_num -> 39 dimension)
 */
vector<vector<vector<double>>> getVariance(vector<vector<vector<double>>>& input_one, vector<vector<vector<double>>>& kernel, vector<vector<vector<int>>>& segIndex)
{
    int tem_num = (int)input_one.size();
    int state_num = (int)kernel.size();
    int gauss_num = (int)kernel[0].size();
    int dimension = (int)kernel[0][0].size();
    
    vector<vector<vector<double>>> variance(state_num, vector<vector<double>>(gauss_num, vector<double>(dimension)));
    vector<vector<int>> frames_per_kernel(state_num, vector<int>(gauss_num));
    
    for (int m = 0; m < tem_num; m++) {
        for (int i = 0; i < state_num; i++) {

            int start = segIndex[m][i][0];
            int end = segIndex[m][i][1];
            for (int j = start; j <= end; j++) {
                double minDis = E_dis(input_one[m][j], kernel[i][0]);
                int minIndex = 0;
                for (int k = 1; k < gauss_num; k++) {
                    double distance = E_dis(input_one[m][j], kernel[i][k]);
                    if (distance < minDis) {
                        minDis = distance;
                        minIndex = k;
                    }
                }
                
                frames_per_kernel[i][minIndex] += 1;
                
                for (int p = 0; p < dimension; p++) {
                    variance[i][minIndex][p] += pow((input_one[m][j][p] - kernel[i][minIndex][p]), 2);
                }
            }
        }
    }
    
    for (int i = 0; i < state_num; i++) {
        for (int k = 0; k < gauss_num; k++) {
            for (int p = 0; p < dimension; p++) {
                variance[i][k][p] /= frames_per_kernel[i][k];
            }
        }
    }
    
    return variance;
}

/**
 * get the transfer for each gauss in each state of one template
 * @param input_one one input template (tem_num -> input_length -> 39 dimension)
 * @param kernel the average of the frame assigned to one gauss (state_num -> gauss_num -> 39 dimension)
 * @param segIndex the seg method (tem_num -> state_num -> start & end)
 *
 * @return transfer (state_num + 1 -> state_num)
 */
vector<vector<int>> getTransfer(vector<vector<vector<double>>>& input_one, vector<vector<vector<double>>>& kernel, vector<vector<vector<int>>>& segIndex)
{
    int tem_num = (int) input_one.size();
    int state_num = (int)kernel.size();
//    int gauss_num = (int)kernel[0].size();
//    int dimension = (int)kernel[0][0].size();
    vector<vector<int>> transfer((state_num + 1),vector<int>(state_num));
    
    for (int k = 0; k < tem_num; k++) {
        for (int i = 0; i < state_num; i ++) {
            int start = segIndex[k][i][0];
            int end = segIndex[k][i][1];
            for (int j = start; j <= end; j++) {
                if (j != end) {
                    transfer[i + 1][i] += 1;
                }
            }
            transfer[i][i] += 1;
        }
    }
    
    return transfer;
}


/**
 * do the segmental k-mean for one template, and get the segment method
 * @param input_one one input template (tem_num -> input_length -> 39 dimension)
 * @param kernel the average of the frame assigned to one gauss (state_num -> gauss_num -> 39 dimension)
 * @param variance the variance of each gaussian state (state_num -> gauss_num -> 39 dimension)
 * @param transfer the transfer num of each state (state_num + 1 -> state_num)
 *
 * @return seg method (tem_num -> state_num -> start & end)
 */
vector<vector<vector<int>>> getSegMethod(vector<vector<vector<double>>>& input_one, vector<vector<vector<double>>>& kernel, vector<vector<vector<double>>>& variance, vector<vector<int>>& transfer)
{
    int tem_num = (int) input_one.size();
//
    int state_num = (int) kernel.size();
    
    vector<vector<vector<int>>> segIndex;

    for (int m = 0; m < tem_num; m++) {
        int frame_num = (int) input_one[m].size();
        vector<vector<int>> temSegIndex;    // store the segment information
        
        // use the method of two column to compute the best cost
        vector<double> col(state_num), prevCol(state_num), colMax(state_num);
        vector<vector<double>> costMap;
        
        // initialize the colMap vector
        for (int i = 0; i < state_num; i++) {
            colMax[i] = INT_MAX / 2;
        }
        
        // initialize the costMap vector vector, temporary use col to size the vector
        for (int i = 0; i < frame_num; i++) {
            costMap.push_back(col);
        }
        
        prevCol = colMax;
        prevCol[0] = gaussian_node_cost(input_one[m][0], kernel[0], variance[0]) + gaussian_edge_cost(0, transfer[0]);
        costMap[0] = prevCol;
        
        // get the best cost and the full map of the cost value
        for (int i = 1; i < frame_num; i++) {
            int top = min({ ( i + 1), state_num});
            col = colMax;
            
            for (int j = 0; j < top; j++) {
                if (j == 0) {
                    col[j] = prevCol[j] + gaussian_node_cost(input_one[m][i], kernel[j], variance[j]) + gaussian_edge_cost(j, transfer[j + 1]);
                }
                else {
                    double cost1 = prevCol[j - 1] + gaussian_edge_cost(j, transfer[j]);
                    double cost2 = prevCol[j] + gaussian_edge_cost(j, transfer[j + 1]);
                    col[j] = min({cost1, cost2}) + gaussian_node_cost(input_one[m][i], kernel[j], variance[j]);
                }
            }
            costMap[i] = col;
            col.swap(prevCol);
        }
        
        
        int dummy_index = state_num - 1;
        int dummy_end = frame_num - 1;
        vector<vector<int>> dummy_segIndex;  // a reverse one
        
        // do the backtracing
        for (int i = frame_num - 2; i >= 0; i--) {
            double cost1, cost2;
            double minCost;
            if (dummy_index >= 1){
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
        
        dummy_segIndex.push_back({0, dummy_end});
        
        for (int i = 0; i < state_num; i++) {
            temSegIndex.push_back(dummy_segIndex[state_num - 1 - i]) ;
        }
        segIndex.push_back(temSegIndex);
    }
    
    return segIndex;
}


/**
 * get the multi-gauss kernel as well as the relative variance and transfer
 * @param input (tem_type -> tem_num -> input_length -> 39 dimension)
 * @param gauss_num the number of gauss, must be the multiple of 2
 * @param allState (tem_type -> tem_num -> state_num -> start & end), the seg method get from one gaussian segmental k-mean
 * @param variance (tem_type -> state_num -> gauss_num -> 39 dimension)
 * @param transfer (tem_type -> state_num + 1 -> state_num)
 *
 * @return the multi-gauss kernel (tem_type -> state_num -> gauss_num -> 39 dimension)
 */
vector<vector<vector<vector<double>>>> multiGaussDtw2hmm(vector<vector<vector<vector<double>>>>& input, int gauss_num, vector<vector<vector<vector<int>>>>& allState, vector<vector<vector<vector<double>>>>& variance, vector<vector<vector<int>>>& transfer)
{
    int tem_type = (int) input.size();
//    int tem_num = (int) input[0].size();
//    int state_num = (int) allState[0][0].size();
//    int dimension = (int) input[0][0][0].size();
    vector<vector<vector<vector<double>>>> multi_kernel;
    
    vector<vector<vector<vector<int>>>> segIndex = allState;
    vector<vector<vector<vector<int>>>> segIndexPrev = allState;
    
    for (int i = 0; i < tem_type; i++) {
        
        vector<vector<vector<double>>> kernel_temp;
        
        for (int k = 2; k <= gauss_num; k *= 2) {
            vector<vector<vector<double>>> kernel;
            vector<vector<vector<double>>> kernelPrev;
            
            if (k == 2) {
                kernel = splitGaussInit(segIndex[i], input[i], k);
            }
            else{
                kernel = splitGaussAfter(k, kernel_temp);
                kernel_temp.clear();
            }
            kernelPrev = kernel;
            
            kernel = getStateFrames(segIndex[i], input[i], kernelPrev, k);
            kernelPrev = kernel;
            vector<vector<vector<double>>> variance_temp = getVariance(input[i], kernel, segIndex[i]);
            vector<vector<int>> transfer_temp = getTransfer(input[i], kernel, segIndex[i]);
            segIndex[i] = getSegMethod(input[i], kernel, variance_temp, transfer_temp);

            while (segIndex[i] != segIndexPrev[i]) {
                segIndexPrev[i] = segIndex[i];
                kernel = getStateFrames(segIndex[i], input[i], kernelPrev, k);
                kernelPrev = kernel;
                vector<vector<vector<double>>> variance_temp = getVariance(input[i], kernel, segIndex[i]);
                vector<vector<int>> transfer_temp = getTransfer(input[i], kernel, segIndex[i]);
                segIndex[i] = getSegMethod(input[i], kernel, variance_temp, transfer_temp);
            }
            kernel_temp = kernel;
            if (k == gauss_num) {
                variance[i] = variance_temp;
                transfer[i] = transfer_temp;
            }
        }
        multi_kernel.push_back(kernel_temp);
    }
    return multi_kernel;
}





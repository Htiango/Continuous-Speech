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
        distance += pow(input1[i], input2[i]);
    }
    return distance;
}


/**
 * do the k-mean and change the kernel
 * @param stateFrames input_length -> 39 dimension
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
 * Using the seg method to get each state's frame
 * @param allState (tem_type -> tem_num -> state_num -> start & end), the seg method
 * @param input (tem_type -> tem_num -> input_length -> 39 dimension)
 * @param gauss_num the number of gauss
 * @param kernel_pre the previous gaussian kernel (tem_type -> state_num -> gauss_num -> 39 dimension)
 * 
 * @return (digit_type -> seg_num -> gaussian_num -> 39 dimension)
 *
 */
vector<vector<vector<vector<double>>>> getStateFrames(vector<vector<vector<vector<int>>>>& allState,vector<vector<vector<vector<double>>>>& input, vector<vector<vector<vector<double>>>>& kernel_pre,int gauss_num)
{
    
    int tem_type = (int)input.size();
    int tem_num = (int)input[0].size();
    int state_num = (int)allState[0][0].size();
    int dimension = (int)input[0][0][0].size();
    
    vector<vector<vector<vector<double>>>> temSeg(tem_type, vector<vector<vector<double>>>(state_num,vector<vector<double>>(gauss_num, vector<double>(dimension))));
    
    vector<vector<vector<vector<double>>>> stateFrame(tem_type, vector<vector<vector<double>>>(state_num, vector<vector<double>>()));
    
    // get all the frames in each state
    for (int i = 0; i < tem_type; i++) {
        for (int j = 0; j < tem_num; j++) {
            for (int k = 0; k < state_num; k++) {
                int start = allState[i][j][k][0];
                int end = allState[i][j][k][1];
                for (int frame_index = start; frame_index <= end; frame_index++) {
                    stateFrame[i][k].push_back(input[i][j][frame_index]);
                }
            }
        }
    }
    
    // do the k-mean
    for (int i = 0; i < tem_type; i++) {
        for (int j = 0; j < tem_num ; j++) {
            for (int k = 0; k < state_num; k++) {
                temSeg[i][k] = kMean(stateFrame[i][k], kernel_pre[i][k]);
            }
        }
    }
    
    return temSeg;
}

/**
 * get the variance for each gauss in each state of one template
 * @param input_one one input template (input_length -> 39 dimension)
 * @param kernel the average of the frame assigned to one gauss (state_num -> gauss_num -> 39 dimension)
 * @param segIndex the seg method (state_num -> start & end)
 *
 * @return variance (state_num -> gauss_num -> 39 dimension)
 */
vector<vector<vector<double>>> getVariance(vector<vector<double>>& input_one, vector<vector<vector<double>>>& kernel, vector<vector<double>>& segIndex)
{
//    int input_length = (int)input_one.size();
    int state_num = (int)kernel.size();
    int gauss_num = (int)kernel[0].size();
    int dimension = (int)kernel[0][0].size();
    
    vector<vector<vector<double>>> variance(state_num, vector<vector<double>>(gauss_num, vector<double>(dimension)));
    
    for (int i = 0; i < state_num; i++) {
        vector<int> frames_per_kernel(gauss_num);
        int start = segIndex[i][0];
        int end = segIndex[i][1];
        for (int j = start; j <= end; j++) {
            double minDis = E_dis(input_one[j], kernel[i][0]);
            int minIndex = 0;
            for (int k = 1; k < gauss_num; k++) {
                double distance = E_dis(input_one[j], kernel[i][k]);
                if (distance < minDis) {
                    minDis = distance;
                    minIndex = k;
                }
            }
            
            frames_per_kernel[minIndex] += 1;
            
            for (int p = 0; p < dimension; p++) {
                variance[i][minIndex][p] += pow((input_one[j][p] - kernel[i][minIndex][p]), 2);
            }
        }
        for (int k = 0; k < gauss_num; k++) {
            for (int p = 0; p < dimension; p++) {
                variance[i][k][p] /= frames_per_kernel[k];
            }
        }
    }
    return variance;
}

/**
 * get the transfer for each gauss in each state of one template
 * @param input_one one input template (input_length -> 39 dimension)
 * @param kernel the average of the frame assigned to one gauss (state_num -> gauss_num -> 39 dimension)
 * @param segIndex the seg method (state_num -> start & end)
 *
 * @return transfer (state_num + 1 -> state_num)
 */
vector<vector<int>> getTransfer(vector<vector<double>>& input_one, vector<vector<vector<double>>>& kernel, vector<vector<double>>& segIndex){
    int state_num = (int)kernel.size();
//    int gauss_num = (int)kernel[0].size();
//    int dimension = (int)kernel[0][0].size();
    vector<vector<int>> transfer((state_num + 1),vector<int>(state_num));
    
    for (int i = 0; i < state_num; i ++) {
        int start = segIndex[i][0];
        int end = segIndex[i][1];
        for (int j = start; j <= end; j++) {
            if (j != end) {
                transfer[i + 1][i] += 1;
            }
        }
        transfer[i][i] += 1;
    }
    return transfer;
}


/**
 * do the segmental k-mean for one template, and get the segment method
 * @param input_one one input template (input_length -> 39 dimension)
 * @param kernel the average of the frame assigned to one gauss (state_num -> gauss_num -> 39 dimension)
 *
 * @return seg method (state_num -> start & end)
 */
vector<vector<int>> getSegMethod(vector<vector<double>>& input_one, vector<vector<vector<double>>>& kernel)
{
    
}




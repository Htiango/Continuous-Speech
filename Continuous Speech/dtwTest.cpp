//
//  dtwTest.cpp
//  DTW
//
//  Created by hty on 01/10/2016.
//  Copyright © 2016 hty. All rights reserved.
//

#include "dtwTest.h"
//
//string wavTemPath = "/Users/hty/desktop/testingData/test data2/template";
//string txtTemPath = "/Users/hty/desktop/testingData/test data2/template";
//string wavInputPath = "/Users/hty/desktop/testingData/test data2/input";
//string txtInputPath = "/Users/hty/desktop/testingData/test data2/input";
string wavTestPath1 = "/Users/hty/desktop/testingData/test data2";
string txtTestPath1 = "/Users/hty/desktop/testingData/test data2";


string wavTemPath1 = "/Users/hty/desktop/testingData/test 4.0/template";
string txtTemPath1 = "/Users/hty/desktop/testingData/test 4.0/template";
string wavInputPath1 = "/Users/hty/desktop/testingData/test 4.0/input";
string txtInputPath1 = "/Users/hty/desktop/testingData/test 4.0/input";

//string wavTemPath = "/Users/hty/desktop/testingData/temp/template";
//string txtTemPath = "/Users/hty/desktop/testingData/temp/template";
//string wavInputPath = "/Users/hty/desktop/testingData/temp/input";
//string txtInputPath = "/Users/hty/desktop/testingData/temp/input";

//


//string wavTemPath = "/Users/hty/Desktop/testingData/Correct DTW Testing Data/test2.0/template";
//string txtTemPath = "/Users/hty/Desktop/testingData/Correct DTW Testing Data/test2.0/template";
//string wavInputPath = "/Users/hty/Desktop/testingData/Correct DTW Testing Data/test2.0/input";
//string txtInputPath = "/Users/hty/Desktop/testingData/Correct DTW Testing Data/test2.0/input";

void getTem(vector<vector<vector<double>>>& temGroup){
    for (unsigned int i = 0; i < TYPE_NUM; i++) {
//    for (unsigned int i = 0; i < 1; i++) {
        for(unsigned int j = 0; j < TEM_NUM; j ++){
            cout << "-----------------------Template " << i << " Instance " << j <<"------------------------" << endl;
            string wavpath = wavTemPath1 +  to_string(i) + "/" + to_string(j)+ "/record.wav";
            
//            capture(wavpath);
            
//            cout << "Enter 0 to go on, otherwise to record again"<< endl;
//            int out = getchar();
//            while (out != 48) {
//                cout << "Enter 0 to go on, otherwise to record again"<< endl;
//                out = getchar();
//                capture(wavpath);
//            }

            
            vector<vector<double>> temFeature;
            string txtpath = txtTemPath1 + to_string(i) +  "/" + to_string(j) +  "/";
            featureExtraction(temFeature, wavpath, txtpath);
            temGroup.push_back(temFeature);
        }
    }
}


void getInput(vector<vector<double>>& input, string& wavPath, string& txtPath){
//    capture(wavPath);
//    getchar();
    featureExtraction(input, wavPath, txtPath);
}

// dtw
void getResult(){
    vector<vector<vector<double>>> temGroup;
    
    getTem(temGroup);
    
    int correctNum =0;
    
    cout << "-------------------------------------------------------DTW result---------------------------------------------------" << endl;
    
    for (unsigned int i = 0; i < TYPE_NUM; i++) {
        
        for (unsigned int j = 0; j < INPUT_NUM; j++) {
            cout << endl;
            cout <<"-----------------------Input " << i <<"   Instance " << j << "------------------------" << endl;
            vector<vector<double>> input;
            string wavPath = wavInputPath1 + to_string(i) + "/" + to_string(j) + "/record.wav";
            string txtPath = txtInputPath1 + to_string(i) + "/" + to_string(j) + "/";
//            capture(wavPath);
//            cout << "Enter 0 to go on, otherwise to record again"<< endl;
//            int out = getchar();
//            while (out != 48) {
//                cout << "Enter 0 to go on, otherwise to record again"<< endl;
//                out = getchar();
//                capture(wavPath);
//            }
            featureExtraction(input, wavPath, txtPath);
            
            double minDtw = dtw(input, temGroup[0]);
            int bestIndex = 0;
            cout << "The input is " << i << ", the dtwValue with 0.0 is " << minDtw << endl;
            
            for (unsigned int k = 1; k < TEM_NUM * TYPE_NUM; k++) {
                double dtwValue = dtw(input, temGroup[k]);
                cout << "The input is " << i << ", the dtwValue with "<< k / TEM_NUM << "."<< k % TEM_NUM << " is " << dtwValue << endl;
                if (dtwValue < minDtw) {
                    bestIndex = k;
                    minDtw = dtwValue;
                }
            }
            cout << "The input is " << i << ", while the match template is " << bestIndex / TEM_NUM << "."<< bestIndex % TEM_NUM << ". Their dtw value is "<< minDtw <<  endl;
            
            if (i == bestIndex / TEM_NUM) {
                correctNum += 1;
                cout << "The matching is correct!! Now the correct number is " << correctNum << endl;
            }
            else{
                cout << "The matching is wrong.........xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
            }
        }
    }
    
    cout << "The total number of input is "<< TYPE_NUM * INPUT_NUM << endl;
    cout << "The correct matching input number is "<< correctNum << endl;
    cout << "The accuracy of the matching is " << (1.0 * correctNum / (TYPE_NUM * INPUT_NUM)) << endl;
}

// beamSynchronousDTW
void getSynDTWResult(){
    vector<vector<vector<double>>> temGroup;
    
    getTem(temGroup);
    
    int correctNum =0;
    
    cout << "-------------------------------------------------------syn DTW result---------------------------------------------------" << endl;
    
    for (unsigned int i = 0; i < TYPE_NUM; i++) {
        
        for (unsigned int j = 0; j < INPUT_NUM; j++) {
            cout << endl;
            cout <<"-----------------------Input " << i <<"   Instance " << j << "------------------------" << endl;
            vector<vector<double>> input;
            string wavPath = wavInputPath1 + to_string(i) + "/" + to_string(j) + "/record.wav";
            string txtPath = txtInputPath1 + to_string(i) + "/" + to_string(j) + "/";
            getInput(input, wavPath, txtPath);
            
            vector<double> costMap = beamSynchronousDTW(input, temGroup);
            
            double minDtw = costMap[0];
            int bestIndex = 0;
            cout << "The input is " << i << ", the dtwValue with 0.0 is " << minDtw << endl;
            
            for (unsigned int k = 1; k < TEM_NUM * TYPE_NUM; k++) {
                double dtwValue = costMap[k];
                cout << "The input is " << i << ", the dtwValue with "<< k / TEM_NUM << "."<< k % TEM_NUM << " is " << dtwValue << endl;
                if (dtwValue < minDtw) {
                    bestIndex = k;
                    minDtw = dtwValue;
                }
            }
            if (minDtw == UINT_MAX / 2) {
                cout << "All the path is rid off. No match template!" << endl;
                bestIndex = UINT_MAX / 2;
            }
            else {
                cout << "The input is " << i << ", while the match template is " << bestIndex / TEM_NUM << "."<< bestIndex % TEM_NUM << ". Their dtw value is "<< minDtw <<  endl;
            }
            
            if (i == bestIndex / TEM_NUM) {
                correctNum += 1;
                cout << "The matching is correct!! Now the correct number is " << correctNum << endl;
            }
            else{
                cout << "The matching is wrong.........xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
            }
        }
    }
    
    cout << "The total number of input is "<< TYPE_NUM * INPUT_NUM << endl;
    cout << "The correct matching input number is "<< correctNum << endl;
    cout << "The accuracy of the matching is " << (1.0 * correctNum / (TYPE_NUM * INPUT_NUM)) << endl;
}


// segmental k-mean
void getSegTem(){
    vector<vector<vector<double>>> segTemGroup;
    for (int i = 0; i < TYPE_NUM; i++) {
        vector<vector<vector<double>>> temGroup;
        for (int j = 0; j < TEM_NUM; j++) {
            cout << "-----------------------Template " << i << " Instance " << j <<"------------------------" << endl;
            string wavpath = wavTemPath1 +  to_string(i) + "/" + to_string(j)+ "/record.wav";
//            capture(wavpath);
            vector<vector<double>> temFeature;
            string txtpath = txtTemPath1 + to_string(i) + "/" + to_string(j) + "/";
            featureExtraction(temFeature, wavpath, txtpath);
            temGroup.push_back(temFeature);
        }
        vector<vector<double>> segTem;
        segTem = dtw2hmm(temGroup);
        cout << "You have got the segment template!!!!!!!!!!!!!!!!!!!"<<endl;
        segTemGroup.push_back(segTem);
    }
    
    int correctNum =0;
    
    cout << "-------------------------------------------------------seg K-mean result---------------------------------------------------" << endl;
    
    for (unsigned int i = 0; i < TYPE_NUM; i++) {
        
        for (unsigned int j = 0; j < INPUT_NUM; j++) {
            cout << endl;
            cout <<"-----------------------Input " << i <<"   Instance " << j << "------------------------" << endl;
            vector<vector<double>> input;
            string wavPath = wavInputPath1 + to_string(i) + "/" + to_string(j) + "/record.wav";
            string txtPath = txtInputPath1 + to_string(i) + "/" + to_string(j) + "/";
            getInput(input, wavPath, txtPath);
            
            double minDtw = dtw(input, segTemGroup[0]);
            int bestIndex = 0;
            cout << "The input is " << i << ", the dtwValue with 0.0 is " << minDtw << endl;
            
            for (unsigned int k = 1; k < TYPE_NUM; k++) {
                double dtwValue = dtw(input, segTemGroup[k]);
                cout << "The input is " << i << ", the dtwValue with "<< k  << " is " << dtwValue << endl;
                if (dtwValue < minDtw) {
                    bestIndex = k;
                    minDtw = dtwValue;
                }
            }
            cout << "The input is " << i << ", while the match template is " << bestIndex  << ". Their dtw value is "<< minDtw <<  endl;
            
            if (i == bestIndex) {
                correctNum += 1;
                cout << "The matching is correct!! Now the correct number is " << correctNum << endl;
            }
            else{
                cout << "The matching is wrong.......xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
            }
        }
    }
    
    cout << "The total number of input is "<< TYPE_NUM * INPUT_NUM << endl;
    cout << "The correct matching input number is "<< correctNum << endl;
    cout << "The accuracy of the matching is " << (1.0 * correctNum / (TYPE_NUM * INPUT_NUM)) << endl;
    
    
}



// test segmental k-mean
void testSegTem(){
    vector<vector<vector<double>>> segTemGroup;
    for (int i = 0; i < TYPE_NUM; i++) {
        vector<vector<vector<double>>> temGroup;
        for (int j = 0; j < TEM_NUM; j++) {
            cout << "-----------------------Template " << i << " Instance " << j <<"------------------------" << endl;
            string wavpath = wavTemPath1 +  to_string(i) + "/" + to_string(j)+ "/record.wav";
            //            capture(wavpath);
            vector<vector<double>> temFeature;
            string txtpath = txtTemPath1 + to_string(i) + "/" + to_string(j) + "/";
            featureExtraction(temFeature, wavpath, txtpath);
            temGroup.push_back(temFeature);
        }
        vector<vector<double>> segTem;
        segTem = dtw2hmm(temGroup);
        cout << "You have got the segment template!!!!!!!!!!!!!!!!!!!"<<endl;
        segTemGroup.push_back(segTem);
    }
    
    
    cout << "Please enter any key to start to input. (Enter 0 to exit)" << endl;
    int i = getchar();
    
    while (i != 48) {
        vector<vector<double>> input;
        string wavPath = wavTestPath1 + "/test/record.wav";
        string txtPath = txtTestPath1 + "/test/";
        featureExtraction(input, wavPath, txtPath);
        
        double minDtw = dtw(input, segTemGroup[0]);
        int bestIndex = 0;
        cout << "The dtwValue with 0 is " << minDtw << endl;
        
        for (unsigned int k = 1; k < TYPE_NUM; k++) {
            double dtwValue = dtw(input, segTemGroup[k]);
            cout << "The dtwValue with "<< k  << " is " << dtwValue << endl;
            if (dtwValue < minDtw) {
                bestIndex = k;
                minDtw = dtwValue;
            }
        }
        cout << "The match template is " << bestIndex  << ". Their dtw value is "<< minDtw <<  endl;
        cout << endl;
    }
    
}


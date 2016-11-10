//
//  main.cpp
//  Continuous Speech
//
//  Created by hty on 03/11/2016.
//  Copyright © 2016 hty. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "ContinuousPhone.h"
#include "readwave.h"
#include "seg_K_mean.h"
#include "captureData.h"
#include "portaudio.h"
#include "featureExtraction.h"   

using namespace std;

//string testPath = "/Users/hty/Desktop/record/1234/record.wav";
string testPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/englishTem/continueTem/record.wav";
string testTxtPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/englishTem/continueTem/";

string wavTestPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/record.wav";
string txtTestPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/";


//used to save segment result
string segmentPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/englishTem/isolateTem/segment.txt";
string variancePath = "/Users/hty/Desktop/Speech Recognition/project/project 6/englishTem/isolateTem/variance.txt";
string transferPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/englishTem/isolateTem/countTransfer.txt";


string wavTemPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/englishTem/isolateTem/";
string txtTemPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/englishTem/isolateTem/";


string wavContinueTemPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/englishTem/requiredTem/";
string txtContinueTemPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/englishTem/requiredTem/";

string txtIsolateFromConPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/englishTem/requiredTem/continue-isolatedword/segment.txt";

//string wavTemPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/test 4.0/template";
//string txtTemPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/test 4.0/template";
//string wavInputPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/test 4.0/input";
//string txtInputPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/test 4.0/input";

//string wavTemPath1 = "/Users/hty/Desktop/Speech Recognition/project/project 6/sample/";
//string txtTemPath1 = "/Users/hty/Desktop/Speech Recognition/project/project 6/sample/";

void writeSeg(string wavPath, string txtPath){
    vector<vector<vector<double>>> segTemGroup;
    vector<vector<vector<double>>> varianceTerm(TYPE_NUM, vector<vector<double>>(SEG_NUM, vector<double>(DIMENSION)));
    vector<vector<vector<int>>> countTransfer(TYPE_NUM, vector<vector<int>>(SEG_NUM + 1, vector<int>(SEG_NUM)));
    for (int i = 0; i < TYPE_NUM; i++) {
        vector<vector<vector<double>>> temGroup;
        for (int j = 0; j < TEM_NUM; j++) {
            cout << "-----------------------Template " << i << " Instance " << j << "------------------------" << endl;
            string wavpath = wavPath + to_string(i) + "/" + to_string(j) + "/record.wav";
//            string wavpath = wavTemPath + "s-" + to_string(i) + "-" + to_string(j + 1) + ".wav";
            //            capture(wavpath);
            vector<vector<double>> temFeature;
            string txtpath = txtPath + to_string(i) + "/" + to_string(j) + "/";
            featureExtraction(temFeature, wavpath, txtpath);
            temGroup.push_back(temFeature);
        }
        vector<vector<double>> segTem;
        segTem = dtw2hmm(temGroup, varianceTerm[i], countTransfer[i]);
        cout << "You have got the segment template!!!!!!!!!!!!!!!!!!!" << endl;
        segTemGroup.push_back(segTem);
    }


    ofstream out(segmentPath);
    ofstream out2(variancePath);
    ofstream out3(transferPath);
    for(int i = 0; i < TYPE_NUM; i++)
    {
        cout << "template " << i << endl;
        
        for (int j = 0; j < SEG_NUM + 1; j++) {
            for (int k = 0; k < SEG_NUM; k++) {
                out3 << countTransfer[i][j][k] << " ";
            }
            out3 << endl;
        }
        
        for(int j = 0; j < SEG_NUM; j++)
        {
            cout << "state " << j << endl;
            for(int k = 0; k < DIMENSION; k++)
            {
                out << segTemGroup[i][j][k] << " ";
                out2 << varianceTerm[i][j][k] << " ";
            }
            out << endl;
            out2 << endl;
        }
        out << endl;
        out2 << endl;
        out3 << endl;
    }
    out.close();
    out2.close();
    out3.close();
}


void problem3(vector<vector<vector<double>>> segTemGroup, vector<vector<double>> testInput, vector<vector<vector<double>>> varianceTerm, vector<vector<vector<int>>> countTransfer)
{
    Trie trie;
    TrieNode* root = trie.getRoot();
    for (int i = 0; i < MAX_BRANCH_NUM - 1; i++)
    {
        root->nextBranch[i]->segTemplate = segTemGroup[i];
    }
    RestrictPhone(trie, testInput, varianceTerm, countTransfer);
    cout << endl;
}


void problem1(vector<vector<vector<double>>> segTemGroup,vector<vector<double>> testInput, vector<vector<vector<double>>> varianceTerm, vector<vector<vector<int>>> countTransfer)
{
    DigitRecognition(DIGIT_NUM4, testInput, segTemGroup, varianceTerm, countTransfer);
    cout << endl;
}

void getResult(){
    vector<vector<vector<double>>> segTemGroup(TYPE_NUM, vector<vector<double>>(SEG_NUM, vector<double>(DIMENSION)));
    vector<vector<vector<double>>> varianceTerm(TYPE_NUM, vector<vector<double>>(SEG_NUM, vector<double>(DIMENSION)));
    vector<vector<vector<int>>> countTransfer(TYPE_NUM, vector<vector<int>>(SEG_NUM + 1, vector<int>(SEG_NUM)));
    
    ifstream in(segmentPath);
    ifstream in2(variancePath);
    ifstream in3(transferPath);
    for (int i = 0; i < TYPE_NUM; i++)
    {
        for (int j = 0; j < SEG_NUM; j++)
        {
            for (int k = 0; k < DIMENSION; k++)
            {
                in >> segTemGroup[i][j][k];
                in2 >> varianceTerm[i][j][k];
            }
        }
        for (int j = 0; j < SEG_NUM + 1; j++) {
            for (int k = 0; k < SEG_NUM; k++) {
                in3 >> countTransfer[i][j][k];
            }
        }
    }
    in.close();
    in2.close();
    in3.close();
    
    
    vector<vector<double>> testInput;
//    featureExtractionTwo(testInput, wavTestPath, txtTestPath);
    featureExtractionTwo(testInput, testPath, testTxtPath);
    
    cout << "problem3 result : ";
    problem3(segTemGroup, testInput, varianceTerm, countTransfer);
    
    cout << endl;
    cout << "problem1 result : ";
    problem1(segTemGroup, testInput, varianceTerm, countTransfer);
}

void trainDigits() {
    vector<vector<vector<double>>> segTemGroup(TYPE_NUM, vector<vector<double>>(SEG_NUM, vector<double>(DIMENSION)));
    vector<vector<vector<double>>> varianceTerm(TYPE_NUM, vector<vector<double>>(SEG_NUM, vector<double>(DIMENSION)));
    vector<vector<vector<int>>> countTransfer(TYPE_NUM, vector<vector<int>>(SEG_NUM + 1, vector<int>(SEG_NUM)));
    ifstream in(segmentPath);
    ifstream in2(variancePath);
    ifstream in3(transferPath);
    for (int i = 0; i < TYPE_NUM; i++)
    {
        for (int j = 0; j < SEG_NUM; j++)
        {
            for (int k = 0; k < DIMENSION; k++)
            {
                in >> segTemGroup[i][j][k];
                in2 >> varianceTerm[i][j][k];
            }
        }
        for (int j = 0; j < SEG_NUM + 1; j++) {
            for (int k = 0; k < SEG_NUM; k++) {
                in3 >> countTransfer[i][j][k];
            }
        }
    }
    in.close();
    in2.close();
    in3.close();
    
    vector<vector<vector<vector<double>>>> input(TRAIN_TYPE, vector<vector<vector<double>>>(TRAIN_NUM, vector<vector<double>>()));
    
    
    string dir[] = { "0123456789", "0987654321", "1234567890", "1357902468", "8642097531", "9876543210" };
    for(int i = 0; i < TRAIN_TYPE; i++)
    {
        for(int j = 0; j < TRAIN_NUM; j++)
        {
            string wavTestPath = wavContinueTemPath + dir[i] + "/" + to_string(j) + "/record.wav";
            string txtTestPath = txtContinueTemPath + dir[i] + "/" + to_string(j) + "/result.txt";
//            string txtTestPath = txtContinueTemPath + dir[i] + "/" + to_string(j) + "/";
            ifstream inTrain(txtTestPath);
            vector<vector<double>> testInput;
            vector<double> singleInput;
            double temp;
            for(int k = 0; !inTrain.eof(); k++)
            {
                if(k == DIMENSION)
                {
                    k = 0;
                    testInput.push_back(singleInput);
                    singleInput.clear();
                }
                inTrain >> temp;
                singleInput.push_back(temp);
            }
//            featureExtractionTwo(testInput, wavTestPath, txtTestPath);
            input[i][j] = testInput;
        }
    }
    
    cout << "Read in finished!" << endl;
    
    vector<vector<vector<vector<int>>>> allState = getAllStateIndex(DIGIT_NUM10, segTemGroup, input, varianceTerm, countTransfer);
    
    cout << "Finish Loop!" << endl;
    
    vector<vector<vector<double>>> variance(TRAIN_TYPE, vector<vector<double>>(DIGIT_NUM * SEG_NUM, vector<double>(DIMENSION)));
    vector<vector<vector<int>>> transfer(TRAIN_TYPE, vector<vector<int>>(DIGIT_NUM * SEG_NUM + 1, vector<int>(DIGIT_NUM * SEG_NUM)));
    vector<vector<vector<vector<int>>>> resultState = conDtw2hmm(input, allState, variance, transfer);
    vector<vector<vector<double>>>resultSeg = getSegFrame(resultState, input);
    
    ofstream out(txtIsolateFromConPath);
    for(int i = 0; i < DIGIT_NUM; i++)
    {
        for(int j = 0; j < SEG_NUM; j++)
        {
            for (int k = 0; k < DIMENSION; k++)
            {
                out << resultSeg[i][j][k] << " ";
            }
            out << endl;
        }
        out << endl;
    }
    out.close();
    cout << endl;
}


void record(){
    string filepath = "/Users/hty/desktop/record/567/record.wav";
    capture(filepath);
}

void readSeg(){
    vector<vector<vector<double>>> segTemGroup(DIGIT_NUM, vector<vector<double>>(SEG_NUM, vector<double>(DIMENSION)));
//    vector<vector<vector<double>>> varianceTerm(TYPE_NUM, vector<vector<double>>(SEG_NUM, vector<double>(DIMENSION)));
//    vector<vector<vector<int>>> countTransfer(TYPE_NUM, vector<vector<int>>(SEG_NUM + 1, vector<int>(SEG_NUM)));
    
    ifstream in(txtIsolateFromConPath);
//    ifstream in2(variancePath);
//    ifstream in3(transferPath);
    for (int i = 0; i < TYPE_NUM; i++)
    {
        for (int j = 0; j < SEG_NUM; j++)
        {
            for (int k = 0; k < DIMENSION; k++)
            {
                in >> segTemGroup[i][j][k];
//                in2 >> varianceTerm[i][j][k];
            }
        }
//        for (int j = 0; j < SEG_NUM + 1; j++) {
//            for (int k = 0; k < SEG_NUM; k++) {
//                in3 >> countTransfer[i][j][k];
//            }
//        }
    }
    in.close();
//    in2.close();
//    in3.close();
    
    vector<vector<double>> testInput;
    //    featureExtractionTwo(testInput, wavTestPath, txtTestPath);
    featureExtractionTwo(testInput, testPath, testTxtPath);
    
    vector<vector<vector<double>>> varianceTerm;
    vector<vector<vector<int>>> countTransfer;
    
    cout << "problem3 result : ";
    problem3(segTemGroup, testInput, varianceTerm, countTransfer);
    
}

// test segmental k-mean
int main(int argc, const char * argv[]) {
//    writeSeg(wavTemPath, txtTemPath);    // read in the isolated word

  
//    trainDigits();
    readSeg();
//    record();
    
    getResult();
    
    return 0;
}





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

using namespace std;

string testPath = "/Users/hty/Desktop/1/record.wav";

string wavTestPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/record.wav";
string txtTestPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/";
//used to save segment result
string segmentPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/segment.txt";


string wavTemPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/test 4.0/template";
string txtTemPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/test 4.0/template";
string wavInputPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/test 4.0/input";
string txtInputPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/test 4.0/input";



void writeSeg(){
     vector<vector<vector<double>>> segTemGroup;
    
     for (int i = 0; i < TYPE_NUM; i++) {
         vector<vector<vector<double>>> temGroup;
         for (int j = 0; j < TEM_NUM; j++) {
             cout << "-----------------------Template " << i << " Instance " << j << "------------------------" << endl;
             string wavpath = wavTemPath + to_string(i) + "/" + to_string(j) + "/record.wav";
             //            capture(wavpath);
             vector<vector<double>> temFeature;
             string txtpath = txtTemPath + to_string(i) + "/" + to_string(j) + "/";
             featureExtraction(temFeature, wavpath, txtpath);
             temGroup.push_back(temFeature);
         }
         vector<vector<double>> segTem;
         segTem = dtw2hmm(temGroup);
         cout << "You have got the segment template!!!!!!!!!!!!!!!!!!!" << endl;
         segTemGroup.push_back(segTem);
     }
     
     
     ofstream out(segmentPath);
     for(int i = 0; i < TYPE_NUM; i++)
     {
     cout << "template " << i << endl;
     for(int j = 0; j < SEG_NUM; j++)
     {
     cout << "state " << j << endl;
     for(int k = 0; k < DIMENSION; k++)
     {
     out << segTemGroup[i][j][k] << " ";
     }
     out << endl;
     }
     out << endl;
     }
    
}


void problem3(vector<vector<vector<double>>> segTemGroup, vector<vector<double>> testInput){
    Trie trie;
    TrieNode* root = trie.getRoot();
    for (int i = 0; i < MAX_BRANCH_NUM - 1; i++)
    {
        root->nextBranch[i]->segTemplate = segTemGroup[i];
    }
    RestrictPhone(trie, testInput);
    cout << endl;
}


void problem1(vector<vector<vector<double>>> segTemGroup,vector<vector<double>> testInput)
{
    DigitRecognition(DIGIT_NUM7, testInput, segTemGroup);
    cout << endl;
}

// test segmental k-mean
int main(){
//    writeSeg();
    
    vector<vector<vector<double>>> segTemGroup(TYPE_NUM, vector<vector<double>>(SEG_NUM, vector<double>(DIMENSION)));
    
    ifstream in(segmentPath);
    for (int i = 0; i < TYPE_NUM; i++)
    {
        for (int j = 0; j < SEG_NUM; j++)
        {
            for (int k = 0; k < DIMENSION; k++)
            {
                in >> segTemGroup[i][j][k];
            }
        }
    }
    vector<vector<double>> testInput;
//    featureExtractionTwo(testInput, wavTestPath, txtTestPath);
    featureExtractionTwo(testInput, testPath, testPath);
    
//    problem3(segTemGroup, testInput);
    
    problem1(segTemGroup, testInput);
    
    return 0;
}





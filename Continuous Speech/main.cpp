//
//  main.cpp
//  Continuous Speech
//
//  Created by hty on 03/11/2016.
//  Copyright © 2016 hty. All rights reserved.
//

#include <iostream>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include "ContinuousPhone.h"
#include "readwave.h"
#include "seg_K_mean.h"
#include "captureData.h"
#include "portaudio.h"
#include "featureExtractionOld.h"
#include "featureExtractionNew.h"

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
string txtIsolateFromConPathVariance = "/Users/hty/Desktop/Speech Recognition/project/project 6/englishTem/requiredTem/continue-isolatedword/variance.txt";
string txtIsolateFromConPathTransfer = "/Users/hty/Desktop/Speech Recognition/project/project 6/englishTem/requiredTem/continue-isolatedword/transfer.txt";


string trainWavPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/hwdata/train/";
string trainTestWavPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/hwdata/test/";
string trainTxtPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/hwdata/train_txt/";
string trainTestTxtPath = "/Users/hty/Desktop/Speech Recognition/project/project 6/hwdata/test_txt/";


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
            featureExtractionOld(temFeature, wavpath, txtpath);
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


void problem3(vector<vector<vector<double>>> segTemGroup, vector<vector<double>> testInput, vector<vector<vector<double>>>& varianceTerm, vector<vector<vector<int>>>& countTransfer, bool ifGussian)
{
    Trie trie;
    TrieNode* root = trie.getRoot();
    for (int i = 0; i < MAX_BRANCH_NUM - 1; i++)
    {
        root->nextBranch[i]->segTemplate = segTemGroup[i];
    }
    if (! ifGussian) {
        RestrictPhone(trie, testInput, varianceTerm, countTransfer);
    }
    else{
        RestrictPhoneGuassian(trie, testInput, varianceTerm, countTransfer);
    }
    cout << endl;
}


void problem1(vector<vector<vector<double>>> segTemGroup,vector<vector<double>> testInput, vector<vector<vector<double>>>& varianceTerm, vector<vector<vector<int>>>& countTransfer, bool ifGussian)
{
    if (! ifGussian) {
        DigitRecognition(DIGIT_NUM4, testInput, segTemGroup, varianceTerm, countTransfer);
    }
    else{
        DigitRecognitionGussian(DIGIT_NUM4, testInput, segTemGroup, varianceTerm, countTransfer);
    }
    
    cout << endl;
}

void getResult(bool ifGuassian){
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
    featureExtractionTwoOld(testInput, testPath, testTxtPath);
    
    cout << "problem3 result : ";
    problem3(segTemGroup, testInput, varianceTerm, countTransfer,ifGuassian);
    
    cout << endl;
    cout << "problem1 result : ";
    problem1(segTemGroup, testInput, varianceTerm, countTransfer, ifGuassian);
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
    
    cout << "Finish segmental K-mean"<< endl;
    
    vector<vector<vector<double>>> varianceSeg(DIGIT_NUM, vector<vector<double>>(SEG_NUM, vector<double>(DIMENSION)));
    vector<vector<vector<int>>> transferSeg(DIGIT_NUM, vector<vector<int>>(SEG_NUM + 1, vector<int>(SEG_NUM)));
    vector<vector<vector<double>>>resultSeg = getSegFrame(resultState, input, varianceSeg, transferSeg);

    ofstream out1(txtIsolateFromConPath);
    ofstream out2(txtIsolateFromConPathVariance);
    ofstream out3(txtIsolateFromConPathTransfer);
    for(int i = 0; i < DIGIT_NUM; i++)
    {
        for(int j = 0; j < SEG_NUM; j++)
        {
            for (int k = 0; k < DIMENSION; k++)
            {
                out1 << resultSeg[i][j][k] << " ";
                out2 << varianceSeg[i][j][k] << " ";
            }
            out1 << endl;
            out2 << endl;
        }
        
        for (int j = 0; j < SEG_NUM + 1; j++)
        {
            for (int k = 0; k < SEG_NUM; k++) {
                out3 << transferSeg[i][j][k] << " ";
            }
            out3 << endl;
        }
        
        out1 << endl;
        out2 << endl;
        out3 << endl;
    }
    out1.close();
    out2.close();
    out3.close();
    cout << endl;
}


void record(){
    string filepath = "/Users/hty/desktop/record/567/record.wav";
    capture(filepath);
}

void readSeg(bool ifGuassian){
    vector<vector<vector<double>>> segTemGroup(DIGIT_NUM, vector<vector<double>>(SEG_NUM, vector<double>(DIMENSION)));
    vector<vector<vector<double>>> varianceTerm(DIGIT_NUM, vector<vector<double>>(SEG_NUM, vector<double>(DIMENSION)));
    vector<vector<vector<int>>> countTransfer(DIGIT_NUM, vector<vector<int>>(SEG_NUM + 1, vector<int>(SEG_NUM)));
    
    ifstream in(txtIsolateFromConPath);
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
    featureExtractionTwoOld(testInput, testPath, testTxtPath);
//    
//    vector<vector<vector<double>>> varianceTerm;
//    vector<vector<vector<int>>> countTransfer;
    
    cout << "problem3 result : ";
    problem3(segTemGroup, testInput, varianceTerm, countTransfer, ifGuassian);
    
}



// first arg: path
// second arg: file name vector
// store in Allfiles.txt
int testReadDir(vector<string>& files)
{

    int vec_len = (int)files.size();
    for(int i = 0; i < vec_len; i++)
    {
        string wavpath = trainWavPath + files[i];
        string txtpath = trainTxtPath + to_string(i);
        vector<vector<double>> testInput;
        featureExtractionNew(testInput, wavpath, txtpath);
        cout << "file index : " << i << " file name: " << files[i] << endl;
    }
    return 0;
}



int testSingleWav(string index)
{

    string wavpath = trainWavPath + index;
    string txtpath = trainTxtPath;
    vector<vector<double>> testInput;
    featureExtractionNew(testInput, wavpath, txtpath);
    return 0;
}


void createDocList(std::vector<std::string> &doc_list){
    int return_code;
    DIR *dir;
    struct dirent entry;
    struct dirent *res;
    string real_dir = trainWavPath;//搜索的目录
    if ((dir = opendir(real_dir.c_str())) != NULL) {//打开目录
        for (return_code = readdir_r(dir, &entry, &res);res != NULL && return_code == 0;return_code = readdir_r(dir, &entry, &res)) {
            if (entry.d_type != DT_DIR) {//存放到列表中
                doc_list.push_back(string(entry.d_name));
            }
        }
        closedir(dir);//关闭目录
    }
}


// test segmental k-mean
int main(int argc, const char * argv[]) {
//    writeSeg(wavTemPath, txtTemPath);    // read in the isolated word

  
//    trainDigits();
//    readSeg(true);
//    record();
    
//    getResult();
    vector<string> files;
    createDocList(files);
    
//    cout << files[19] << endl;
//    for(int i = 0; i < files.size(); i++){
//        cout << files[i] << endl;
//    }
    testReadDir(files);
//    testSingleWav(files[19]);
    
    return 0;
}





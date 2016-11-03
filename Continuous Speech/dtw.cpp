#include "dtw.h"

/**
 *  compute the Euclidean distance of two 39-dimension vector
 *
 *  @param matrix1                  the first vector
 *  @param matrix2    the second vector
 *
 *  @return the double distance
 */
double dis(vector<double>& matrix1, vector<double>& matrix2){
    double distance = 0;
    unsigned int num1 = (unsigned int)matrix1.size();
    unsigned int num2 = (unsigned int)matrix2.size();
    
    if (num1 == num2) {
        for (unsigned int i = 0; i < num1; i++) {
            distance += pow((matrix1[i] - matrix2[i]), 2);
        }
        distance = sqrt(distance);
    }
    else{
        cout << "------------------ERROR------------------"<< endl;
        cout << "THE DIMENSION OF EACH VECTOR IS NOT SAME!"<< endl;
    }
    
    return distance;
}


/**
 *  use dtw to compute the cost
 *
 *  @param inputAduio input audio feature, seperate into different frames and each frame with a 39-D features
 *  @param temAduio   pre-input template audio feature, the same as input
 *
 *  @return the minimun cost from the best path.
 */
double dtw(vector<vector<double>>& inputAduio, vector<vector<double>>& temAduio){
    
    unsigned int inputFrameNum = (unsigned int)inputAduio.size();
    unsigned int temFrameNum = (unsigned int)temAduio.size();
    
    vector<double> col(temFrameNum), prevCol(temFrameNum), colMax(temFrameNum);
    
    for (unsigned i = 0; i < temFrameNum; i++) {
        colMax[i] = UINT_MAX / 2;
    }
    
    prevCol = colMax;
    
    prevCol[0] = dis(inputAduio[0], temAduio[0]);
    prevCol[1] = dis(inputAduio[0], temAduio[1]);
    
    for (unsigned int i = 1; i < inputFrameNum; i++) {
        
        unsigned int temp = min({2 * (i + 1), temFrameNum});
        col = colMax;
        
        for (unsigned int j = 0; j < temp; j++) {
            
            if (j == 0) {
                col[j] = prevCol[j] + dis(inputAduio[i], temAduio[j]);
            }
            
            else if (j == 1){
                col[j] = min({prevCol[j - 1], prevCol[j]}) + dis(inputAduio[i], temAduio[j]);
            }
            else
                col[j] = min({prevCol[j - 2], prevCol[j - 1], prevCol[j]}) +  dis(inputAduio[i], temAduio[j]);
        }
        
        col.swap(prevCol);
    }
    
    return prevCol[temFrameNum - 1];
}


double beamDTW(vector<vector<double>>& inputAudio, vector<vector<double>>& temAudio)
{
    unsigned int inputLen = (unsigned int)inputAudio.size();
    unsigned int temLen = (unsigned int)temAudio.size();
    bool** flagArray;
    flagArray = new bool*[temLen];
    for (int i = 0; i < temLen; i++)
    {
        flagArray[i] = new bool[inputLen];
        memset(flagArray[i], true, inputLen * sizeof(bool));
    }
    vector<double> col(temLen), preCol(temLen), colMax(temLen);
    for (unsigned int i = 0; i < temLen; i++)
        colMax[i] = UINT_MAX / 2;
    preCol = colMax;
    preCol[0] = dis(inputAudio[0], temAudio[0]);
    preCol[1] = dis(inputAudio[0], temAudio[1]);
    for (unsigned int i = 1; i < inputLen; i++)
    {
        double beamTemp = UINT_MAX / 2;
        col = colMax;
        unsigned int tempLen = min(2 * i + 2, temLen);
        for (unsigned int j = 0; j < tempLen; j++)
        {
            if (j == 0 && flagArray[j][i - 1])
                col[j] = preCol[j] + dis(inputAudio[i], temAudio[j]);
            else if (j == 1)
            {
                if (flagArray[j][i - 1] && flagArray[j - 1][i - 1])
                    col[j] = min(preCol[j], preCol[j - 1]) + dis(inputAudio[i], temAudio[j]);
                else if (flagArray[j][i - 1] && !flagArray[j - 1][i - 1])
                    col[j] = preCol[j] + dis(inputAudio[i], temAudio[j]);
                else if (!flagArray[j][i - 1] && flagArray[j - 1][i - 1])
                    col[j] = preCol[j - 1] + dis(inputAudio[i], temAudio[j]);
            }
            else if (j > 1)
            {
                if (flagArray[j][i - 1] && flagArray[j - 1][i - 1] && flagArray[j - 2][i - 1])
                    col[j] = min({ preCol[j], preCol[j - 1], preCol[j - 2] }) + dis(inputAudio[i], temAudio[j]);
                else if (!flagArray[j][i - 1] && flagArray[j - 1][i - 1] && flagArray[j - 2][i - 1])
                    col[j] = min({ preCol[j - 1], preCol[j - 2] }) + dis(inputAudio[i], temAudio[j]);
                else if (flagArray[j][i - 1] && !flagArray[j - 1][i - 1] && flagArray[j - 2][i - 1])
                    col[j] = min({ preCol[j], preCol[j - 2] }) + dis(inputAudio[i], temAudio[j]);
                else if (flagArray[j][i - 1] && flagArray[j - 1][i - 1] && !flagArray[j - 2][i - 1])
                    col[j] = min({ preCol[j], preCol[j - 1] }) + dis(inputAudio[i], temAudio[j]);
                else if (!flagArray[j][i - 1] && !flagArray[j - 1][i - 1] && flagArray[j - 2][i - 1])
                    col[j] = preCol[j - 2] + dis(inputAudio[i], temAudio[j]);
                else if (flagArray[j][i - 1] && !flagArray[j - 1][i - 1] && !flagArray[j - 2][i - 1])
                    col[j] = preCol[j] + dis(inputAudio[i], temAudio[j]);
                else if (!flagArray[j][i - 1] && flagArray[j - 1][i - 1] && !flagArray[j - 2][i - 1])
                    col[j] = preCol[j - 1] + dis(inputAudio[i], temAudio[j]);
            }
            if (beamTemp > col[j])
                beamTemp = col[j];
        }
        for (unsigned int j = 0; j < tempLen; j++)
        {
            if (col[j] - beamTemp > DTW_BEAM)
                flagArray[j][i] = false;
        }
        col.swap(preCol);
    }
    delete[] flagArray;
    return preCol[temLen - 1];
}


//DTW with synchronous multiple templates
vector<double> beamSynchronousDTW(vector<vector<double>>& inputAudio, vector<vector<vector<double>>>& temAudio)
{
    unsigned int tempNum = (unsigned int)temAudio.size();
    vector<unsigned int> everyTempSize(tempNum);
    unsigned int largeTempSize = 0;
    for (unsigned int i = 0; i < tempNum; i++)
    {
        everyTempSize[i] = temAudio[i].size();
        if (largeTempSize < everyTempSize[i])
            largeTempSize = everyTempSize[i];
    }
    
    unsigned int inputSize = (unsigned int)inputAudio.size();
    bool*** flagArray = new bool**[tempNum];
    for (unsigned int i = 0; i < tempNum; i++)
    {
        flagArray[i] = new bool*[everyTempSize[i]];
        for (unsigned int j = 0; j < everyTempSize[i]; j++)
        {
            flagArray[i][j] = new bool[inputSize];
            memset(flagArray[i][j], true, inputSize * sizeof(bool));
        }
    }
    
    vector<vector<double>> preCol(tempNum, vector<double>(largeTempSize, UINT_MAX / 2));
    vector<vector<double>> col(tempNum, vector<double>(largeTempSize, UINT_MAX / 2));
    vector<vector<double>> maxCol(tempNum, vector<double>(largeTempSize, UINT_MAX / 2));
    
    double zeroMin = UINT_MAX / 2;
    for (unsigned int i = 0; i < tempNum; i++)
    {
        preCol[i][0] = dis(inputAudio[0], temAudio[i][0]);
        preCol[i][1] = dis(inputAudio[0], temAudio[i][1]);
        if (zeroMin > preCol[i][0])
            zeroMin = preCol[i][0];
        if (zeroMin > preCol[i][1])
            zeroMin = preCol[i][1];
    }
    
    for (unsigned int i = 0; i < tempNum; i++)
    {
        if (preCol[i][0] - zeroMin > DTW_BEAM)
            flagArray[i][0][0] = false;
        if (preCol[i][1] - zeroMin > DTW_BEAM)
            flagArray[i][1][0] = false;
    }
    
    for (unsigned int i = 1; i < inputSize; i++)
    {
        //vector<double> beamTemp(tempNum, UINT_MAX / 2);
        double beamTemp = UINT_MAX / 2;
        vector<unsigned int> tempSize(tempNum);
        col = maxCol;
        largeTempSize = 0;
        for (unsigned int p = 0; p < tempNum; p++)
        {
            tempSize[p] = min({ 2 * i + 2, everyTempSize[p] });
            if (largeTempSize < tempSize[p])
                largeTempSize = tempSize[p];
        }
        for (unsigned int j = 0; j < largeTempSize; j++)
        {
            for (unsigned int k = 0; k < tempNum; k++)
            {
                if (j == 0 && flagArray[k][j][i - 1])
                    col[k][j] = preCol[k][j] + dis(inputAudio[i], temAudio[k][j]);
                else if (j == 1)
                {
                    if (flagArray[k][j][i - 1] && flagArray[k][j - 1][i - 1])
                        col[k][j] = min(preCol[k][j], preCol[k][j - 1]) + dis(inputAudio[i], temAudio[k][j]);
                    else if (flagArray[k][j][i - 1] && !flagArray[k][j - 1][i - 1])
                        col[k][j] = preCol[k][j] + dis(inputAudio[i], temAudio[k][j]);
                    else if (!flagArray[k][j][i - 1] && flagArray[k][j - 1][i - 1])
                        col[k][j] = preCol[k][j - 1] + dis(inputAudio[i], temAudio[k][j]);
                }
                else if (j > 1 && j < tempSize[k])
                {
                    if (flagArray[k][j][i - 1] && flagArray[k][j - 1][i - 1] && flagArray[k][j - 2][i - 1])
                        col[k][j] = min({ preCol[k][j], preCol[k][j - 1], preCol[k][j - 2] }) + dis(inputAudio[i], temAudio[k][j]);
                    else if (!flagArray[k][j][i - 1] && flagArray[k][j - 1][i - 1] && flagArray[k][j - 2][i - 1])
                        col[k][j] = min({ preCol[k][j - 1], preCol[k][j - 2] }) + dis(inputAudio[i], temAudio[k][j]);
                    else if (flagArray[k][j][i - 1] && !flagArray[k][j - 1][i - 1] && flagArray[k][j - 2][i - 1])
                        col[k][j] = min({ preCol[k][j], preCol[k][j - 2] }) + dis(inputAudio[i], temAudio[k][j]);
                    else if (flagArray[k][j][i - 1] && flagArray[k][j - 1][i - 1] && !flagArray[k][j - 2][i - 1])
                        col[k][j] = min({ preCol[k][j], preCol[k][j - 1] }) + dis(inputAudio[i], temAudio[k][j]);
                    else if (!flagArray[k][j][i - 1] && !flagArray[k][j - 1][i - 1] && flagArray[k][j - 2][i - 1])
                        col[k][j] = preCol[k][j - 2] + dis(inputAudio[i], temAudio[k][j]);
                    else if (flagArray[k][j][i - 1] && !flagArray[k][j - 1][i - 1] && !flagArray[k][j - 2][i - 1])
                        col[k][j] = preCol[k][j] + dis(inputAudio[i], temAudio[k][j]);
                    else if (!flagArray[k][j][i - 1] && flagArray[k][j - 1][i - 1] && !flagArray[k][j - 2][i - 1])
                        col[k][j] = preCol[k][j - 1] + dis(inputAudio[i], temAudio[k][j]);
                }
                if (beamTemp > col[k][j])
                    beamTemp = col[k][j];
            }
        }
        for (unsigned int j = 0; j < largeTempSize; j++)
        {
            for (unsigned int k = 0; k < tempNum; k++)
            {
                if (col[k][j] - beamTemp > DTW_BEAM && j < everyTempSize[k])
                    flagArray[k][j][i] = false;
            }
        }
        for (unsigned int j = 0; j < tempNum; j++)
            col[j].swap(preCol[j]);
    }
    vector<double> result(tempNum);
    for (int i = 0; i < tempNum; i++)
        result[i] = preCol[i][everyTempSize[i] - 1];
    delete[] flagArray;
    return result;
}

//print the path of dtw
double printDTWPath(vector<vector<double>>& s1, vector<vector<double>>& s2, ofstream& out) {
    const std::size_t len1 = s1.size(), len2 = s2.size();
    vector<vector <double> > printArray(len1, vector<double>(len2, 0));
    bool** flagArray;
    flagArray = new bool*[len1];
    for (int i = 0; i < len1; i++) {
        flagArray[i] = new bool[len2];
        memset(flagArray[i], false, len2 * sizeof(bool));
    }
    vector<double> col(len2), prevCol(len2), colMax(len2);
    for (unsigned int i = 0; i < len2; i++) {
        colMax[i] = UINT_MAX / 2;
    }
    prevCol = colMax;
    prevCol[0] = dis(s1[0], s2[0]);
    prevCol[1] = dis(s1[0], s2[1]);
    printArray[0] = prevCol;
    for (unsigned int i = 1; i < len1; i++) {
        
        unsigned int temp = 2 * (i + 1) < len2 ? 2 * (i + 1) : len2;
        col = colMax;
        for (unsigned int j = 0; j < temp; j++) {
            if (j == 0) {
                col[j] = prevCol[j] + dis(s1[i], s2[j]);
            }
            else if (j == 1) {
                col[j] = min({ prevCol[j - 1], prevCol[j] }) + dis(s1[i], s2[j]);
            }
            else
                col[j] = min({ prevCol[j - 2], prevCol[j - 1], prevCol[j] }) + dis(s1[i], s2[j]);
        }
        printArray[i] = col;
        col.swap(prevCol);
    }
    for (int i = len1 - 1; i >= 0; i--)
    {
        for (int j = len2 - 1; j >= 0; j--)
        {
            if (i != 0 && j != 0) {
                if (i == len1 - 1 && j == len2 - 1)
                {
                    flagArray[i][j] = true;
                    double temp = min({ printArray[i - 1][j - 2], printArray[i - 1][j - 1], printArray[i - 1][j] });
                    if (printArray[i - 1][j] == temp)
                    {
                        flagArray[i - 1][j] = true;
                        break;
                    }
                    if (printArray[i - 1][j - 1] == temp)
                    {
                        flagArray[i - 1][j - 1] = true;
                        break;
                    }
                    if (printArray[i - 1][j - 2] == temp)
                    {
                        flagArray[i - 1][j - 2] = true;
                        break;
                    }
                }
                else if(j == 1)
                {
                    if (flagArray[i][j]) {
                        double temp = min({printArray[i - 1][j - 1], printArray[i - 1][j] });
                        if (printArray[i - 1][j] == temp)
                        {
                            flagArray[i - 1][j] = true;
                            break;
                        }
                        if (printArray[i - 1][j - 1] == temp)
                        {
                            flagArray[i - 1][j - 1] = true;
                            break;
                        }
                    }
                }
                else
                {
                    if(flagArray[i][j])
                    {
                        double temp = min({ printArray[i - 1][j - 2], printArray[i - 1][j - 1], printArray[i - 1][j] });
                        if (printArray[i - 1][j] == temp)
                        {
                            flagArray[i - 1][j] = true;
                            break;
                        }
                        if (printArray[i - 1][j - 1] == temp)
                        {
                            flagArray[i - 1][j - 1] = true;
                            break;
                        }
                        if (printArray[i - 1][j - 2] == temp)
                        {
                            flagArray[i - 1][j - 2] = true;
                            break;
                        }
                    }
                }
            }
            else if(i != 0 && j == 0)
            {
                if(flagArray[i][j])
                {
                    flagArray[i - 1][j] = true;
                }
            }
            else if(i == 0 && j != 0)
            {
                if (flagArray[i][j])
                {
                    flagArray[i][j - 1] = true;
                }
            }
        }
        
    }
    if (!out.is_open())
        cout << "fail to open file in printDTWPath" << endl;
    for (int i = 0; i <= len2; i++)
    {
        for (int j = len1; j >= 0; j--)
        {
            if (i == len2)
            {
                if (j == len1) {
                    cout << setw(TABLE_GAP) << " ";
                    out << setw(TABLE_GAP) << " ";
                }
                else {
                    cout << setw(TABLE_GAP) << len1 - j;
                    out << setw(TABLE_GAP) << len1 - j;
                }
            }
            else if(j == len1)
            {
                cout << setw(TABLE_GAP) << len2 - i;
                out << setw(TABLE_GAP) << len2 - i;
            }
            else
            {
                if (printArray[len1 - 1 - j][len2 - 1 - i] >= UINT_MAX / 2) {
                    cout << setw(TABLE_GAP) << "INF";
                    out << setw(TABLE_GAP) << "INF";
                }
                else
                {
                    //set flag
                    cout.setf(ios::fixed);
                    //set to one digit
                    cout << setw(TABLE_GAP) << setprecision(1) << printArray[len1 - 1 - j][len2 - 1 - i];
                    //restore original flag
                    cout.unsetf(ios::fixed);
                    //restore original digits
                    cout << setprecision(6);
                    
                    out.setf(ios::fixed);
                    out << setw(TABLE_GAP) << setprecision(1) << printArray[len1 - 1 - j][len2 - 1 - i];
                    out.unsetf(ios::fixed);
                    out << setprecision(6);
                }
                if (flagArray[len1 - 1 - j][len2 - 1 - i]) {
                    cout << "*";
                    out << "*";
                }
            }
        }
        cout << endl;
        out << endl;
    }
    return prevCol[len2 - 1];
}







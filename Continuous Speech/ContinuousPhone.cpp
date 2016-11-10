#include "ContinuousPhone.h"

//Euclidean distance
double Dis(vector<double>& matrix1, vector<double>& matrix2) {
    double distance = 0;
    unsigned int num1 = (unsigned int)matrix1.size();
    unsigned int num2 = (unsigned int)matrix2.size();
    
    if (num1 == num2) {
        for (unsigned int i = 0; i < num1; i++) {
            distance += pow((matrix1[i] - matrix2[i]), 2);
        }
        distance = sqrt(distance);
    }
    else {
        cout << "------------------ERROR------------------" << endl;
        cout << "THE DIMENSION OF EACH VECTOR IS NOT SAME!" << endl;
    }
    return distance;
}

//caculate the minimal value of Euclidean distance add leaf or leaf's parent, and record
//the position to know which digit is the result
double costUtil(vector<double>& vec, double c, int& pos)
{
    int length = (int)vec.size();
    double cost = INT_MAX / 2;
    for(int i = 0; i < length; i++)
    {
        if(cost > vec[i] + c)
        {
            cost = vec[i] + c;
            pos = i;
        }
    }
    return cost;
}


// return the minCost of the last one vector and rewrite the pos
double minLast(vector<vector<double>> vec, int& pos, int state)
{
    int lenth = (int)vec.size();
    double cost = INT_MAX / 2;
    for (int i = 0; i < lenth; i++) {
        if (cost > vec[i][state]) {
            cost = vec[i][state];
            pos = i;
        }
    }
    return cost;
}

// get the index of the traceMatrix
int getMatrixIndex(int digit_index, int tem_index, int state)
{
    return digit_index * TYPE_NUM * SEG_NUM + tem_index *SEG_NUM + state;
}

// get digit based on the index
int getDigit(int index){
    return (index / (TYPE_NUM * SEG_NUM));
}

// get tem index
int getTem(int index){
    return (index % (TYPE_NUM * SEG_NUM)) / SEG_NUM;
}


   // using guassian distance
/*
void RestrictPhone(Trie& trie, vector<vector<double>>& input, vector<vector<vector<double>>> varianceTerm, vector<vector<vector<int>>> countTransfer)
{
    int input_size = (int)input.size();
    TrieNode* root = trie.getRoot();
    
    //back table is used to record every frame comes from which state in which template
    //vector<int> contains the state num and whether have loop back
    //backTable[][][0] refer the state number, backTable[][][1] refer whether loop back
    vector<vector<vector<int>>> backTable(SEG_NUM * TYPE_NUM, vector<vector<int>>(input_size, vector<int>(2)));
    
    //leaf of every template
    vector<double> last(MAX_BRANCH_NUM - 1, INT_MAX / 2);
    //leaf's parent of every template
    vector<double> lastTwo(MAX_BRANCH_NUM - 1, INT_MAX / 2);
    
    for(int i = 0; i < input_size; i++)
    {
        for(int j = 0; j < MAX_BRANCH_NUM - 1; j++)
        {
            vector<double> temp(SEG_NUM, INT_MAX / 2);
            root->nextBranch[j]->curNodeCost = temp;
            if(i == 0)
            {
                root->nextBranch[j]->preNodeCost[0] = nodeCost(input[i], root->nextBranch[j]->segTemplate[0], varianceTerm[j][0]) + edgeCost(0, countTransfer[j][0]);
                root->nextBranch[j]->preNodeCost[1] = nodeCost(input[i], root->nextBranch[j]->segTemplate[1], varianceTerm[j][1]) + edgeCost(1, countTransfer[j][0]);
            }
            else
            {
                for (int k = 0; k < SEG_NUM; k++)
                {
                    double var1 = nodeCost(input[i], root->nextBranch[j]->segTemplate[k], varianceTerm[j][k]);
                    double var2 = root->nextBranch[j]->preNodeCost[k] + var1 + edgeCost(k, countTransfer[j][k + 1]);
//                    double var3 = root->nextBranch[j]->preNodeCost[k - 1] + var1 + edgeCost(k, countTransfer[j][k]);
//                    double var4 = root->nextBranch[j]->preNodeCost[k - 2] + var1 + edgeCost(k, countTransfer[j][k - 1]);
                    int pos1 = 0;
                    int pos2 = 0;
                    double var5 = costUtil(last, var1, pos1) + PENALTY;
                    double var6 = costUtil(lastTwo, var1, pos2) + PENALTY;
                    if (k == 0) {
                        var5 += edgeCost(k, countTransfer[j][0]);
                        var6 += edgeCost(k, countTransfer[j][0]);
                        if (var5 < INT_MAX / 2 )
                            root->nextBranch[j]->curNodeCost[k] = min({ var2, var5 , var6 });
                        else if (var6 < INT_MAX / 2)
                            root->nextBranch[j]->curNodeCost[k] = min({ var2, var6 });
                        else
                            root->nextBranch[j]->curNodeCost[k] = var2;
                        if (root->nextBranch[j]->curNodeCost[k] == var5)
                        {
                            backTable[SEG_NUM * j + k][i][0] = SEG_NUM * pos1 + (SEG_NUM - 1);
                            backTable[SEG_NUM * j + k][i][1] = 1;
                        }
                        else if (root->nextBranch[j]->curNodeCost[k] == var6)
                        {
                            backTable[SEG_NUM * j + k][i][0] = SEG_NUM * pos2 + (SEG_NUM - 2);
                            backTable[SEG_NUM * j + k][i][1] = 1;
                        }
                        else if (root->nextBranch[j]->curNodeCost[k] == var2)
                        {
                            backTable[SEG_NUM * j + k][i][0] = SEG_NUM * j + k;
                            //backTable[SEG_NUM * j + k][i][1] = backTable[SEG_NUM * j + k][i - 1][1];
                        }
                    }
                    else if (k == 1) {
                        var5 += edgeCost(k, countTransfer[j][0]);
                        double var3 = root->nextBranch[j]->preNodeCost[k - 1] + var1 + edgeCost(k, countTransfer[j][k]);
                        if (var5 < INT_MAX / 2)
                            root->nextBranch[j]->curNodeCost[k] = min({ var3, var2, var5 });
                        else
                            root->nextBranch[j]->curNodeCost[k] = min({ var3, var2 });
                        if (root->nextBranch[j]->curNodeCost[k] == var5)
                        {
                            backTable[SEG_NUM * j + k][i][0] = SEG_NUM * pos1 + (SEG_NUM - 1);
                            backTable[SEG_NUM * j + k][i][1] = 1;
                        }
                        else if (root->nextBranch[j]->curNodeCost[k] == var2)
                        {
                            backTable[SEG_NUM * j + k][i][0] = SEG_NUM * j + k;
                            //backTable[SEG_NUM * j + k][i][1] = backTable[SEG_NUM * j + k][i - 1][1];
                        }
                        else if (root->nextBranch[j]->curNodeCost[k] == var3)
                        {
                            backTable[SEG_NUM * j + k][i][0] = SEG_NUM * j + (k - 1);
                            //backTable[SEG_NUM * j + k][i][1] = backTable[SEG_NUM * j + (k - 1)][i - 1][1];
                        }
                    }
                    else
                    {
                        double var3 = root->nextBranch[j]->preNodeCost[k - 1] + var1 + edgeCost(k, countTransfer[j][k]);
                        double var4 = root->nextBranch[j]->preNodeCost[k - 2] + var1 + edgeCost(k, countTransfer[j][k - 1]);
                        root->nextBranch[j]->curNodeCost[k] = min({ var4, var3, var2 });
                        if (root->nextBranch[j]->curNodeCost[k] == var4)
                        {
                            backTable[SEG_NUM * j + k][i][0] = SEG_NUM * j + (k - 2);
                            //backTable[SEG_NUM * j + k][i][1] = backTable[SEG_NUM * j + (k - 2)][i - 1][1];
                        }
                        else if (root->nextBranch[j]->curNodeCost[k] == var2)
                        {
                            backTable[SEG_NUM * j + k][i][0] = SEG_NUM * j + k;
                            //backTable[SEG_NUM * j + k][i][1] = backTable[SEG_NUM * j + k][i - 1][1];
                        }
                        else if (root->nextBranch[j]->curNodeCost[k] == var3)
                        {
                            backTable[SEG_NUM * j + k][i][0] = SEG_NUM * j + (k - 1);
                            //backTable[SEG_NUM * j + k][i][1] = backTable[SEG_NUM * j + (k - 1)][i - 1][1];
                        }
                    }
                }
                last[j] = root->nextBranch[j]->curNodeCost[SEG_NUM - 1];
                lastTwo[j] = root->nextBranch[j]->curNodeCost[SEG_NUM - 2];
            }
        }
        if (i > 0)
        {
            trie.swapNodeCost();
        }
    }
    
    //get the minimal state at last frame
    double minLast = INT_MAX / 2;
    int posLast = 0;
    for(int i = 0; i < MAX_BRANCH_NUM - 1; i++)
    {
        if(minLast > last[i])
        {
            minLast = last[i];
            posLast = i * SEG_NUM + (SEG_NUM - 1);
        }
    }
    
    //back tracing
    stack<int> resultPhone;
    resultPhone.push(posLast / SEG_NUM);
    int tracePos = posLast;
    for(int i = input_size - 1; i >= 0; i--)\
    {
        int curPos = backTable[tracePos][i][0];
        int loopBack = backTable[tracePos][i][1];
        if (loopBack == 1)
        {
            resultPhone.push(curPos / SEG_NUM);
        }
        tracePos = curPos;
    }
    while(!resultPhone.empty())
    {
        cout << resultPhone.top() << " ";
        resultPhone.pop();
    }
    return;
}
*/

//return the position in input to decide which frame is the end frame of one digit
stack<int> RestrictPhone(Trie& trie, vector<vector<double>>& input, vector<vector<vector<double>>>& varianceTerm, vector<vector<vector<int>>>& countTransfer)
{
    int input_size = (int)input.size();
    TrieNode* root = trie.getRoot();
    
    //back table is used to record every frame comes from which state in which template
    //vector<int> contains the state num and whether have loop back
    //backTable[][][0] refer the state number, backTable[][][1] refer whether loop back
    vector<vector<vector<int>>> backTable(SEG_NUM * TYPE_NUM, vector<vector<int>>(input_size, vector<int>(2)));
    
    //leaf of every template
    vector<double> last(MAX_BRANCH_NUM - 1, INT_MAX / 2);
    //leaf's parent of every template
    vector<double> lastTwo(MAX_BRANCH_NUM - 1, INT_MAX / 2);
    
    for (int i = 0; i < input_size; i++)
    {
        for (int j = 0; j < MAX_BRANCH_NUM - 1; j++)
        {
            vector<double> temp(SEG_NUM, INT_MAX / 2);
            root->nextBranch[j]->curNodeCost = temp;
            if (i == 0)
            {
                root->nextBranch[j]->preNodeCost[0] = Dis(input[i], root->nextBranch[j]->segTemplate[0]) + edgeCost(0, countTransfer[j][0]);
                root->nextBranch[j]->preNodeCost[1] = Dis(input[i], root->nextBranch[j]->segTemplate[1]) + edgeCost(1, countTransfer[j][0]);
            }
            else
            {
                for (int k = 0; k < SEG_NUM; k++)
                {
                    double var1 = Dis(input[i], root->nextBranch[j]->segTemplate[k]);
                    double var2 = root->nextBranch[j]->preNodeCost[k] + var1 + edgeCost(k, countTransfer[j][k + 1]);
                    //                    double var3 = root->nextBranch[j]->preNodeCost[k - 1] + var1 + edgeCost(k, countTransfer[j][k]);
                    //                    double var4 = root->nextBranch[j]->preNodeCost[k - 2] + var1 + edgeCost(k, countTransfer[j][k - 1]);
                    int pos1 = 0;
                    int pos2 = 0;
                    double var5 = costUtil(last, var1, pos1) + PENALTY;
                    double var6 = costUtil(lastTwo, var1, pos2) + PENALTY;
                    if (k == 0) {
                        var5 += edgeCost(k, countTransfer[j][0]);
                        var6 += edgeCost(k, countTransfer[j][0]);
                        if (var5 < INT_MAX / 2)
                            root->nextBranch[j]->curNodeCost[k] = min({ var2, var5 , var6 });
                        else if (var6 < INT_MAX / 2)
                            root->nextBranch[j]->curNodeCost[k] = min({ var2, var6 });
                        else
                            root->nextBranch[j]->curNodeCost[k] = var2;
                        if (root->nextBranch[j]->curNodeCost[k] == var5)
                        {
                            backTable[SEG_NUM * j + k][i][0] = SEG_NUM * pos1 + (SEG_NUM - 1);
                            backTable[SEG_NUM * j + k][i][1] = 1;
                        }
                        else if (root->nextBranch[j]->curNodeCost[k] == var6)
                        {
                            backTable[SEG_NUM * j + k][i][0] = SEG_NUM * pos2 + (SEG_NUM - 2);
                            backTable[SEG_NUM * j + k][i][1] = 1;
                        }
                        else if (root->nextBranch[j]->curNodeCost[k] == var2)
                        {
                            backTable[SEG_NUM * j + k][i][0] = SEG_NUM * j + k;
                            //backTable[SEG_NUM * j + k][i][1] = backTable[SEG_NUM * j + k][i - 1][1];
                        }
                    }
                    else if (k == 1) {
                        var5 += edgeCost(k, countTransfer[j][0]);
                        double var3 = root->nextBranch[j]->preNodeCost[k - 1] + var1 + edgeCost(k, countTransfer[j][k]);
                        if (var5 < INT_MAX / 2)
                            root->nextBranch[j]->curNodeCost[k] = min({ var3, var2, var5 });
                        else
                            root->nextBranch[j]->curNodeCost[k] = min({ var3, var2 });
                        if (root->nextBranch[j]->curNodeCost[k] == var5)
                        {
                            backTable[SEG_NUM * j + k][i][0] = SEG_NUM * pos1 + (SEG_NUM - 1);
                            backTable[SEG_NUM * j + k][i][1] = 1;
                        }
                        else if (root->nextBranch[j]->curNodeCost[k] == var2)
                        {
                            backTable[SEG_NUM * j + k][i][0] = SEG_NUM * j + k;
                            //backTable[SEG_NUM * j + k][i][1] = backTable[SEG_NUM * j + k][i - 1][1];
                        }
                        else if (root->nextBranch[j]->curNodeCost[k] == var3)
                        {
                            backTable[SEG_NUM * j + k][i][0] = SEG_NUM * j + (k - 1);
                            //backTable[SEG_NUM * j + k][i][1] = backTable[SEG_NUM * j + (k - 1)][i - 1][1];
                        }
                    }
                    else
                    {
                        double var3 = root->nextBranch[j]->preNodeCost[k - 1] + var1 + edgeCost(k, countTransfer[j][k]);
                        double var4 = root->nextBranch[j]->preNodeCost[k - 2] + var1 + edgeCost(k, countTransfer[j][k - 1]);
                        root->nextBranch[j]->curNodeCost[k] = min({ var4, var3, var2 });
                        if (root->nextBranch[j]->curNodeCost[k] == var4)
                        {
                            backTable[SEG_NUM * j + k][i][0] = SEG_NUM * j + (k - 2);
                            //backTable[SEG_NUM * j + k][i][1] = backTable[SEG_NUM * j + (k - 2)][i - 1][1];
                        }
                        else if (root->nextBranch[j]->curNodeCost[k] == var2)
                        {
                            backTable[SEG_NUM * j + k][i][0] = SEG_NUM * j + k;
                            //backTable[SEG_NUM * j + k][i][1] = backTable[SEG_NUM * j + k][i - 1][1];
                        }
                        else if (root->nextBranch[j]->curNodeCost[k] == var3)
                        {
                            backTable[SEG_NUM * j + k][i][0] = SEG_NUM * j + (k - 1);
                            //backTable[SEG_NUM * j + k][i][1] = backTable[SEG_NUM * j + (k - 1)][i - 1][1];
                        }
                    }
                }
                last[j] = root->nextBranch[j]->curNodeCost[SEG_NUM - 1];
                lastTwo[j] = root->nextBranch[j]->curNodeCost[SEG_NUM - 2];
            }
        }
        if (i > 0)
        {
            trie.swapNodeCost();
        }
    }
    
    stack<int> resultPos;
    resultPos = backTrace(input, backTable, last);
    return resultPos;
}

//return the position in input to decide which frame is the end frame of one digit
stack<int> backTrace(vector<vector<double>>& input, vector<vector<vector<int>>>& backTable, vector<double>& last)
{
    int input_size = input.size();
    //get the minimal state at last frame
    double minLast = INT_MAX / 2;
    int posLast = 0;
    for (int i = 0; i < MAX_BRANCH_NUM - 1; i++)
    {
        if (minLast > last[i])
        {
            minLast = last[i];
            posLast = i * SEG_NUM + (SEG_NUM - 1);
        }
    }
    
    //result position of input
    stack<int> resultPos;
    //back tracing
    stack<int> resultPhone;
    resultPhone.push(posLast / SEG_NUM);
    int tracePos = posLast;
    for (int i = input_size - 1; i >= 0; i--)\
    {
        int curPos = backTable[tracePos][i][0];
        int loopBack = backTable[tracePos][i][1];
        if (loopBack == 1)
        {
            resultPhone.push(curPos / SEG_NUM);
            resultPos.push(i);
        }
        tracePos = curPos;
    }
    while (!resultPhone.empty())
    {
        cout << resultPhone.top() << " ";
        resultPhone.pop();
    }
    return resultPos;
}

//cut digit frame from continuous speech
vector<vector<vector<double>>> getContinuousSeg(int digit_num, vector<vector<vector<double>>>& segTemGroup, vector<vector<double>>& input, vector<vector<vector<double>>>& varianceTerm, vector<vector<vector<int>>>& countTransfer)
{
    stack<int> resultPos;
    resultPos = DigitRecognition(digit_num, input, segTemGroup, varianceTerm, countTransfer);
    vector<vector<vector<double>>> inputSeg(DIGIT_NUM, vector<vector<double>>());
    int segSize = resultPos.size() + 1;
    //check the seg size
    if(segSize != DIGIT_NUM)
    {
        cout << "seg num is wrong, please check the penalty!" << endl;
        return inputSeg;
    }
    
    int count = 0;
    int curX = 0;
    int preX = 0;
    while(!resultPos.empty())
    {
        curX = resultPos.top();
        int segLength = curX - preX;
        if (preX == 0)
        {
            for (int i = 0; i <= segLength; i++)
            {
                inputSeg[count].push_back(input[i + preX]);
            }
        }
        else
        {
            for (int i = 0; i < segLength; i++)
            {
                inputSeg[count].push_back(input[(i + 1) + preX]);
            }
        }
        preX = curX;
        count++;
        resultPos.pop();
    }
    int input_length = input.size();
    //get the last seg
    for(int i = 0; i < (input_length - 1 - preX); i++)
    {
        inputSeg[count].push_back(input[(i + 1) + preX]);
    }
    return inputSeg;
}

//record state index in input
vector<vector<int>> getStateIndex(int digit_num, vector<vector<vector<double>>>& segTemGroup, vector<vector<double>>& input, vector<vector<vector<double>>>& varianceTerm, vector<vector<vector<int>>>& countTransfer)
{
    stack<int> resultPos;
    resultPos = DigitRecognition(digit_num, input, segTemGroup, varianceTerm, countTransfer);
    vector<vector<int>> stateIndex(DIGIT_NUM * SEG_NUM, vector<int>(2));
    int segSize = resultPos.size() + 1;
    //check the seg size
    if (segSize != DIGIT_NUM)
    {
        cout << "seg num is wrong, please check the penalty!" << endl;
        return stateIndex;
    }
    int curX = 0;
    int preX = 0;
    int count = 0;
    while (!resultPos.empty())
    {
        curX = resultPos.top();
        if (preX == 0)
        {
            int interval = (curX - preX + 1) / SEG_NUM;
            int preVal = preX;
            for(int i = 0; i < SEG_NUM; i++)
            {
                if (i == SEG_NUM - 1)
                {
                    stateIndex[count * SEG_NUM + i][0] = preVal;
                    stateIndex[count * SEG_NUM + i][1] = curX;
                    break;
                }
                //start index
                stateIndex[count * SEG_NUM + i][0] = preVal;
                //end index
                stateIndex[count * SEG_NUM + i][1] = preVal + interval - 1;
                preVal = preVal + interval;
            }
        }
        else
        {
            int interval = (curX - preX) / SEG_NUM;
            int preVal = preX;
            for (int i = 0; i < SEG_NUM; i++)
            {
                if (i == SEG_NUM - 1)
                {
                    stateIndex[count * SEG_NUM + i][0] = preVal + 1;
                    stateIndex[count * SEG_NUM + i][1] = curX;
                    break;
                }
                //start index
                stateIndex[count * SEG_NUM + i][0] = preVal + 1;
                //end index
                stateIndex[count * SEG_NUM + i][1] = preVal + interval;
                preVal = preVal + interval;
            }
        }
        preX = curX;
        count++;
        resultPos.pop();
    }
    int input_length = input.size();
    int last_interval = (input_length - 1 - preX) / SEG_NUM;
    int last_pre = preX;
    //get the last digit state index
    for (int i = 0; i < SEG_NUM; i++)
    {
        if (i == SEG_NUM - 1)
        {
            stateIndex[count * SEG_NUM + i][0] = last_pre + 1;
            stateIndex[count * SEG_NUM + i][1] = input_length - 1;
            break;
        }
        stateIndex[count * SEG_NUM + i][0] = last_pre + 1;
        stateIndex[count * SEG_NUM + i][1] = last_pre + last_interval;
        last_pre = last_pre + last_interval;
    }
    return stateIndex;
}

/* return: [tem_type][tem_num][digit_type][start_end]
 * input: [tem_type][tem_num][tem_length][dimension]
 */
vector<vector<vector<vector<int>>>> getAllStateIndex(int digit_num, vector<vector<vector<double>>>& segTemGroup, vector<vector<vector<vector<double>>>>& input, vector<vector<vector<double>>>& varianceTerm, vector<vector<vector<int>>>& countTransfer)
{
    vector<vector<vector<vector<int>>>> allStateIndex(TRAIN_TYPE, vector<vector<vector<int>>>(TRAIN_NUM, vector<vector<int>>()));
    for(int i = 0; i < TRAIN_TYPE; i++)
    {
        for(int j = 0; j < TRAIN_NUM; j++)
        {
            allStateIndex[i][j] = getStateIndex(digit_num, segTemGroup, input[i][j], varianceTerm, countTransfer);
        }
    }
    return allStateIndex;
}


//use the state index from k-means to cut the frame from template
vector<vector<vector<double>>> getSegFrame(vector<vector<vector<vector<int>>>>& allState, vector<vector<vector<vector<double>>>>& input)
{
    vector<vector<vector<double>>> temSeg(DIGIT_NUM, vector<vector<double>>(SEG_NUM, vector<double>(DIMENSION)));
    vector<vector<vector<vector<double>>>> stateFrame(DIGIT_NUM, vector<vector<vector<double>>>(SEG_NUM, vector<vector<double>>()));
    vector<vector<int>> digits = { {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 9, 8, 7, 6, 5, 4, 3, 2, 1}, {1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
        {1, 3, 5, 7, 9, 0, 2, 4, 6, 8}, {8, 6, 4, 2, 0, 9, 7, 5, 3, 1}, {9, 8, 7, 6, 5, 4, 3, 2, 1, 0 } };
    
    //extract frame
    for(int i = 0; i < TRAIN_TYPE; i++)
    {
        for(int j = 0; j < DIGIT_NUM; j++)
        {
            for(int k = 0; k < TRAIN_NUM; k++)
            {
                for (int p = 0; p < SEG_NUM; p++)
                {
                    int start = allState[i][k][j * SEG_NUM + p][0];
                    int end = allState[i][k][j * SEG_NUM + p][1];
                    for(int x = start; x <= end; x++)
                    {
                        stateFrame[digits[i][j]][p].push_back(input[i][k][x]);
                    }	
                }
            }
        }
    }
    
    //average frame
    for(int i = 0; i < DIGIT_NUM; i++)
    {
        for(int j = 0; j < SEG_NUM; j++)
        {
            int len = (int)stateFrame[i][j].size();
            for(int k = 0; k < DIMENSION; k++)
            {
                int sum = 0;
                for(int p = 0; p < len; p++)
                {
                    sum += stateFrame[i][j][p][k];
                }
                temSeg[i][j][k] = sum / len;
            }
        }
    }
    return temSeg;
}


stack<int> DigitRecognition(int digit_num, vector<vector<double>>& input, vector<vector<vector<double>>>& segTemGroup, vector<vector<vector<double>>>& varianceTerm, vector<vector<vector<int>>>& countTransfer)
{
    int input_length = (int)input.size();
    vector<vector<int>> traceMatrix(input_length, vector<int>(digit_num * TYPE_NUM * SEG_NUM, -1));
    vector<bool> digit_able;
    for (int i = 0; i < digit_num; i++) {
        digit_able.push_back(false);
    }
    digit_able[0] = true;
    
    vector<vector<vector<double>>> pre_col(digit_num, vector<vector<double>>(TYPE_NUM, vector<double>(SEG_NUM)));
    
    // initialize the pre_col
    for (int i = 0; i < digit_num; i++) {
        for (int j = 0; j < TYPE_NUM; j++) {
            for (int k = 0; k < SEG_NUM; k++) {
                pre_col[i][j][k] = INT_MAX / 2;
            }
        }
    }
    
    vector<vector<vector<double>>> cur_col = pre_col;
    
    double last_one, last_two;
    int posOne = -1;
    int posTwo = -1;
    
    double preMinCost;
    
    // get the minimum cost and fill the trace matrix
    for (int i = 0; i < input_length; i++) {
        for (int digit = 0; digit < digit_num; digit++) {
            for (int tem_index = 0; tem_index < TYPE_NUM; tem_index++) {
                //				if (digit_num == DIGIT_NUM7 && tem_index == 0 && digit == 0) {
                //					tem_index += 2;
                //				}
                if (i == 0)
                {
                    if (digit == 0) {
                        cur_col[0][tem_index][0] = Dis(input[i], segTemGroup[tem_index][0]);
                        cur_col[0][tem_index][1] = Dis(input[i], segTemGroup[tem_index][1]);
                        pre_col[0][tem_index][0] = Dis(input[i], segTemGroup[tem_index][0]);
                        pre_col[0][tem_index][1] = Dis(input[i], segTemGroup[tem_index][1]);
                    }
                }
                for (int state_index = 0; state_index < SEG_NUM; state_index++) {
                    //                    if (i == 0) // the first column
                    //                    {
                    //                        //  the first column can only be handled at digit 0
                    //                        if (digit == 0) {
                    //                            cur_col[0][tem_index][0] = Dis(input[i], segTemGroup[tem_index][0]);
                    //                            cur_col[0][tem_index][1] = Dis(input[i], segTemGroup[tem_index][1]);
                    //                        }
                    //                    }
                    if (i != 0)
                    {
                        if (digit_able[digit]) {
                            if (digit > 0) {
                                if (state_index == 0)
                                {
                                    posOne = -1;
                                    posTwo = -1;
                                    last_one = minLast(pre_col[digit - 1], posOne, SEG_NUM - 1);
                                    last_two = minLast(pre_col[digit - 1], posTwo, SEG_NUM - 2);
                                    preMinCost = min({ last_one, last_two, pre_col[digit][tem_index][state_index] });
                                    cur_col[digit][tem_index][state_index] = preMinCost + Dis(input[i], segTemGroup[tem_index][state_index]);
                                    if (preMinCost == last_one) {
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit - 1, posOne, SEG_NUM - 1);
                                    }
                                    else if (preMinCost == last_two) {
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit - 1, posTwo, SEG_NUM - 2);
                                    }
                                    else {
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index);
                                    }
                                }
                                else if (state_index == 1)
                                {
                                    posOne = -1;
                                    last_one = minLast(pre_col[digit - 1], posOne, SEG_NUM - 1);
                                    preMinCost = min({ last_one, pre_col[digit][tem_index][state_index - 1], pre_col[digit][tem_index][state_index] });
                                    cur_col[digit][tem_index][state_index] = preMinCost + Dis(input[i], segTemGroup[tem_index][state_index]);
                                    if (preMinCost == last_one) {
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit - 1, posOne, SEG_NUM - 1);
                                    }
                                    else if (preMinCost == pre_col[digit][tem_index][state_index - 1]) {
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index - 1);
                                    }
                                    else {
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index);
                                    }
                                }
                                else
                                {
                                    preMinCost = min({ pre_col[digit][tem_index][state_index], pre_col[digit][tem_index][state_index - 1], pre_col[digit][tem_index][state_index - 2] });
                                    cur_col[digit][tem_index][state_index] = preMinCost + Dis(input[i], segTemGroup[tem_index][state_index]);
                                    if (digit < digit_num - 1) {
                                        if (!digit_able[digit + 1])
                                        {
                                            if ((state_index == SEG_NUM - 2) && (cur_col[digit][tem_index][state_index] < INT_MAX)) {
                                                digit_able[digit + 1] = true;
                                            }
                                        }
                                    }
                                    if (preMinCost == pre_col[digit][tem_index][state_index - 2]) {
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index - 2);
                                    }
                                    else if (preMinCost == pre_col[digit][tem_index][state_index - 1]) {
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index - 1);
                                    }
                                    else {
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index);
                                    }
                                }
                            }
                            
                            // digit == 0
                            else
                            {
                                if (state_index == 0) {
                                    cur_col[digit][tem_index][state_index] = pre_col[digit][tem_index][state_index] + Dis(input[i], segTemGroup[tem_index][state_index]);
                                    traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index);
                                }
                                else if (state_index == 1) {
                                    preMinCost = min({ pre_col[digit][tem_index][state_index], pre_col[digit][tem_index][state_index - 1] });
                                    cur_col[digit][tem_index][state_index] = preMinCost + Dis(input[i], segTemGroup[tem_index][state_index]);
                                    if (preMinCost == pre_col[digit][tem_index][state_index - 1]) {
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index - 1);
                                    }
                                    else {
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index);
                                    }
                                }
                                else {
                                    preMinCost = min({ pre_col[digit][tem_index][state_index], pre_col[digit][tem_index][state_index - 1], pre_col[digit][tem_index][state_index - 2] });
                                    cur_col[digit][tem_index][state_index] = preMinCost + Dis(input[i], segTemGroup[tem_index][state_index]);
                                    if (!digit_able[digit + 1])
                                    {
                                        if ((state_index == SEG_NUM - 2) && (cur_col[digit][tem_index][state_index] < INT_MAX)) {
                                            digit_able[digit + 1] = true;
                                        }
                                    }
                                    if (preMinCost == pre_col[digit][tem_index][state_index - 2]) {
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index - 2);
                                    }
                                    else if (preMinCost == pre_col[digit][tem_index][state_index - 1]) {
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index - 1);
                                    }
                                    else {
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        cur_col.swap(pre_col);
    }
    
    posOne = -1;
    posTwo = -1;
    last_one = minLast(pre_col[digit_num - 1], posOne, SEG_NUM - 1);
    last_two = minLast(pre_col[digit_num - 1], posTwo, SEG_NUM - 2);
    
    double minCost = min({ last_one, last_two });
    int minIndex = -1;
    if (minCost == last_two) {
        minIndex = getMatrixIndex(digit_num - 1, posTwo, SEG_NUM - 2);
    }
    else {
        minIndex = getMatrixIndex(digit_num - 1, posOne, SEG_NUM - 1);
    }
    
    
    int cur_index = minIndex;
    int pre_index;
    
    stack<int> resultPhone;
    stack<int> resultWords;
    resultWords.push(getTem(cur_index));
    //	resultPhone.push(cur_index);
    
    int cur_digit = -1;
    int pre_digit = -1;
    
    // do the backtracing
    for (int i = input_length - 1; i >= 0; i--) {
        pre_index = traceMatrix[i][cur_index];
        if (pre_index < 0 && i != 0) {
            cout << "Output error";
            return resultPhone;
        }
        
        if (i > 0) {
            cur_digit = getDigit(cur_index);
            pre_digit = getDigit(pre_index);
            if (cur_digit != pre_digit) {
                //				int temp = getTem(pre_index);
                int temp = getTem(pre_index);
                resultWords.push(temp);
                resultPhone.push(i);
            }
        }
        cur_index = pre_index;
    }

    while (!resultWords.empty())
    {
        cout << resultWords.top() << " ";
//        cout << resultPhone.top() << " ";
//        resultPhone.pop();
        resultWords.pop();
    }
    cout << endl;
    return resultPhone;
}

 // using guassian distance
/*
// for cost Matrix, value is cost, (input_length, digit, different_template)
// for segTemGroup, (tem_index, state, dimension)
void DigitRecognition(int digit_num, vector<vector<double>>& input, vector<vector<vector<double>>> segTemGroup, vector<vector<vector<double>>> varianceTerm, vector<vector<vector<int>>> countTransfer)
{
    int input_length = (int)input.size();
    vector<vector<int>> traceMatrix(input_length, vector<int>(digit_num * TYPE_NUM * SEG_NUM, -1));
    vector<bool> digit_able;
    for (int i = 0 ; i < digit_num; i++) {
        digit_able.push_back(false);
    }
    digit_able[0] = true;
    
    vector<vector<vector<double>>> pre_col(digit_num, vector<vector<double>>(TYPE_NUM, vector<double>(SEG_NUM)));
    
    // initialize the pre_col
    for (int i = 0; i < digit_num; i++) {
        for (int j = 0; j < TYPE_NUM; j++) {
            for (int k = 0; k < SEG_NUM; k++) {
                pre_col[i][j][k] = INT_MAX / 2;
            }
        }
    }
    
    vector<vector<vector<double>>> cur_col= pre_col;
    
    double last_one, last_two;
    int posOne = -1;
    int posTwo = -1;
    
    double preMinCost;
    
    // get the minimum cost and fill the trace matrix
    for (int i = 0; i < input_length; i++) {
        for (int digit = 0; digit < digit_num; digit++) {
            for (int tem_index = 0; tem_index < TYPE_NUM; tem_index++) {
                if (digit_num == DIGIT_NUM7 && tem_index == 0 && digit == 0) {
                    tem_index += 2;
                }
                if (i == 0)
                {
                    if (digit == 0) {
                        cur_col[0][tem_index][0] = nodeCost(input[i], segTemGroup[tem_index][0], varianceTerm[tem_index][0]) + edgeCost(0, countTransfer[tem_index][0]);
                        cur_col[0][tem_index][1] = nodeCost(input[i], segTemGroup[tem_index][1], varianceTerm[tem_index][1]) + edgeCost(1, countTransfer[tem_index][0]);
                        pre_col[0][tem_index][0] = nodeCost(input[i], segTemGroup[tem_index][0], varianceTerm[tem_index][0]) + edgeCost(0, countTransfer[tem_index][0]);
                        pre_col[0][tem_index][1] = nodeCost(input[i], segTemGroup[tem_index][1], varianceTerm[tem_index][1]) + edgeCost(1, countTransfer[tem_index][0]);
                    }
                }
                for (int state_index = 0; state_index < SEG_NUM; state_index++) {
                    if(i != 0)
                    {
                        if (digit_able[digit]) {
                            if (digit > 0) {
                                if (state_index == 0)
                                {
                                    posOne = - 1;
                                    posTwo = - 1;
                                    last_one = minLast(pre_col[digit - 1], posOne, SEG_NUM - 1) + edgeCost(state_index, countTransfer[tem_index][0]);
                                    last_two = minLast(pre_col[digit - 1], posTwo, SEG_NUM - 2) + edgeCost(state_index, countTransfer[tem_index][0]);
                                    double var = pre_col[digit][tem_index][state_index] + edgeCost(state_index, countTransfer[tem_index][state_index + 1]);
                                    preMinCost = min({last_one, last_two, var});
                                    cur_col[digit][tem_index][state_index] = preMinCost + nodeCost(input[i], segTemGroup[tem_index][state_index], varianceTerm[tem_index][state_index]);
                                    if (preMinCost == last_one) {
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit - 1, posOne, SEG_NUM - 1);
                                    }
                                    else if (preMinCost == last_two){
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit - 1, posTwo, SEG_NUM - 2);
                                    }
                                    else{
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index);
                                    }
                                }
                                else if (state_index == 1)
                                {
                                    posOne = - 1;
                                    last_one = minLast(pre_col[digit - 1], posOne, SEG_NUM - 1) + edgeCost(state_index, countTransfer[tem_index][0]);
                                    double var1 = pre_col[digit][tem_index][state_index - 1] + edgeCost(state_index, countTransfer[tem_index][state_index]);
                                    double var2 = pre_col[digit][tem_index][state_index] + edgeCost(state_index, countTransfer[tem_index][state_index + 1]);
                                    preMinCost = min({last_one, var1, var2}) ;
                                    cur_col[digit][tem_index][state_index] = preMinCost + nodeCost(input[i], segTemGroup[tem_index][state_index], varianceTerm[tem_index][state_index]);
                                    if (preMinCost == last_one) {
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit - 1, posOne, SEG_NUM - 1);
                                    }
                                    else if (preMinCost == var1){
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index - 1);
                                    }
                                    else{
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index);
                                    }
                                }
                                else
                                {
                                    double var1 = pre_col[digit][tem_index][state_index - 2] + edgeCost(state_index, countTransfer[tem_index][state_index - 1]);
                                    double var2 = pre_col[digit][tem_index][state_index - 1] + edgeCost(state_index, countTransfer[tem_index][state_index]);
                                    double var3 = pre_col[digit][tem_index][state_index] + edgeCost(state_index, countTransfer[tem_index][state_index + 1]);
                                    
                                    preMinCost = min({var3, var2, var1}) ;
                                    cur_col[digit][tem_index][state_index] = preMinCost + nodeCost(input[i], segTemGroup[tem_index][state_index], varianceTerm[tem_index][state_index]);
                                    if (digit < digit_num - 1 ) {
                                        if(!digit_able[digit + 1])
                                        {
                                            if ((state_index == SEG_NUM - 2) && (cur_col[digit][tem_index][state_index] < INT_MAX / 2 )) {
                                                digit_able[digit + 1] = true;
                                            }
                                        }
                                    }
                                    if (preMinCost == var1) {
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index - 2);
                                    }
                                    else if (preMinCost == var2){
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index - 1);
                                    }
                                    else{
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index);
                                    }
                                }
                            }
                            
                            // digit == 0
                            else
                            {
                                if (state_index == 0) {
                                    cur_col[digit][tem_index][state_index] = pre_col[digit][tem_index][state_index] + nodeCost(input[i], segTemGroup[tem_index][state_index], varianceTerm[tem_index][state_index]) + edgeCost(state_index, countTransfer[tem_index][state_index + 1]);
                                    traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index);
                                }
                                else if (state_index == 1){
                                    double var1 = pre_col[digit][tem_index][state_index] + edgeCost(state_index, countTransfer[tem_index][state_index + 1]);
                                    double var2 = pre_col[digit][tem_index][state_index - 1] + edgeCost(state_index, countTransfer[tem_index][state_index]);
                                    preMinCost = min({var1, var2});
                                    cur_col[digit][tem_index][state_index] = preMinCost + nodeCost(input[i], segTemGroup[tem_index][state_index], varianceTerm[tem_index][state_index]);
                                    if (preMinCost == var2){
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index - 1);
                                    }
                                    else{
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index);
                                    }
                                }
                                else{
                                    double var1 = pre_col[digit][tem_index][state_index - 2] + edgeCost(state_index, countTransfer[tem_index][state_index - 1]);
                                    double var2 = pre_col[digit][tem_index][state_index - 1] + edgeCost(state_index, countTransfer[tem_index][state_index]);
                                    double var3 = pre_col[digit][tem_index][state_index] + edgeCost(state_index, countTransfer[tem_index][state_index + 1]);
                                    preMinCost = min({var1, var2, var3});
                                    cur_col[digit][tem_index][state_index] = preMinCost + nodeCost(input[i], segTemGroup[tem_index][state_index], varianceTerm[tem_index][state_index]);
                                    if(!digit_able[digit + 1])
                                    {
                                        if ((state_index == SEG_NUM - 2) && (cur_col[digit][tem_index][state_index] < INT_MAX / 2 )) {
                                            digit_able[digit + 1] = true;
                                        }
                                    }
                                    if (preMinCost == var1) {
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index - 2);
                                    }
                                    else if (preMinCost == var2){
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index - 1);
                                    }
                                    else{
                                        traceMatrix[i][getMatrixIndex(digit, tem_index, state_index)] = getMatrixIndex(digit, tem_index, state_index);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        cur_col.swap(pre_col);
    }
    
    posOne = -1;
    posTwo = -1;
    last_one = minLast(pre_col[digit_num -1], posOne, SEG_NUM - 1);
    last_two = minLast(pre_col[digit_num -1], posTwo, SEG_NUM - 2);
    
    double minCost = min({last_one, last_two});
    int minIndex = -1;
    if (minCost == last_two) {
        minIndex = getMatrixIndex(digit_num - 1, posTwo, SEG_NUM - 2);
    }
    else{
        minIndex = getMatrixIndex(digit_num - 1, posOne, SEG_NUM - 1);
    }
    
    
    int cur_index = minIndex;
    int pre_index;
    
    stack<int> resultPhone;
    resultPhone.push(getTem(cur_index));
    
    int cur_digit = -1;
    int pre_digit = -1;
    
    // do the backtracing
    for (int i = input_length - 1; i >= 0; i--) {
        pre_index = traceMatrix[i][cur_index];
        if (pre_index < 0 && i != 0) {
            cout << "Output error";
            return;
        }
        
        if (i > 0) {
            cur_digit = getDigit(cur_index);
            pre_digit = getDigit(pre_index);
            if (cur_digit != pre_digit) {
                int temp = getTem(pre_index);
                resultPhone.push(temp);
            }
        }
        cur_index = pre_index;
    }
    
    while(!resultPhone.empty())
    {
        cout << resultPhone.top() << " ";
        resultPhone.pop();
    }
    return;
}

*/





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
    int length = vec.size();
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

void RestrictPhone(Trie& trie, vector<vector<double>>& input)
{
    int input_size = input.size();
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
                root->nextBranch[j]->preNodeCost[0] = Dis(input[i], root->nextBranch[j]->segTemplate[0]);
                root->nextBranch[j]->preNodeCost[1] = Dis(input[i], root->nextBranch[j]->segTemplate[1]);
            }
            else
            {
                for (int k = 0; k < SEG_NUM; k++)
                {
                    double var1 = Dis(input[i], root->nextBranch[j]->segTemplate[k]);
                    double var2 = root->nextBranch[j]->preNodeCost[k] + var1;
                    double var3 = root->nextBranch[j]->preNodeCost[k - 1] + var1;
                    double var4 = root->nextBranch[j]->preNodeCost[k - 2] + var1;
                    int pos1 = 0;
                    int pos2 = 0;
                    double var5 = costUtil(last, var1, pos1) + PENALTY;
                    double var6 = costUtil(lastTwo, var1, pos2) + PENALTY;
                    if (k == 0) {
                        if (var5 < INT_MAX / 2 )
                            root->nextBranch[j]->curNodeCost[k] = min({ var2, var5, var6 });
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

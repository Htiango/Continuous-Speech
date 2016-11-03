#ifndef Trie_h
#define Trie_h

#include <iostream>
#include <string>
#include <algorithm>
#include <assert.h>
#include <sstream>
#include <vector>
#include <map>

using namespace std;

#define MAX_BRANCH_NUM  11   // we have 26 characters. double it so that can store the one that ends here
#define SEG_NUM     (5)
#define DIMENSION   (39)
#define PI          (3.1416)
#define BEAM (15)

/* define the class for the node of the trie tree*/
class TrieNode
{
public:
    char letter;     // one letter of the word that store there
    TrieNode* nextBranch[MAX_BRANCH_NUM];   // the pointers' vectors that point to the next branch
    TrieNode* parentBranch;
    vector<double> preNodeCost;    //every state in template have a previous node cost
    vector<double> curNodeCost;   //every state in template have a current node cost
    vector<TrieNode*> vectorNode;
    vector<bool> vectorBool;
    bool leaf;
	vector<vector<double>> segTemplate;
	vector<int> nonEmitting;
    

public:
    TrieNode() : letter(NULL)
    {
        memset(nextBranch,NULL,sizeof(TrieNode*) * MAX_BRANCH_NUM);
        parentBranch = NULL;
		vector<double> preTemp(SEG_NUM, INT_MAX / 2);
        preNodeCost = preTemp;
		curNodeCost = preTemp;
        leaf = false;
    }
    bool setNodeLetter(char c);
    char getNodeLetter();
    TrieNode* getParent();
    bool isLeaf();              // return true is it's a leaf
    void setVectorNode(int len);
    void setVectorBool(int len);
	void setSegTemplate(vector<vector<double>>& temp);
    string getWord();
};


/* define the class for the trie tree */
class Trie{
public:
    Trie();
    ~Trie();
    TrieNode* getRoot();        //get root node
    void swapNodeCost();
    void setNodeVector(int len);
private:
    TrieNode* pRoot;
    void setNodeVectorUtil(TrieNode* node, int len);
private:
    void Destory(TrieNode* pRoot);
	void Init();
};



#endif /* Trie_h */

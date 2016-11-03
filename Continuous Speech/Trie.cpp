#include "Trie.h"


bool TrieNode::setNodeLetter(char c)
{
    letter = c;
    return true;
}

char TrieNode::getNodeLetter()
{
    return letter;
}

TrieNode* TrieNode::getParent()
{
    return parentBranch;
}

bool TrieNode::isLeaf()
{
    for (int i = 0; i < MAX_BRANCH_NUM; i++)
    {
        if (nextBranch[i] != NULL)
            return false;
    }
    return true;
}

void TrieNode::setVectorBool(int len)
{
    for (int i = 0; i < len; i++) {
        vectorBool.push_back(0);
    }
}

void TrieNode::setVectorNode(int len)
{
    for (int i = 0; i < len; i++) {
        vectorNode.push_back(NULL);
    }
}

void TrieNode::setSegTemplate(vector<vector<double>>& temp)
{
	segTemplate = temp;
}

string TrieNode::getWord()
{
    string str;
    vector<char> strVec;
    strVec.push_back(letter);
    TrieNode* node = getParent();
    while (node->getParent() != NULL)
    {
        strVec.push_back(node->letter);
        node = node->getParent();
    }
    int len = int(strVec.size());
    for (int i = len - 1; i >= 0; i--) {
        str += strVec[i];
    }
    
    return str;
}

Trie::Trie()
{
	Init();
}

Trie::~Trie()
{
    Destory(pRoot);
}

/* destory trie tree */
void Trie::Destory(TrieNode* pRoot)
{
    if (NULL == pRoot)
    {
        return;
    }
    for (int i = 0;i < MAX_BRANCH_NUM;i++)
    {
        Destory(pRoot->nextBranch[i]);
    }
    // destory the space occupied by letter
    if (pRoot->letter != '\0')
    {
//        delete []pRoot->letter;
        pRoot->letter = NULL;
    }
    delete pRoot;           //destory the node
    pRoot = NULL;
}


TrieNode* Trie::getRoot()
{
    return pRoot;
}

void Trie::swapNodeCost()
{
	pRoot->preNodeCost = pRoot->curNodeCost;
	for (int i = 0; i < MAX_BRANCH_NUM; i++)
		pRoot->nextBranch[i]->preNodeCost = pRoot->nextBranch[i]->curNodeCost;
}

void Trie::setNodeVector(int len)
{
    setNodeVectorUtil(pRoot, len);
}

void Trie::setNodeVectorUtil(TrieNode *node, int len)
{
    if (node == NULL) {
        return;
    }
    
    node->setVectorBool(len);
    node->setVectorNode(len);
    
    for (int i = 0; i < MAX_BRANCH_NUM; i++) {
        setNodeVectorUtil(node->nextBranch[i], len);
    }
}

void Trie::Init()
{
	pRoot = new TrieNode();     
	pRoot->setNodeLetter('*');   //non emitting state as root 
	for(int i = 0; i < MAX_BRANCH_NUM; i++)
	{
		pRoot->nextBranch[i] = new TrieNode();
		if(i == MAX_BRANCH_NUM - 1)
		{
			pRoot->nextBranch[i]->setNodeLetter('*');   //non emitting state
		}
		else
		{
			pRoot->nextBranch[i]->setNodeLetter(i + 48);   //template 0-9     
		}
		pRoot->nextBranch[i]->parentBranch = pRoot;
	}
}








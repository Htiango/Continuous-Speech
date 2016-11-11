#ifndef READ_DIR


#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>

#ifdef linux
#include <unistd.h>
#include <dirent.h>
#endif
#ifdef WIN32
#include <direct.h>
#include <io.h>
#endif

using namespace std;

vector<string> getFiles(string cate_dir);


#define READ_DIR
#endif



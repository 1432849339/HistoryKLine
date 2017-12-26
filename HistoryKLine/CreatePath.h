#ifndef  __CREATEPATH_H_
#define  __CREATEPATH_H_

#include <string.h>
#include <iostream>
#include <vector>
#include <dirent.h>
#include <map>
#include <algorithm>
#include <unistd.h>
#include <sys/stat.h> 
#include <stdio.h>
#include <stdlib.h>
#include "GetUkey.h"

using namespace std;

void SH_IndexCodeTrans(char *ChCode);
//@指定目录;创建目录
string itostr(const int64_t num);
bool CreatePath(const string Path);
bool mkdir_path(const string BasePath);

#endif //  __CREATEPATH_H_



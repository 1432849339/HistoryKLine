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


//@获取指定目录下的所有文件,然后通过文件名查询对应的ukey,然后将ukey作为key 全路径文件名作为value写入map中
//@arg file_map:存文件的一个容器,需要自己传入
//@arg specify_path:指定目录,程序将到哪个目录下去搜索文件
bool GetSpecifyPathAllFileToMap(map<int64_t, std::string>& file_map, const std::string& specify_path);



#endif //  __CREATEPATH_H_



#include "CreatePath.h"

void SH_IndexCodeTrans(char *ChCode)
{
	switch (atoi(ChCode))
	{
	case 999999:
		memcpy(ChCode, "000001", 6);
		break;
	case 999998:
		memcpy(ChCode, "000002", 6);
		break;
	case 999997:
		memcpy(ChCode, "000003", 6);
		break;
	case 999996:
		memcpy(ChCode, "000004", 6);
		break;
	case 999995:
		memcpy(ChCode, "000005", 6);
		break;
	case 999994:
		memcpy(ChCode, "000006", 6);
		break;
	case 999993:
		memcpy(ChCode, "000007", 6);
		break;
	case 999992:
		memcpy(ChCode, "000008", 6);
		break;
	case 999991:
		memcpy(ChCode, "000010", 6);
		break;
	case 999990:
		memcpy(ChCode, "000011", 6);
		break;
	case 999989:
		memcpy(ChCode, "000012", 6);
		break;
	case 999988:
		memcpy(ChCode, "000013", 6);
		break;
	case 999987:
		memcpy(ChCode, "000016", 6);
		break;
	case 999986:
		memcpy(ChCode, "000015", 6);
		break;
	default:
		if (strlen(ChCode) > 2)
			memcpy(ChCode, "00", 2);
		break;
	}
}

string itostr(const int64_t num)
{
	char num_str[32];
	sprintf(num_str, "%ld", num);
	return string(num_str);
}

bool CreatePath(const string Path)
{
	if (access(Path.c_str(), 0) == -1)
	{
		if (mkdir(Path.c_str(), 0777))
		{
			cout << "make dir error " << __LINE__ << endl;
			return false;
		}
	}
	return true;
}

bool mkdir_path(const string BasePath)
{
	if (access(BasePath.c_str(), F_OK) == 0)
	{
		return true;
	}
	else
	{
		string::size_type pos = 1;
		while ((pos = BasePath.find('/', pos)) != string::npos)
		{
			string SubStr = BasePath.substr(0, pos);
			if (access(SubStr.c_str(), F_OK) == -1)
			{
				if (mkdir(SubStr.c_str(), 0777))
				{
					if (access(SubStr.c_str(), F_OK) == -1)
					{
						return false;
					}
				}
			}
			pos++;
		}
		return true;
	}
}

bool GetSpecifyPathAllFileToMap(map<int64_t, std::string>& file_map, const std::string & specify_path)
{
	DIR* dir = nullptr;
	struct dirent* ptr = nullptr;
	char windcode[32]{ 0 };

	if ((dir = opendir(specify_path.c_str())) == nullptr)
	{
		cerr << "打开目录 :[" << specify_path << "] 失败!" << endl;
		return false;
	}
	while ((ptr = readdir(dir)) != nullptr)
	{
		//当前目录 和 上级目录
		if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
		{
			continue;
		}//是文件,且后缀为.bz2;及文件为行情压缩文件
		else if(ptr->d_type == 8 && string(ptr->d_name).rfind(".bz2") != string::npos)
		{
			string filename = specify_path;
			filename += "/";
			filename += ptr->d_name;
			auto pos_1 = filename.rfind('/') + 1;
			auto pos_2 = filename.rfind(".csv");
			memset(windcode, 0, sizeof(windcode));
			if (pos_1 == string::npos || pos_2 == string::npos)
			{
				continue;
			}//获取文件的wincode部分
			strcpy(windcode, filename.substr(pos_1, pos_2 - pos_1).c_str());
			//如果是上海的指数,则转换会真正的code
			if (market == "SH" && variety == "index")
			{
				SH_IndexCodeTrans(windcode);
			}
			auto ukey = GetUkey(market, windcode);
			if (ukey == 0)
			{
				continue;
			}
			file_map[ukey] = filename;
		}
		else if (ptr->d_type == 10)
		{
			continue;
		}
		else if (ptr->d_type == 4)
		{
			string Base;
			Base = specify_path;
			Base += "/";
			Base += ptr->d_name;
			GetSpecifyPathAllFileToMap(file_map, specify_path);
		}
	}
	return false;
}

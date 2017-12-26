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


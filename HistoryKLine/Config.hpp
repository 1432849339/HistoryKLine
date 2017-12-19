#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <algorithm>
#include <map>
#include <fstream>
#include <cstring>
#include <iostream>

using namespace std;
class Config
{
public:
	static  Config* GetConfigPtr()
	{
		static Config* config_ptr = new Config;
		return config_ptr;
	}

	bool LoadConfig(std::string& config_file_name)
	{
		ifstream fin(config_file_name);
		if (!fin.is_open())
		{
			cerr << "打开配置文件失败!"<< endl;
			return false;
		}
		char temp[1024];
		memset((void*)temp, 0, sizeof(temp));
		while (fin)
		{
			memset((void*)temp, 0, sizeof(temp));
			fin.getline(temp, sizeof(temp));
			std::string str(temp);
			auto pos = str.find("=");
			if (pos != std::string::npos)
			{
				m_value.emplace(str.substr(0, pos), str.substr(pos + 1));
			}
		}
		fin.close();
		has_init = true;
		return true;
	}

	bool Has_Load()
	{
		return has_init;
	}

	auto GetConfigByName(std::string name)->decltype(name)
	{
		auto index = std::find_if(m_value.begin(), m_value.end(), [&name](decltype(m_value)::value_type value){
			return name == value.first;
		});
		return index != m_value.end() ? (*index).second : string();
	}
protected:
private:
	Config() :has_init(false)
	{}
private:
	std::map<std::string, std::string>		m_value;
	bool									has_init;
	std::string								SoucePath;
	std::string								QuotePath;
	std::string								RequestPath;
};
#endif//!__CONFIG_H__




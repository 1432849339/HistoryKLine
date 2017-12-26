#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <algorithm>
#include <map>
#include <initializer_list>
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

	void show()
	{
		for (auto &it:m_value)
		{
			cout << "key = " << it.first << "            value = " << it.second << endl;
		}
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


class MyLog
{
public:
	MyLog() {};

	MyLog(string  log_name) {
		_log_name = log_name;
	}

	~MyLog() {
		if (fout.is_open())
		{
			fout.close();
		}
	}

	bool  OpenLog() {
		if (fout.is_open())
		{
			return true;
		}
		this->fout.open(_log_name);
		if (!fout.is_open())
		{
			return false;
		}
		return true;
	}

	void SetTitle(initializer_list<string>& title) {
		for (auto &it:title)
		{
			fout << it << ",";
		}
		fout << endl;
	}

	template<typename T,typename...Args>
	void log2file(T& head, Args&&...args) {
		fout << head << ",";
		log2file(args...);
	}
protected:
	template<typename T>
	void log2file(T& head) {
		fout << head << endl;
	}
private:
	ofstream   fout;
	string     _log_name;
};
#endif//!__CONFIG_H__




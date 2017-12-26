#ifndef __TEMPTOKLINE_H__
#define __TEMPTOKLINE_H__

#include "KLineDataDef.hpp"
#include "shm.h"
#include "ukey.h"
#include "CreatePath.h"



template<typename T, int Step>
class ToKLine
{
public:
	typedef  KLineValue<T,Step>		ValueType;
	//typedef  shared_ptr<ValueType>	ValuePtr;
	typedef list<ValueType>			Trd_list;
	typedef map<int64_t, Trd_list>  M_List;
public:
	ToKLine() {
		_handle = 0;
		_date = GetCurrentDate();
		flag_next = true;
	}

	ToKLine(const string& date) :_date(date) {
		_handle = 0;
		flag_next = true;
	}

	virtual ~ToKLine() {

	}
public:

	/*
	 *@设置内存块前缀:如TRD  ORD ORQ...... 
	 */
	void SetShmName(string shm_name){
		_shm_name += shm_name;
		_shm_name += _date;
	}

	/*
	 *@设置配置文件
	 */
	void SetConfigFileName(string config_filename){
		Config* config = Config::GetConfigPtr();
		if (!config->Has_Load())
		{
			config->LoadConfig(config_filename);
		}
	}

	/*
	 *@初始化当天的内存块用于读,失败返回false
	 */
	bool InitShmAndGetHandle();

	/*
	 *@读取内存块中的数据,并开启实时生成K线的线程,且写入文件
	 */
	void ReadShmAndGetKLine();

public:
	//@获取当前日期
	static string GetCurrentDate();
	//@获取当前时间戳(单位:微秒)
	static int64_t GetCurrentTime();
	//@返回日期
	string  GetDate()
	{
		return _date;
	}
private:
	/*
	*@计算K线的线程函数
	*/
	void CalKLine();

	/*
	 *@写入文件
	 */
	bool WriteToFile(KLine& kline);

	/*
	*@KLine结构体转换为文本形式
	*/
	string KlineToStr(KLine& kline);

	/*
	*@ToKline
	*/
	virtual bool SDSToKline(KLine& kline, int64_t ukey, list<ValueType>& value) = 0;

	/*
	 *@监视线程判断是否跨天
	 */
	void is_next_day();
private:
	map<int64_t, KLine>			LastValue;
	atomic<bool>				flag_next;//是否跨天
	int							_handle;
	mutex						_mutex;
	string						_shm_name;
	string						_date;
	M_List						_container;
};



template<typename T, int Step>
inline string ToKLine<T, Step>::GetCurrentDate()
{
	auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm* ptm = localtime(&tt);
	char date[32] = { 0 };
	sprintf(date, "%d%02d%02d",
		(int)ptm->tm_year + 1900,
		(int)ptm->tm_mon + 1,
		(int)ptm->tm_mday);
	return std::string(date);
}

template<typename T, int Step>
inline int64_t ToKLine<T, Step>::GetCurrentTime()
{
	auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	int64_t used = tt * 1000000;
	return used;
}

template<typename T, int Step>
bool ToKLine<T, Step>::InitShmAndGetHandle()
{
	int retcode = 0;
	char appname[] = "shmToKLine";
	retcode = shmInit(9901, appname);
	if (retcode != 0)
	{
		cerr << " shmInit error=[" << retcode << "] func = " << __FUNCTION__ << " line:" << __LINE__ << endl;
		return false;
	}
	retcode = shmOpenForRead(const_cast<char*>(_shm_name.c_str()));
	if (retcode != 0)
	{
		cerr << " shmOpenForRead error=[" << retcode << "] func = " << __FUNCTION__ << " line:" << __LINE__ << endl;
		return false;
	}
	retcode = shmHandle(const_cast<char*>(_shm_name.c_str()));
	if (retcode < 0)
	{
		cerr << " shmOpenGetHandle error=[" << retcode << "] func = " << __FUNCTION__ << " line:" << __LINE__ << endl;
		return false;
	}
	_handle = retcode;
	puts("***********************初始化成功*************************");
	cout << "*********************" << _shm_name << "*********************\n";
	return true;
}

template<typename T, int Step>
void ToKLine<T, Step>::ReadShmAndGetKLine()
{
	if (0 == shmStat(_handle))
	{
		cerr << "无效的句柄 handle=[" << _handle << "] func = " << __FUNCTION__ << " line:" << __LINE__ << endl;
		return;
	}
	int index = 0;
	char *pData = new char[1024];
	ValueType  sds20TrdKline;
	char windcode[32];
	auto sds20TrdKlineSize = sizeof(ValueType);
	//开启计算K线的线程
	thread calcuKline(&ToKLine::CalKLine, this);
	//开启判断是否跨天的线程
	thread is_next(&ToKLine::is_next_day, this);
	while (this->flag_next)
	{
		if (index < shmRecnum(_handle))
		{
			memset((void*)&sds20TrdKline, 0, sds20TrdKlineSize);
			if (index != shmRead(_handle, pData, index + 1) - 1)
			{
				cerr << "读取数据出错 index=[" << index + 1 << "] func=" << __FUNCTION__ << " line:" << __LINE__ << endl;
				return;
			}
			index++;
			memcpy((void*)&sds20TrdKline, pData, sds20TrdKlineSize);
			sds20TrdKline.SetBelong();
			string wind_code = sds20TrdKline.szWindCode;
			if (typeid(T).name() == typeid(SDS20INDEX).name())
			{
				memset((void*)windcode, 0, 32);
				strcpy(windcode, wind_code.c_str());
				SH_IndexCodeTrans(windcode);
				wind_code = windcode;
			}
			int64_t ukey = GetUkey(wind_code.substr(wind_code.rfind('.') + 1), wind_code.substr(0, wind_code.rfind('.')));
			if (ukey == 0)
			{
				continue;
			}
			{
				lock_guard<mutex> loker(_mutex);
				_container[ukey].emplace_back(sds20TrdKline);
				//cout << "ukey=" << ukey << "\t" << "windcode = " << sds20TrdKline.szWindCode <<"\t"<< "time" << sds20TrdKline.nTime << endl;
				//this_thread::sleep_for(chrono::seconds(1));
			}
		}
		else
		{
			this_thread::sleep_for(chrono::seconds(5));
		}
	}
	delete[] pData;
	is_next.join();
	calcuKline.join();
	shmClose(const_cast<char*>(_shm_name.c_str()));
}

template<typename T, int Step>
void ToKLine<T, Step>::CalKLine()
{
	M_List				sdsKline;
	chronos::KLine		kline{ 0 };
	vector<KLine>		cache;
	int64_t belong = 0;
	while (this->flag_next)
	{
		cache.clear();
		sdsKline.clear();
		belong = 0;
		this_thread::sleep_for(chrono::seconds(5));
		{
			lock_guard<mutex>  locker(this->_mutex);
			for (auto &it : this->_container)
			{
				if (!it.second.empty())
				{
					belong = it.second.front().GetBelong();
					auto pos = std::find_if(it.second.begin(), it.second.end(), [&belong](ValueType& value) {
						return belong != value.GetBelong();
					});
					if (pos == it.second.end())
					{
						continue;
					}
					it.second.remove_if([&](ValueType& value) {
						if (value.GetBelong() == belong)
						{
							sdsKline[it.first].emplace_back(value);
							return true;
						}
						return false;
					});
				}
			}
		}
		//计算K线
		for (auto &it : sdsKline)
		{
			if (SDSToKline(kline, it.first, it.second))
			{
				cache.emplace_back(kline);
			}	
		}
		//写入文件
		for (auto &it : cache)
		{
			//if (LastValue.find(it.ukey) != LastValue.end())
			//{
			//	while ((it.belong / 100000 - Step) != LastValue[it.ukey].belong/100000)
			//	{
			//		if (LastValue[it.ukey].belong / 100000 == 1130 || (LastValue[it.ukey].belong % 10000000)/100000 == 60- Step)
			//		{
			//			break;
			//		}
			//		LastValue[it.ukey].timeus += Step * 60000000;	//+60 sed
			//		LastValue[it.ukey].belong += Step * 100000;	//+1 minu
			//		LastValue[it.ukey].match_num = 0;
			//		LastValue[it.ukey].turnover = 0;
			//		LastValue[it.ukey].volume = 0;
			//		WriteToFile(LastValue[it.ukey]);
			//	}
			//}
			//else
			//{
			//	int start_time = 0;
			//	if (typeid(T) == typeid(SDS20LEVEL2))//9:31 - 11:30  13:01-15:00
			//	{
			//		start_time = 93100000;
			//	}
			//	else if (typeid(T) == typeid(SDS20FUTURE))//21:01-01:00   09:30
			//	{
			//		start_time = 210100000;
			//	}
			//	else if(typeid(T) == typeid(SDS20INDEX))
			//	{
			//		start_time = 93100000;
			//	}
			//	else if (typeid(T) == typeid(SDS20TRANSACTION))
			//	{
			//		start_time = 93100000;
			//	}
			//	/*while ((kline.belong - Step * 100000) != )
			//	{
			//	}*/
			//}
			WriteToFile(it);
			//LastValue[it.ukey] = it;
		}
	}
	//处理剩下的
	while(!_container.empty())
	{
		belong = 0;
		sdsKline.clear();
		for (auto &it:_container)
		{
			if (!it.second.empty())
			{
				belong = it.second.front().GetBelong();
				it.second.remove_if([&](ValueType& value) {
					if (value.GetBelong() == belong)
					{
						sdsKline[it.first].emplace_back(value);
						return true;
					}
					return false;
				});
			}	
		}
		//计算K线
		for (auto &it : sdsKline)
		{
			if (SDSToKline(kline, it.first, it.second))
			{
				cache.emplace_back(kline);
			}
		}
		//写入文件
		for (auto &it : cache)
		{
			WriteToFile(it);
		}
	}
}

template<typename T, int Step>
bool ToKLine<T, Step>::WriteToFile(KLine& kline)
{
	static map<int, string> MarketID{
		// 亚太地区
		//{ MARKET_ALL, "ALL" },
		{ MARKET_SZA, "SZA" },
		{ MARKET_SHA, "SHA" },
		{ MARKET_CFE, "CFE" },
		{ MARKET_SHF, "SHF" },
		{ MARKET_CZC, "CZC" },
		{ MARKET_DCE, "DCE" },
		{ MARKET_SGE, "SGE" },
		{ MARKET_SZB, "SZB" },
		{ MARKET_SHB, "SHB" },
		{ MARKET_HK, "HK" },
		{ MARKET_IBBM, "IBBM" },
		{ MARKET_OTC, "OTC" },
		{ MARKET_TAIFEX, "TAIFEX" },
		{ MARKET_SGX, "SGX" },
		{ MARKET_SICOM, "SICOM" },
		{ MARKET_JPX, "JPX" },
		{ MARKET_TOCOM, "TOCOM" },
		{ MARKET_BMD, "BMD" },
		{ MARKET_TFEX, "TFEX" },
		{ MARKET_AFET, "AFET" },
		{ MARKET_KRX, "KRX" },


		// 欧洲地区
		{ MARKET_LME, "LME" },
		{ MARKET_ICE, "ICE" },
		{ MARKET_LIFFE, "LIFFE" },
		//{ MARKET_XEurex, "XEurex" },

		// 美洲地区
		{ MARKET_CME, "CME" },
		{ MARKET_CBOT, "CBOT" },
		{ MARKET_NYBOT, "NYBOT" },
		{ MARKET_NYMEX_COMEX, "NYMEX_COMEX" },
		{ MARKET_ICE_CANOLA, "ICE_CANOLA" },
		{ MARKET_eCBOT, "eCBOT" },
		{ MARKET_CBOE, "CBOE" },

		// 其他地区
		{ MARKET_SFE, "SFE" },
		{ MARKET_DME, "DME" },
		{ MARKET_DGCX, "DGCX" },
	};

	static map<int, string> VarID{
		//{ VARIETY_ALL, "all" },
		{ VARIETY_STOCK, "stock" },
		{ VARIETY_BOND, "bond" },
		{ VARIETY_FUND, "fund" },
		{ VARIETY_SPOT, "spot" },
		{ VARIETY_MONEY_MARKET, "money" },
		{ VARIETY_INDEX, "index" },
		{ VARIETY_FUTURE, "future" },
		{ VARIETY_OPTION, "option" },
		{ VARIETY_WARRANT, "warrant" },
		{ VARIETY_STOCK_OPTION, "stcopt" },
	};

	Config* config = Config::GetConfigPtr();
	if (!config->Has_Load())
	{
		cerr << "无效的配置!" << endl;
		return false;
	}
	string request_name = config->GetConfigByName("request_path");
	if (*(request_name.rbegin()) != '/')
	{
		request_name += "/";
	}
	int mk = 0, ty = 0;
	get_variety_market_by_ukey(kline.ukey, ty, mk);
	if ((MarketID.find(mk) != MarketID.end()) && (VarID.find(ty) != VarID.end()))
	{
		request_name += MarketID[mk];
		request_name += "/";
		request_name += VarID[ty];
		request_name += "/";
		request_name += itostr(kline.ukey);
		request_name += "/";
		request_name += _date.substr(0, 4);
		request_name += "/";
		//request_name += _date;
		request_name += _shm_name;
		request_name += "/";
		while (!mkdir_path(request_name))
		{
			cerr << "waitting mkdir " << request_name << endl;
		}
		request_name += _date + ".csv";
		ofstream fout_csv(request_name, ios::app);
		if (!fout_csv.is_open())
		{
			cerr << "create " << request_name << "  error" << endl;
			return false;
		}
		string str;
		fout_csv << KlineToStr(kline);
		fout_csv.close();
	}
	return true;
}

template<typename T, int Step>
inline string ToKLine<T, Step>::KlineToStr(KLine & kline)
{
	stringstream ss;
	ss << kline.ukey << ",";
	ss << kline.trday << ",";
	ss << kline.belong << ",";
	ss << kline.timeus << ",";
	ss << kline.open << ",";
	ss << kline.high << ",";
	ss << kline.low << ",";
	ss << kline.close << ",";
	ss << kline.volume << ",";
	ss << kline.turnover << ",";
	ss << kline.match_num << ",";
	ss << kline.interest << endl;
	return ss.str();
}

template<typename T, int Step>
void ToKLine<T, Step>::is_next_day()
{
	using std::chrono::system_clock;
	Config* config = Config::GetConfigPtr();
	if (!config->Has_Load())
	{
		cerr << "配置无效!" << endl;
		return;
	}
	auto endHour = atoi(config->GetConfigByName("AcrossHour").c_str());
	auto endMinu = atoi(config->GetConfigByName("AcrossMinu").c_str());
	time_t tt = time(nullptr);
	struct std::tm * ptm = std::localtime(&tt);
	if (ptm->tm_hour > endHour || (ptm->tm_gmtoff == endHour && ptm->tm_min >= endMinu))
	{
		ptm->tm_mday += 1;
	}
	ptm->tm_hour = endHour;
	ptm->tm_min = endMinu;
	ptm->tm_sec = 0;
	std::this_thread::sleep_until(system_clock::from_time_t(mktime(ptm)));
	flag_next = false;
}

#endif//!__TEMPTOKLINE_H__


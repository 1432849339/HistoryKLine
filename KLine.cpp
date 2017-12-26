#include "KLine.h"


map<int, string> MarketID{
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

map<int, string> VarID{
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

string TrdToKLine::GetCurrentDate()
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

int64_t TrdToKLine::GetCurrentTime()
{
	auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	int64_t used = tt * 1000000;
	return used;
}

bool TrdToKLine::InitShmAndGetHandle()
{
	int retcode = 0;
	char appname[] = "TrdToMinKLine";
	retcode = shmInit(9901, appname);
	if (retcode != 0)
	{
		cerr << " shmInit error=[" << retcode << "] func" << __FUNCTION__ <<" line:"<<__LINE__<< endl;
		return false;
	}
	retcode = shmOpenForRead(const_cast<char*>(_shm_name.c_str()));
	if (retcode != 0)
	{
		cerr << " shmOpenForRead error=[" << retcode << "] func" << __FUNCTION__ << " line:" << __LINE__ << endl;
		return false;
	}
	retcode = shmHandle(const_cast<char*>(_shm_name.c_str()));
	if (retcode < 0)
	{
		cerr << " shmOpenGetHandle error=[" << retcode << "] func" << __FUNCTION__ << " line:" << __LINE__ << endl;
		return false;
	}
	_handle = retcode;
	puts("**********************初始化成功************************");
	cout << "**********recnum = " << shmRecnum(retcode) <<"************"<< endl;
	cout << "**********shm_name = " << _shm_name << "**********" << endl;
	return true;
}

void TrdToKLine::ReadShmAndGetKline()
{
	if (0 == shmStat(_handle))
	{
		cerr << "无效的句柄 handle=[" << _handle << "] func" << __FUNCTION__ << " line:" << __LINE__ << endl;
		return;
	}
	int index = 0;
	char *pData = new char[1024];
	SDS20TrdKLine<1>  sds20TrdKline;
	auto sds20TrdKlineSize = sizeof(sds20TrdKline);
	//开启计算K线的线程
	thread calcuKline(&TrdToKLine::CalculateKLine, this);
	while (this->ne)
	{
		memset((void*)&sds20TrdKline, 0, sds20TrdKlineSize);
		if (index < shmRecnum(_handle))
		{
			if (index != shmRead(_handle, pData, index + 1) - 1)
			{
				cerr << "读取数据出错 index=[" << index + 1 << "] func" << __FUNCTION__ << " line:" << __LINE__ << endl;
				return;
			}
			index++;
			memcpy((void*)&sds20TrdKline, pData, sds20TrdKlineSize);
			sds20TrdKline.SetBelong();
			string wind_code = sds20TrdKline.szWindCode;
			int64_t ukey = GetUkey(wind_code.substr(wind_code.rfind('.') + 1), wind_code.substr(0, wind_code.rfind('.')));
			if (ukey == 0)
			{
				continue;
			}
			{
				lock_guard<mutex> loker(_mutex);
				_container[ukey].emplace_back(sds20TrdKline);
			}
		}
		else
		{
			this_thread::sleep_for(chrono::seconds(5));
		}
	}
	delete[] pData;
	calcuKline.join();
}

void TrdToKLine::CalculateKLine()
{
	extern atomic<bool> flag_next;
	M_List				sdsKline;
	chronos::KLine		kline{ 0 };
	vector<KLine>		cache;
	int belong = 0;
	while (flag_next)
	{
		cache.clear();
		sdsKline.clear();
		belong = 0;
		this_thread::sleep_for(chrono::seconds(5));
		{
			lock_guard<mutex>  locker(this->_mutex);
			for (auto &it : this->_container)
			{
				belong = it.second.front().GetBelong();
				auto pos = std::find_if(it.second.begin(), it.second.end(), [&belong](ValueType& value) {
					return belong != value.GetBelong();
				});
				if (pos == it.second.end())
				{
					continue;
				}
				std::remove_if(it.second.begin(), it.second.end(), [&](ValueType& value) {
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
		for (auto &it:sdsKline)
		{
			TrdToKline(kline, it.first, it.second);
			cache.emplace_back(kline);
		}
		//写入文件
		for (auto &it:cache)
		{
			if (!WriteToFile(it))
			{
				cerr << "写入失败! [" << it.ukey << "] 时间[" << it.timeus << "]\n";
			}
		}
	}
}

string TrdToKLine::KlineToStr(KLine & kline)
{
	stringstream ss;
	ss << kline.ukey << ",";
	ss << kline.trday << ",";
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

bool TrdToKLine::WriteToFile(KLine& kline)
{
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
		request_name += _date;
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
		return true;
	}
	return false;
}

inline void TrdToKLine::TrdToKline(KLine & kline, int64_t  ukey, list<ValueType> & value)
{
	kline.ukey = ukey;
	kline.trday = atoi(this->_date.c_str());
	kline.timeus = value.front().GetBelong();
	kline.open = value.front().nPrice;
	kline.close = value.back().nPrice;
	kline.high = std::max_element(value.begin(), value.end(), [](ValueType& value1, ValueType& value2) {
		return value1.nPrice < value2.nPrice;
	})->nPrice;
	kline.low = std::min_element(value.begin(), value.end(), [](ValueType& value1, ValueType& value2) {
		return value1.nPrice < value2.nPrice;
	})->nPrice;
	/*kline.volume = value.back().nVolume;
	kline.turnover = value.back().nTurnover;*/
	for (auto &it : value)
	{
		kline.volume += it.nVolume;
	}
	for (auto &it:value)
	{
		kline.turnover += it.nTurnover;
	}
	kline.match_num = (int32_t)value.size();
	//kline.interest=
}

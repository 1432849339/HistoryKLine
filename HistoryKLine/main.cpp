/************************************************************************
	创建日期:2017 - 12 - 20
	作者 : fqy
	功能 : 读取内存块数据, 并实时的生成K线
	最后修改 : 2017 - 12 - xx
************************************************************************/

#include "TempToKLine.hpp"

#define  DEBUG

struct LastMinuValue
{
	int64_t last_minu_volume;
	int64_t last_minu_turnover;
	int     nNumTrades;
};

int64_t GetMsTime(int64_t ymd, int64_t hmsu) {
	struct tm timeinfo = { 0 };
	time_t second;
	int64_t  usecond;
	timeinfo.tm_year = static_cast<int>(ymd / 10000 - 1900);
	timeinfo.tm_mon = static_cast<int>((ymd % 10000) / 100 - 1);
	timeinfo.tm_mday = static_cast<int>(ymd % 100);
	second = mktime(&timeinfo);
	//80000000

	int hou = static_cast<int>(hmsu / 10000000);
	int min = static_cast<int>((hmsu % 10000000) / 100000);
	int sed = static_cast<int>((hmsu % 100000) / 1000);
	int used = static_cast<int>(hmsu % 1000);

	usecond = second + hou * 3600 + min * 60 + sed;
	usecond *= 1000000;
	usecond += used * 1000;
	return usecond;
}

template<typename ValueT,int KlineType>
class shmToKline :public ToKLine<ValueT, KlineType>
{
	typedef KLineValue<ValueT, KlineType>  ValueType;
private:
	virtual bool SDSToKline(KLine& kline, int64_t ukey, list<ValueType>& value) {
		return false;
	}

};

template<int KlineType>
class shmToKline<SDS20TRANSACTION, KlineType> :public ToKLine<SDS20TRANSACTION, KlineType>
{
	typedef KLineValue<SDS20TRANSACTION, KlineType>  ValueType;
private:
	virtual bool SDSToKline(KLine& kline, int64_t ukey, list<ValueType>& value) {
#ifdef DEBUG
		/*----------------------------------------------------------------------------------------------------*/
		static MyLog  mylog("/home/sds/fqy/kline_trd.log");
		if (!mylog.OpenLog())
		{
			return false;
		}
		if (ukey == 1114113)
		{
			mylog.log2file(
				"GetBelong",
				"szWindCode",
				"nActionDay",
				"nTime",
				"nIndex",
				"nPrice",
				"nVolume",
				"nTurnover",
				"nBSFlag",
				"chOrderKind",
				"chFunctionCode",
				"nAskOrder",
				"nBidOrder",
				"nRecvTime",
				"nSn"
			);
		}
		for_each(value.begin(), value.end(), [&mylog, &ukey](ValueType& value1) {
			if (ukey == 1114113)
			{
				auto belong = value1.GetBelong();
				mylog.log2file(
					belong,
					value1.szWindCode,
					value1.nActionDay,
					value1.nTime,
					value1.nIndex,
					value1.nPrice,
					value1.nVolume,
					value1.nTurnover,
					value1.nBSFlag,
					value1.chOrderKind,
					value1.chFunctionCode,
					value1.nAskOrder,
					value1.nBidOrder,
					value1.nRecvTime,
					value1.nSn
				);
				return true;
			}
			return false;
		});
		/*----------------------------------------------------------------------------------------------------*/
#endif
		value.remove_if([this](ValueType& value1) {
			return (value1.nBSFlag == ' '
				|| (value1.nActionDay != atoi(this->GetDate().c_str()) && value1.GetBelong() / 10000000 <= 15)
				);
		});
		if (value.empty())
		{
			return false;
		}

		kline.ukey = ukey;
		kline.trday = atoi(this->GetDate().c_str());
		kline.belong = value.front().GetBelong();
		kline.timeus = GetMsTime(value.front().nActionDay, value.front().GetBelong());
		kline.open = value.front().nPrice;
		kline.close = value.back().nPrice;
		kline.high = std::max_element(value.begin(), value.end(), [](ValueType& value1, ValueType& value2) {
			return value1.nPrice < value2.nPrice;
		})->nPrice;
		kline.low = std::min_element(value.begin(), value.end(), [](ValueType& value1, ValueType& value2) {
			return value1.nPrice < value2.nPrice;
		})->nPrice;
		kline.volume = 0;
		for (auto &it : value)
		{
			kline.volume += it.nVolume;
		}
		kline.turnover = 0;
		for (auto &it : value)
		{
			kline.turnover += it.nTurnover;
		}
		kline.match_num = (int32_t)value.size();
		kline.interest = 0;//未知
	   /*----------------------------------------------------------------------------------------------------*/
		return true;
	}
	
};

template<int KlineType>
class shmToKline<SDS20LEVEL2, KlineType> :public ToKLine<SDS20LEVEL2, KlineType>
{
	typedef KLineValue<SDS20LEVEL2, KlineType>  ValueType;
private:
	virtual bool SDSToKline(KLine& kline, int64_t ukey, list<ValueType>& value) {
		value.remove_if([this](ValueType& value1) {
			return (value1.nMatch == 0
				|| value1.iVolume == 0
				|| value1.iTurnover == 0
				|| (value1.nActionDay != atoi(this->GetDate().c_str()) && value1.GetBelong() / 10000000 <= 15));
		});
		if (value.empty())
		{
			return false;
		}
#ifdef DEBUG
		/*---------------------------------------------------------------------------------------------------*/
		static MyLog  mylog("/home/sds/fqy/kline_lvt.log");
		if (!mylog.OpenLog())
		{
			return false;
		}
		if (ukey == 1114113)
		{
			mylog.log2file(
				"belong",
				"szWindCode",
				"nActionDay",
				"nTime",
				"nMatch",
				"iVolume",
				"iTurnover",
				"nIOPV",
				"nNumTrades"
			);
		}
		for_each(value.begin(), value.end(), [&mylog, &ukey](ValueType& value1) {
			if (ukey == 1114113)
			{
				auto belong = value1.GetBelong();
				mylog.log2file(
					belong,
					value1.szWindCode,
					value1.nActionDay,
					value1.nTime,
					value1.nMatch,
					value1.iVolume,			
					value1.iTurnover,		
					value1.nIOPV,
					value1.nNumTrades
				);
				return true;
			}
			return false;
		});
		/*---------------------------------------------------------------------------------------------------*/
#endif // DEBUG
		kline.ukey = ukey;
		kline.trday = atoi(this->GetDate().c_str());
		kline.belong = value.front().GetBelong();
		kline.timeus = GetMsTime(value.front().nActionDay,value.front().GetBelong());
		kline.open = value.front().nMatch;
		kline.close = value.back().nMatch;
		kline.high = std::max_element(value.begin(), value.end(), [](ValueType& value1, ValueType& value2) {
			return value1.nMatch < value2.nMatch;
		})->nMatch;
		kline.low = std::min_element(value.begin(), value.end(), [](ValueType& value1, ValueType& value2) {
			return value1.nMatch < value2.nMatch;
		})->nMatch;
		kline.volume = value.back().iVolume - LastValue[ukey].last_minu_volume;
		LastValue[ukey].last_minu_volume = value.back().iVolume;
		/*if (kline.volume == 0)
		{
			return false;
		}*/
		kline.turnover = value.back().iTurnover - LastValue[ukey].last_minu_turnover;
		LastValue[ukey].last_minu_turnover = value.back().iTurnover;
		/*if (kline.turnover == 0)
		{
			return false;
		}*/
		kline.match_num = value.back().nNumTrades - LastValue[ukey].nNumTrades;
		LastValue[ukey].nNumTrades = value.back().nNumTrades;
		int variety = 0, market_id = 0;
		get_variety_market_by_ukey(ukey, variety, market_id);
		if (variety == VARIETY_FUND)
		{
			kline.interest = value.back().nIOPV;
		}
		else
		{
			kline.interest = 0;
		}
		return true;
	}
private:
	map<int64_t, LastMinuValue>  LastValue;
};

template<int KlineType>
class shmToKline<SDS20INDEX, KlineType> :public ToKLine<SDS20INDEX, KlineType>
{
	typedef KLineValue<SDS20INDEX, KlineType>  ValueType;
private:
	virtual bool SDSToKline(KLine& kline, int64_t ukey, list<ValueType>& value) {
		value.remove_if([this](ValueType& value1) {
			return (value1.nLastIndex == 0
				|| value1.iTotalVolume == 0
				|| value1.iTurnover == 0
				|| (value1.nActionDay != atoi(this->GetDate().c_str()) && value1.GetBelong() / 10000000 <= 15));
		});
		if (value.empty())
		{
			return false;
		}
		kline.ukey = ukey;
		kline.trday = atoi(this->GetDate().c_str());
		kline.belong = value.front().GetBelong();
		kline.timeus = GetMsTime(value.front().nActionDay, value.front().GetBelong());
		kline.open = value.front().nLastIndex;
		kline.close = value.back().nLastIndex;
		kline.high = std::max_element(value.begin(), value.end(), [](ValueType& value1, ValueType& value2) {
			return value1.nLastIndex < value2.nLastIndex;
		})->nLastIndex;
		kline.low = std::min_element(value.begin(), value.end(), [](ValueType& value1, ValueType& value2) {
			return value1.nLastIndex < value2.nLastIndex;
		})->nLastIndex;
		kline.volume = (value.back().iTotalVolume - LastValue[ukey].last_minu_volume) * 100;
		LastValue[ukey].last_minu_volume = value.back().iTotalVolume;
		/*if (kline.volume == 0)
		{
			return false;
		}*/
		kline.turnover = (value.back().iTurnover - LastValue[ukey].last_minu_turnover) * 100;
		LastValue[ukey].last_minu_turnover = value.back().iTurnover;
		/*if (kline.turnover == 0)
		{
			return false;
		}*/
		kline.match_num = 0;//未知笔数
		kline.interest = 0;
		return true;
	}
private:
	map<int64_t, LastMinuValue>  LastValue;
};

template<int KlineType>
class shmToKline<SDS20FUTURE, KlineType> :public ToKLine<SDS20FUTURE, KlineType>
{
	typedef KLineValue<SDS20FUTURE, KlineType>  ValueType;
private:
	virtual bool SDSToKline(KLine& kline, int64_t ukey, list<ValueType>& value) {
		value.remove_if([this](ValueType& value1) {
			return (value1.nMatch == 0
				|| value1.iVolume == 0
				|| value1.iTurnover == 0
				|| (value1.nActionDay != atoi(this->GetDate().c_str()) && value1.GetBelong() / 10000000 <= 15));
		});
		if (value.empty())
		{
			return false;
		}
		kline.ukey = ukey;
		kline.trday = atoi(this->GetDate().c_str());
		kline.belong = value.front().GetBelong();
		kline.timeus = GetMsTime(value.front().nActionDay, value.front().GetBelong());
		kline.open = value.front().nMatch;
		kline.close = value.back().nMatch;
		kline.high = std::max_element(value.begin(), value.end(), [](ValueType& value1, ValueType& value2) {
			return value1.nMatch < value2.nMatch;
		})->nMatch;
		kline.low = std::min_element(value.begin(), value.end(), [](ValueType& value1, ValueType& value2) {
			return value1.nMatch < value2.nMatch;
		})->nMatch;
		kline.volume = value.back().iVolume - LastValue[ukey].last_minu_volume;
		LastValue[ukey].last_minu_volume = value.back().iVolume;
		/*if (kline.volume == 0)
		{
			return false;
		}*/
		kline.turnover = value.back().iTurnover - LastValue[ukey].last_minu_turnover;
		LastValue[ukey].last_minu_turnover = value.back().iTurnover;
		/*if (kline.turnover == 0)
		{
			return false;
		}*/
		kline.match_num = 0;//未知笔数
		kline.interest = value.back().iOpenInterest;
		return true;
	}
private:
	map<int64_t, LastMinuValue>  LastValue;
};

template<typename ValueT,int KlineStep>
void AddThreadShmToKline(string shmname_prefixes,string config_name,int sleep_sed)
{
	this_thread::sleep_for(chrono::seconds(sleep_sed));
	shmToKline<ValueT, KlineStep>  toKline;
	toKline.SetShmName(shmname_prefixes);
	toKline.SetConfigFileName(config_name);
	while (!toKline.InitShmAndGetHandle()) {
		cerr << "初始化内存块:" << shmname_prefixes << "失败!" << endl;
		this_thread::sleep_for(chrono::seconds(5));
	}
	toKline.ReadShmAndGetKLine();
}

int main(int argc,char* argv[])
{
	if (argc != 2)
	{
		cerr << "请输入配置文件!" << endl;
		return -1;
	}
	string config_file_name = argv[1];
	Config* config = Config::GetConfigPtr();
	if (!config->Has_Load())
	{
		config->LoadConfig(config_file_name);
	}
	config->show();

	set<string> market_set;
	if (config->GetConfigByName("conn_way") == "mysql")
	{
		if (!Init_ukdb09(market_set, false))
		{
			cerr << "init database ukdb09 error" << "func=" << __FUNCTION__ << " line" << __LINE__ << endl;
			return -1;
		}
	}
	else
	{
		if (!Init_ukdb09(market_set, true))
		{
			cerr << "init database ukdb09 error" << "func" << __FUNCTION__ << " line" << __LINE__ << endl;
			return -1;
		}
	}
	thread t1(AddThreadShmToKline<SDS20TRANSACTION,1>, "TRD", config_file_name,1);
	thread t2(AddThreadShmToKline<SDS20LEVEL2, 1>, "LVT", config_file_name, 3);
	thread t3(AddThreadShmToKline<SDS20INDEX, 1>, "IDX", config_file_name, 5);
	thread t4(AddThreadShmToKline<SDS20FUTURE, 1>, "SPI", config_file_name, 7);
	thread t5(AddThreadShmToKline<SDS20FUTURE, 1>, "CFE", config_file_name, 9);

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();

    return 0;
}
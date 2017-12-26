#ifndef __KLINE_H__
#define __KLINE_H__

#include <iostream>
#include <algorithm>
#include <functional>
#include <cstdio>
#include <cstring>
#include <chrono>
#include <cstdlib>
#include <cstddef>
#include <ctype.h>
#include <list>
#include <sstream>
#include <typeinfo>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <typeinfo>
#include <atomic>
#include <condition_variable>
#include <array>
#include <map>
#include <set>
#include "shm.h"
#include "ukey.h"
#include "CreatePath.h"
#include "SDS20Struct-2017.h"
#include "quote-1017-lxl.h"

template<int Step>
struct SDS20TrdKLine;

class TrdToKLine
{
	using   ValueType = SDS20TrdKLine<1>;
	typedef list<ValueType>			Trd_list;
	typedef map<int64_t, Trd_list>  M_List;
public:
	TrdToKLine() {
		_handle = 0;
		_date = GetCurrentDate();
		_shm_name = "TRD" + _date;
	}

	TrdToKLine(const string& date):_date(date){
		_handle = 0;
		_shm_name = "TRD" + _date;
	}

	~TrdToKLine(){

	}
public:
	//@获取当前日期
	static string GetCurrentDate();
	//@获取当前时间戳(单位:微秒)
	static int64_t GetCurrentTime();
public:
	//@初始化共享内存用于读;且返回可操作的句柄
	bool InitShmAndGetHandle();
	//@读取共享内存;并计算分钟线
	void ReadShmAndGetKline();
	//@计算分钟线,且清除缓存
	void  CalculateKLine();
private:

	string KlineToStr(KLine& kline);

	bool WriteToFile(KLine& kline);

	inline void TrdToKline(KLine& kline, int64_t ukey, list<ValueType>& value);
private:
	int				_handle;
	mutex			_mutex;
	string			_shm_name;
	string			_date;
	M_List			_container;
};

template<int Step>
struct SDS20TrdKLine:public SDS20TRANSACTION
{
	SDS20TrdKLine() :belong(0) {
		memset((void*)this, 0, sizeof(SDS20TrdKLine));
	}
	void SetBelong() {
		auto hour = this->nTime / 10000000;
		auto minu = (this->nTime % 10000000) / 100000;
		minu = minu - minu % Step;
		belong = hour * 10000000 + minu * 100000;
	}
	int GetBelong()
	{
		return belong;
	}
private:
	int belong;
};

#endif // !__KLINE_H__


#ifndef __KLINEDATADEF_H__
#define __KLINEDATADEF_H__

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
#include "SDS20Struct-2017.h"
#include "quote-1017-lxl.h"


enum KLineType
{
	TRD = 0,
	LVT,
	CFE,
	SPI,
	IDX,
};

#pragma  pack(push)
#pragma pack(1)
template<typename T, int  _step>
class KLineValue :public T
{
public:
	KLineValue() {
		belong = 0;
		memset((void*)this, 0, sizeof(KLineValue));
	}

	void SetBelong() {
		auto hour = this->nTime / 10000000;
		auto minu = (this->nTime % 10000000) / 100000;
		minu = minu - minu % _step;
		if (minu == 60 - _step)//hour+1;跳过小时
		{
			minu = 0;
			hour += 1;
			belong = hour * 10000000 + minu * 100000;
			return;
		}
		if (hour == 9 && minu < 30)
		{
			minu = 30 + _step;
			belong = hour * 10000000 + minu * 100000;
			return;
		}
		if (hour == 11 && minu >= 30)
		{
			minu = 30;
			belong = hour * 10000000 + minu * 100000;
			return;
		}
		if (hour > 12 && hour < 13)
		{
			hour = 13;
			minu = _step;
			belong = hour * 10000000 + minu * 100000;
			return;
		}
		if (hour == 15)
		{
			minu = 0;
			belong = hour * 10000000 + minu * 100000;
			return;
		}
		belong = hour * 10000000 + (minu + _step) * 100000;
	}

	int64_t GetBelong() {
		return belong;
	}

private:
	int64_t		belong;
};

template<int  _step>
class KLineValue<SDS20FUTURE, _step> :public SDS20FUTURE
{
public:
	KLineValue() {
		belong = 0;
		memset((void*)this, 0, sizeof(KLineValue));
	}

	void SetBelong() {
		auto hour = this->nTime / 10000000;
		auto minu = (this->nTime % 10000000) / 100000;
		minu = minu - minu % _step;
		if (minu == 60 - _step)//hour+1;跳过小时
		{
			minu = 0;
			hour += 1;
			belong = hour * 10000000 + minu * 100000;
			return;
		}
		if ((hour == 9 && minu < _step) || (hour <= 8 && hour > 1))
		{
			minu = _step;
			hour = 9;
			belong = hour * 10000000 + minu * 100000;
			return;
		}
		if ((hour == 21 && minu < _step) || (hour < 21 && hour > 19))
		{
			minu = _step;
			hour = 21;
			belong = hour * 10000000 + minu * 100000;
			return;
		}
		if (hour == 11 && minu >= 30)
		{
			minu = 30;
			belong = hour * 10000000 + minu * 100000;
			return;
		}
		if (hour > 12 && hour < 13)
		{
			hour = 13;
			minu = _step;
			belong = hour * 10000000 + minu * 100000;
			return;
		}
		if (hour == 15)
		{
			minu = 0;
			belong = hour * 10000000 + minu * 100000;
			return;
		}
		belong = hour * 10000000 + (minu + _step) * 100000;
	}

	int64_t GetBelong() {
		return belong;
	}

private:
	int64_t		belong;
};

#pragma pack(pop)


#endif //!__KLINEDATADEF_H__
	
#ifndef __HONGKING_QUOTE_H
#define __HONGKING_QUOTE_H
#include <stdint.h>
#pragma pack(push)
#pragma pack(1)

namespace chronos
{


//
//本文档涉及价格((a double number + 0.00005) * 10000) 成交量挂单量单位股票 股 基金债券 份 期货期权 手
//Snapshot 可能有多种变种,(1) 1档,5档,10档.(2) 是否有orderqueue. 
//h5方式存储历史数据时 国内股票Snapshot带委托队列 同时存下orderqueue

struct Snapshot	//成交量单位股票 股 基金债券 份 期货期权 手
{
    int64_t		  ukey;				        //丽海弘金ukey码，可以取出大类型、市场id
    int32_t     trday;              //交易日20170301
    int64_t     timeus;             //UNIX时间戳（格林威治时间）1970年1月1日0时0分0秒到现在的微秒数
    int64_t     recvus;             //UNIX时间戳（格林威治时间）1970年1月1日0时0分0秒到现在的微秒数
    int32_t     status;             //状态值
    int64_t     pre_close;          //昨收 
    int64_t     high;               //最高
    int64_t     low;                //最低
    int64_t     open;             	//开盘
    int64_t     last;             	//最新价
    int64_t     match_num;          //成交笔数
    int64_t		  volume;             //累计成交量
    int64_t     turnover;           //累计成交额(元)
    int64_t     info[10];           //非标准数据,一个大类型一套数据,10个为限有数据字典

    //行情存储结构以下肯定会有 发送行情根据不同等级发送
    //档位信息 根据不同等级的行情 分为1挡  5挡 10挡，10档有委托队列信息
    int64_t		  ask_price[10];		  //叫卖价
    int64_t     ask_volume[10];     //叫卖量
    int64_t     bid_price[10];      //叫买价
    int64_t     bid_volume[10];     //叫买量

    int64_t     ask_orders_num;     //卖一档位上订单个数
    int64_t     bid_orders_num;     //买一档位上订单个数
    int64_t     ask_queue[50];      //卖挂单详情 0结束
    int64_t     bid_queue[50];      //买挂单详情 0结束
};

//逐笔成交
struct Transaction
{
    int64_t     ukey;               //丽海弘金ukey码，可以取出大类型、市场id
    int32_t     trday;              //交易日20170301
    int64_t     timeus;             //UNIX时间戳（格林威治时间）1970年1月1日0时0分0秒到现在的微秒数
    int64_t     recvus;             //UNIX时间戳（格林威治时间）1970年1月1日0时0分0秒到现在的微秒数
    int64_t     index;              //成交编号
    int64_t		  price;              //成交价格
    int64_t     volume;             //成交数量
    int64_t     ask_order;          //叫卖方委托序号
    int64_t     bid_order;          //叫买方委托序号
    int32_t     trade_type; 
};

//逐笔订单
struct Order
{
    int64_t		  ukey;               //丽海弘金ukey码，可以取出大类型、市场id
    int32_t     trday;              //交易日20170301
    int64_t     timeus;             //UNIX时间戳（格林威治时间）1970年1月1日0时0分0秒到现在的微秒数
    int64_t     recvus;             //UNIX时间戳（格林威治时间）1970年1月1日0时0分0秒到现在的微秒数
    int64_t     index;              //委托号
    int64_t     price;              //委托价格
    int64_t     volume;             //委托数量
    int32_t     order_type;
};

struct OrderQueue
{
    int64_t		  ukey;				        //丽海弘金ukey码，可以取出大类型、市场id
    int32_t     trday;              //交易日20170301
    int64_t     timeus;             //UNIX时间戳（格林威治时间）1970年1月1日0时0分0秒到现在的微秒数
    int64_t     recvus;             //UNIX时间戳（格林威治时间）1970年1月1日0时0分0秒到现在的微秒数
    int32_t     side;
    int64_t     price;
    int64_t     orders_num;         //档位上订单个数
    int64_t     queue[50];
};

struct DailyClear
{
  int64_t   ukey;                   //uk编码
  int32_t   trday;                  //交易日
  int64_t   timeus;                 //UNIX时间戳（格林威治时间）1970年1月1日0时0分0秒到现在的微秒数
  int64_t   upper_limit;            //涨停价格 *10000
  int64_t   lower_limit;            //跌停价格 *10000
  int64_t   pre_settlement;         //昨结算   *10000
  int64_t   pre_close;              //昨收盘   *10000
  int64_t   pre_interest;           //昨持仓   期货 手
  int64_t   open;                   //开盘价   *10000
  int64_t   high;                   //最高价   *10000
  int64_t   low;                    //最低价   *10000
  int64_t   close;                  //最新价   收盘后是收盘价 *10000
  int64_t   settlement;             //结算价   *10000
  int64_t   volume;                 //成交量   股数 期货 手
  int64_t   turnover;               //成交金额 元
  int64_t   interest;               //持仓总量 期货 手
  int32_t   match_num;              //成交笔数
};

struct KLine
{
    int64_t ukey;                   // UKEY码
    int32_t trday;                  // 交易日20170301
    int64_t timeus;                 // UNIX时间戳（格林威治时间）1970年1月1日0时0分0秒到现在的微秒数
    int64_t open;                   // 开盘价
    int64_t high;                   // 最高价
    int64_t low;                    // 最低价
    int64_t close;                  // 收盘价
    int64_t volume;                 // 成交量
    int64_t turnover;               // 成交额   
    int32_t match_num;              // 成交笔数   
    int64_t interest;               // 持仓量(期货)、IOPV(基金)、利息(债券)      
};

/* 
info[10] 匹配格式
股票 ukey majortype=1
int64_t upper_limit;                        //涨停价格
int64_t lower_limit;                        //跌停价格
int64_t	total_bid_volume;                   //委托买入总量 股
int64_t total_ask_volume;                   //委托卖出总量 股
int64_t weighted_avg_bid_price;				      //加权平均委买价格 *10000
int64_t weighted_avg_ask_price;             //加权平均委卖价格 *10000
int64_t iopv;                               //IOPV净值估值
int64_t yield_to_maturity;                  //利息(债券) nYieldToMaturity到期收益率
int64_t ttm;                                //市盈率1 
int64_t lyr;                                //市盈率2
基金 majortype=2
int64_t upper_limit;                        //涨停价格
int64_t lower_limit;                        //跌停价格
int64_t total_bid_volume;                   //委托买入总量 股
int64_t total_ask_volume;                   //委托卖出总量 股
int64_t weighted_avg_bid_price;            	//加权平均委买价格 *10000
int64_t weighted_avg_ask_price;            	//加权平均委卖价格 *10000
int64_t iopv;                               //IOPV净值估值
int64_t yield_to_maturity;                  //利息(债券) nYieldToMaturity到期收益率

债券 majortype=3
int64_t upper_limit;             // 涨停价格
int64_t lower_limit;             // 跌停价格
int64_t total_bid_volume;                   //委托买入总量 股
int64_t total_ask_volume;                   //委托卖出总量 股
int64_t weighted_avg_bid_price;            	//加权平均委买价格 *10000
int64_t weighted_avg_ask_price;            	//加权平均委卖价格 *10000
int64_t iopv;                               //IOPV净值估值
int64_t yield_to_maturity;                  //利息(债券) nYieldToMaturity到期收益率

指数 majortype=6
int64_t index;                     			//不加权指数
int64_t stocks;                    			//品种总数
int64_t ups;                       			//上涨品种数
int64_t downs;                     			//下跌品种数
int64_t hold_lines;                			//持平品种数

期货 majortype=10 期权 majortype=11
int64_t upperlimit;             //涨停价格
int64_t lowerlimit;             //跌停价格
int64_t	settle;                 //结算价((a double number + 0.00005) *10000)
int64_t pre_settle;             //昨结算((a double number + 0.00005) *10000)
int64_t position;               //持仓量
int64_t pre_position;           //昨持仓
int64_t delta;                  //虚实度
int64_t pre_delta;              //昨虚实度
*/

/*
Transaction::trade_type, Order::order_type的组成
    char        sys_flag;               //系统标记.用于附加处理的,表明这个订单是否是内部订单
    char        bs_flag;                //买卖方向(买：'B', 卖：'S', 不明：' ')
    char        order_kind;             //成交类别
    char        function_code;          //成交代码
*/
inline char get_sys_flag(int type)
{
    return (reinterpret_cast<char*>(&type))[3];
}
inline char get_bs_flag(int type)
{
    return (reinterpret_cast<char*>(&type))[2];
}
inline char get_order_kind(int type)
{
    return (reinterpret_cast<char*>(&type))[1];
}
inline char get_function_code(int type)
{
    return *(reinterpret_cast<char*>(&type));
}

inline int make_trade_type(char sys_flag, char bs_flag, char order_kind, char function_code)
{
    int type;
    auto p = reinterpret_cast<char*>(&type);
    *p = function_code;
    *++p = order_kind;
    *++p = bs_flag;
    *++p = sys_flag;
    return type;
}
// 不区分是否内部订单时
inline int make_trade_type(char bs_flag, char order_kind, char function_code)
{
    return make_trade_type('\0', bs_flag, order_kind, function_code);
}

inline int make_order_type(char sys_flag, char order_kind, char function_code)
{
    return make_trade_type(sys_flag, '\0', order_kind, function_code);
}
// 不区分是否内部订单时
inline int make_order_type(char order_kind, char function_code)
{
    return make_trade_type('\0', '\0', order_kind, function_code);
}

inline void get_variety_market_by_ukey(int64_t ukey, int &variety, int &market)
{
	variety = static_cast<int>((ukey & (0x0F << 16)) >> 16);
	market =  static_cast<int>((ukey & (0x03FF << 20)) >> 20);
}

}   // namespace hk_quote

#pragma pack(pop)

#endif	// __HONGKING_QUOTE_H

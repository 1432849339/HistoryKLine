#ifndef _SDS20_STRUCT_2017_H_
#define _SDS20_STRUCT_2017_H_
#pragma  pack(push)
#pragma pack(1)

#ifndef LPVOID
typedef void* LPVOID;
#endif

#ifndef THANDLE
typedef void* THANDLE;
#endif

#ifndef __int64
#define __int64 long long
#endif

//数据类型编码
enum SDS20DATATYPE
{
	HHCODE = 100,				//宏汇股票代码表数据类型编码
	HHLEVEL2,					//宏汇level2数据类型编码 
	HHTRANS,					//宏汇transaction数据类型编码 
	HHORDER,					//宏汇order数据类型编码 
	HHORDERQUEUE,				//宏汇orderqueu数据类型编码 
	HHINDEX,					//宏汇index数据类型编码
	HHFUTURE,					//宏汇future(期权 期货)数据类型编码
	INCODE = 110,				//内部股票代码表数据类型编码
	INLEVEL2,					//内部level2数据类型编码 
	INTRANS,					//内部transaction数据类型编码 
	INORDER,					//内部order数据类型编码 
	INORDERQUEUE,				//内部orderqueu数据类型编码 
	ININDEX,					//内部index数据类型编码
	INFUTURE,                   //内部future(期权 期货)数据类型编码
	//TIMEINDEX = 900,            //内部future(期权 期货)数据类型编码
};

//code对应结构
struct SDS20CODE
{
	char szWindCode[32];    //Wind Code: AG1302.SHF
	char szMarket[8];       //market code: SHF
	char szCode[32];        //original code:ag1302
	char szENName[32];
	char szCNName[32];      //chinese name: 沪银1302
	int nType;
};

//期权code对应结构
struct SDS20OPTIONCODE
{
	SDS20CODE basicCode;

	char szContractID[32];// 期权合约代码
	char szUnderlyingSecurityID[32];//// 标的证券代码
	char chCallOrPut;               // 认购认沽C1        认购，则本字段为“C”；若为认沽，则本字段为“P”
	int  nExerciseDate;             // 期权行权日，YYYYMMDD    
	//扩充字段
	char chUnderlyingType;			// 标的证券类型C3    0-A股 1-ETF (EBS – ETF， ASH – A 股)
	char chOptionType;              // 欧式美式C1        若为欧式期权，则本字段为“E”；若为美式期权，则本字段为“A”

	char chPriceLimitType;          // 涨跌幅限制类型C1 ‘N’表示有涨跌幅限制类型, ‘R’表示无涨跌幅限制类型
	int  nContractMultiplierUnit;	// 合约单位,         经过除权除息调整后的合约单位, 一定是整数
	int  nExercisePrice;            // 期权行权价,       经过除权除息调整后的期权行权价，右对齐，精确到厘
	int  nStartDate;                // 期权首个交易日,YYYYMMDD
	int  nEndDate;                  // 期权最后交易日/行权日，YYYYMMDD
	int  nExpireDate;               // 期权到期日，YYYYMMDD
};

//market对应结构
struct SDS20LEVEL2
{
	char         szWindCode[32];        //sh.600001 
	int          nActionDay;            //业务发生日(自然日)
	int			 nTime;					//交易所时间(HHMMSSmmm)
	int			 nStatus;				//状态
	unsigned int nPreClose;				//前收盘价
	unsigned int nOpen;					//开盘价
	unsigned int nHigh;					//最高价
	unsigned int nLow;					//最低价
	unsigned int nMatch;				//最新价
	unsigned int nAskPrice[10];			//申卖价
	unsigned int nAskVol[10];			//申卖量
	unsigned int nBidPrice[10];			//申买价
	unsigned int nBidVol[10];			//申买量
	unsigned int nNumTrades;			//成交笔数
	__int64		 iVolume;				//成交总量
	__int64		 iTurnover;				//成交总金额
	__int64		 nTotalBidVol;			//委托买入总量
	__int64		 nTotalAskVol;			//委托卖出总量
	unsigned int nWeightedAvgBidPrice;	//加权平均委买价格
	unsigned int nWeightedAvgAskPrice;  //加权平均委卖价格
	int			 nIOPV;					//IOPV净值估值
	int			 nYieldToMaturity;		//到期收益率
	unsigned int nHighLimited;			//涨停价
	unsigned int nLowLimited;			//跌停价
	char		 chPrefix[4];			//证券信息前缀
	int			 nSyl1;					//市盈率1
	int			 nSyl2;					//市盈率2
	int			 nSD2;					//升跌2（对比上一笔）
	int			 nRecvTime;				//收到时间(HHMMSSmmm)
	unsigned long long	 nSn;					//收到序号
};

//transaction对应结构
struct SDS20TRANSACTION
{
	char			szWindCode[32];             //sh.600001 
	int				nActionDay;                 //自然日
	int 			nTime;		                //成交时间(HHMMSSmmm)
	int 			nIndex;		                //成交编号
	int				nPrice;		                //成交价格
	int 			nVolume;	                //成交数量
	int				nTurnover;	                //成交金额
	int				nBSFlag;                    //买卖方向(买：'B', 卖：'S', 不明：' ')
	char			chOrderKind;                //成交类别
	char			chFunctionCode;             //成交代码
	int				nAskOrder;	                //叫卖方委托序号
	int				nBidOrder;	                //叫买方委托序号
	int				nRecvTime;				    //收到时间(HHMMSSmmm)
	unsigned long long		nSn;						//收到序号
};

//order对应结构
struct SDS20ORDER
{
	char			szWindCode[32]; //sh.600001 
	int 			nActionDay;	    //委托日期(YYMMDD)
	int 			nTime;			//委托时间(HHMMSSmmm)
	int 			nOrder;	        //委托号
	int				nPrice;			//委托价格
	int 			nVolume;		//委托数量
	char			chOrderKind;	//委托类别
	char			chFunctionCode;	//委托代码('B','S','C')
	int				nRecvTime;		//收到时间(HHMMSSmmm)
	unsigned long long		nSn;			//收到序号
};

//orderqueue对应结构
struct SDS20ORDERQUEUE
{
	char			szWindCode[32]; //sh.600001 
	int				nActionDay;     //自然日
	int 			nTime;			//时间(HHMMSSmmm)
	int				nSide;			//买卖方向('B':Bid 'A':Ask)
	int				nPrice;			//委托价格
	int 			nOrders;		//订单数量
	int 			nABItems;		//明细个数
	int 			nABVolume[50];	//订单明细
	int				nRecvTime;		//收到时间(HHMMSSmmm)
	unsigned long long		nSn;			//收到序号
};

//index对应结构
struct SDS20INDEX
{
	char			szWindCode[32];         //600001.SH
	int				nActionDay;             //业务发生日(自然日)
	int				nTime;			        //时间(HHMMSSmmm)
	int				nOpenIndex;		        //今开盘指数
	int 			nHighIndex;		        //最高指数
	int 			nLowIndex;		        //最低指数
	int 			nLastIndex;		        //最新指数
	__int64			iTotalVolume;	        //参与计算相应指数的交易数量
	__int64			iTurnover;		        //参与计算相应指数的成交金额
	int				nPreCloseIndex;	        //前盘指数
	int				nRecvTime;				//收到时间(HHMMSSmmm)
	unsigned long long		nSn;					//收到序号
};

//future对应结构
struct SDS20FUTURE
{
	char			szWindCode[32];         //期货IF1510.CF 期权10000034.SH 8位 
	int				nActionDay;             //业务发生日(自然日)
	int				nTime;					//时间(HHMMSSmmm)	
	int				nStatus;				//状态
	__int64			iPreOpenInterest;		//昨持仓
	unsigned int	nPreClose;				//昨收盘价
	unsigned int	nPreSettlePrice;		//昨结算
	unsigned int	nOpen;					//开盘价	
	unsigned int	nHigh;					//最高价
	unsigned int	nLow;					//最低价
	unsigned int	nMatch;					//最新价
	__int64			iVolume;				//成交总量
	__int64			iTurnover;				//成交总金额
	__int64			iOpenInterest;			//持仓总量
	unsigned int	nClose;					//今收盘
	unsigned int	nSettlePrice;			//今结算
	unsigned int	nHighLimited;			//涨停价
	unsigned int	nLowLimited;			//跌停价
	int				nPreDelta;			    //昨虚实度
	int				nCurrDelta;				//今虚实度
	unsigned int	nAskPrice[5];			//申卖价
	unsigned int	nAskVol[5];				//申卖量
	unsigned int	nBidPrice[5];			//申买价
	unsigned int	nBidVol[5];				//申买量
	//Add 20140605
	int	nAuctionPrice;						//波动性中断参考价
	int	nAuctionQty;						//波动性中断集合竞价虚拟匹配量	
	int nAvgPrice;							//郑商所期货均价
	int				nRecvTime;				//收到时间(HHMMSSmmm)
	unsigned long long		nSn;					//收到序号
};

// add 20161202
// 增加一个内存块记录时间股票当前数据块条数
//记录内存块
struct SDS20RECORD
{
	char        szWindCode[32];         //期货IF1510.CF 期权10000034.SH 8位 为避免未知情况暂定32char
	int         nActionDay;             //业务发生日(自然日)
	int			nExTime;				//时间(HHMMSSmmm)
	int			nIndex;					//每个数据类型顺序
	int			nRecvTime;				//收到的时间
	int			nType;					//数据类型
};

#pragma pack(pop)
#endif

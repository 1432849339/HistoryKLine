#ifndef _SHM_20150725_H__
#define _SHM_20150725_H__
// 主目录的ID. 
#define MAINDIR_NUM 1000
#define MAINUSR_NUM 100000
// --------------------------------------------
// 主目录的结构: 
// (1) MAINHEAD  主目录的头
// (2) MAINDIR   主目录表项 (1000个)
// (3) MAINUSR   主目录日志 (100000个)
// --------------------------------------------
struct MAINHEAD { // 只有一条记录。
	int maindirnum; // 主目录目前的数量
	int maindirnum_max; // 最大主目录数量
	int mainusernum; // 主目录日志类型
	int mainusernum_max; // 主目录日志最大量
	char owner[32];  // 所有者,创建者实际就是所有者。 
	char group[32];  // 所有者所在的组，
	int createtm;    // 创建的时间,秒.
};

struct MAINDIR { // maintype是关键字。
	char maintype[32]; // 目录类型名称.
	int  shmid;    // 相关的shmid, linux实际的shmid是 shmkey*100+blockno(从1开始). windows是shm_[shmkey]_[blockno从1开始]
	int  recnum;   // 目前已经记载的记录数。（有效记录数) 
	int  recmax;   // 目前可记载的最大记录。 (最大可记载的记录数，物理的,根据blocknum增加会增加。). 
	int  blocknum; // 目前有的块个数，
	int  stat;     // 状态: 0：非正常， 1：正常。
	int  shmtype;  //  0:不存在， 1：空表静态数据， 2：普通队列。 3： 循环队列(循环队列只会有一个块，不能有多个). 
	int  headp;  // 循环队列用，头指针。 物理地址
	int  tailp;  // 循环队列用, 尾指正。 物理地址
	int  headno; // 循环队列用，头记录号.
	int  tailno; // 循环队列用，尾记录号.
	// ------------------------------
	int  signature; // 控制的PID. 0表示为空,无控制。如果非本pid的进程写，是不会通过的。
	int  createtm;  // 创建的时间
	int  updatetm;  // 更改的时间
	// ------------------------------
	int  bytes_per_record; // 一个记录的字节数。
	int  recnum_per_block; // 一个数据块的记录个数。
};

// 目录将循环使用. 
struct MAINUSR{ // maintype+pid是关键字。
	char  maintype[32]; // 类别.
	int   pid; // 使用者的PID.
	char  appname[32]; // 用户的appname.
	int   usertype;  // 使用者登记类型,写还是读.
	int   checkintm; // 使用者登记的时间.
	int   checkouttm; // 使用者登出的时间.
	char  g_owner[32];
	char  g_group[32];
};
#ifdef OS_WIN
struct MAINDATA{
	struct MAINHEAD head;
	struct MAINDIR  mdir[MAINDIR_NUM];
	struct MAINUSR  musr[MAINUSR_NUM];
};
#else
struct MAINDATA{
	MAINHEAD head;
	MAINDIR  mdir[MAINDIR_NUM];
	MAINUSR  musr[MAINUSR_NUM];
};
#endif
// 本地数据结构 //
struct LOCALDATA {
	char  maintype[32]; // ID
	int   stat;       // 打开之读写类型. 0:空，未打开。 1：读取打开， 2：读写打开。
	int   maindiridx;   // 相关的目录序号;
	int   mainusridx;   // 相关用户登记序号;
	int   shmtype; //  0:不存在， 1：空表静态数据， 2：普通队列。 3： 循环队列。 
	char  *pshm;        // 目前的指针。 
	int   blockidx;     // 当前指针指向的blockidx. 
	int   unix_shmid;   // UNIX下的共享内存handle
};
//
extern "C"{
	int shmInit(int mainshmid, char * appname);  // 初始化系统， 必须首先被调用，否则什么也做不了。0:ok, other:errcode
	int shmHandle(char * psharetype); // 把类型名转成handle. 需要在本地已经调用shmOpen. 0:err, >0 shmhandle
	int shmStat(int shmhandle); // 查询状态,返回： 0:不能访问， 不存在， 或全局未初始化。 1.可读， 2：可写。 
	int shmType(int shmhandle); // 返回当前类型: 0:不存在， 1：空表静态数据， 2：普通队列。 3： 循环队列。
	int shmRecnum(int shmhandle); // 返回目前的记录总数。 0：表示没有任何一条记录， 或非法句柄， 或无访问权限。 >0 , 目前的记录数。 
	int shmAppend(int shmhandle, char * pdata); // 追加一条记录,返回记录号， <0 表示错误。>0表示当前append的记录号。
	int shmRead(int shmhandle, char * pdata, int recno); // 取得当前记录下的数据。  <0错误。 >0表示读取的记录号
	int shmOpenForRead(char * psharetype); // 打开一个类型的数据， mode='r' 表示读取打开。 mode='w'表示写打开。 
	int shmClose(char * psharetype); // 关闭一个数据类型。 （关闭的本地访问权， 并未关闭shm中的数据。 
	int shmCreateForWrite(char * maintype, int shmid, int shmtype, int bytes_per_record, int recnum_per_block);
	int shmError(char * pmsg); // 取得上一次的错误信息和错误码。 pmsg为char[200];可以传NULL,则只取得错误编号。
	int shmVersion(); // 取得当前版本号. 101表示1.01，
	int shmReadMore(int shmhandle, char * pdata, int recno, int recnum); // 读取多条记录,在shmRead上增加了记录数recnum， 系统会自动处理分块的事情
	int shmAppendMore(int shmhandle, char * pdata, int recnum); // 追加多条记录，在shmAppend上增加了recnum,系统会自动处理分块的事情。
	int shmWrite(int shmhandle, char * pdata, int recno);  // 修改记录。需要shmCreateForWrite权限。
}
//
#endif
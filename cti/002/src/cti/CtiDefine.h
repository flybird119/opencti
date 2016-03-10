//opencti：它是一个通用的CTI开发库，支持东进、三汇等厂商的语音卡、交换机等产品。
//网址：http://www.opencti.cn
//QQ群：21596142
//联系：opencti@qq.com
//
// This file is a part of the opencti Library.
// The use and distribution terms for this software are covered by the
// Common Public License 1.0 (http://opensource.org/licenses/cpl.php)
// which can be found in the file CPL.txt at this distribution. By using
// this software in any fashion, you are agreeing to be bound by the terms
// of this license. You must not remove this notice, or any other, from
// this software.


/********************************************************************
	created:	2009/04/12
	created:	12:4:2009   11:30
	filename: 	e:\project\opencti.code\trunk\002\src\cti\CtiDefine.h
	file path:	e:\project\opencti.code\trunk\002\src\cti
	file base:	CtiDefine
	file ext:	h
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/

/// \file cticonst.h
/// \brief opencti事件和参数

#pragma once

#include <windows.h>
#include <tchar.h>


#pragma warning(disable:4996)


/// \defgroup EventandParam 事件和参数 
/// \brief 
/// \details 
/// \{ 







/// \defgroup EventParam 事件参数 
/// \brief 
/// \details 
/// \{ 






enum CHANNELSTATE;
enum CHANNELTYPE;
enum CHANNELWAY;

/// \brief 增加通道信息
/// \see \link CHANNELEVENTTYPE::EVENT_ADDCHANNEL EVENT_ADDCHANNEL \endlink  
typedef struct AddChannelInfotag
{
    CHANNELTYPE    chtype;	///< 通道类型 参见::CHANNELTYPE
    CHANNELSTATE   chstate; ///< 通道状态 参见::CHANNELSTATE
    CHANNELWAY     chway;   ///< 通道方向 参见::CHANNELWAY
	UINT		   chtotal;	///< 系统的通道总数

} ADDCHANNELPARAM,*LPADDCHANNELPARAM;


/// \brief 移除通道信息
/// \see \link CHANNELEVENTTYPE::EVENT_DELCHANNEL EVENT_DELCHANNEL \endlink  
typedef struct DelChannelInfotag
{
	DWORD reason;  ///< 原因

} DELCHANNELPARAM,*LPDELCHANNELPARAM;


/// \brief 通道释放信息
/// \see \link CHANNELEVENTTYPE::EVENT_RELEASE EVENT_RELEASAE \endlink  
typedef struct ReleaseInfotag
{

} RELEASEPARAM,*LPRELEASEPARAM;



/// \brief 通道空闲信息
/// \see \link CHANNELEVENTTYPE::EVENT_IDLE EVENT_IDLE \endlink  
typedef struct IdleInfotag
{

} IDLEPARAM,*LPIDLEPARAM;



/// \brief 通道不可能信息
/// \see \link CHANNELEVENTTYPE::EVENT_UNUSABLE EVENT_UNUSABLE \endlink  
typedef struct UnusableInfotag
{
	

} UNUSABLEPARAM,*LPUNUSABLEPARAM;



/// \brief 通道状态信息
/// \see \link CHANNELEVENTTYPE::EVENT_STATE EVENT_STATE \endlink  
typedef struct StateInfotag
{
	LPCTSTR statetext; ///< 通道状态的文本描述。

} STATEPARAM,*LPSTATEPARAM;



/// \brief 号码信息
typedef struct CallerInfotag
{
    TCHAR called[128];		///< 被叫
    TCHAR calling[128];		///< 主叫
    TCHAR origcalled[128];	///< 原始被叫
} CALLERINFO,*LPCALLERINFO;


/// \brief 呼入信息
/// \see \link CHANNELEVENTTYPE::EVENT_CALLIN EVENT_CALLIN \endlink  
typedef  CallerInfotag CALLINPARAM,*LPCALLINPARAM;


/// \brief 拔号信息
/// \see \link CHANNELEVENTTYPE::EVENT_DIALUP EVENT_DIALUP \endlink  
typedef CallerInfotag DIALUPPARAM,*LPDIALUPPARAM;

/// \brief 回铃信息
/// \see \link CHANNELEVENTTYPE::EVENT_RINGBACK EVENT_RINGBACK \endlink  
typedef struct RingbackInfotag
{

} RINGBACKPARAM,*LPRINGBACKPARAM;

/// \brief 振铃信息
/// \see \link CHANNELEVENTTYPE::EVENT_RINGING EVENT_RINGING \endlink  
typedef struct RingingInfotag
{

} RINGINGPARAM,*LPRINGINGPARAM;


/// \brief 接通信息
/// \see \link CHANNELEVENTTYPE::EVENT_TALKING EVENT_TALKING \endlink  
typedef struct TalkingInfotag
{

} TALKINGPARAM,*LPTALKINGPARAM;


enum RELEASEATTRIBUTE;
/// \brief 挂机信息
/// \see \link CHANNELEVENTTYPE::EVENT_HANGUP EVENT_HANGUP \endlink 
typedef struct HangInfotag
{
    RELEASEATTRIBUTE reason;			///< 挂机原因，参见::RELEASEATTRIBUTE

} HANGUPPARAM , *LPHANGUPPARAM;


/// \brief 接收到DTMF
/// \see \link CHANNELEVENTTYPE::EVENT_RECVDTMF EVENT_RECVDTMF \endlink 
typedef struct RecvDtmftag
{
    int     result;         ///< 0:超时  1:接收到DTMF 2:停止
	TCHAR   dtmf;			///< 接收到的DTMF字符
} RECVDTMFPARAM,*LPRECVDTMFPARAM;



/// \brief 发送DTMF完成
/// \see \link CHANNELEVENTTYPE::EVENT_SENDDTMF EVENT_SENDDTMF \endlink 
typedef struct SendDtmfInfotag
{
    int     result;          ///< 0:失败 1:完成 2:停止

} SENDDTMFPARAM,*LPSENDDTMFPARAM;


/// \brief 发送FSK完成
/// \see \link CHANNELEVENTTYPE::EVENT_SENDFSK EVENT_SENDFSK \endlink 
typedef struct SendFskInfotag
{
    int     result;          ///< 0:失败 1:完成 2:停止
} SENDFSKPARAM,*LPSENDFSKPARAM;


/// \brief 接收到FSK
/// \see \link CHANNELEVENTTYPE::EVENT_RECVFSK EVENT_RECVFSK \endlink 
typedef struct RecvFskInfotag
{
    int    result;          ///< 0:超时  1:接收到FSK(还有后续数据) 2:停止 3:接收到FSK数据(没有后续数据)
    int    size;			///< 接收到FSK数据的缓冲大小
    BYTE   buf;				///< 接收到FSK数据的缓冲首地针
} RECVFSKPARAM,*LPRECVFSKPARAM;



/// \brief 接收到信号音
/// \see \link CHANNELEVENTTYPE::EVENT_RECVTONE EVENT_RECVTONE \endlink 
typedef struct RecvToneInfotag
{
    int     result;          ///< 0:接收超时  1:接收到TONE
	DWORD   tone;            ///< 接收到的信号时类型，参见:TONEATTRIBUTE
} RECVTONEPARAM,*LPRECVTONEPARAM;


/// \brief 信号音发送完成
/// \see \link CHANNELEVENTTYPE::EVENT_SENDTONE EVENT_SENDTONE \endlink 
typedef struct SendToneInfotag
{
    int     result;          ///< 0:失败 1:超时 2:停止
	DWORD   tone;            ///< 信号音类型，参见::TONEATTRIBUTE
} SENDTONEPARAM,*LPSENDTONEPARAM;



enum PLAYATTRIBUTE;
/// \brief 放音完成
/// \see \link CHANNELEVENTTYPE::EVENT_PLAY EVENT_PLAY \endlink 
typedef struct PlayInfotag
{
    int             result;     ///< 0:失败  1:完成 2:停止 3:DTMF打断
	PLAYATTRIBUTE   attribute;  ///< 放音类型 参见::PLAYATTRIBUTE
    TCHAR           dtmf;       ///< 打断放音的DTMF
} PLAYPARAM,*LPPLAYPARAM;



/// \brief 内存录音放音缓冲区信息
typedef struct MemoryBlockbag
{
	int		index;		/// 0:block1有效，1:block2有效
	BYTE*	block1;		/// 缓冲区地址
	int		size1;		/// 绑冲区大小
	BYTE*	block2;		/// 缓冲区地址
	int		size2;		/// 绑冲区大小
} MEMORYBLOCK,*LPMEMORYBLOCK;
                             
/// \brief 内存录音缓冲区信息
/// \see \link CHANNELEVENTTYPE::EVENT_UPDATERECORDMEMORY EVENT_UPDATERECORDMEMORY \endlink 
typedef struct MemoryBlockbag UPDATERECORDMEMORYPARAM,*LPUPDATERECORDMEMORYPARAM;

/// \brief 内存放音缓冲区信息
/// \see \link CHANNELEVENTTYPE::EVENT_UPDATEPLAYMEMORY EVENT_UPDATEPLAYMEMORY \endlink 
typedef struct MemoryBlockbag UPDATEPLAYMEMORYPARAM,*LPUPDATEPLAYMEMORYPARAM;


enum RECORDATTRIBUTE;
/// \brief 录音完成
/// \see \link CHANNELEVENTTYPE::EVENT_RECORD EVENT_RECORD \endlink 
typedef struct RecordInfotag
{
    int               result;          /// 0:失败  1:完成 2:停止 3:DTMF打断
	RECORDATTRIBUTE   attribute;       /// 参见::RECORDATTRIBUTE
    TCHAR             dtmf;            ///< 打断放音的DTMF
} RECORDPARAM,*LPRECORDPARAM;

/// \brief 闪断完成
/// \see \link CHANNELEVENTTYPE::EVENT_SENDFLASH EVENT_SENDFLASH \endlink 
typedef struct SendFlashInfotag
{

} SENDFLASHPARAM,*LPSENDFLASHPARAM;

/// \brief 接收到闪断
/// \see \link CHANNELEVENTTYPE::EVENT_RECVFLASH EVENT_RECVFLASH \endlink 
typedef struct RecvFlashInfotag
{

} RECVFLASHPARAM,*LPRECVFLASHPARAM;




/// \brief 挂起
/// \see \link CHANNELEVENTTYPE::EVENT_SLEEP EVENT_SLEEP \endlink 
typedef struct SleepInfotag
{

} SLEEPPARAM,*LPSLEEPPARAM; 





/// \brief 本端闭塞事件信息
/// \see \link CHANNELEVENTTYPE::EVENT_LOCAL_BLOCK EVENT_LOCAL_BLOCK \endlink 
typedef struct LocalBlockInfotag
{
} LOCALBLOCKPARAM,*LPLOCALBLOCKPARAM;




/// \brief 远端闭塞事件信息
/// \see \link CHANNELEVENTTYPE::EVENT_REMOTE_BLOCK EVENT_REMOTE_BLOCK \endlink 
typedef struct RemoteBlockInfotag
{

} REMOTEBLOCKPARAM,*LPREMOTEBLOCKPARAM;

/// \brief 用户事件信息
/// \see \link CHANNELEVENTTYPE::EVENT_USEREVENT EVENT_USEREVENT \endlink 
typedef struct UserEventInfotag
{
    UINT  delayid;          ///< delayid
	BOOL  once;				///< 只产生一次
	PVOID eventdata;        ///< ::CTI_DelayChEvent传入的事件参数指针
	int eventsize;          ///< ::CTI_DelayChEvent传入的事件参数大小
} USEREVENTPARAM,*LPUSEREVENTPARAM;



/// \brief 系统事件信息
/// \see \link CHANNELEVENTTYPE::EVENT_SYSEVENT EVENT_SYSEVENT \endlink 
typedef struct SysEventInfotag
{
	PVOID eventdata;        ///< ::CTI_DelaySysEvent传入的事件参数指针
	int eventsize;          ///< ::CTI_DelaySysEvent传入的事件参数大小

} SYSEVENTPARAM,*LPSYSEVENTPARAM;


/// \} 

/// \defgroup EventType 事件类型
/// \brief 
/// \details 
/// \{ 


enum CHANNELEVENTTYPE
{
    EVENT_NONE,
	/// \brief 系统事件
	/// \par 触发条件:
	/// ::CTI_DelayedSysEvent
	/// \par 事件参数:
	/// ::SYSEVENTPARAM
	EVENT_SYSEVENT = 0x100,


	/// \brief 增加通道
	/// \par 触发条件:
	/// ::CTI_Init
	/// \par 事件参数:
	/// ::ADDCHANNELPARAM
    EVENT_ADDCHANNEL,     

	/// \brief 移除通道
	/// \par 触发条件:
	/// (通道移除)由设备触发
	/// \par 事件参数:
	/// ::DELCHANNELPARAM
    EVENT_DELCHANNEL,
    

    /// \brief 通道释放
    /// \par 触发条件:
    /// ::CTI_Hangup，::EVENT_HANGU
    /// \par 事件参数:
    /// ::CLEANPARAM
    EVENT_RELEASE,

	/// \brief 通道进入::STATE_IDLE状态
	/// \par 触发条件:
	/// (通道空闲)由设备触发
	/// \par 事件参数:
	/// ::IDLEPARAM
    EVENT_IDLE  =   0x200,

	/// \brief 通道进入::STATE_UNUSABLE状态
	/// \par 触发条件:
	/// (通道不可用)由设备触发
	/// \par 事件参数:
	/// ::UNUSABLEPARAM
    EVENT_UNUSABLE,

	/// \brief 通道内部状态改变
	/// \par 触发条件:
	/// 由设备触发
	/// \par 事件参数:
	/// ::STATEPARAM
	EVENT_STATE,

	/// \brief 通道进入::STATE_CALLIN状态
	/// \par 触发条件:
	/// (呼叫到达)由设备触发
	/// \par 事件参数:
	/// ::CALLINPARAM
	EVENT_CALLIN,

	/// \brief 通道进入::STATE_RINGING状态
	/// \par 触发条件:
	/// ::CTI_Ringback
	/// \par 事件参数:
	/// ::RINGINGPARAM
	EVENT_RINGING,

	/// \brief 通道进入::STATE_DIALUP状态
	/// \par 触发条件:
	/// ::CTI_Dialup
	/// \par 事件参数:
	/// ::DIALUPPARAM
    EVENT_DIALUP,

	/// \brief 通道进入::STATE_RINGBACK状态
	/// \par 触发条件:
	/// (回铃音)由设备触发
	/// \par 事件参数:
	/// ::RINGBACKPARAM
	EVENT_RINGBACK,

	/// \brief 通道进入::STATE_TALKING状态
	/// \par 触发条件:
	/// ::CTI_Pickup
	/// \par 事件参数:
	/// ::TALKINGPARAM
	EVENT_TALKING,

	/// \brief 通道进入::STATE_HANGUP状态
	/// \par 触发条件:
	/// (对端挂机)由设备触发
	/// \par 事件参数:
	/// ::HANGUPPARAM
	EVENT_HANGUP,

	/// \brief 本端闭塞(可以呼出，不能呼入)
    /// \par 触发条件
    /// (本端闭塞)由设备触发
    /// \par 事件参数
    /// ::LOCALBLOCKPARAM
    EVENT_LOCAL_BLOCK,

	/// \brief 远端闭塞(可以呼入，不能呼出)
    /// \par 触发条件
    /// (远端闭塞)由设备触发
    /// \par 事件参数
    /// ::REMOTEBLOCKPARAM
    EVENT_REMOTE_BLOCK,

	/// \brief 接收到DTMF
	/// \par 触发条件:
	/// ::CTI_RecvDtmf
	/// \par 事件参数:
	/// ::RECVDTMFPARAM
    EVENT_RECVDTMF  =   0x300,

	/// \brief 发送DTMF完成
	/// \par 触发条件:
	/// ::CTI_SendDtmf
	/// \par 事件参数:
	/// ::SENDDTMFPARAM
    EVENT_SENDDTMF,

	/// \brief 接收到FSK
	/// \par 触发条件:
	/// ::CTI_RecvFsk
	/// \par 事件参数:
	/// ::RECVFSKPARAM
    EVENT_RECVFSK,

	/// \brief 发送FSK完成
	/// \par 触发条件:
	/// ::CTI_SendFsk
	/// \par 事件参数:
	/// ::SENDFSKPARAM
    EVENT_SENDFSK,

	/// \brief 发送TONE完成
	/// \par 触发条件:
	/// ::CTI_RecvTone
	/// \par 事件参数:
	/// ::RECVTONEPARAM
    EVENT_RECVTONE,

	/// \brief 发送TONE完成
	/// \par 触发条件:
	/// ::CTI_SendTone
	/// \par 事件参数:
	/// ::SENDTONEPARAM
	EVENT_SENDTONE,

	/// \brief 放音完成
	/// \par 触发条件
	/// ::CTI_PlayFile,::CTI_PlayQueue
	/// \par 事件参数
	/// ::PLAYPARAM
	EVENT_PLAY,

	/// \brief 录音完成
	/// \par 触发条件
	/// ::CTI_RecordFile
	/// \par 事件参数
	/// ::RECORDPARAM
    EVENT_RECORD,

	/// \brief 内存放音缓冲区已放完
	/// \par 触发条件
	/// ::CTI_PlayMemory
	/// \par 事件参数
	/// ::UPDATEPLAYMEMORYPARAM
	EVENT_UPDATEPLAYMEMORY,

	/// \brief 内存录音缓冲区已录满
	/// \par 触发条件
	/// ::CTI_RecordMemory
	/// \par 事件参数
	/// ::UPDATERECORDMEMORYPARAM
	EVENT_UPDATERECORDMEMORY,

    /// \brief 发送闪断
    /// \par 触发条件
    /// ::CTI_SendFlash
    /// \par 事件参数
    /// ::SENDFLASHPARAM
    EVENT_SENDFLASH,            

    /// \brief 接收闪断
    /// \par 触发条件
    /// ::CTI_RecvFlash
    /// ::RECVFLASHPARAM
	EVENT_RECVFLASH,


    /// \brief 挂起
    /// \par 触发条件
    /// (挂起)由设备触发
    /// \par 事件参数
    /// ::SLEEPPARAM
    EVENT_SLEEP,



    /// \brief 用户事件
    /// \par 触发条件
    /// ::CTI_DelayedChEvent
    /// \par 事件参数
    /// ::USEREVENTPARAM
	EVENT_USEREVENT = 0x400,


    EVENT_TOTAL     = 0x1000,
};
/// \} 

/// \defgroup FUNCTIONPARAM 函数参数
/// \brief 
/// \details 
/// \{ 



/// \brief 事件描述
/// \see CTIEVENTPROC
typedef struct ChannelEventDescriptor
{
    int              chid;				///< 通道ID
	CHANNELEVENTTYPE eventid;			///< 事件类型 参见:: CHANNELEVENTTYPE 
    LPVOID           eventdata;			///< 事件参数指针
    int              eventsize;			///< 事件参数大小
    LPARAM           notifyparam;		///< 回调函数参数 
	DWORD            chownerdata;		///< 通道关联数据 参见::CTI_SetChOwnerData
#if _WIN32_WINNT<0x0600 || WINVER<0x0600
	DWORD            time;				///< 事件产生的时间
#else
	ULONGLONG        time;				///< 事件产生的时间
#endif
    ChannelEventDescriptor()
    {
    }

    ChannelEventDescriptor(int ch,CHANNELEVENTTYPE type,LPVOID data,int size,LPARAM param,DWORD owner):
        chid(ch),
        eventid(type),
        eventdata(data),
        eventsize(size),
        notifyparam(param),
        chownerdata(owner),
#if _WIN32_WINNT<0x0600 || WINVER<0x0600
        time(::GetTickCount())
#else
		time(::GetTickCount64())
#endif
    {

    }
   
} CHANNELEVENT,*LPCHANNELEVENT;


static LPCTSTR EventDescription(CHANNELEVENTTYPE eventid)
{
    if(eventid>=EVENT_USEREVENT)
    {
        static TCHAR EventName[256];
        _stprintf(EventName,_T("EVENT_USEREVENT+%d"),eventid-EVENT_USEREVENT);
        return EventName;
    }

    switch(eventid)
    {
    case EVENT_SYSEVENT:    return _T("EVENT_SYSEVENT");
    case EVENT_ADDCHANNEL:  return _T("EVENT_ADDCHANNEL");
    case EVENT_DELCHANNEL:  return _T("EVENT_DELCHANNEL");
    case EVENT_RELEASE:     return _T("EVENT_RELEASE");
    case EVENT_IDLE:        return _T("EVENT_IDLE");
    case EVENT_TALKING:     return _T("EVENT_TALKING");
    case EVENT_HANGUP:      return _T("EVENT_HANGUP");
    case EVENT_RECVDTMF:    return _T("EVENT_RECVDTMF");
    case EVENT_SENDDTMF:    return _T("EVENT_SENDDTMF");
    case EVENT_RECVFSK:     return _T("EVENT_RECVFSK");
    case EVENT_SENDFSK:     return _T("EVENT_SENDFSK");
    case EVENT_PLAY:        return _T("EVENT_PLAY");
    case EVENT_RECORD:      return _T("EVENT_RECORD");
    case EVENT_RECVFLASH:   return _T("EVENT_RECVFLASH");
    case EVENT_SENDFLASH:	return _T("EVENT_SENDFLASH");
    case EVENT_UNUSABLE:    return _T("EVENT_UNUSABLE");
    case EVENT_STATE:		return _T("EVENT_STATE");
    case EVENT_CALLIN:      return _T("EVENT_CALLIN");
    case EVENT_RINGING:     return _T("EVENT_RINGING");
    case EVENT_DIALUP:      return _T("EVENT_DIALUP");
    case EVENT_RINGBACK:    return _T("EVENT_RINGBACK");
    case EVENT_RECVTONE:    return _T("EVENT_RECVTONE");
    case EVENT_SENDTONE:    return _T("EVENT_SENDTONE");
    case EVENT_LOCAL_BLOCK: return _T("EVENT_LOCAL_BLOCK");
    case EVENT_REMOTE_BLOCK:return _T("EVENT_REMOTE_BLOCK");
    case EVENT_UPDATEPLAYMEMORY:return _T("EVENT_UPDATEPLAYMEMORY");
    case EVENT_UPDATERECORDMEMORY:return _T("EVENT_UPDATERECORDMEMORY");
    case EVENT_SLEEP:       return _T("EVENT_SLEEP");
    case EVENT_NONE:        return _T("EVENT_NONE");

    }

    return _T("EVENT_UNKNOWN");
}








typedef void (WINAPI *CTIEVENTPROC)(LPCHANNELEVENT lpchevent);


/// \brief 设备描述
/// \see ::CTI_Init
struct DeviceDescriptor
{
    HWND         monitorwnd;        ///< 通道监视窗口
    CTIEVENTPROC notifyfun;			///< 事件通知回调函数地址
    LPARAM       notifyparam;		///< 事件通知回调函数参数

    int          threadmode;        ///< 线程模式 0：1个线程处理64通道事件； 1：1个线程处理1个通道事件；其它值：系统默认线程模式。

};

/// \brief 通道类型
enum CHANNELTYPE
{
    TYPE_UNKNOWN		=	0x00,	///< 未知
    TYPE_ANALOG_TRUNK	=	0x01,	///< 模拟中继
    TYPE_ANALOG_USER	=	0x02,	///< 模拟用户
    TYPE_ANALOG_RECORD	=	0x04,	///< 模拟录音
    TYPE_ANALOG_EMPTY	=	0x08,	///< 模拟无模块
    TYPE_SS7_TUP		=	0x10,	///< SS7TUP
    TYPE_SS7_ISUP		=	0x20,	///< SS7ISUP
    TYPE_DSS1_USER		=	0x40,	///< ISDN用户侧
    TYPE_DSS1_NET		=	0x80,	///< ISDN网络侧
    TYPE_SS1			=	0x100,	///< SS1通道
	TYPE_FAX			=	0x200,	///< 传真

};

static LPCTSTR TypeDescription(CHANNELTYPE type)
{
    switch(type)
    {
    case TYPE_UNKNOWN:          return _T("未知");
    case TYPE_ANALOG_EMPTY:     return _T("模拟空闲");
    case TYPE_ANALOG_RECORD:    return _T("模拟录音");
    case TYPE_ANALOG_TRUNK:     return _T("模拟中继");
    case TYPE_ANALOG_USER:      return _T("模拟用户");
    case TYPE_SS7_ISUP:         return _T("SS7ISUP");
    case TYPE_SS7_TUP:          return _T("SS7TUP");
    case TYPE_DSS1_NET:         return _T("ISDN网络侧");
    case TYPE_DSS1_USER:        return _T("ISDN用户侧");
    case TYPE_SS1:              return _T("SS1");
    case TYPE_FAX:              return _T("传真");
    default:                    return _T("ERROR");
    }
    return _T("ERROR"); 
}

/// \brief 通道状态
enum  CHANNELSTATE
{
    STATE_UNKNOWN,		///< 未知
    STATE_INITIAL, 		///< 初始化
    STATE_UNUSABLE,		///< 不可用
    STATE_IDLE,			///< 空闲
    STATE_DIALUP,		///< 拔号
    STATE_CALLIN,		///< 呼入
    STATE_RINGBACK,		///< 回铃
    STATE_RINGING,		///< 振铃
    STATE_TALKING,		///< 通话
    STATE_HANGUP,		///< 挂断
	STATE_RELEASE,		///< 释放
    STATE_LOCAL_BLOCK,	///< 本端阻塞
    STATE_REMOTE_BLOCK, ///< 远端阻塞
    STATE_SLEEP,        ///< 挂起
};
static LPCTSTR StateDescription(CHANNELSTATE state )
{
    switch(state)
    {
    case STATE_UNKNOWN:         return _T("未知");
    case STATE_INITIAL:         return _T("初始化");
    case STATE_UNUSABLE:        return _T("不可用");
    case STATE_IDLE:            return _T("空闲");
    case STATE_DIALUP:          return _T("呼出");
    case STATE_CALLIN:          return _T("呼入");
    case STATE_RINGING:         return _T("振铃");
    case STATE_RINGBACK:        return _T("回铃");
    case STATE_TALKING:         return _T("通话");
    case STATE_HANGUP:          return _T("挂断");
	case STATE_RELEASE:			return _T("释放");
    case STATE_LOCAL_BLOCK:     return _T("本地阻塞");
    case STATE_REMOTE_BLOCK:    return _T("远端阻塞");
    case STATE_SLEEP:           return _T("挂起");
    default:                    return _T("ERROR");
    }
}


/// \brief 通道呼叫方向
enum CHANNELWAY
{
    WAY_UNKNOWN,	///< 未知
    WAY_IDLE,		///< 空闲
    WAY_CALLIN,		///< 呼入
    WAY_CALLOUT,	///< 呼出
};
static LPCTSTR WayDescription(CHANNELWAY way)
{
    switch(way)
    {
    case WAY_UNKNOWN:           return _T("未知");
    case WAY_IDLE:              return _T("空闲");
    case WAY_CALLIN:            return _T("呼入");
    case WAY_CALLOUT:           return _T("呼出");
    default:                    return _T("ERROR");
    }
}
/// \brief 拔号标志或应答标志
/// \see ::CTI_Dialup
/// \note 对于模拟外线通过信号音来检测呼叫结果和发现忙音触发挂机事件
enum CHECKFLAG
{
    CHECKFLAG_NONE       =   0x00,		///< 未指号
	CHECKFLAG_CHECKTALK  =   0x01,		///< 检测到说话声，发送 ::EVENT_TALKING 事件。
    CHECKFLAG_CHECKHANG  =   0x02,		///< 检测到忙音，发送 ::EVENT_HANGUP 事件。
};


/// \brief 拔号号码性质
/// \see ::CTI_Dialup
enum NUMBERATTRIBUTE
{
    NUMBER_NONE,		 ///< 未指定
    NUMBER_CITY,		 ///< 市话
    NUMBER_COUNTRY,		 ///< 长途
    NUMBER_INTERNATIONAL,///< 国际
};


/// \brief 释放原因
/// \see ::CTI_Hangup
enum RELEASEATTRIBUTE
{
	RELEASE_NONE,	   ///< 未指定
    RELEASE_NORMAL,    ///< 正常
    RELEASE_USERBUSY,  ///< 用户忙
    RELEASE_VACANT,    ///< 空号
    RELEASE_REJECTED,  ///< 拒绝
    RELEASE_NOANSWER,  ///< 无应答
    RELEASE_LINEFAULT, ///< 线路故障
    RELEASE_UNUSABLE,  ///< 不可用
    
};

static LPCTSTR ReleaseAttributeDescription(RELEASEATTRIBUTE releaseattribute)
{
	switch(releaseattribute)
	{
	case RELEASE_NORMAL:    return _T("正常");
	case RELEASE_USERBUSY:  return _T("用户忙");
	case RELEASE_VACANT:    return _T("空号");
	case RELEASE_REJECTED:  return _T("拒绝");
	case RELEASE_NOANSWER:  return _T("无应答");
	case RELEASE_LINEFAULT: return _T("线路故障");
	case RELEASE_UNUSABLE:  return _T("不可用");
	default:				return _T("未知");
	}

	return _T("\0");
}


#define STOPSOUNDCHARSETALL       _T("0123456789*#ABCD")
#define STOPSOUNDCHARSETSIMPLE    _T("0123456789*#")
#define STOPSOUNDCHARSETNONE      _T("")



/*
1、振铃信号：用来呼叫被叫用户。铃流为25±3Hz正弦波，谐波失真不大于10％，输出电压有效值90+-15V，振铃采用5s断续，即一秒送，4s断，断、续时间偏差不超过±10％。
2、拨号音－：用来通知主叫用户可以拨号。拨号音采用频率为450±25Hz的交流电源，发送电平为-10±3dBm，是连续的信号音。
3、回铃音－：表示被叫用户处于被振铃状态，采用频率为450±25Hz的交流电源，发送电平为-10±3dBm，它是5s断续的信号音，即1s送，4s断，与振铃音一致。
4、忙音－：表示本次接续遇到机线忙或被叫用户忙，采用频率为450±25Hz的交流电源，发送电平为-10±3dBm，它是0.7断续的信号音，即0.35送，0.35断。久叫不应90s后送忙音。
5、长途通知音－：用于通知正在进行市内通话的用户有长途电话，采用频率为450±25Hz的交流电源，发送电平为-20±3dBm，它是1.2s不等间隔断续的信号音，即0.2s送，0.2s断，0.2s送，0.6断。
6、空号音：用于通知主叫用户，所呼叫的被叫号码为空号或受限制的号码，采用频率为450±25Hz的交流电源，发送电平为-10±3dBm，它是1.4s不等间隔断续的信号音，即重复三次0.1s送、0.1s断后，0.4s送，0.4断，步进制电话局遇空号用忙音代替空号音。
7、排队等待音－：用于具有排队性能的接续，以通知主叫用户等待应答，采用频率450±25Hz的交流电源，发送电平为-10±3dBm，需要时可用回铃音或用录音通知，不另设专用信号音。
8、证实音：它是由立去台话务员自发自收的信号，用于核实主叫用户号码的正确性，采用频率为950±50Hz的交流电源，发送电平为-20±3dBm，发连续信号音。
9、催挂音＋：它是由测量台发送给久不挂机用户的信号，其目的是通知用户挂机，采用频率950±50Hz的交流电源，发送电平为0～25dBm，发送连续信号音，响度变化分五级，由最低级逐步升高。
上述各种信号中，除铃流外，根据信号音种类，信号音源为450±25Hz或950±50Hz，谐波失真不大于10％；各种信号音断、续偏差分别不得超过10%。

1.特种拨号音：是一种“嘟、嘟……、嘟、嘟……”的一短一长的声音（响40毫秒，停40毫秒），频率是450赫。当您的电话登记了某种新业务功能后，您拿起听筒听到的拨号音就是这种特殊拨号音。它用以提醒您，但并不妨碍打电话。 
2.拥塞音：是一种“嘟、嘟”的短音（响0.7秒，间隔0.7秒），频率是450赫。它有点像忙音，但比忙音长，表示程控交换机因某种原因机线拥塞不通。 
3.空号音：是一种“嘟、嘟、嘟、嘟……”的三短一长的声音（短音持续0.1秒，长音持续0.4秒），频率450赫，表示您拨叫的电话号码是尚未使用的空号。
4.催挂音：是一种频率为950赫的连续音，采用五级响度，声音由小逐渐变大。当您用完电话没把听筒搁回话机或听筒没有搁好时，话机发出催挂音，提醒您把听筒搁好。
5.忙音：是一种“嘟、嘟、嘟、嘟”的短促音（响0.35秒，断0.35秒），表示局内机线被占用，或您拨叫的电话正在使用。


*/

/// \brief 信号音类型
/// \see ::CTI_SendTone,::CTI_RecvTone
enum TONEATTRIBUTE
{
	TONE_NONE		=	0x00,	///< 未指定
    TONE_SILENCE	=	0x01,   ///< 静音
    TONE_DIAL		=	0x02,   ///< 拨号音
    TONE_RINGBACK	=	0x04,	///< 回铃音
    TONE_BUSY		=	0x08,   ///< 忙音
    TONE_BLOCK		=	0x10,   ///< 拥塞音
    TONE_CUSTOM     =	0x20	///< 自定义
};

/// \brief 放音方式
enum PLAYATTRIBUTE
{
	PLAY_NONE,		///< 未指定
    PLAY_FILE,      ///< 播放文件
    PLAY_MEMORY,    ///< 播放内存
    PLAY_QUEUE,     ///< 播放列队
    PLAY_INDEX,     ///< 播放索引
};

/// \brief 录音方式
enum RECORDATTRIBUTE
{
	RECORD_NONE,	///< 未指定
	RECORD_FILE,	///< 录音到文件
	RECORD_MEMORY,  ///< 录音到内存
};


/// \brief 函数索引
enum FUNCTIONADDRINDEX
{
    Addr_None               = 0,
    Addr_Init               = 1,
    Addr_Term               = 2,

    Addr_GetChType			= 10,
    Addr_SetChOwnerData     = 11,
    Addr_GetChOwnerData     = 12,
	Addr_GetChState			= 13,

    Addr_GetChTotal         = 20,
    Addr_SearchIdleCh       = 21,
    Addr_LoadPlayIndex      = 22,
    Addr_Features           = 23,
    Addr_GetNativeAPI       = 24,
    Addr_GetLastErrorCode   = 25,
    Addr_FormatErrorCode    = 26,


    Addr_Ringback           = 40,
    Addr_Pickup             = 41,
    Addr_Dialup             = 42,
    Addr_Hangup             = 43,


    Addr_SendDtmf           = 50,
    Addr_StopSendDtmf       = 51,
    Addr_IsSendDtmf         = 52,

    Addr_RecvDtmf           = 60,
    Addr_StopRecvDtmf       = 61,
    Addr_IsRecvDtmf         = 62,


    Addr_SendFsk            = 70,
    Addr_SendFskFull        = 71,
    Addr_StopSendFsk        = 72,
    Addr_IsSendFsk          = 73,

    Addr_RecvFsk            = 80,
    Addr_StopRecvFsk        = 81,
    Addr_IsRecvFsk          = 82,


    Addr_SendTone           = 90,
    Addr_SendToneEx         = 91,
    Addr_StopSendTone       = 92,
    Addr_IsSendTone         = 93,

    Addr_RecvTone           = 100,
    Addr_StopRecvTone       = 101,
    Addr_IsRecvTone         = 102,

    Addr_PlayFile           = 110,
    Addr_PlayMemory         = 111,
    Addr_PlayQueue          = 112,
    Addr_PlayIndex          = 114,
    Addr_StopPlay           = 115,
    Addr_IsPlay             = 116,
    Addr_UpDatePlayMemory   = 117,


    Addr_RecordFile         = 130,
    Addr_RecordMemory       = 131,
    Addr_StopRecord         = 132,
    Addr_IsRecord           = 133,
    Addr_UpDateRecordMemory = 134,

    Addr_SendFlash          = 140,
    Addr_RecvFlash          = 141,
    Addr_StopRecvFlash      = 142,
    Addr_IsRecvFlash        = 143,



    Addr_Listen             = 160,
    Addr_UnListen           = 161,


    Addr_DelayChEvent       = 170,
    Addr_DelaySysEvent      = 171,
	Addr_CancelDelay	    = 172,
	Addr_BindExclusiveThread= 173,
	Addr_UnBindExclusiveThread=174,
    Addr_GetSelectCh        = 175,

	Addr_InsertChEvent      = 176,

	Addr_PauseCh			= 177,
	Addr_ResumeCh		    = 178,



    Addr_Total              = 400,
};

/// \} 

/// \} 






#pragma warning(default:4996)
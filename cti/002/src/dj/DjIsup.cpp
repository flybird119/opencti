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
	created:	2009/06/14
	created:	14:6:2009   11:07
	filename: 	e:\project\cti\002\src\dj\CDjIsup.cpp
	file path:	e:\project\cti\002\src\dj
	file base:	CDjIsup
	file ext:	cpp
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "DjIsup.h"


#include "log/LogMessage.h"

#include "./DjInterface.h"



#include "ISUPDll.h"
#pragma comment(lib,"TCISUP.lib")



//#define LOG_INIT(...)
//#define TraceLog(...)
//#define EventLog(...)
//#define LEVEL_SENSITIVE
#define TraceLog(LEVEL,...)    TRACE_FULL("CDjIsup",LEVEL)<<__VA_ARGS__
#define EventLog(LEVEL,...)    TRACE_FULL("CDjIsup",LEVEL)<<__VA_ARGS__



#define CH					   ch/30,ch%30





CDjIsup::CDjIsup(CMessageQueue* pMessageQueue,CDjInterface* pdjinterface)
:CDjDigital(pMessageQueue,pdjinterface)
{
	LOG_INIT("CDjIsup",TRUE,LEVEL_SENSITIVE,100*1024*1024);
}


CDjIsup::~CDjIsup()
{
	TraceLog(LEVEL_WARNING,"CDjIsup 释构");
}


LRESULT CDjIsup::OnInitial(MessageData* pdata,BOOL& bHandle)
{

	for(int i=0;i<m_totalline;++i)
	{
		DjIsupChannelInfo*pch=new DjIsupChannelInfo(m_pdjinterface->GetCh(this,i));
		m_channelmgr.AddChannel(pch);
		pch->Reset();
		pch->SetType(CDjIsup::GetChType(i));
		CHANNELTYPE chtype=pch->GetType();
		CHANNELSTATE chstate=STATE_INITIAL;
		CHANNELWAY chway=WAY_UNKNOWN;


		pch->internalstate=::DJISUP_GetChnState(i/30,i%30);
		if(pch->internalstate==ISUP_CH_FREE)
		{
			chstate=STATE_IDLE;
			chway=WAY_IDLE;
		}
		NotifyAddChannel(pch,chtype,chstate,chway,m_pdjinterface->GetChTotal());
	}
	DelayMessage(100,FALSE,EVT_POLL,NULL);
	return 0;
}


LRESULT CDjIsup::OnPoll(MessageData* pdata,BOOL& bHandle)
{
	::DJISUP_GetEvent() ;
	for(int i=0;i<m_totalline;++i)
	{
		DjIsupChannelInfo* pch=GetChInfo(i);
		PollJunction(i,pch);
		CDjDigital::Poll(i,pch);
	}

	bHandle=FALSE;
	return 0;
}


void   CDjIsup::PollJunction(int ch,DjIsupChannelInfo* pch)
{

	switch(pch->GetState())
	{
	case STATE_DIALUP:
		{
			int callstate=::DJISUP_GetCalloutState(CH);

			if(callstate&(CALLOUT_DUAL_SEIZURE|CALLOUT_REV_BLO|CALLOUT_REV_RSC|CALLOUT_REV_ACM|CALLOUT_REV_CPG|CALLOUT_REV_UNREASONABLE))
			{
				TraceLog(LEVEL_INFO,__FUNCTION__<<" 通道:"<<ch<<" 呼叫结果:"<<CallStateToDescription(callstate));

				/*
				if(callstate&CALLOUT_DUAL_SEIZURE)//同抢
				{

				}
				else if(callstate&CALLOUT_REV_BLO)//收到电路锁闭
				{

				}
				else if(callstate&CALLOUT_REV_RSC)//收到电路复原
				{

				}
				else if(callstate&CALLOUT_REV_ACM)//收到地址全
				{

				}
				else if(callstate&CALLOUT_REV_CPG)//收到呼叫进展
				{

				}
				else if(callstate&CALLOUT_REV_UNREASONABLE)//收到不合理的消息
				{

				}
				*/
			}


		}
		break;
	}


	int newstate=::DJISUP_GetChnState(CH);

	if(newstate!=pch->internalstate)
	{
		int oldstate=pch->internalstate;
		pch->internalstate=newstate;
		NotifyState(pch,StateToText(newstate));
		TraceLog(LEVEL_INFO,__FUNCTION__<<" 通道:"<<ch<<" 状态变化:"<<ChStateToDescription(newstate,oldstate));


		switch(newstate)
		{
		case ISUP_CH_FREE:
				NotifyIdle(pch);
			break;
		case ISUP_CH_WAIT_RELEASE:
			{
				int reasion=::DJISUP_GetReleaseReason(CH);

				
				::DJISUP_SetChnState(CH ,ISUP_CH_SET_FREE);
				

				if(reasion!=RSN_LOCAL_RLC && pch->IsCallStates())
					NotifyHangup(pch,ConvHanupReasion(reasion));

			}
			break;
		case ISUP_CH_LOCAL_BLOCK:

			break;
		case ISUP_CH_REMOTE_BLOCK:

			break;
		case ISUP_CH_LOCAL_AND_REMOTE_BLOCK:

			break;
		case ISUP_CH_UNAVIABLE:
				NotifyUnusable(pch);
			break;
		case ISUP_CALLER_WAIT_ANM:
			{
				NotifyRingback(pch);
			}
			break;
		case ISUP_CALLER_CONNECT:
				_ASSERT(pch->GetWay()==WAY_CALLOUT);
				NotifyTalking(pch);
			break;
		case ISUP_CALLEE_RECV_IAM:
			{
				char called[128]="\0";
				char calling[128]="\0";
				char origcalled[128]="\0";
				::DJISUP_GetCalleeNum(CH,called);	//被叫
				::DJISUP_GetCallerNum(CH,calling);	//主叫
				::DJISUP_GetOraCalleeNum(CH,origcalled);//原始主叫


#if defined(_UNICODE) || defined(UNICODE)
				TCHAR wcalled[128]=_T("\0");
				TCHAR wcalling[128]=_T("\0");
				TCHAR worigcalled[128]=_T("\0");
				COMMON::ASCIIChar acharcalling(calling);
				_tcsncpy(wcalling,acharcalling,128-1);
				COMMON::ASCIIChar acharcalled(called);
				_tcsncpy(wcalled,acharcalled,128-1);
				COMMON::ASCIIChar acharorigcalled(origcalled);
				_tcsncpy(worigcalled,acharorigcalled,128-1);
				NotifyCallin(pch,wcalling,wcalled,worigcalled);

#else
				NotifyCallin(pch,calling,called,origcalled);
#endif
			}
			break;
		case ISUP_CALLEE_CONNECT:
				_ASSERT(pch->GetWay()==WAY_CALLIN);
				NotifyTalking(pch);
			break;
		case ISUP_CH_LOCAL_SUSPEND:

			break;
		case ISUP_CH_REMOTE_SUSPEND:

			break;
		case ISUP_CH_CON_CONNECT:

			break;
		case ISUP_CH_CON_NOTCON:

			break;	

			
		}
	}
}





DjIsupChannelInfo* CDjIsup::GetChInfo(int ch)
{
	return (DjIsupChannelInfo*)m_channelmgr.FindChannel(m_pdjinterface->GetCh(this,ch));
}




int CDjIsup::Init(DeviceDescriptor* pctidevicedescriptor)
{

	if(	::DJISUP_InitSystem() == -1 ) 
	{
		TraceLog(LEVEL_WARNING,"Initialize No.7 Driver Fail!") ;
		return 0;
	}


	if(CDjDigital::Init(pctidevicedescriptor))
	{
		CMessageHandler::DelayMessage(0,TRUE,EVT_INITIAL,0);
		return 1;
	}		
	else
	{
		::DJISUP_ExitSystem();
	}
	return 0;
}


void CDjIsup::Term()
{
	if(m_totalline>0)
	{
		CDjDigital::Term();
		::DJISUP_ExitSystem();
		m_totalline=0;
	}
	m_channelmgr.ClearChannel();
}

CHANNELTYPE CDjIsup::GetChType(int ch)
{
	return TYPE_SS7_ISUP;
}


int CDjIsup::Dialup(int ch,LPCTSTR calling,NUMBERATTRIBUTE callingattribute,LPCTSTR called,LPCTSTR origcalled,NUMBERATTRIBUTE origcalledattribute,BOOL CallerIDBlock,int flags)
{

	if(!(called && origcalled && origcalled))
	{
		m_lasterror.SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}

	const char *szcalling,*szcalled,*szorigcalled;

#if defined(UNICODE) || defined(_UNICODE)
	COMMON::UNICODEChar  wcalled(called);
	COMMON::UNICODEChar  wcalling(calling);
	COMMON::UNICODEChar  worigcalled(origcalled);

	szcalled=wcalled;
	szcalling=wcalling;
	szorigcalled=worigcalled;

#else
	szcalling=calling;
	szcalled=called;
	szorigcalled=origcalled;
#endif

	::DJISUP_SetOraCalleeNumber(CH,(char*)szorigcalled);
	int bRetrivl=::DJISUP_Callout(CH,(char*)szcalling,(char*)szcalled);
	if(bRetrivl)
	{
		NotifyDialup(GetChInfo(ch),calling,called,origcalled);
	}
	return bRetrivl;
}

int CDjIsup::Pickup(int ch,int flags)
{
	::DJISUP_SetChnState(CH, ISUP_CALLEE_SET_ANM );//send offhook signal
	return 1;
}

int CDjIsup::Ringback(int ch)
{
	int iret=0;
	DjIsupChannelInfo*pch=GetChInfo(ch);

	::DJISUP_SetChnState(CH, ISUP_CALLEE_SET_ACM );//send ack signal
	NotifyRinging(pch);

	return 1;
}

int CDjIsup::Hangup(int ch,RELEASEATTRIBUTE attribute)
{
	::DJISUP_SetChnState(CH, ISUP_CH_SET_REL );
	return 1;
}



















LPCTSTR CDjIsup::StateToText(int state)
{
	switch(state)
	{
		case ISUP_CH_FREE:						return _T("空闲状态");
		case ISUP_CH_WAIT_RELEASE:				return _T("等待应用层同步释放资源");
		case ISUP_CH_LOCAL_BLOCK:				return _T("本地锁闭状态");
		case ISUP_CH_REMOTE_BLOCK:				return _T("远端锁闭状态");
		case ISUP_CH_LOCAL_AND_REMOTE_BLOCK:	return _T("本地和远端锁闭状态");
		case ISUP_CH_UNAVIABLE:					return _T("不可用态(网络不通等原因导致)");
		case ISUP_CALLER_WAIT_ANM:				return _T("被叫闲等待被叫应答");
		case ISUP_CALLER_CONNECT:				return _T("被叫应答去话连通");
		case ISUP_CALLEE_RECV_IAM:				return _T("呼叫到达接收全地址");
		case ISUP_CALLEE_CONNECT:				return _T("摘机来话连通");
		case ISUP_CH_LOCAL_SUSPEND:				return _T("本地暂停");
		case ISUP_CH_REMOTE_SUSPEND:			return _T("远端暂停");
		case ISUP_CH_CON_CONNECT:				return _T("收到接续消息并已建立连接");
		case ISUP_CH_CON_NOTCON:				return _T("收到接续消息正在处理分段消息");		
		default:
			{
				static TCHAR buf[64];
				_stprintf(buf,_T("未知状态:%d"),state);
				return buf;
			}
	}

	return _T("\0");
}

LPCTSTR CDjIsup::ChStateToDescription(int newstate,int oldstate)
{


#if defined(UNICODE) || defined(_UNICODE)
	static std::wstring  DescriptionStr;
#else
	static std::string  DescriptionStr;
#endif

	DescriptionStr.erase(DescriptionStr.begin(),DescriptionStr.end());

	DescriptionStr.append(StateToText(oldstate));
	DescriptionStr.append(_T("to"));
	DescriptionStr.append(StateToText(newstate));


	return DescriptionStr.c_str();


}

LPCTSTR CDjIsup::CallStateToDescription(int callstate)
{

#if defined(UNICODE) || defined(_UNICODE)
	static std::wstring  DescriptionStr;
#else
	static std::string  DescriptionStr;
#endif

	DescriptionStr.erase(DescriptionStr.begin(),DescriptionStr.end());

	if(callstate&CALLOUT_DUAL_SEIZURE)//同抢
	{
		DescriptionStr.append(_T("同抢"));
	}
	else if(callstate&CALLOUT_REV_BLO)//收到电路锁闭
	{
		DescriptionStr.append(_T("收到电路锁闭"));
	}
	else if(callstate&CALLOUT_REV_RSC)//收到电路复原
	{
		DescriptionStr.append(_T("收到电路复原"));
	}
	else if(callstate&CALLOUT_REV_ACM)//收到地址全
	{
		DescriptionStr.append(_T("收到地址全"));
	}
	else if(callstate&CALLOUT_REV_CPG)//收到呼叫进展
	{
		DescriptionStr.append(_T("收到呼叫进展"));
	}
	else if(callstate&CALLOUT_REV_UNREASONABLE)//收到不合理的消息
	{
		DescriptionStr.append(_T("收到不合理的消息"));
	}
	return DescriptionStr.c_str();
}



RELEASEATTRIBUTE CDjIsup::ConvHanupReasion(int reasion)
{
	switch(reasion)
	{
	case RSN_UNALLOCATED_NUMBER:		//空号
	case RSN_INCOMPLETE_NUMBER:			//不完整的号码
		return RELEASE_VACANT;
	case RSN_USER_BUSY:					//用户忙
		return RELEASE_USERBUSY;
	case RSN_USER_ALERTING_NO_ANSWER:	//被叫无应答，超时未摘机
		return RELEASE_NOANSWER;
	case RSN_CALL_REJECTED:				//呼叫拒绝
		return RELEASE_REJECTED;
	case RSN_NORMAL_CALL_CLEARING:		//正常的呼叫清除
	case RSN_NORMAL_NO_SET:				//正常，未规定
		return RELEASE_NORMAL;
	case RSN_NO_USER_RESPONDING:		//无用户响应，超时没有收到ACM
	case RSN_NO_CIRCUIT_CHANNELAVAILABLE://无可用的电路
	case RSN_REQUESTEDCIRCUIT_CHANNEL_NOTAVAILABLE: //请求的电路不可用
	case RSN_RECOVERY_ON_TIME_EXPIRY:	//定时器超时恢复
	case RSN_RECEIVE_RSC:				//电路复原
		return RELEASE_LINEFAULT; 
	case RSN_LOCAL_RLC:					//本地释放电路
		return RELEASE_UNUSABLE;
	}
	return RELEASE_NONE;
}



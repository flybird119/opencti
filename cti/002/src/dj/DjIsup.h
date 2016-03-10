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
	filename: 	e:\project\cti\002\src\dj\CDjIsup.h
	file path:	e:\project\cti\002\src\dj
	file base:	CDjIsup
	file ext:	h
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/

#pragma once


#include "..\Cti\CtiBase.h"
#include "..\cti\CtiError.h"
#include "..\cti\CtiChannel.h"
#include "..\cti\CtiNotify.h"
#include ".\MessageQueue\MessageQueue.h"




#include ".\DjDigital.h"


extern COMMON::CLock  CTIAPILock;
#define BLOCK_()   LOCKSCOPE(CTIAPILock)




class DjIsupChannelInfo : public DjDigitalChannelInfo 
{
public:
	DjIsupChannelInfo(UINT chid)
		:DjDigitalChannelInfo(chid)
	{

	}
	int				internalstate;      //内部状态        

	virtual void Reset()
	{
		DjDigitalChannelInfo::Reset();
	}
};


class CDjInterface;

class CDjIsup : public CDjDigital
{
public:
	CDjIsup(CMessageQueue* pMessageQueue,CDjInterface* pdjinterface);
	~CDjIsup();


public:
	BEGIN_YSMESSAGE_MAP()
		BLOCK_();
		YSMESSAGE_HANDLER(EVT_INITIAL,OnInitial)
		YSMESSAGE_HANDLER(EVT_POLL,OnPoll)
		CHAIN_YSMESSAGE_MAP(CDjDigital)
	END_YSMESSAGE_MAP()


protected:
	LRESULT OnInitial(MessageData* pdata,BOOL& bHandle);
	LRESULT OnPoll(MessageData* pdata,BOOL& bHandle);
	void   PollJunction(int ch,DjIsupChannelInfo* pch);
	inline DjIsupChannelInfo* GetChInfo(int ch);

public:
	//系统服务函数
	virtual int Init(DeviceDescriptor* pctidevicedescriptor);
	virtual void Term();
	virtual BOOL Features(FUNCTIONADDRINDEX descriptor){return 0;};
	virtual void* GetNativeAPI(LPCTSTR funname){return 0;};
	virtual CHANNELTYPE GetChType(int ch);
	virtual int SearchIdleCh(CHANNELTYPE chtype, int* pChFilter, int FilterLen,int*pChExcept,int ExceptLen){return 0;};



	//接续函数
	virtual int Dialup(int ch,LPCTSTR calling,NUMBERATTRIBUTE callingattribute,LPCTSTR called,LPCTSTR origcalled,NUMBERATTRIBUTE origcalledattribute,BOOL CallerIDBlock,int flags);
	virtual int Pickup(int ch,int flags);
	virtual int Ringback(int ch);
	virtual int Hangup(int ch,RELEASEATTRIBUTE attribute);



private:
	LPCTSTR	StateToText(int state);
	LPCTSTR ChStateToDescription(int newstate,int oldstate);
	LPCTSTR CallStateToDescription(int callstate);


	RELEASEATTRIBUTE ConvHanupReasion(int reasion);
};
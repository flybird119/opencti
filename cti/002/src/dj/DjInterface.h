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
	created:	14:6:2009   11:39
	filename: 	e:\project\cti\002\src\dj\DjInterface.h
	file path:	e:\project\cti\002\src\dj
	file base:	DjInterface
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

#include ".\SwapInterface.h"

#include ".\DjIsup.h"
#include ".\DjAnalog.h"
#include ".\DjDigital.h"



#define EVT_BIGIN                (EVENT_TOTAL+100)



#define EVT_SIMULATETIMER        (EVT_BIGIN+0x0103)





class CDjInterface : public ICtiBase  , public CMessageHandler , public CtiNotify
{
public:
	
    CDjInterface(void);
    ~CDjInterface(void);

	BEGIN_YSMESSAGE_MAP()
		YSMESSAGE_HANDLER(EVT_SIMULATETIMER,OnSimulateTimer)
	END_YSMESSAGE_MAP()

public:
	friend CDjAnalog;
	friend CDjDigital;



private:
	LRESULT OnSimulateTimer(MessageData* pdata,BOOL& bHandle);


public:

	//系统服务函数
	virtual int Init(DeviceDescriptor* pctidevicedescriptor);
	virtual void Term();


	virtual BOOL Features(FUNCTIONADDRINDEX descriptor);
	virtual void* GetNativeAPI(LPCTSTR funname);

	virtual int GetLastErrorCode();
	virtual LPCTSTR FormatErrorCode(int code);

	virtual CHANNELTYPE GetChType(int ch);

	virtual int GetChTotal();

	virtual int SearchIdleCh(CHANNELTYPE chtype, int* pChFilter, int FilterLen,int*pChExcept,int ExceptLen);




	//接续函数

	virtual int  Dialup(int ch,LPCTSTR calling,NUMBERATTRIBUTE callingattribute,LPCTSTR called,LPCTSTR origcalled,NUMBERATTRIBUTE origcalledattribute,BOOL CallerIDBlock,int flags);
	virtual int  Pickup(int ch,int flags);

	virtual int  Ringback(int ch);
	virtual int  Hangup(int ch,RELEASEATTRIBUTE attribute);


	//DTMF函数

	virtual int  SendDtmf(int ch,LPCTSTR dtmf,int len,int flags);
	virtual int  StopSendDtmf(int ch);

	virtual int  RecvDtmf(int ch,int time,int flags);
	virtual int  StopRecvDtmf(int ch);



	//FSK函数

	virtual int  SendFsk(int ch,const BYTE* fsk,int len,int flags);
	virtual int  SendFskFull(int ch,int sync,int mark,const BYTE* fsk,int len,int flags);
	virtual int  StopSendFsk(int ch);

	virtual int  RecvFsk(int ch,int time,int flags);
	virtual int  StopRecvFsk(int ch);


	//放音函数

	virtual int PlayFile(int ch,LPCTSTR fullpath,LPCTSTR StopstrDtmfCharSet);
	virtual int PlayMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet);
	virtual int PlayQueue(int ch,LPCTSTR fullpath[],int size,LPCTSTR StopstrDtmfCharSet);

	virtual int LoadPlayIndex(LPCTSTR fullpath,LPCTSTR Alias);
	virtual int PlayIndex(int ch,LPCTSTR Alias,LPCTSTR StopstrDtmfCharSet);

	virtual int StopPlay(int ch);

	virtual int UpDatePlayMemory(int ch,int index,BYTE* block,int size);



	//录音函数
	virtual int RecordFile(int ch,LPCTSTR fullpath,int time,LPCTSTR StopstrDtmfCharSet);
	virtual int RecordMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet);
	virtual int StopRecord(int ch);

	virtual int UpDateRecordMemory(int ch,int index,BYTE* block,int size);




	//TONE函数

	virtual int SendTone(int ch,TONEATTRIBUTE attribute,int time,int flags);
	virtual int SendToneEx(int ch, int nFreq1, int nVolume1, int nFreq2, int nVolume2,DWORD dwOnTime, DWORD dwOffTime,int time,int flags);
	virtual int StopSendTone(int ch);

	virtual int RecvTone(int ch,TONEATTRIBUTE attribute,int time,int flags);
	virtual int StopRecvTone(int ch,TONEATTRIBUTE attribute);


	virtual int Listen(int initiativech,int passivelych);
	virtual int UnListen(int initiativech,int passivelych);




	virtual void  ResetCh(int ch);



public:

	int GetCti(int ch,ICtiBase** ppcti);
	int GetCh(ICtiBase* pcti,int origch);

	void UpDateRecvDtmf(DjChannelInfo*pch);


private:
	inline DjChannelInfo* GetChInfo(int ch);


private:

	struct ChannelCtiMap
	{
		ICtiBase*	pcti;
		int			offset;
		int			total;
	};



	std::vector<ChannelCtiMap>		m_ctimap;				//每个CTI设备中通道的起始编号

};

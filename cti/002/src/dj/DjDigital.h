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
	filename: 	e:\project\cti\002\src\dj\DjIsup.h
	file path:	e:\project\cti\002\src\dj
	file base:	DjIsup
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




#define EVT_BIGIN                (EVENT_TOTAL+100)

#define EVT_INITIAL              (EVT_BIGIN+0x0101)
#define EVT_POLL				 (EVT_BIGIN+0x0102)








class DjDigitalChannelInfo : public DjChannelInfo
{
public:
	DjDigitalChannelInfo(UINT chid)
		:DjChannelInfo(chid)
	{

	}

	struct 
	{
		TCHAR	StopPlayDtmfCharSet[20];
		int		StopPlayDtmfCharlen;
		BOOL	MtStop;
		TCHAR   StopPlayDtmfChar;
	} play;

	struct  
	{
		BOOL    MtStop; //标识，是否是手动停止
	} senddtmf;

	struct  
	{
		BOOL    MtStop; //标识，是否是手动停止
	} sendfsk;


	struct 
	{

		int         index;
		BYTE*       block1;
		int         size1;
		BYTE*       block2;
		int         size2;

	} playmemory;


	struct 
	{
		TCHAR	StopRecordDtmfCharSet[20];
		int		StopRecordDtmfCharlen;
		BOOL	MtStop;
		TCHAR   StopRecordDtmfChar;
	} record;

	struct 
	{

		int         index;
		BYTE*       block1;
		int         size1;
		BYTE*       block2;
		int         size2;

	} recordmemory;



	virtual void Reset()
	{
		DjChannelInfo::Reset();
	}
};


class CDjInterface;

class CDjDigital : public ICtiBase  , public CMessageHandler , public CtiNotify
{
public:
	CDjDigital(CMessageQueue* pMessageQueue,CDjInterface* pdjinterface);
	~CDjDigital();


public:
	BEGIN_YSMESSAGE_MAP()
		YSMESSAGE_HANDLER(EVT_POLL,OnPoll)
	END_YSMESSAGE_MAP()



protected:

	LRESULT OnPoll(MessageData* pdata,BOOL& bHandle);
	void    Poll(int ch,DjDigitalChannelInfo* pch);

	void	CheckDtmf(int ch,DjDigitalChannelInfo* pch);
	void	CheckFsk(int ch,DjDigitalChannelInfo* pch);
	void	CheckPlay(int ch,DjDigitalChannelInfo* pch);
	void	CheckRecord(int ch,DjDigitalChannelInfo* pch);
	void	CheckTone(int ch,DjDigitalChannelInfo* pch);
	void	CheckFlash(int ch,DjDigitalChannelInfo* pch);



	inline DjDigitalChannelInfo* GetChInfo(int ch);
	int FskCoder(int sync,int mark,const BYTE* fsk,int len,BYTE** coderbuf);

public:



	//系统服务函数
	virtual int Init(DeviceDescriptor* pctidevicedescriptor);
	virtual void Term();


	virtual BOOL Features(FUNCTIONADDRINDEX descriptor){return 0;};
	virtual void* GetNativeAPI(LPCTSTR funname){return 0;};

	virtual int GetLastErrorCode();
	virtual void SetLastErrorCode(int code){}
	virtual LPCTSTR FormatErrorCode(int code);


	virtual int GetChTotal();



	virtual int SetChOwnerData(int ch,DWORD OwnerData){return 0;};
	virtual DWORD GetChOwnerData(int ch){return 0;};

	virtual CHANNELSTATE GetChState(int ch){return STATE_UNKNOWN;}


	//DTMF函数

	virtual int  SendDtmf(int ch,LPCTSTR dtmf,int len,int flags);
	virtual int  StopSendDtmf(int ch);

	virtual int  IsSendDtmf(int ch){return 0;};


	virtual int  RecvDtmf(int ch,int time,int flags){return 0;};
	virtual int  StopRecvDtmf(int ch){return 0;};
	virtual int  IsRecvDtmf(int ch){return 0;};


	//FSK函数

	virtual int  SendFsk(int ch,const BYTE* fsk,int len,int flags);
	virtual int  SendFskFull(int ch,int sync,int mark,const BYTE* fsk,int len,int flags);
	virtual int  StopSendFsk(int ch);
	virtual int  IsSendFsk(int ch){return 0;};

	virtual int  RecvFsk(int ch,int time,int flags);
	virtual int  StopRecvFsk(int ch);
	virtual int  IsRecvFsk(int ch){return 0;};






	//放音函数

	virtual int PlayFile(int ch,LPCTSTR fullpath,LPCTSTR StopstrDtmfCharSet);
	virtual int PlayMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet);
	virtual int PlayQueue(int ch,LPCTSTR fullpath[],int size,LPCTSTR StopstrDtmfCharSet);
	virtual int LoadPlayIndex(LPCTSTR fullpath,LPCTSTR Alias);
	virtual int PlayIndex(int ch,LPCTSTR Alias,LPCTSTR StopstrDtmfCharSet);
	virtual int StopPlay(int ch);
	virtual int IsPlay(int ch){return 0;}
	virtual int UpDatePlayMemory(int ch,int index,BYTE* block,int size);



	//录音函数
	virtual int RecordFile(int ch,LPCTSTR fullpath,int time,LPCTSTR StopstrDtmfCharSet);
	virtual int RecordMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet);
	virtual int StopRecord(int ch);
	virtual int IsRecord(int ch){return 0;};
	virtual int UpDateRecordMemory(int ch,int index,BYTE* block,int size){return 0;};


	//Exchange总线
	virtual int Listen(int initiativech,int passivelych);
	virtual int UnListen(int initiativech,int passivelych);


	//TONE函数

	virtual int SendTone(int ch,TONEATTRIBUTE attribute,int time,int flags){return 0;};
	virtual int SendToneEx(int ch, int nFreq1, int nVolume1, int nFreq2, int nVolume2,DWORD dwOnTime, DWORD dwOffTime,int time,int flags){return 0;};
	virtual int StopSendTone(int ch){return 0;};
	virtual int IsSendTone(int ch){return 0;};

	virtual int  RecvTone(int ch,TONEATTRIBUTE attribute,int time,int flags){return 0;};
	virtual int  StopRecvTone(int ch,TONEATTRIBUTE attribute){return 0;};
	virtual int  IsRecvTone(int ch,TONEATTRIBUTE attribute){return 0;};


	//闪断
	virtual int SendFlash(int ch,int time){return 0;};
	virtual int RecvFlash(int ch){return 0;};
	virtual int StopRecvFlash(int ch){return 0;};
	virtual int IsRecvFlash(int ch){return 0;};





	//辅助
	virtual int DelayChEvent(int cmsDelay,BOOL once,int ch,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize){return 0;};
	virtual int DelaySysEvent(int cmsDelay,BOOL once,int identify,LPVOID eventdata,int eventsize){return 0;};
	virtual int InsertChEvent(int ch,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize){return 0;};

	virtual int CancelDelay(int ch,int delayid){return 0;};

	virtual int BindExclusiveThread(int ch){return 0;};
	virtual int UnBindExclusiveThread(int ch){return 0;};



	virtual int  GetSelectCh(){return 0;};
	virtual void ResetCh(int ch);

	virtual void PauseCh(int ch){};
	virtual void ResumeCh(int ch){};

	//Fax函数


	//Conf函数


public:
	virtual void  SendNotify(int ch,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize);
	void		  SetErrFlag(int errcode=0);


protected:
	int				m_totalline;
	ChannelMgr&		m_channelmgr; 
	CLastError&		m_lasterror;
	CDjInterface*	m_pdjinterface;



private:
	const static byte s_errtype=0x01;
	int				m_errlastcode;
};
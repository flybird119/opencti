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
	created:	12:4:2009   11:29
	filename: 	e:\project\opencti.code\trunk\002\src\cti\CtiInterface.h
	file path:	e:\project\opencti.code\trunk\002\src\cti
	file base:	CtiInterface
	file ext:	h
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/

#pragma once

#include "CtiVerify.h"
#include "CtiProcessor.h"
#include "CtiEventTrace.h"


#ifdef SSMDEVICE
#include "../ssm/SsmInterface.h"
typedef CSsmInterface CDeviceInterface;
#endif


#ifdef DJDEVICE
#include "../dj/DjInterface.h"
typedef CDjInterface CDeviceInterface;
#endif


#ifdef DJKEYGOEDEVICE
#include "DJKeygoeWrap.h"
#endif



#define EVT_CTI_BIGIN                (EVENT_TOTAL+1)
#define EVT_CTI_DELAYED              (EVT_CTI_BIGIN+1)

extern COMMON::CLock  CTIAPILock;
#define BLOCK_()   LOCKSCOPE(CTIAPILock)


class CtiInterface : public CDeviceInterface
{
public:


public:
    CtiInterface(void);
    ~CtiInterface(void);

public:
    BEGIN_YSMESSAGE_MAP()
        BLOCK_();
        YSMESSAGE_HANDLER(EVT_CTI_DELAYED,OnDelayed)
        CHAIN_YSMESSAGE_MAP(CDeviceInterface)
    END_YSMESSAGE_MAP()



private:
    LRESULT OnDelayed(MessageData* pdata,BOOL& bHandle);


public:

    //系统服务函数
    virtual int Init(DeviceDescriptor* pctidevicedescriptor);
	virtual void Term();

	//virtual BOOL Features(FUNCTIONADDRINDEX descriptor)=0;
	//virtual void* GetNativeAPI(LPCTSTR funname)=0;


    virtual int GetLastErrorCode();
	virtual void SetLastErrorCode(int code);
    virtual LPCTSTR FormatErrorCode(int code);

	//virtual int GetChTotal()=0;
    virtual CHANNELTYPE GetChType(int ch);

	//virtual int SearchIdleCh(CHANNELTYPE chtype, int* pChFilter, int FilterLen,int*pChExcept,int ExceptLen)=0;


    virtual int SetChOwnerData(int ch,DWORD OwnerData);
    virtual DWORD GetChOwnerData(int ch);

	virtual CHANNELSTATE GetChState(int ch);



    //接续函数

	virtual int  Dialup(int ch,LPCTSTR calling,NUMBERATTRIBUTE callingattribute,LPCTSTR called,LPCTSTR origcalled,NUMBERATTRIBUTE origcalledattribute,BOOL CallerIDBlock,int flags); //CallerIDBlock 去电隐藏
	virtual int  Pickup(int ch,int flags);
	virtual int  Ringback(int ch);
	virtual int  Hangup(int ch,RELEASEATTRIBUTE attribute);


    //DTMF函数

    virtual int  SendDtmf(int ch,LPCTSTR dtmf,int len,int flags);
    virtual int  StopSendDtmf(int ch);
    virtual int  IsSendDtmf(int ch);

    virtual int  RecvDtmf(int ch,int time,int flags);
    virtual int  StopRecvDtmf(int ch);
    virtual int  IsRecvDtmf(int ch);


    //FSK函数

    virtual int  SendFsk(int ch,const BYTE* fsk,int len,int flags);
    virtual int  SendFskFull(int ch,int sync,int mark,const BYTE* fsk,int len,int flags);
    virtual int  StopSendFsk(int ch);
    virtual int  IsSendFsk(int ch);

    virtual int  RecvFsk(int ch,int time,int flags);
    virtual int  StopRecvFsk(int ch);
    virtual int  IsRecvFsk(int ch);



    //放音函数

    virtual int PlayFile(int ch,LPCTSTR fullpath,LPCTSTR StopstrDtmfCharSet);
    virtual int PlayMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet);
    virtual int PlayQueue(int ch,LPCTSTR fullpath[],int size,LPCTSTR StopstrDtmfCharSet);

	//virtual int LoadIndex()=0;

    virtual int PlayIndex(int ch,LPCTSTR Alias,LPCTSTR StopstrDtmfCharSet);


    virtual int StopPlay(int ch);
    virtual int IsPlay(int ch);
    virtual int UpDatePlayMemory(int ch,int index,BYTE* block,int size);


    //录音函数
    virtual int RecordFile(int ch,LPCTSTR fullpath,int time,LPCTSTR StopstrDtmfCharSet);
    virtual int RecordMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet);
    virtual int StopRecord(int ch);
    virtual int IsRecord(int ch);
    virtual int UpDateRecordMemory(int ch,int index,BYTE* block,int size);


    //Exchange总线
    virtual int Listen(int initiativech,int passivelych);
    virtual int UnListen(int initiativech,int passivelych);


    //TONE函数
    virtual int SendTone(int ch,TONEATTRIBUTE attribute,int time,int flags);
    virtual int SendToneEx(int ch, int nFreq1, int nVolume1, int nFreq2, int nVolume2,DWORD dwOnTime, DWORD dwOffTime,int time,int flags);
    virtual int StopSendTone(int ch);
    virtual int IsSendTone(int ch);

    virtual int RecvTone(int ch,TONEATTRIBUTE attribute,int time,int flags);
    virtual int StopRecvTone(int ch,TONEATTRIBUTE attribute);
    virtual int IsRecvTone(int ch,TONEATTRIBUTE attribute);


    //闪断
    virtual int SendFlash(int ch,int time){return 0;}
    virtual int RecvFlash(int ch){return 0;}
    virtual int StopRecvFlash(int ch){return 0;}
    virtual int IsRecvFlash(int ch){return 0;}


    //辅助
    virtual int  DelayChEvent(int cmsDelay,BOOL once,int ch,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize);
    virtual int  DelaySysEvent(int cmsDelay,BOOL once,int identify,LPVOID eventdata,int eventsize);
	virtual int  InsertChEvent(int ch,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize);

	virtual int  CancelDelay(int ch,int delayid);

	virtual int  BindExclusiveThread(int ch);
	virtual int  UnBindExclusiveThread(int ch);

    virtual int  GetSelectCh();
    virtual void ResetCh(int ch);

	virtual void PauseCh(int ch);
	virtual void ResumeCh(int ch);


    //Fax函数


    //Conf函数


   

public:
    void  SendNotify(int ch,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize);

private:
    CProcessorMgr   m_processormgr;
	CtiEventTrace	m_eventtrace;

    ChannelProcessor* m_psysprocessor;


    int               m_threadmode;




};

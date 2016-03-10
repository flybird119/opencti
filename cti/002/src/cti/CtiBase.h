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
	filename: 	e:\project\opencti.code\trunk\002\src\cti\CtiBase.h
	file path:	e:\project\opencti.code\trunk\002\src\cti
	file base:	CtiBase
	file ext:	h
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/

#pragma once

#include <windows.h>
#include <tchar.h>

#include "CtiDefine.h"
#include "CtiChannel.h"
#include "CtiVerify.h"
#include "CtiError.h"





class ICtiBase
{
public:
    ICtiBase()
        :m_verify(m_channelmgr,m_lasterror)
        ,m_eventfun(NULL)
        ,m_eventparm(0)
    {

    }

    virtual ~ICtiBase()
    {

    }

    //系统服务函数
    virtual int Init(DeviceDescriptor* pctidevicedescriptor)=0;
    virtual void Term()=0;
 
 
    virtual BOOL Features(FUNCTIONADDRINDEX descriptor)=0;
    virtual void* GetNativeAPI(LPCTSTR funname)=0;

    virtual int GetLastErrorCode()=0;
	virtual void SetLastErrorCode(int code)=0;
    virtual LPCTSTR FormatErrorCode(int code)=0;


    virtual int GetChTotal()=0;
    virtual CHANNELTYPE GetChType(int ch)=0;

    virtual int SearchIdleCh(CHANNELTYPE chtype, int* pChFilter, int FilterLen,int*pChExcept,int ExceptLen)=0;

    virtual int SetChOwnerData(int ch,DWORD OwnerData)=0;
    virtual DWORD GetChOwnerData(int ch)=0;

	virtual CHANNELSTATE GetChState(int ch)=0;




    //接续函数

    virtual int Dialup(int ch,LPCTSTR calling,NUMBERATTRIBUTE callingattribute,LPCTSTR called,LPCTSTR origcalled,NUMBERATTRIBUTE origcalledattribute,BOOL CallerIDBlock,int flags)=0; //CallerIDBlock 去电隐藏
    virtual int Pickup(int ch,int flags)=0;
    virtual int Ringback(int ch)=0;
    virtual int Hangup(int ch,RELEASEATTRIBUTE attribute)=0;


    //DTMF函数

    virtual int  SendDtmf(int ch,LPCTSTR dtmf,int len,int flags)=0;
    virtual int  StopSendDtmf(int ch)=0;
    virtual int  IsSendDtmf(int ch)=0;
    virtual int  RecvDtmf(int ch,int time,int flags)=0;
    virtual int  StopRecvDtmf(int ch)=0;
    virtual int  IsRecvDtmf(int ch)=0;


    //FSK函数

    virtual int  SendFsk(int ch,const BYTE* fsk,int len,int flags)=0;
    virtual int  SendFskFull(int ch,int sync,int mark,const BYTE* fsk,int len,int flags)=0;
    virtual int  StopSendFsk(int ch)=0;
    virtual int  IsSendFsk(int ch)=0;

    virtual int  RecvFsk(int ch,int time,int flags)=0;
    virtual int  StopRecvFsk(int ch)=0;
    virtual int  IsRecvFsk(int ch)=0;






    //放音函数
    
    virtual int PlayFile(int ch,LPCTSTR fullpath,LPCTSTR StopstrDtmfCharSet)=0;
    virtual int PlayMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet)=0;
    virtual int PlayQueue(int ch,LPCTSTR fullpath[],int size,LPCTSTR StopstrDtmfCharSet)=0;
    virtual int LoadPlayIndex(LPCTSTR fullpath,LPCTSTR Alias)=0;
    virtual int PlayIndex(int ch,LPCTSTR Alias,LPCTSTR StopstrDtmfCharSet)=0;
    virtual int StopPlay(int ch)=0;
    virtual int IsPlay(int ch)=0;
    virtual int UpDatePlayMemory(int ch,int index,BYTE* block,int size)=0;



    //录音函数
    virtual int RecordFile(int ch,LPCTSTR fullpath,int time,LPCTSTR StopstrDtmfCharSet)=0;
    virtual int RecordMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet)=0;
    virtual int StopRecord(int ch)=0;
    virtual int IsRecord(int ch)=0;
    virtual int UpDateRecordMemory(int ch,int index,BYTE* block,int size)=0;


    //Exchange总线
    virtual int Listen(int initiativech,int passivelych)=0;
    virtual int UnListen(int initiativech,int passivelych)=0;


    //TONE函数

    virtual int SendTone(int ch,TONEATTRIBUTE attribute,int time,int flags)=0;
    virtual int SendToneEx(int ch, int nFreq1, int nVolume1, int nFreq2, int nVolume2,DWORD dwOnTime, DWORD dwOffTime,int time,int flags)=0;
    virtual int StopSendTone(int ch)=0;
    virtual int IsSendTone(int ch)=0;

    virtual int  RecvTone(int ch,TONEATTRIBUTE attribute,int time,int flags)=0;
    virtual int  StopRecvTone(int ch,TONEATTRIBUTE attribute)=0;
    virtual int  IsRecvTone(int ch,TONEATTRIBUTE attribute)=0;


    //闪断
    virtual int SendFlash(int ch,int time)=0;
    virtual int RecvFlash(int ch)=0;
    virtual int StopRecvFlash(int ch)=0;
    virtual int IsRecvFlash(int ch)=0;





    //辅助
    virtual int DelayChEvent(int cmsDelay,BOOL once,int ch,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize)=0;
    virtual int DelaySysEvent(int cmsDelay,BOOL once,int identify,LPVOID eventdata,int eventsize)=0;
	virtual int InsertChEvent(int ch,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize)=0;

	virtual int CancelDelay(int ch,int delayid)=0;

	virtual int BindExclusiveThread(int ch)=0;
	virtual int UnBindExclusiveThread(int ch)=0;



    virtual int  GetSelectCh()=0;
    virtual void ResetCh(int ch)=0;


	virtual void PauseCh(int ch)=0;
	virtual void ResumeCh(int ch)=0;


	

    //Fax函数


    //Conf函数


 



	public:
        CLastError			m_lasterror;
        CtiVerify           m_verify;
        ChannelMgr          m_channelmgr; 


        CTIEVENTPROC	    m_eventfun;
        LPARAM			    m_eventparm;


};


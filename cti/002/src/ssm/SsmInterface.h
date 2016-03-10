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
	created:	12:4:2009   11:27
	filename: 	e:\project\opencti.code\trunk\002\src\ssm\SsmInterface.h
	file path:	e:\project\opencti.code\trunk\002\src\ssm
	file base:	SsmInterface
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


#include "shpa3api.h"


#include <list>



#define EVT_BIGIN                (EVENT_TOTAL+100)

#define EVT_INITIAL              (EVT_BIGIN+0x0101)

#define EVT_SIMULATETIMER        (EVT_BIGIN+0x0103)
#define EVT_UPDATEPLAY           (EVT_BIGIN+0x0105)
#define EVT_UPDATERECORD         (EVT_BIGIN+0x0106)



#define EVT_BOARDBEGIN           EVENT_TOTAL+0x0400

#define EVT_ChState       (EVT_BOARDBEGIN+E_CHG_ChState)            //状态机：通道状态发生变化
#define EVT_HookState     (EVT_BOARDBEGIN+E_CHG_HookState)          //坐席通道：话机上检测到摘机或挂机动作
#define EVT_CIDExBuf      (EVT_BOARDBEGIN+E_CHG_CIDExBuf)           //DTMF检测器：主叫号码扩展缓冲区的长度发生变化
#define EVT_RingCount     (EVT_BOARDBEGIN+E_CHG_RingCount)          //模拟中继线通道：铃流信号检测器中信号周期的计数器发生变化
#define EVT_SendFSK       (EVT_BOARDBEGIN+E_PROC_SendFSK)           //FSK数据发送器完成全部数据的发送
#define EVT_AutoDial      (EVT_BOARDBEGIN+E_PROC_AutoDial)          //状态机：AutoDial任务有进展
#define EVT_ToneAnalyze   (EVT_BOARDBEGIN+E_CHG_ToneAnalyze)        //信号音检测器：分析结果发生变化
#define EVT_RecvDTMF      (EVT_BOARDBEGIN+E_CHG_RcvDTMF)            //DTMF检测器：收到一个DTMF字符
#define EVT_SendDTMF      (EVT_BOARDBEGIN+E_PROC_SendDTMF)          //DTMF发生器：发送DTMF的任务完成，发送DTMF任务由函数SsmTxDtmf启动
#define EVT_RecvFSK       (EVT_BOARDBEGIN+E_PROC_RcvFSK)            //RcvFSK任务结束
#define EVT_PlayEnd       (EVT_BOARDBEGIN+E_PROC_PlayEnd)           //放音操作：放音任务结束
#define EVT_RecordEnd     (EVT_BOARDBEGIN+E_PROC_RecordEnd)         //录音操作：录音任务终止
#define EVT_BusyTone      (EVT_BOARDBEGIN+E_CHG_BusyTone)           //信号音检测器：呼叫进程音检测器检测到的忙音周期的个数发生变化
#define EVT_RxPhoNumBuf   (EVT_BOARDBEGIN+E_CHG_RxPhoNumBuf)        //DTMF检测器：收到新的被叫号码




enum RECVCID
{
	RECVCID_IDLE,
	RECVCID_RECV,
	RECVCID_WAIT,
	RECVCID_RING,
	RECVCID_FINISH,
};

class SsmChannelInfo : public ChannelInfo
{
public:
	SsmChannelInfo(UINT chid)
		:ChannelInfo(chid)
	{

	}

    int          internalstate;      //内部状态        

	struct  
	{
		int         ringcount;
		RECVCID     cidsetup;
		UINT        remoteblocktimer;   //防止响铃一声就挂断，通道不能复位

	} recvcid;

	struct 
	{
		bool        bpick;

	} sendcid;

    struct  
    {
        UINT        flags;
    } pickup;

    struct  
    {
        UINT        flags;
    } dialup;

    struct  
    {
        UINT        timer;
    } sendtone;
    struct  
    {
        UINT        timer;
        UINT        elapse;

    } recvtone;


    struct  
    {
        BOOL        bstop; //三汇EVT_SendDTMF事件，有bug,增加这个变量来标识，是否是SsmStopTxDtmf停止
    } stopdtmf;

    struct  
    {
        UINT        timer;
        UINT        elapse;
    } recvdtmf;

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

        int         index;
        BYTE*       block1;
        int         size1;
        BYTE*       block2;
        int         size2;

    } recordmemory;

    virtual void Reset()
    {
        ChannelInfo::Reset();
        recvcid.cidsetup=RECVCID_IDLE;
        recvcid.remoteblocktimer=0;
        recvcid.ringcount=0;
    }
};



class CSsmInterface : public ICtiBase  , public CMessageHandler , public CtiNotify
{
public:
	CSsmInterface();
	~CSsmInterface(void);





public:

    BEGIN_YSMESSAGE_MAP()
        YSMESSAGE_HANDLER(EVT_INITIAL,OnInitial)
        YSMESSAGE_HANDLER(EVT_SIMULATETIMER,OnSimulateTimer)
   
        
        YSMESSAGE_HANDLER(EVT_UPDATEPLAY,OnUpdatePlay)
        YSMESSAGE_HANDLER(EVT_UPDATERECORD,OnUpdateRecord)



        YSMESSAGE_HANDLER(EVT_ChState,OnChState)
        YSMESSAGE_HANDLER(EVT_HookState,OnHookState)
        YSMESSAGE_HANDLER(EVT_CIDExBuf,OnCIDExBuf)
        YSMESSAGE_HANDLER(EVT_RingCount,OnRingCount)


        YSMESSAGE_HANDLER(EVT_SendFSK,OnSendFsk)
        YSMESSAGE_HANDLER(EVT_AutoDial,OnAutoDial)
        YSMESSAGE_HANDLER(EVT_ToneAnalyze,OnToneAnalyze)
        YSMESSAGE_HANDLER(EVT_RecvDTMF,OnRecvDTMF)
        YSMESSAGE_HANDLER(EVT_SendDTMF,OnSendDTMF)
        YSMESSAGE_HANDLER(EVT_RecvFSK,OnRecvFSK)
        YSMESSAGE_HANDLER(EVT_PlayEnd,OnPlayEnd)
        YSMESSAGE_HANDLER(EVT_RecordEnd,OnRecordEnd)
        //YSMESSAGE_HANDLER(EVT_BusyTone,OnBusyTone)



		YSMESSAGE_HANDLER(EVT_RxPhoNumBuf,OnRxPhoNumBuf)



        
    END_YSMESSAGE_MAP()


private:
    LRESULT OnInitial(MessageData* pdata,BOOL& bHandle);



    LRESULT OnSimulateTimer(MessageData* pdata,BOOL& bHandle);
    LRESULT OnUpdatePlay(MessageData* pdata,BOOL& bHandle);
    LRESULT OnUpdateRecord(MessageData* pdata,BOOL& bHandle);



    LRESULT OnChState(MessageData* pdata,BOOL& bHandle);
	LRESULT OnHookState(MessageData* pdata,BOOL& bHandle);
    LRESULT OnCIDExBuf(MessageData* pdata,BOOL& bHandle);
    LRESULT OnRingCount(MessageData* pdata,BOOL& bHandle);
    LRESULT OnSendFsk(MessageData* pdata,BOOL& bHandle);

    LRESULT OnAutoDial(MessageData* pdata,BOOL& bHandle);
    LRESULT OnToneAnalyze(MessageData* pdata,BOOL& bHandle);
    LRESULT OnRecvDTMF(MessageData* pdata,BOOL& bHandle);
    LRESULT OnSendDTMF(MessageData* pdata,BOOL& bHandle);
    LRESULT OnRecvFSK(MessageData* pdata,BOOL& bHandle);
    LRESULT OnPlayEnd(MessageData* pdata,BOOL& bHandle);
    LRESULT OnRecordEnd(MessageData* pdata,BOOL& bHandle);
    //LRESULT OnBusyTone(MessageData* pdata,BOOL& bHandle);
    
	LRESULT OnRxPhoNumBuf(MessageData* pdata,BOOL& bHandle);

public:

    //系统服务函数
    virtual int Init(DeviceDescriptor* pctidevicedescriptor);
    virtual void Term();


    virtual BOOL Features(FUNCTIONADDRINDEX descriptor){return FALSE;};
    virtual void* GetNativeAPI(LPCTSTR funname){return NULL;}

	virtual int GetLastErrorCode();
	virtual LPCTSTR FormatErrorCode(int code);

    virtual CHANNELTYPE GetChType(int ch);

    virtual int GetChTotal();

    virtual int SearchIdleCh(CHANNELTYPE chtype, int* pChFilter, int FilterLen,int*pChExcept,int ExceptLen);




    //接续函数

    virtual int  Dialup(int ch,LPCTSTR calling,NUMBERATTRIBUTE callingattribute,LPCTSTR called,LPCTSTR origcalled,NUMBERATTRIBUTE origcalledattribute,BOOL CallerIDBlock,int flags); //CallerIDBlock 去电隐藏
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


	virtual void ResetCh(int ch);



public:
    static int CALLBACK EventCallBackProc(WORD wEvent, int nReference, DWORD dwParam, DWORD dwUser);




    static BOOL CALLBACK playmemblockhangler(int ch, int nEndReason, PUCHAR pucBuf, DWORD dwStopOffset, PVOID pV);
    static BOOL CALLBACK recordmemblockhangler(int ch, int nEndReason, PUCHAR pucBuf, DWORD dwStopOffset, PVOID pV);

private:

    void  OnStatePending(SsmChannelInfo* pch);
  

private:
    LPCTSTR StateToText(int state);
    LPCTSTR EventToDescription(int wevent);
    LPCTSTR EventToName(int wevent);

    LPCTSTR CallPendingToDescription(DWORD reason);

    LPCTSTR EventToDetail(int ch,int wevent,DWORD dwParam);
	LPCTSTR ChStateToDescription(int ch,DWORD dwParam);
	LPCTSTR AutoDialToDescription(int ch,DWORD dwParam);
	LPCTSTR RemoteChBlockToDescription(int ch,DWORD dwParam);
	LPCTSTR RemotePCMBlockToDescription(int ch,DWORD dwParam);
	LPCTSTR CIDExBufToDescription(int ch,DWORD dwParam);
    LPCTSTR HookStateToDescription(int ch,DWORD dwParam);
    LPCTSTR SendFSKToDescription(int ch,DWORD dwParam);
    LPCTSTR RingCountToDescription(int ch,DWORD dwParam);
    LPCTSTR ToneAnalyzeToDescription(int ch,DWORD dwParam);
    LPCTSTR AmdToDescription(int ch,DWORD dwParam);
    LPCTSTR SendDTMFToDescription(int ch,DWORD dwParam);
    LPCTSTR RecvDTMFToDescription(int ch,DWORD dwParam);
    LPCTSTR RecvFSKToDescription(int ch,DWORD dwParam);
    LPCTSTR PlayEndToDescription(int ch,DWORD dwParam);
    LPCTSTR PlayFileListToDescription(int ch,DWORD dwParam);
    LPCTSTR PlayMemoDescription(int ch,DWORD dwParam);
    LPCTSTR PlayFileToDescription(int ch,DWORD dwParam);
    LPCTSTR RecordFileToDescription(int ch,DWORD dwParam);
    LPCTSTR RecordEndToDescription(int ch,DWORD dwParam);
    LPCTSTR BargeInToDescription(int ch,DWORD dwParam);
    LPCTSTR BusyToneToDescription(int ch,DWORD dwParam);
	LPCTSTR RxPhoNumBufToDescription(int ch,DWORD dwParam);
    LPCTSTR AutoDialFailedToDescription(int ch,DWORD dwParam);
	LPCTSTR Mtp2StatusToDescription(int ch,DWORD dwParam);
    LPCTSTR OvrlEnrgLevelToDescription(int ch,DWORD dwParam);
    LPCTSTR OverallEnergyToDescription(int ch,DWORD dwParam);
    LPCTSTR PeakFrqToDescription(int ch,DWORD dwParam);

    

    inline void  ShowWarn(LPCTSTR Text);
    inline SsmChannelInfo* GetChInfo(int ch);

	void  GetInternalCID(int ch,TCHAR* cid);
    void  DetectRingdown(int ch);


	RELEASEATTRIBUTE DialFailedToReleaseAttribute(int Failedreason);
    void  DtmfStopPlay(int ch,LPCTSTR StopstrDtmfCharSet);
    void  DtmfStopRecord(int ch,LPCTSTR StopstrDtmfCharSet);

    int   FindIdleCh(CHANNELTYPE chtype,int filtermin,int filtermax,int*pChExcept,int ExceptLen);


private:
    int				    m_totalline;

};

//opencti������һ��ͨ�õ�CTI�����⣬֧�ֶ���������ȳ��̵����������������Ȳ�Ʒ��
//��ַ��http://www.opencti.cn
//QQȺ��21596142
//��ϵ��opencti@qq.com
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

#define EVT_ChState       (EVT_BOARDBEGIN+E_CHG_ChState)            //״̬����ͨ��״̬�����仯
#define EVT_HookState     (EVT_BOARDBEGIN+E_CHG_HookState)          //��ϯͨ���������ϼ�⵽ժ����һ�����
#define EVT_CIDExBuf      (EVT_BOARDBEGIN+E_CHG_CIDExBuf)           //DTMF����������к�����չ�������ĳ��ȷ����仯
#define EVT_RingCount     (EVT_BOARDBEGIN+E_CHG_RingCount)          //ģ���м���ͨ���������źż�������ź����ڵļ����������仯
#define EVT_SendFSK       (EVT_BOARDBEGIN+E_PROC_SendFSK)           //FSK���ݷ��������ȫ�����ݵķ���
#define EVT_AutoDial      (EVT_BOARDBEGIN+E_PROC_AutoDial)          //״̬����AutoDial�����н�չ
#define EVT_ToneAnalyze   (EVT_BOARDBEGIN+E_CHG_ToneAnalyze)        //�ź����������������������仯
#define EVT_RecvDTMF      (EVT_BOARDBEGIN+E_CHG_RcvDTMF)            //DTMF��������յ�һ��DTMF�ַ�
#define EVT_SendDTMF      (EVT_BOARDBEGIN+E_PROC_SendDTMF)          //DTMF������������DTMF��������ɣ�����DTMF�����ɺ���SsmTxDtmf����
#define EVT_RecvFSK       (EVT_BOARDBEGIN+E_PROC_RcvFSK)            //RcvFSK�������
#define EVT_PlayEnd       (EVT_BOARDBEGIN+E_PROC_PlayEnd)           //���������������������
#define EVT_RecordEnd     (EVT_BOARDBEGIN+E_PROC_RecordEnd)         //¼��������¼��������ֹ
#define EVT_BusyTone      (EVT_BOARDBEGIN+E_CHG_BusyTone)           //�ź�������������н������������⵽��æ�����ڵĸ��������仯
#define EVT_RxPhoNumBuf   (EVT_BOARDBEGIN+E_CHG_RxPhoNumBuf)        //DTMF��������յ��µı��к���




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

    int          internalstate;      //�ڲ�״̬        

	struct  
	{
		int         ringcount;
		RECVCID     cidsetup;
		UINT        remoteblocktimer;   //��ֹ����һ���͹Ҷϣ�ͨ�����ܸ�λ

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
        BOOL        bstop; //����EVT_SendDTMF�¼�����bug,���������������ʶ���Ƿ���SsmStopTxDtmfֹͣ
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

    //ϵͳ������
    virtual int Init(DeviceDescriptor* pctidevicedescriptor);
    virtual void Term();


    virtual BOOL Features(FUNCTIONADDRINDEX descriptor){return FALSE;};
    virtual void* GetNativeAPI(LPCTSTR funname){return NULL;}

	virtual int GetLastErrorCode();
	virtual LPCTSTR FormatErrorCode(int code);

    virtual CHANNELTYPE GetChType(int ch);

    virtual int GetChTotal();

    virtual int SearchIdleCh(CHANNELTYPE chtype, int* pChFilter, int FilterLen,int*pChExcept,int ExceptLen);




    //��������

    virtual int  Dialup(int ch,LPCTSTR calling,NUMBERATTRIBUTE callingattribute,LPCTSTR called,LPCTSTR origcalled,NUMBERATTRIBUTE origcalledattribute,BOOL CallerIDBlock,int flags); //CallerIDBlock ȥ������
    virtual int  Pickup(int ch,int flags);

    virtual int  Ringback(int ch);
    virtual int  Hangup(int ch,RELEASEATTRIBUTE attribute);


    //DTMF����

    virtual int  SendDtmf(int ch,LPCTSTR dtmf,int len,int flags);
    virtual int  StopSendDtmf(int ch);

    virtual int  RecvDtmf(int ch,int time,int flags);
    virtual int  StopRecvDtmf(int ch);



    //FSK����

    virtual int  SendFsk(int ch,const BYTE* fsk,int len,int flags);
    virtual int  SendFskFull(int ch,int sync,int mark,const BYTE* fsk,int len,int flags);
    virtual int  StopSendFsk(int ch);

    virtual int  RecvFsk(int ch,int time,int flags);
    virtual int  StopRecvFsk(int ch);


    //��������

    virtual int PlayFile(int ch,LPCTSTR fullpath,LPCTSTR StopstrDtmfCharSet);
    virtual int PlayMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet);
    virtual int PlayQueue(int ch,LPCTSTR fullpath[],int size,LPCTSTR StopstrDtmfCharSet);

    virtual int LoadPlayIndex(LPCTSTR fullpath,LPCTSTR Alias);
    virtual int PlayIndex(int ch,LPCTSTR Alias,LPCTSTR StopstrDtmfCharSet);

    virtual int StopPlay(int ch);

    virtual int UpDatePlayMemory(int ch,int index,BYTE* block,int size);



    //¼������
    virtual int RecordFile(int ch,LPCTSTR fullpath,int time,LPCTSTR StopstrDtmfCharSet);
    virtual int RecordMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet);
    virtual int StopRecord(int ch);

    virtual int UpDateRecordMemory(int ch,int index,BYTE* block,int size);




    //TONE����

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

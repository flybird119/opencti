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
	created:	12:4:2009   11:30
	filename: 	e:\project\opencti.code\trunk\002\src\cti\CtiChannel.h
	file path:	e:\project\opencti.code\trunk\002\src\cti
	file base:	CtiChannel
	file ext:	h
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/

#pragma once

#include "CtiDefine.h"
#include <Windows.h>
#include <map>
#include <queue>
#include <list>

#include "Common/lock.h"

#include "Common/frequent.h"


enum OperationFlag
{
    OF_NONE             =   0x00,
	OF_SENDDTMF			=	0x01,
	OF_RECVDTMF			=	0x02,
	OF_SENDFSK			=   0x04,
	OF_RECVFSK			=	0x08,
	OF_PLAYSOUND		=   0x10,
	OF_RECORDSOUND		=   0x20,
	OF_SENDTONE			=   0x40,
	OF_RECVTONE			=   0x80,
};



class ChannelProcessor
{
public:

    ChannelProcessor();

    //���ô�����ԭ��
    //1�Ѵ������¼�����m_QueuehEventδ�ˣ����Ŷ��˴������¼�
    //2���˴�����ִ��GetMessageʱ���ִ����߳�ID�ı�,��m_QueuehEventɾ�����˴������¼����������¶��˴������¼�
    void SetProcessor(HANDLE hEvent,UINT uiThreadid);

    /// �ָ�ԭʼ������
    void ComebackProcessor();


	//��ͣ����
	void PauseProcessor();
	//�ָ�����
	void ResumeProcessor();



    /// ��ȡ�¼�
    /// \retval 1 ��ȡ��Ϣ�ɹ�
    /// \retval 0 ��ȡ��Ϣʧ��
    /// \retval -1 �Ѹ��´����� �μ�::SetProcessor
    int GetMessage(LPCHANNELEVENT* ppMsg);

    //�����¼�
    void PushMessage(const LPCHANNELEVENT pMsg);

	//�����¼�
	void InsertMessage(const LPCHANNELEVENT pMsg);


    void SetCallback(CTIEVENTPROC chnotifyfun,LPARAM chnotifyparam);
    void GetCallback(CTIEVENTPROC*pchnotifyfun,LPARAM* pchnotifyparam);
    CTIEVENTPROC GetCallback();


    void  SetOwnerData(DWORD chownerdata);
    DWORD  GetOwnerData();



protected:

	//setup 0:������Ϣ 1:������Ϣ
	//����ֵ 1:��Ϣ��Ч����Ϣ������
	virtual int MessageFilter(LPCHANNELEVENT* ppMsg,int setup){return setup;}

	
protected:

	COMMON::CLock       m_lock;

private:

    //�¼��ʹ����߳�
    std::queue<HANDLE>  m_QueuehEvent;
    UINT                m_uiThreadID;

    HANDLE				m_hOriginalEvent;
    UINT				m_uiOriginalThreadID;


    //��Ϣ�ж�
    std::queue<LPCHANNELEVENT>  m_QueueMessage;


    CTIEVENTPROC    m_chnotifyfun;	
    LPARAM          m_chnotifyparam;

    DWORD			m_chownerdata;


	BOOL			m_bpause;

};






class ChannelInfo : public ChannelProcessor
{
public:
    ChannelInfo(int chid);
    virtual ~ChannelInfo(void);

private:

	 int MessageFilter(LPCHANNELEVENT* ppMsg,int setup);


	int OnAddChannel(const LPCHANNELEVENT pMessage);
	int OnCallin(const LPCHANNELEVENT pMessage);
	int OnState(const LPCHANNELEVENT pMessage);
	int OnHangup(const LPCHANNELEVENT pMessage);
	int OnRelease(const LPCHANNELEVENT pMessage);  
	int OnIdle(const LPCHANNELEVENT pMessage);
	int OnRemoteBlock(const LPCHANNELEVENT pMessage);
	int OnRingback(const LPCHANNELEVENT pMessage);
	int OnRinging(const LPCHANNELEVENT pMessage);
	int OnUnusable(const LPCHANNELEVENT pMessage);
	int OnTalking(const LPCHANNELEVENT pMessage);
	int OnDialup(const LPCHANNELEVENT pMessage);
	int OnLocalBlock(const LPCHANNELEVENT pMessage);
	int OnSleep(const LPCHANNELEVENT pMessage);
	int OnSendTone(const LPCHANNELEVENT pMessage);
	int OnSendDtmf(const LPCHANNELEVENT pMessage);
	int OnRecvDtmf(const LPCHANNELEVENT pMessage);
	int OnPlay(const LPCHANNELEVENT pMessage);
	int OnUpdatePlayMemory(const LPCHANNELEVENT pMessage);
	int OnSysEvent(const LPCHANNELEVENT pMessage);
	int OnUserEvent(const LPCHANNELEVENT pMessage);
	int OnRecord(const LPCHANNELEVENT pMessage);
	int OnUpdateRecordMemory(const LPCHANNELEVENT pMessage);
	int OnSendFsk(const LPCHANNELEVENT pMessage);
	int OnRecvFsk(const LPCHANNELEVENT pMessage);



public:

	/// �ı��־
	/// \retval ԭʼ�ı�־
	UINT ModifyOperationFlag(OperationFlag addflag,OperationFlag removeflag);

    void SetPlayVoice(PLAYATTRIBUTE playattribute);
    void SetRecordVoice(RECORDATTRIBUTE recordattribute);
    void SetSendTone(TONEATTRIBUTE toneattribute);
    void SetRecvTone(TONEATTRIBUTE toneattribute);

    PLAYATTRIBUTE GetPlayVoice();
    RECORDATTRIBUTE GetRecordVoice();
    TONEATTRIBUTE GetSendTone();
    TONEATTRIBUTE GetRecvTone();


	bool IsSendDTMF();
	bool IsRecvDTMF();
	bool IsSendFSK();
	bool IsRecvFSK();
	bool IsPlayVoice(PLAYATTRIBUTE playattribute);
	bool IsRecordVoice(RECORDATTRIBUTE recordattribute);
	bool IsSendTone(TONEATTRIBUTE toneattribute);
	bool IsRecvTone(TONEATTRIBUTE toneattribute);
	bool IsSound();


	int  GetID();
    void SetType(CHANNELTYPE chtype);
	CHANNELTYPE GetType();

	//����ͨ��
	bool IsDigitalTrunkCh();
	//ģ��ͨ��
	bool IsAnalogTrunkCh();
	//����ͨ��
	bool IsTrunkCh();
	//¼��ͨ��
	bool IsRecordCh();
	//��Դͨ��
	bool IsResourceCh();
	//����ͨ��
	bool IsUsableCh();


	void  SetState(CHANNELSTATE chstate);
	CHANNELSTATE  GetState();
	CHANNELSTATE  GetPreviousState();
	UINT  GetKeepTime();

	//STATE_DIALUP STATE_RINGBACK
	BOOL  IsCalloutStates();
	//STATE_CALLIN STATE_RINGING
	BOOL  IsCallInStates();
	//STATE_DIALUP STATE_RINGBACK STATE_CALLIN STATE_RINGING
	BOOL  IsJoinStates();
	//STATE_DIALUP STATE_RINGBACK STATE_CALLIN STATE_RINGING STATE_TALKING
	BOOL  IsCallStates();

	//STATE_DIALUP STATE_RINGBACK STATE_CALLIN STATE_RINGING STATE_TALKING STATE_HANGUP STATE_RELEASE
	BOOL  IsWorkStates();


    void  SetWay(CHANNELWAY chway);
    CHANNELWAY  GetWay();


    void    SetCalled(LPCTSTR telnumber);
    void    SetCalling(LPCTSTR telnumber);
    void    SetOrigCalled(LPCTSTR telnumber);
    LPCTSTR GetCalled();
    LPCTSTR GetCalling();
    LPCTSTR GetOrigCalled();
    void    ClearDtmf();
    LPCTSTR GetDtmf();
    void    AppendDtmf(TCHAR str);

    LPCTSTR GetRemark();
    void    SetRemark(LPCTSTR remark);

    LPCTSTR GetInternalstate();
    void    SetInternalstate(LPCTSTR internalstate);

    LPCTSTR GetChIDStr();


    void    InsertInitiative(int Initiative);
    void    RemoveInitiative(int Initiative);
    BOOL    IsInitiative(int Initiative);
    int     GetInitiativeSize();
    void    GetInitiative(std::vector<int>& Initiative);
    void    ClearInitiative();

    void    InsertPassively(int Passively);
    void    RemovePassively(int Passively);
    BOOL    IsPassively(int Passively);
    int     GetPassivelySize();
    void    GetPassively(std::vector<int>& Passively);
    void    ClearPassively();
    

    bool    IsDelayID(UINT delayid);
    void    AddDelayID(UINT delayid);
	void    DelDelayID(UINT delayid);
	int     GetDelayID(UINT** ppdelayid);
    void    ClearDelayID();

    virtual void Reset();

private:




//������־
	UINT			m_operationflag;
    PLAYATTRIBUTE   m_playattribute;
    RECORDATTRIBUTE m_recordattribute;
    TONEATTRIBUTE   m_sendtoneattribute;
    TONEATTRIBUTE   m_recvtoneattribute;


//ͨ������
	int  			m_chid;
	CHANNELTYPE		m_chtype;
	CHANNELSTATE	m_chstate;
	CHANNELSTATE	m_chpreviousstate;
	Millisecond		m_chstarttime;         //ͨ��״̬��ʼʱ��
	CHANNELWAY      m_chway;



	std::list<int> m_chinitiative;
	std::list<int> m_chpassively;

	
    TCHAR          m_chcalling[128];
    TCHAR          m_chcalled[128];
    TCHAR          m_chorigcalled[128];

#if defined(_UNICODE) || defined(UNICODE)
    std::wstring   m_chdtmf;
#else
    std::string    m_chdtmf;
#endif

#if defined(_UNICODE) || defined(UNICODE)
    std::wstring  m_chremark;
#else
    std::string   m_chremark;
#endif


#if defined(_UNICODE) || defined(UNICODE)
    std::wstring  m_chinternalstate;
#else
    std::string   m_chinternalstate;
#endif


#if defined(_UNICODE) || defined(UNICODE)
    std::wstring  m_chidstr;
#else
    std::string   m_chidstr;
#endif


    std::vector<UINT>  m_delayidlist;


};



class ChannelMgr
{


public:

    ChannelMgr()
        :m_lock(COMMON::CRITICAl)
    {


    }
    ~ChannelMgr()
    {
    }

	bool AddChannel(ChannelInfo* pchannel)
	{
        LOCKSCOPE(m_lock);
		bool bret=false;
		std::pair <std::map<int,ChannelInfo*>::iterator, bool> retinsert =
			m_pchannelmap.insert(std::pair<int,ChannelInfo*>(pchannel->GetID(),pchannel));
		bret=retinsert.second;
		return bret;
	}

	bool DelChannel(int chid)
	{
        LOCKSCOPE(m_lock);
		bool bret=false;
		std::map<int,ChannelInfo*>::size_type n=m_pchannelmap.erase(chid);
		bret=n>0;
		return bret;
	}

	ChannelInfo* FindChannel(int chid)
	{
        LOCKSCOPE(m_lock);
		ChannelInfo* pchannel=NULL;
		std::map<int,ChannelInfo*>::const_iterator channel_constiter=m_pchannelmap.find(chid);
		if(channel_constiter!=m_pchannelmap.end())
		{
			pchannel=channel_constiter->second;
		}
		return pchannel;
	}


	void ClearChannel()
	{
        LOCKSCOPE(m_lock);
		for(std::map<int,ChannelInfo*>::iterator chiter=m_pchannelmap.begin();chiter!=m_pchannelmap.end();++chiter)
		{
			delete chiter->second;
		}
		m_pchannelmap.clear();
	}


private:
	std::map<int,ChannelInfo*>			m_pchannelmap;
    COMMON::CLock                       m_lock;
};


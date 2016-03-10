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
	filename: 	e:\project\opencti.code\trunk\002\src\cti\CtiChannel.cpp
	file path:	e:\project\opencti.code\trunk\002\src\cti
	file base:	CtiChannel
	file ext:	cpp
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/


#include "StdAfx.h"
#include "CtiChannel.h"

#include "ctiinterface.h"


#pragma warning(disable:4996)

extern COMMON::CLock  CTIAPILock;
#define BLOCK_()   LOCKSCOPE(CTIAPILock)

extern CtiInterface* g_pCtiInterface;


ChannelProcessor::ChannelProcessor()
:m_uiThreadID(0)
,m_hOriginalEvent(INVALID_HANDLE_VALUE)
,m_uiOriginalThreadID(0)
,m_lock(COMMON::CRITICAl)
,m_chownerdata(NULL)
,m_bpause(FALSE)
{

}

void ChannelProcessor::SetProcessor(HANDLE hEvent,UINT uiThreadid)
{
    LOCKSCOPE(m_lock);

    if(m_QueuehEvent.empty())
    {
        m_uiOriginalThreadID=uiThreadid;
        m_hOriginalEvent=hEvent;
    }

    m_QueuehEvent.push(hEvent);
    m_uiThreadID=uiThreadid;

    HANDLE& CurrenthEvent=m_QueuehEvent.front();		
    if(CurrenthEvent!=hEvent && !m_QueueMessage.empty())
    {
        ::SetEvent(CurrenthEvent);
    }
}

void ChannelProcessor::ComebackProcessor()
{
    LOCKSCOPE(m_lock);


    m_QueuehEvent.push(m_hOriginalEvent);
    m_uiThreadID=m_uiOriginalThreadID;

    HANDLE& CurrenthEvent=m_QueuehEvent.front();		
    if(CurrenthEvent!=m_hOriginalEvent || !m_QueueMessage.empty())
    {
        ::SetEvent(CurrenthEvent);
    }

}



void ChannelProcessor::PauseProcessor()
{
	LOCKSCOPE(m_lock);

	m_bpause=TRUE;
}


void ChannelProcessor::ResumeProcessor()
{
	LOCKSCOPE(m_lock);

	m_bpause=FALSE;


	if(!m_QueueMessage.empty() && !m_QueuehEvent.empty())
	{
		HANDLE& hEvent=m_QueuehEvent.front();
		SetEvent(hEvent);
	}

}



int ChannelProcessor::GetMessage(LPCHANNELEVENT* ppMsg)
{
    m_lock.Lock();

    int ret=0;

    if(::GetCurrentThreadId()==m_uiThreadID)
    {
        if(!m_QueueMessage.empty())
        {
            *ppMsg=m_QueueMessage.front();
            (*ppMsg)->chownerdata=m_chownerdata;
            (*ppMsg)->notifyparam=m_chnotifyparam;

			if(MessageFilter(ppMsg,0)!=1)
			{
				m_QueueMessage.pop();
			}
		
            if(!m_QueueMessage.empty())
            {
                HANDLE& hEvent=m_QueuehEvent.front();
                SetEvent(hEvent);
            }
        }
		else
		{
			_ASSERT(0);
		}
    }
    else
    {
        m_QueuehEvent.pop();
        _ASSERT(!m_QueuehEvent.empty());

        if(!m_QueueMessage.empty())
        {
            HANDLE& hEvent=m_QueuehEvent.front();
            SetEvent(hEvent);
        }
        ret=-1;
    }

	m_lock.UnLock();

	if(ret==0)
	{
		ret=MessageFilter(ppMsg,1);

		if(ret==0)
		{
			if((*ppMsg)->eventdata && (*ppMsg)->eventsize)
			{
				free((*ppMsg)->eventdata);
			}
			delete *ppMsg;
		}
	}

    return ret;
}


void ChannelProcessor::PushMessage(const LPCHANNELEVENT pMsg)
{
    LOCKSCOPE(m_lock);
    m_QueueMessage.push(pMsg);

    if(!m_bpause && !m_QueuehEvent.empty())
    {
        HANDLE& hEvent=m_QueuehEvent.front();
        SetEvent(hEvent);
    }



}


void ChannelProcessor::InsertMessage(const LPCHANNELEVENT pMsg)
{
	LOCKSCOPE(m_lock);

	if(m_QueueMessage.empty())
	{
		m_QueueMessage.push(pMsg);
	}
	else
	{
		std::queue<LPCHANNELEVENT> tmpqueue;
		tmpqueue.push(pMsg);
		while(!m_QueueMessage.empty())
		{
			tmpqueue.push(m_QueueMessage.front());
			m_QueueMessage.pop();
		}
		m_QueueMessage=tmpqueue;
	}


	if(!m_bpause &&  !m_QueuehEvent.empty())
	{
		HANDLE& hEvent=m_QueuehEvent.front();
		SetEvent(hEvent);
	}

}



void ChannelProcessor::SetCallback(CTIEVENTPROC chnotifyfun,LPARAM chnotifyparam)
{
    LOCKSCOPE(m_lock);

    m_chnotifyfun=chnotifyfun;
    m_chnotifyparam=chnotifyparam;
}

void ChannelProcessor::GetCallback(CTIEVENTPROC*pchnotifyfun,LPARAM* pchnotifyparam)
{
    LOCKSCOPE(m_lock);

    *pchnotifyfun=m_chnotifyfun;
    *pchnotifyparam=m_chnotifyparam;
}

CTIEVENTPROC ChannelProcessor::GetCallback()
{
    LOCKSCOPE(m_lock);

    return m_chnotifyfun;
}

void  ChannelProcessor::SetOwnerData(DWORD chownerdata)
{
    LOCKSCOPE(m_lock);

    m_chownerdata=chownerdata;
}
DWORD  ChannelProcessor::GetOwnerData()
{
    LOCKSCOPE(m_lock);

    return m_chownerdata;
}


ChannelInfo::ChannelInfo(int chid)
:m_operationflag(0)
,m_playattribute(PLAY_NONE)
,m_recordattribute(RECORD_NONE)
,m_sendtoneattribute(TONE_NONE)
,m_recvtoneattribute(TONE_NONE)
,m_chid(chid)
,m_chtype(TYPE_UNKNOWN)
,m_chstate(STATE_UNKNOWN)
,m_chpreviousstate(STATE_UNKNOWN)
,m_chstarttime(COMMON::GetMillisecond())
,m_chway(WAY_UNKNOWN)
{

    memset(m_chcalled,0,sizeof(m_chcalled));
    memset(m_chcalling,0,sizeof(m_chcalling));
    memset(m_chorigcalled,0,sizeof(m_chorigcalled));

    TCHAR idstr[16];
    _itot(chid,idstr,10);
    m_chidstr=idstr;
}

ChannelInfo::~ChannelInfo(void)
{
}





int ChannelInfo::MessageFilter(LPCHANNELEVENT* ppMsg,int setup)
{


	int ret=0;

	LPCHANNELEVENT pMsg=*ppMsg;

	if(setup==0)
	{
		switch(pMsg->eventid)
		{
		case EVENT_IDLE:
		case EVENT_UNUSABLE:
		case EVENT_LOCAL_BLOCK:
		case EVENT_REMOTE_BLOCK:
			{
				switch(GetState())
				{
				case STATE_CALLIN:
				case STATE_RINGING:
				case STATE_TALKING:
				case STATE_DIALUP:
				case STATE_RINGBACK:
					{

						LPHANGUPPARAM lphangupparam=(LPHANGUPPARAM)malloc(sizeof(HANGUPPARAM));
						lphangupparam->reason=RELEASE_UNUSABLE;
						CHANNELEVENT *pMessage=new CHANNELEVENT(pMsg->chid,EVENT_HANGUP,lphangupparam,sizeof(HANGUPPARAM),(*ppMsg)->notifyparam,(*ppMsg)->chownerdata);
						*ppMsg=pMessage;

						ret = 1;
					}
					break;
				}
			}
			break;
		}
	}
	else
	{
		BLOCK_();

		switch(pMsg->eventid)
		{
		case EVENT_ADDCHANNEL:  ret=OnAddChannel(pMsg);           break;
		case EVENT_CALLIN:      ret=OnCallin(pMsg);               break;
		case EVENT_STATE:       ret=OnState(pMsg);                break;
		case EVENT_HANGUP:      ret=OnHangup(pMsg);               break;
		case EVENT_RELEASE:     ret=OnRelease(pMsg);              break;
		case EVENT_IDLE:        ret=OnIdle(pMsg);                 break;
		case EVENT_REMOTE_BLOCK:ret=OnRemoteBlock(pMsg);          break;
		case EVENT_RINGBACK:    ret=OnRingback(pMsg);             break;
		case EVENT_RINGING:     ret=OnRinging(pMsg);              break;
		case EVENT_UNUSABLE:    ret=OnUnusable(pMsg);             break;
		case EVENT_TALKING:     ret=OnTalking(pMsg);              break;
		case EVENT_DIALUP:      ret=OnDialup(pMsg);               break;
		case EVENT_LOCAL_BLOCK: ret=OnLocalBlock(pMsg);           break;
		case EVENT_SLEEP:       ret=OnSleep(pMsg);                break;
		case EVENT_SENDTONE:    ret=OnSendTone(pMsg);             break;
		case EVENT_SENDDTMF:    ret=OnSendDtmf(pMsg);             break;
		case EVENT_RECVDTMF:    ret=OnRecvDtmf(pMsg);             break;
		case EVENT_PLAY:        ret=OnPlay(pMsg);                 break;
		case EVENT_UPDATEPLAYMEMORY:ret=OnUpdatePlayMemory(pMsg); break;
		case EVENT_SYSEVENT:    ret=OnSysEvent(pMsg);             break;
		case EVENT_RECORD:      ret=OnRecord(pMsg);               break;
		case EVENT_UPDATERECORDMEMORY:ret=OnUpdateRecordMemory(pMsg);break;
		case EVENT_SENDFSK:     ret=OnSendFsk(pMsg);              break;
		case EVENT_RECVFSK:     ret=OnRecvFsk(pMsg);              break;
		default:
			if(pMsg->eventid>=EVENT_USEREVENT)
				ret=OnUserEvent(pMsg); 
		}
	}

	return ret;
}

int ChannelInfo::OnAddChannel(const LPCHANNELEVENT pMessage)
{
	LPADDCHANNELPARAM lpaddchannelparam=(LPADDCHANNELPARAM)pMessage->eventdata;
	_ASSERT(pMessage->eventsize==sizeof(ADDCHANNELPARAM));
	SetWay(lpaddchannelparam->chway);
	SetState(lpaddchannelparam->chstate);
	SetType(lpaddchannelparam->chtype);
	return 1;
}

int ChannelInfo::OnCallin(const LPCHANNELEVENT pMessage)
{
	LPCALLINPARAM pcallid=(LPCALLINPARAM)pMessage->eventdata;
	_ASSERT(pMessage->eventsize==sizeof(CALLINPARAM));
	_ASSERT(GetState()==STATE_REMOTE_BLOCK || GetState()==STATE_IDLE || GetState()==STATE_SLEEP);
	CHANNELSTATE chstate=GetState();
	if(chstate==STATE_REMOTE_BLOCK || chstate==STATE_IDLE || chstate==STATE_SLEEP)
	{
		SetCalled(pcallid->called);
		SetOrigCalled(pcallid->origcalled);
		SetCalling(pcallid->calling);
		SetWay(WAY_CALLIN);
		SetState(STATE_CALLIN);
		return 1;
	}
	return 0;
}

int ChannelInfo::OnState(const LPCHANNELEVENT pMessage)
{
	LPSTATEPARAM lpstateparam=(LPSTATEPARAM)pMessage->eventdata;
	_ASSERT(pMessage->eventsize==sizeof(STATEPARAM));
	SetInternalstate(lpstateparam->statetext);
	return 1;
}

int ChannelInfo::OnHangup(const LPCHANNELEVENT pMessage)
{

	LPHANGUPPARAM lphangupparam=(LPHANGUPPARAM)pMessage->eventdata;
	_ASSERT(pMessage->eventsize==sizeof(HANGUPPARAM));
	_ASSERT(GetState()==STATE_CALLIN || GetState()==STATE_RINGING || GetState()==STATE_TALKING || GetState()==STATE_DIALUP || GetState()==STATE_RINGBACK);
	if(IsCallStates())
	{
		SetState(STATE_HANGUP);
		return 1;
	}
	return 0;
}

int ChannelInfo::OnIdle(const LPCHANNELEVENT pMessage)
{

	LPIDLEPARAM lpidleparam=(LPIDLEPARAM)pMessage->eventdata;
	_ASSERT(sizeof(IDLEPARAM)==pMessage->eventsize);
	SetWay(WAY_IDLE);
	SetState(STATE_IDLE);
	
	g_pCtiInterface->ResetCh(m_chid);

	return 1;
}


int ChannelInfo::OnRelease(const LPCHANNELEVENT pMessage)
{
	LPRELEASEPARAM lpreleaseparam=(LPRELEASEPARAM)pMessage->eventdata;
	_ASSERT(sizeof(RELEASEPARAM)==pMessage->eventsize);

	_ASSERT(GetState()==STATE_CALLIN || GetState()==STATE_RINGING || GetState()==STATE_TALKING || GetState()==STATE_DIALUP || GetState()==STATE_RINGBACK);

	if(IsCallStates())
	{
		SetState(STATE_RELEASE);
		return 1;
	}


	return 0;
}

int ChannelInfo::OnRemoteBlock(const LPCHANNELEVENT pMessage)
{
	LPREMOTEBLOCKPARAM lpremoteblockparam=(LPREMOTEBLOCKPARAM)pMessage->eventdata;
	_ASSERT(sizeof(REMOTEBLOCKPARAM)==pMessage->eventsize);

	SetWay(WAY_IDLE);
	SetState(STATE_REMOTE_BLOCK);
	
	
	g_pCtiInterface->ResetCh(m_chid);

	return 1;
}

int ChannelInfo::OnRingback(const LPCHANNELEVENT pMessage)
{

	LPRINGBACKPARAM lpringbackparam=(LPRINGBACKPARAM)pMessage->eventdata;
	_ASSERT(sizeof(RINGBACKPARAM)==pMessage->eventsize);

	_ASSERT(GetState()==STATE_DIALUP);

	if(GetState()==STATE_DIALUP)
	{
		SetState(STATE_RINGBACK);

		return 1;
	}



	return 0;
}


int ChannelInfo::OnRinging(const LPCHANNELEVENT pMessage)
{

	LPRINGINGPARAM lpringingparam=(LPRINGINGPARAM)pMessage->eventdata;
	_ASSERT(sizeof(RINGINGPARAM)==pMessage->eventsize);
	_ASSERT(GetState()==STATE_CALLIN);
	if(GetState()==STATE_CALLIN)
	{
		SetState(STATE_RINGING);
		return 1;
	}


	return 0;
}


int ChannelInfo::OnUnusable(const LPCHANNELEVENT pMessage)
{
	
	LPUNUSABLEPARAM lpunusableparam=(LPUNUSABLEPARAM)pMessage->eventdata;
	_ASSERT(sizeof(UNUSABLEPARAM)==pMessage->eventsize);
	SetState(STATE_UNUSABLE);
	return 1;
}

int ChannelInfo::OnTalking(const LPCHANNELEVENT pMessage)
{
	LPTALKINGPARAM lptalkingparam=(LPTALKINGPARAM)pMessage->eventdata;
	_ASSERT(sizeof(TALKINGPARAM)==pMessage->eventsize);

	_ASSERT(GetState()==STATE_RINGBACK || GetState()==STATE_RINGING);

	CHANNELSTATE chstate=GetState();
	if(chstate==STATE_RINGBACK || chstate==STATE_RINGING)
	{
		SetState(STATE_TALKING);
		return 1;
	}



	return 0;
}

int ChannelInfo::OnDialup(const LPCHANNELEVENT pMessage)
{

	LPDIALUPPARAM pcallid=(LPDIALUPPARAM)pMessage->eventdata;
	_ASSERT(pMessage->eventsize==sizeof(DIALUPPARAM));

	_ASSERT(GetState()==STATE_IDLE || GetState()==STATE_LOCAL_BLOCK);

	CHANNELSTATE chstate=GetState();
	if(chstate==STATE_IDLE || chstate==STATE_LOCAL_BLOCK)
	{
		SetCalled(pcallid->called);
		SetOrigCalled(pcallid->origcalled);
		SetCalling(pcallid->calling);

		SetWay(WAY_CALLOUT);
		SetState(STATE_DIALUP);

		return 1;
	}

	return 0;
}

int ChannelInfo::OnLocalBlock(const LPCHANNELEVENT pMessage)
{
	LPLOCALBLOCKPARAM lplocalblockparam=(LPLOCALBLOCKPARAM)pMessage->eventdata;
	_ASSERT(sizeof(LOCALBLOCKPARAM)==pMessage->eventsize);

	SetWay(WAY_IDLE);
	SetState(STATE_LOCAL_BLOCK);

	g_pCtiInterface->ResetCh(m_chid);

	return 1;
}

int ChannelInfo::OnSleep(const LPCHANNELEVENT pMessage)
{
	LPSLEEPPARAM lpsleepparam=(LPSLEEPPARAM)pMessage->eventdata;
	_ASSERT(sizeof(SLEEPPARAM)==pMessage->eventsize);

	_ASSERT(GetState()==STATE_REMOTE_BLOCK || GetState()==STATE_IDLE);
	CHANNELSTATE chstate=GetState();
	if(chstate==STATE_REMOTE_BLOCK || chstate==STATE_IDLE)
	{

		SetState(STATE_SLEEP);
	}
	return 1;
}

int ChannelInfo::OnSendTone(const LPCHANNELEVENT pMessage)
{
	LPSENDTONEPARAM lpsendtoneparam=(LPSENDTONEPARAM)pMessage->eventdata;
	_ASSERT(sizeof(SENDTONEPARAM)==pMessage->eventsize);

	SetSendTone(TONE_NONE);
	if(IsCallStates())
	{
		return 1;
	}
	return 0;
}


int ChannelInfo::OnSendDtmf(const LPCHANNELEVENT pMessage)
{
	LPSENDDTMFPARAM lpsenddtmfparam=(LPSENDDTMFPARAM)pMessage->eventdata;
	_ASSERT(sizeof(SENDDTMFPARAM)==pMessage->eventsize);

	_ASSERT(IsSendDTMF());

	ModifyOperationFlag(OF_NONE,OF_SENDDTMF);

	if(IsCallStates())
	{
		return 1;
	}
	return 0;
}

int ChannelInfo::OnRecvDtmf(const LPCHANNELEVENT pMessage)
{
	LPRECVDTMFPARAM lprecvdtmfparam=(LPRECVDTMFPARAM)pMessage->eventdata;
	_ASSERT(sizeof(RECVDTMFPARAM)==pMessage->eventsize);



	if(IsCallStates())
	{
		if(IsRecvDTMF())
		{

			if(lprecvdtmfparam->result!=1)
			{
				ModifyOperationFlag(OF_NONE,OF_RECVDTMF);
			}
			else
			{
				AppendDtmf(lprecvdtmfparam->dtmf);
			}
			return 1;
		}

	}
	else
	{
		ModifyOperationFlag(OF_NONE,OF_RECVDTMF);
	}
	return 0;
}

int ChannelInfo::OnPlay(const LPCHANNELEVENT pMessage)
{
	LPPLAYPARAM lpplayparam=(LPPLAYPARAM)pMessage->eventdata;
	_ASSERT(sizeof(PLAYPARAM)==pMessage->eventsize);

	_ASSERT(IsPlayVoice(lpplayparam->attribute));


	SetPlayVoice(PLAY_NONE);

	if(IsCallStates())
	{
		return 1;
	}

	return 0;
}

int ChannelInfo::OnUpdatePlayMemory(const LPCHANNELEVENT pMessage)
{
	LPUPDATEPLAYMEMORYPARAM lpupdatepalymemory=(LPUPDATEPLAYMEMORYPARAM)pMessage->eventdata;
	_ASSERT(sizeof(UPDATEPLAYMEMORYPARAM)==pMessage->eventsize);

	_ASSERT(IsPlayVoice(PLAY_MEMORY));

	_ASSERT(IsCallStates());

	if(IsCallStates())
	{
		return 1;
	}
	return 0;
}





int ChannelInfo::OnSysEvent(const LPCHANNELEVENT pMessage)
{
	return 1;
}

int ChannelInfo::OnUserEvent(const LPCHANNELEVENT pMessage)
{
	LPUSEREVENTPARAM lpusereventparam=(LPUSEREVENTPARAM)pMessage->eventdata;
	_ASSERT(sizeof(USEREVENTPARAM)==pMessage->eventsize);

	_ASSERT(IsWorkStates());


	if(IsWorkStates() && (lpusereventparam->delayid ? IsDelayID(lpusereventparam->delayid) : 1))
	{
		if(lpusereventparam->delayid && lpusereventparam->once)
		{
			DelDelayID(lpusereventparam->delayid);
		}

		return 1;
	}

	if(lpusereventparam->delayid)
		DelDelayID(lpusereventparam->delayid);


	return 0;
}

int ChannelInfo::OnRecord(const LPCHANNELEVENT pMessage)
{
	LPRECORDPARAM lprecordparam=(LPRECORDPARAM)pMessage->eventdata;
	_ASSERT(sizeof(RECORDPARAM)==pMessage->eventsize);
	_ASSERT(IsRecordVoice(lprecordparam->attribute));
	SetRecordVoice(RECORD_NONE);
	if(IsCallStates())
	{
		return 1;
	}

	return 0;
}

int ChannelInfo::OnUpdateRecordMemory(const LPCHANNELEVENT pMessage)
{
	LPUPDATERECORDMEMORYPARAM lpupdaterecordmemory=(LPUPDATERECORDMEMORYPARAM)pMessage->eventdata;
	_ASSERT(sizeof(UPDATERECORDMEMORYPARAM)==pMessage->eventsize);
	_ASSERT(IsRecordVoice(RECORD_MEMORY));
	_ASSERT(IsCallStates());
	if(IsCallStates())
	{
		return 1;
	}
	return 0;
}

int ChannelInfo::OnSendFsk(const LPCHANNELEVENT pMessage)
{
	LPSENDFSKPARAM lpsendfskparam=(LPSENDFSKPARAM)pMessage->eventdata;
	_ASSERT(sizeof(SENDFSKPARAM)==pMessage->eventsize);

	_ASSERT(IsSendFSK());

	ModifyOperationFlag(OF_NONE,OF_SENDFSK);

	if(IsCallStates())
	{
		return 1;
	}
	return 0;
}

int ChannelInfo::OnRecvFsk(const LPCHANNELEVENT pMessage)
{
	LPRECVFSKPARAM lprecvfskparam=(LPRECVFSKPARAM)pMessage->eventdata;
	_ASSERT(sizeof(RECVFSKPARAM)==pMessage->eventsize);
	_ASSERT(IsRecvFSK());
	if(IsCallStates())
	{
		if(lprecvfskparam->result!=1)
		{
			ModifyOperationFlag(OF_NONE,OF_RECVFSK);
		}
		return 1;
	}
	else
	{
		ModifyOperationFlag(OF_NONE,OF_RECVFSK);
	}
	return 0;
}


UINT ChannelInfo::ModifyOperationFlag(OperationFlag addflag,OperationFlag removeflag)
{
	LOCKSCOPE(m_lock);
	UINT operationflag=m_operationflag;
	m_operationflag|=addflag;
	m_operationflag&=~removeflag;
	return operationflag;
}

void ChannelInfo::SetPlayVoice(PLAYATTRIBUTE playattribute)
{
    LOCKSCOPE(m_lock);
    if(playattribute==PLAY_NONE)
    {
        m_operationflag&=~OF_PLAYSOUND;
    }
    else
    {
        m_operationflag|=OF_PLAYSOUND;
    }
    m_playattribute=playattribute;
}

void ChannelInfo::SetRecordVoice(RECORDATTRIBUTE recordattribute)
{
    LOCKSCOPE(m_lock);
    if(recordattribute==RECORD_NONE)
    {
        m_operationflag&=~OF_RECORDSOUND;
    }
    else
    {
        m_operationflag|=OF_RECORDSOUND;
    }
    m_recordattribute=recordattribute;
}

void ChannelInfo::SetSendTone(TONEATTRIBUTE toneattribute)
{
    LOCKSCOPE(m_lock);
    if(toneattribute==TONE_NONE)
    {
        m_operationflag&=~OF_SENDTONE;
    }
    else
    {
        m_operationflag|=OF_SENDTONE;
    }
    m_sendtoneattribute=toneattribute;

}

void ChannelInfo::SetRecvTone(TONEATTRIBUTE toneattribute)
{
    LOCKSCOPE(m_lock);
    if(toneattribute==TONE_NONE)
    {
        m_operationflag&=~OF_RECVTONE;
    }
    else
    {
        m_operationflag|=OF_RECVTONE;
    }
    m_recvtoneattribute=toneattribute;
}

PLAYATTRIBUTE ChannelInfo::GetPlayVoice()
{
    LOCKSCOPE(m_lock);
    return m_playattribute;
}

RECORDATTRIBUTE ChannelInfo::GetRecordVoice()
{
    LOCKSCOPE(m_lock);
    return m_recordattribute;
}

TONEATTRIBUTE ChannelInfo::GetSendTone()
{
    LOCKSCOPE(m_lock);
    return m_sendtoneattribute;
}

TONEATTRIBUTE ChannelInfo::GetRecvTone()
{
    LOCKSCOPE(m_lock);
    return m_recvtoneattribute;
}

bool ChannelInfo::IsSendDTMF()
{
	LOCKSCOPE(m_lock);
	return m_operationflag&OF_SENDDTMF ? true : false;
}

bool ChannelInfo::IsRecvDTMF()
{
	LOCKSCOPE(m_lock);
	return m_operationflag&OF_RECVDTMF ? true : false;
}

bool ChannelInfo::IsSendFSK()
{
	LOCKSCOPE(m_lock);
	return m_operationflag&OF_SENDFSK ? true : false;
}

bool ChannelInfo::IsRecvFSK()
{
	LOCKSCOPE(m_lock);
	return m_operationflag&OF_RECVFSK ? true : false;
}
bool ChannelInfo::IsPlayVoice(PLAYATTRIBUTE playattribute)
{
	LOCKSCOPE(m_lock);
    if(m_operationflag&OF_PLAYSOUND)
    {
        if(playattribute==PLAY_NONE)
        {
            return true;
        }
        else if(m_playattribute==playattribute)
        {
            return true;
        }
    }
	return  false;
}

bool ChannelInfo::IsRecordVoice(RECORDATTRIBUTE recordattribute)
{
    LOCKSCOPE(m_lock);
    if(m_operationflag&OF_RECORDSOUND)
    {
        if(recordattribute==RECORD_NONE)
        {
            return true;
        }
        else if(m_recordattribute==recordattribute)
        {
            return true;
        }
    }
    return  false;

}

bool ChannelInfo::IsSendTone(TONEATTRIBUTE toneattribute)
{
    LOCKSCOPE(m_lock);
    if(m_operationflag&OF_SENDTONE)
    {
        if(toneattribute==TONE_NONE)
        {
            return true;
        }
        else if((m_sendtoneattribute&toneattribute)==toneattribute)
        {
            return true;
        }
    }
    return  false;
}

bool ChannelInfo::IsRecvTone(TONEATTRIBUTE toneattribute)
{
    LOCKSCOPE(m_lock);
    if(m_operationflag&OF_RECVTONE)
    {
        if(toneattribute==TONE_NONE)
        {
            return true;
        }
        else if((m_recvtoneattribute&toneattribute)==toneattribute)
        {
            return true;
        }
    }
    return  false;
}

bool ChannelInfo::IsSound()
{
	LOCKSCOPE(m_lock);
	return m_operationflag&(OF_SENDDTMF|OF_SENDFSK|OF_PLAYSOUND|OF_SENDTONE) ? true : false;
}


int ChannelInfo::GetID()
{
	LOCKSCOPE(m_lock);
	return m_chid;
}

void ChannelInfo::SetType(CHANNELTYPE chtype)
{
    LOCKSCOPE(m_lock);
    m_chtype=chtype;
}

CHANNELTYPE ChannelInfo::GetType()
{
	LOCKSCOPE(m_lock);
	return m_chtype;
}



bool ChannelInfo::IsDigitalTrunkCh()
{
	LOCKSCOPE(m_lock);
	return m_chtype&(TYPE_SS7_TUP|TYPE_SS7_ISUP|TYPE_DSS1_USER|TYPE_DSS1_NET|TYPE_SS1) ? true : false;
}

bool ChannelInfo::IsAnalogTrunkCh()
{
	LOCKSCOPE(m_lock);
	return m_chtype&(TYPE_ANALOG_TRUNK|TYPE_ANALOG_USER) ? true : false;
}

bool ChannelInfo::IsTrunkCh()
{
	LOCKSCOPE(m_lock);
	return (IsDigitalTrunkCh() || IsAnalogTrunkCh()) ? true : false;
}

bool ChannelInfo::IsRecordCh()
{
	LOCKSCOPE(m_lock);
	return m_chtype&(TYPE_ANALOG_RECORD) ? true : false;
}

bool ChannelInfo::IsResourceCh()
{
	LOCKSCOPE(m_lock);
	return m_chtype&(TYPE_ANALOG_RECORD|TYPE_FAX) ? true : false;
}

bool ChannelInfo::IsUsableCh()
{
	LOCKSCOPE(m_lock);
	return m_chtype&(TYPE_ANALOG_EMPTY|TYPE_UNKNOWN) ? false : true;
}


void  ChannelInfo::SetState(CHANNELSTATE chstate)
{
	LOCKSCOPE(m_lock);
	m_chpreviousstate=m_chstate;
	m_chstate=chstate;

	m_chstarttime=COMMON::GetMillisecond();
}
CHANNELSTATE  ChannelInfo::GetState()
{
	LOCKSCOPE(m_lock);
	return m_chstate;
}
CHANNELSTATE  ChannelInfo::GetPreviousState()
{
	LOCKSCOPE(m_lock);
	return m_chpreviousstate;
}

UINT  ChannelInfo::GetKeepTime()
{
	LOCKSCOPE(m_lock);
	return (UINT)(COMMON::GetMillisecond()-m_chstarttime);
}


//STATE_DIALUP STATE_RINGBACK
BOOL  ChannelInfo::IsCalloutStates()
{
    LOCKSCOPE(m_lock);
	return (m_chstate==STATE_DIALUP || m_chstate==STATE_RINGBACK);
}

//STATE_CALLIN STATE_RINGING
BOOL  ChannelInfo::IsCallInStates()
{
    LOCKSCOPE(m_lock);
	return (m_chstate==STATE_CALLIN || m_chstate==STATE_RINGING);
}

//STATE_DIALUP STATE_RINGBACK STATE_CALLIN STATE_RINGING
BOOL  ChannelInfo::IsJoinStates()
{
    LOCKSCOPE(m_lock);
	return (m_chstate==STATE_DIALUP || m_chstate==STATE_RINGBACK ||  m_chstate==STATE_CALLIN ||  m_chstate==STATE_RINGING);

}

//STATE_DIALUP STATE_RINGBACK STATE_CALLIN STATE_RINGING STATE_TALKING
BOOL  ChannelInfo::IsCallStates()
{
	LOCKSCOPE(m_lock);
	return (m_chstate==STATE_DIALUP || m_chstate==STATE_RINGBACK ||  m_chstate==STATE_CALLIN ||  m_chstate==STATE_RINGING || m_chstate==STATE_TALKING);
}

//STATE_DIALUP STATE_RINGBACK STATE_CALLIN STATE_RINGING STATE_TALKING STATE_HANGUP STATE_RELEASE
BOOL  ChannelInfo::IsWorkStates()
{
	LOCKSCOPE(m_lock);
	return (m_chstate==STATE_DIALUP || m_chstate==STATE_RINGBACK ||  m_chstate==STATE_CALLIN ||  m_chstate==STATE_RINGING || m_chstate==STATE_TALKING || m_chstate==STATE_HANGUP || m_chstate==STATE_RELEASE);

}




void  ChannelInfo::SetWay(CHANNELWAY chway)
{
	LOCKSCOPE(m_lock);
    m_chway=chway;
}

CHANNELWAY  ChannelInfo::GetWay()
{
	LOCKSCOPE(m_lock);
    return m_chway;
}




void ChannelInfo::SetCalled(LPCTSTR telnumber)
{
	LOCKSCOPE(m_lock);
    _tcsncpy(m_chcalled,telnumber,sizeof(m_chcalled)/sizeof(TCHAR)-1);
}

void ChannelInfo::SetCalling(LPCTSTR telnumber)
{
	LOCKSCOPE(m_lock);
    _tcsncpy(m_chcalling,telnumber,sizeof(m_chcalling)/sizeof(TCHAR)-1);
}

void ChannelInfo::SetOrigCalled(LPCTSTR telnumber)
{
	LOCKSCOPE(m_lock);
    _tcsncpy(m_chorigcalled,telnumber,sizeof(m_chorigcalled)/sizeof(TCHAR)-1);
}

LPCTSTR ChannelInfo::GetCalled()
{
	LOCKSCOPE(m_lock);
    return m_chcalled;
}

LPCTSTR ChannelInfo::GetCalling()
{
	LOCKSCOPE(m_lock);
    return m_chcalling;
}

LPCTSTR ChannelInfo::GetOrigCalled()
{
	LOCKSCOPE(m_lock);
    return m_chorigcalled;
}

void ChannelInfo::ClearDtmf()
{
	LOCKSCOPE(m_lock);
#if _MSC_VER>1200
    m_chdtmf.clear();
#else
    m_chdtmf.erase(m_chdtmf.begin(), m_chdtmf.end());
#endif
}

LPCTSTR ChannelInfo::GetDtmf()
{
	LOCKSCOPE(m_lock);
    return m_chdtmf.c_str();
}

void   ChannelInfo::AppendDtmf(TCHAR str)
{
	LOCKSCOPE(m_lock);
    m_chdtmf.push_back(str);
}

LPCTSTR ChannelInfo::GetRemark()
{
	LOCKSCOPE(m_lock);
    return m_chremark.c_str();
}

void    ChannelInfo::SetRemark(LPCTSTR remark)
{
	LOCKSCOPE(m_lock);
    m_chremark=remark;
}

LPCTSTR ChannelInfo::GetInternalstate()
{
	LOCKSCOPE(m_lock);
    return m_chinternalstate.c_str();
}

void    ChannelInfo::SetInternalstate(LPCTSTR internalstate)
{
	LOCKSCOPE(m_lock);
    m_chinternalstate=internalstate;
}

LPCTSTR ChannelInfo::GetChIDStr()
{
	LOCKSCOPE(m_lock);
    return m_chidstr.c_str();
}

void    ChannelInfo::InsertInitiative(int Initiative)
{
    LOCKSCOPE(m_lock);
    m_chinitiative.push_back(Initiative);
}

void    ChannelInfo::RemoveInitiative(int Initiative)
{
    LOCKSCOPE(m_lock);
    m_chinitiative.erase(std::find(m_chinitiative.begin(),m_chinitiative.end(),Initiative));
}

BOOL    ChannelInfo::IsInitiative(int Initiative)
{
    LOCKSCOPE(m_lock);
    return (std::find(m_chinitiative.begin(),m_chinitiative.end(),Initiative)!=m_chinitiative.end());
}
int     ChannelInfo::GetInitiativeSize()
{
    LOCKSCOPE(m_lock);
    return (int)m_chinitiative.size();
}


void ChannelInfo::GetInitiative(std::vector<int>& Initiative)
{
    LOCKSCOPE(m_lock);
    for(std::list<int>::iterator Iter=m_chinitiative.begin();Iter!=m_chinitiative.end();++Iter)
    {
        Initiative.push_back(*Iter);
    }
}

void    ChannelInfo::ClearInitiative()
{
    LOCKSCOPE(m_lock);
    m_chinitiative.clear();
}

void    ChannelInfo::InsertPassively(int Passively)
{
    LOCKSCOPE(m_lock);
    m_chpassively.push_back(Passively);
}

void    ChannelInfo::RemovePassively(int Passively)
{
    LOCKSCOPE(m_lock);
    m_chpassively.erase(std::find(m_chpassively.begin(),m_chpassively.end(),Passively));
}

BOOL    ChannelInfo::IsPassively(int Passively)
{
    LOCKSCOPE(m_lock);
    return (std::find(m_chpassively.begin(),m_chpassively.end(),Passively)!=m_chpassively.end());
}

int     ChannelInfo::GetPassivelySize()
{
    LOCKSCOPE(m_lock);
    return (int)m_chpassively.size();
}

void    ChannelInfo::GetPassively(std::vector<int>& Passively)
{
    LOCKSCOPE(m_lock);
    for(std::list<int>::iterator Iter=m_chpassively.begin();Iter!=m_chpassively.end();++Iter)
    {
        Passively.push_back(*Iter);
    }
}

void     ChannelInfo::ClearPassively()
{
    LOCKSCOPE(m_lock);
    m_chpassively.clear();

}

bool    ChannelInfo::IsDelayID(UINT delayid)
{
    LOCKSCOPE(m_lock);
    return std::find(m_delayidlist.begin(),m_delayidlist.end(),delayid)!=m_delayidlist.end();
}


void    ChannelInfo::AddDelayID(UINT delayid)
{
    LOCKSCOPE(m_lock);
    m_delayidlist.push_back(delayid);
}

void    ChannelInfo::DelDelayID(UINT delayid)
{
	LOCKSCOPE(m_lock);

	std::vector<UINT>::iterator delayiter=std::find(m_delayidlist.begin(),m_delayidlist.end(),delayid);
	if(delayiter!=m_delayidlist.end())
	{
		m_delayidlist.erase(delayiter);
	}

}

int     ChannelInfo::GetDelayID(UINT** ppdelayid)
{
    LOCKSCOPE(m_lock);

    if(!m_delayidlist.empty())
    {
        *ppdelayid=&m_delayidlist[0];
    }
    
    return m_delayidlist.size();
}

void    ChannelInfo::ClearDelayID()
{
    LOCKSCOPE(m_lock);
    m_delayidlist.clear();
}



void ChannelInfo::Reset()
{
    LOCKSCOPE(m_lock);

    _tcsnset(m_chcalled,_T('\0'),sizeof(m_chcalled)/sizeof(TCHAR));
    _tcsnset(m_chcalling,_T('\0'),sizeof(m_chcalling)/sizeof(TCHAR));
    _tcsnset(m_chorigcalled,_T('\0'),sizeof(m_chorigcalled)/sizeof(TCHAR));

    ClearDtmf();
   
}

#pragma warning(default:4996)
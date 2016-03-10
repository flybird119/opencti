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
	filename: 	e:\project\opencti.code\trunk\002\src\cti\CtiInterface.cpp
	file path:	e:\project\opencti.code\trunk\002\src\cti
	file base:	CtiInterface
	file ext:	cpp
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/

#include "StdAfx.h"
#include "CtiInterface.h"
#include "CtiEventTrace.h"






struct DelayedInfotag
{
	int                 ch;
	CHANNELEVENTTYPE    evtid;
	LPVOID              evtdata;
	int                 evtsize;
};



CtiInterface::CtiInterface(void)
:m_processormgr(m_eventtrace)
,m_eventtrace(m_channelmgr)
,m_psysprocessor(NULL)
,m_threadmode(0)
{

}

CtiInterface::~CtiInterface(void)
{

}





LRESULT CtiInterface::OnDelayed(MessageData* pdata,BOOL& bHandle)
{

    const DelayedInfotag& DelayInfo=UseMessageData<DelayedInfotag>(pdata);
    
    if(DelayInfo.evtid==EVENT_SYSEVENT)
    {
        NotifySysEvent(DelayInfo.ch,DelayInfo.evtdata,DelayInfo.evtsize);
    }
    else if(DelayInfo.evtid>=EVENT_USEREVENT && DelayInfo.evtid<EVENT_TOTAL)
    {
        ChannelInfo*pChannel = m_channelmgr.FindChannel(DelayInfo.ch);
        _ASSERT(pChannel);
        NotifyChEvent(pChannel,DelayInfo.evtid,DelayInfo.evtdata,DelayInfo.evtsize,pdata->id_,IsMessageOnce(pdata->id_));
    }

    return 0;
}


int CtiInterface::Init(DeviceDescriptor* pctidevicedescriptor)
{
   
    pctidevicedescriptor->monitorwnd=m_eventtrace.ShowMonitor(::GetModuleHandle(NULL),pctidevicedescriptor->monitorwnd);

    if(m_psysprocessor==NULL)
    {
        m_threadmode=pctidevicedescriptor->threadmode;
        m_processormgr.SetThreadMode(m_threadmode);
        m_psysprocessor=new ChannelProcessor;
        m_psysprocessor->SetCallback(pctidevicedescriptor->notifyfun,pctidevicedescriptor->notifyparam);
        HANDLE hEvent=m_processormgr.GetSingleProcessor()->AddChannel(m_psysprocessor);
        _ASSERT(hEvent!=INVALID_HANDLE_VALUE);
    }
    int ret=CDeviceInterface::Init(pctidevicedescriptor);
    return ret;
}

void CtiInterface::Term()
{
	CDeviceInterface::Term();
	
    m_eventtrace.Clear();
    m_channelmgr.ClearChannel();
	m_processormgr.ClearProcessor();

    if(m_psysprocessor)
    {
        delete m_psysprocessor;
        m_psysprocessor=NULL;

    }


}

int CtiInterface::GetLastErrorCode()
{
	if(m_lasterror.GetLastErrorType()==0)
	{
		return m_lasterror.GetLastErrorCode();
	}
	return CDeviceInterface::GetLastErrorCode();
}

void CtiInterface::SetLastErrorCode(int code)
{
	m_lasterror.SetLastErrorCode(code);
}


LPCTSTR CtiInterface::FormatErrorCode(int code)
{
	if(m_lasterror.GetLastErrorType()==0)
	{
		return m_lasterror.FormatErrorCode(code);
	}
	return CDeviceInterface::FormatErrorCode(code);
}



CHANNELTYPE CtiInterface::GetChType(int ch)
{
    VERIFY(TYPE_UNKNOWN,Addr_GetChType)
    return pChannel->GetType();
}


int CtiInterface::SetChOwnerData(int ch,DWORD OwnerData)
{
    VERIFY(0,Addr_SetChOwnerData)
    pChannel->SetOwnerData(OwnerData);
    return 1;
}

DWORD CtiInterface::GetChOwnerData(int ch)
{
    VERIFY(0,Addr_GetChOwnerData)
    return pChannel->GetOwnerData();
}

CHANNELSTATE CtiInterface::GetChState(int ch)
{
	VERIFY(STATE_UNKNOWN,Addr_GetChState)
	return pChannel->GetState();
}

int  CtiInterface::Dialup(int ch,LPCTSTR calling,NUMBERATTRIBUTE callingattribute,LPCTSTR called,LPCTSTR origcalled,NUMBERATTRIBUTE origcalledattribute,BOOL CallerIDBlock,int flags)
{
	VERIFY(0,Addr_Dialup)
	return CDeviceInterface::Dialup(ch,calling,callingattribute,called,origcalled,origcalledattribute,CallerIDBlock,flags);
}




int  CtiInterface::Pickup(int ch,int flags)
{
	VERIFY(0,Addr_Pickup)
	return CDeviceInterface::Pickup(ch,flags);
}

int  CtiInterface::Ringback(int ch)
{
	VERIFY(0,Addr_Ringback)
	return CDeviceInterface::Ringback(ch);
}

int  CtiInterface::Hangup(int ch,RELEASEATTRIBUTE attribute)
{
	VERIFY(0,Addr_Hangup)
	if(CDeviceInterface::Hangup(ch,attribute))
	{
		NotifyRelease(pChannel);

		if(pChannel->GetType()==TYPE_ANALOG_USER)
		{
			NotifyIdle(pChannel);
		}
		return 1;
	}

	return 0;
}


int  CtiInterface::SendDtmf(int ch,LPCTSTR dtmf,int len,int flags)
{
    VERIFY(0,Addr_SendDtmf)
    if(CDeviceInterface::SendDtmf(ch,dtmf,len,flags))
    {
        pChannel->ModifyOperationFlag(OF_SENDDTMF,OF_NONE);
        return 1;
    }
    return 0;
}

int  CtiInterface::StopSendDtmf(int ch)
{
    VERIFY(0,Addr_StopSendDtmf)
    if(CDeviceInterface::StopSendDtmf(ch))
    {
        return 1;
    }
    return 0;

}

int  CtiInterface::IsSendDtmf(int ch)
{
    VERIFY(0,Addr_IsSendDtmf)
    return (pChannel->IsSendDTMF() ? 1 : 0);
}


int  CtiInterface::RecvDtmf(int ch,int time,int flags)
{
    VERIFY(0,Addr_RecvDtmf)
    if(CDeviceInterface::RecvDtmf(ch,time,flags))
    {
        pChannel->ModifyOperationFlag(OF_RECVDTMF,OF_NONE);
        return 1;
    }
    return 0;
}

int  CtiInterface::StopRecvDtmf(int ch)
{
    VERIFY(0,Addr_StopRecvDtmf)
    if(CDeviceInterface::StopRecvDtmf(ch))
    {
        return 1;
    }
    return 0;
}

int  CtiInterface::IsRecvDtmf(int ch)
{
    VERIFY(0,Addr_IsRecvDtmf)
    return (pChannel->IsRecvDTMF() ? 1 : 0);
}


int  CtiInterface::SendFsk(int ch,const BYTE* fsk,int len,int flags)
{
    VERIFY(0,Addr_SendFsk)
    if(CDeviceInterface::SendFsk(ch,fsk,len,flags))
    {
        pChannel->ModifyOperationFlag(OF_SENDFSK,OF_NONE);
        return 1;
    }
    return 0;
}

int  CtiInterface::SendFskFull(int ch,int sync,int mark,const BYTE* fsk,int len,int flags)
{
    VERIFY(0,Addr_SendFskFull)
    if(CDeviceInterface::SendFskFull(ch,sync,mark,fsk,len,flags))
    {
        pChannel->ModifyOperationFlag(OF_SENDFSK,OF_NONE);
        return 1;
    }
    return 0;
}

int  CtiInterface::StopSendFsk(int ch)
{
    VERIFY(0,Addr_StopSendFsk)
    if(CDeviceInterface::StopSendFsk(ch))
    {
        return 1;
    }
    return 0;
}

int  CtiInterface::IsSendFsk(int ch)
{
    VERIFY(0,Addr_IsSendFsk)
    return (pChannel->IsSendFSK() ? 1 : 0);
}


int  CtiInterface::RecvFsk(int ch,int time,int flags)
{
    VERIFY(0,Addr_RecvFsk)
    if(CDeviceInterface::RecvFsk(ch,time,flags))
    {
        pChannel->ModifyOperationFlag(OF_RECVFSK,OF_NONE);
        return 1;
    }
    return 0;
}

int  CtiInterface::StopRecvFsk(int ch)
{
    VERIFY(0,Addr_StopRecvFsk)
    if(CDeviceInterface::StopRecvFsk(ch))
    {
        return 1;
    }
    return 0;
}

int  CtiInterface::IsRecvFsk(int ch)
{
    VERIFY(0,Addr_IsRecvFsk)
    return (pChannel->IsRecvFSK() ? 1 : 0);
}




int CtiInterface::PlayFile(int ch,LPCTSTR fullpath,LPCTSTR StopstrDtmfCharSet)
{
    VERIFY(0,Addr_PlayFile)
    if(CDeviceInterface::PlayFile(ch,fullpath,StopstrDtmfCharSet))
    {
        pChannel->SetPlayVoice(PLAY_FILE);
        return 1;
    }
    return 0;
}

int CtiInterface::PlayMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet)
{
    VERIFY(0,Addr_PlayMemory)
    if(CDeviceInterface::PlayMemory(ch,block1,size1,block2,size2,StopstrDtmfCharSet))
    {
        pChannel->SetPlayVoice(PLAY_MEMORY);
        return 1;
    }
    return 0;
}

int CtiInterface::UpDatePlayMemory(int ch,int index,BYTE* block,int size)
{
    VERIFY(0,Addr_UpDatePlayMemory)
    return CDeviceInterface::UpDatePlayMemory(ch,index,block,size);
}


int CtiInterface::PlayQueue(int ch,LPCTSTR fullpath[],int size,LPCTSTR StopstrDtmfCharSet)
{
    VERIFY(0,Addr_PlayQueue)
    if(CDeviceInterface::PlayQueue(ch,fullpath,size,StopstrDtmfCharSet))
    {
        pChannel->SetPlayVoice(PLAY_QUEUE);
        return 1;
    }
    return 0;
}

int CtiInterface::PlayIndex(int ch,LPCTSTR Alias,LPCTSTR StopstrDtmfCharSet)
{
    VERIFY(0,Addr_PlayIndex)
    if(CDeviceInterface::PlayIndex(ch,Alias,StopstrDtmfCharSet))
    {
        pChannel->SetPlayVoice(PLAY_INDEX);
        return 1;
    }
    return 0;
}




int CtiInterface::StopPlay(int ch)
{
    VERIFY(0,Addr_StopPlay)
    if(CDeviceInterface::StopPlay(ch))
    {
        return 1;
    }
    return 0;
}

int CtiInterface::IsPlay(int ch)
{
    VERIFY(0,Addr_IsPlay)
    return (pChannel->IsPlayVoice(PLAY_NONE) ? 1 : 0);
}



int CtiInterface::RecordFile(int ch,LPCTSTR fullpath,int time,LPCTSTR StopstrDtmfCharSet)
{
    VERIFY(0,Addr_RecordFile)
    if(CDeviceInterface::RecordFile(ch,fullpath,time,StopstrDtmfCharSet))
    {
        pChannel->SetRecordVoice(RECORD_FILE);
        return 1;
    }
    return 0;
}

int CtiInterface::RecordMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet)
{
    VERIFY(0,Addr_RecordMemory)
    if(CDeviceInterface::RecordMemory(ch,block1,size1,block2,size2,StopstrDtmfCharSet))
    {
        pChannel->SetRecordVoice(RECORD_MEMORY);
        return 1;
    }
    return 0;
}

int CtiInterface::StopRecord(int ch)
{
    VERIFY(0,Addr_StopRecord)
    if(CDeviceInterface::StopRecord(ch))
    {
        return 1;
    }
    return 0;
}

int CtiInterface::IsRecord(int ch)
{
    VERIFY(0,Addr_IsRecord)
    return (pChannel->IsRecordVoice(RECORD_NONE) ? 1 : 0);
}

int CtiInterface::UpDateRecordMemory(int ch,int index,BYTE* block,int size)
{
    VERIFY(0,Addr_UpDateRecordMemory)
    return CDeviceInterface::UpDateRecordMemory(ch,index,block,size);
}



int CtiInterface::Listen(int initiativech,int passivelych)
{
    ChannelInfo* pinitiativech=m_verify.VerifyCh(initiativech,Addr_Listen);
    if(!pinitiativech) return 0;
    ChannelInfo* ppassivelych=m_verify.VerifyCh(passivelych,Addr_Listen);
    if(!ppassivelych) return 0;

    if(pinitiativech->GetInitiativeSize()>0      || 
        pinitiativech->IsInitiative(passivelych) ||
        initiativech==passivelych)
    {
        m_lasterror.SetLastErrorCode(ERRCODE_OPERATIONALCONFICT);
        return 0;
    }

    if(CDeviceInterface::Listen(initiativech,passivelych))
    {
        pinitiativech->InsertInitiative(passivelych);
        ppassivelych->InsertPassively(initiativech);
        return 1;
    }
    return 0;
}

int CtiInterface::UnListen(int initiativech,int passivelych)
{
    ChannelInfo* pinitiativech=m_verify.VerifyCh(initiativech,Addr_UnListen);
    if(!pinitiativech) return 0;
    ChannelInfo* ppassivelych=m_verify.VerifyCh(passivelych,Addr_UnListen);
    if(!ppassivelych) return 0;



    if(!pinitiativech->IsInitiative(passivelych) ||
        !ppassivelych->IsPassively(initiativech) ||
        initiativech==passivelych)
    {
        m_lasterror.SetLastErrorCode(ERRCODE_NOIMPLEMENTEDOPERATE);
        return 0;
    }

    if(CDeviceInterface::UnListen(initiativech,passivelych))
    {
        pinitiativech->RemoveInitiative(passivelych);
        ppassivelych->RemovePassively(initiativech);
        return 1;
    }
    return 0;
}





int CtiInterface::SendTone(int ch,TONEATTRIBUTE attribute,int time,int flags)
{
    VERIFY(0,Addr_SendTone)

    if(CDeviceInterface::SendTone(ch,attribute,time,flags))
    {
        pChannel->SetSendTone(attribute);
        return 1;
    }
    return 0;
}

int CtiInterface::SendToneEx(int ch, int nFreq1, int nVolume1, int nFreq2, int nVolume2,DWORD dwOnTime, DWORD dwOffTime,int time,int flags)
{
    VERIFY(0,Addr_SendToneEx)
    if(CDeviceInterface::SendToneEx(ch,nFreq1,nVolume1,nFreq2,nVolume2,dwOnTime,dwOffTime,time,flags))
    {
        pChannel->SetSendTone(TONE_CUSTOM);
        return 1;
    }
    return 0;
}

int CtiInterface::StopSendTone(int ch)
{
    VERIFY(0,Addr_StopSendTone)

    if( CDeviceInterface::StopSendTone(ch))
    {
        return 1;
    }
    return 0;
}

int CtiInterface::IsSendTone(int ch)
{
    VERIFY(0,Addr_IsSendTone)
    return (pChannel->IsSendTone(TONE_NONE) ? 1 : 0);
}


int CtiInterface::RecvTone(int ch,TONEATTRIBUTE attribute,int time,int flags)
{
    VERIFY(0,Addr_RecvTone)
    if(pChannel->IsRecvTone(attribute))
    {
        m_lasterror.SetLastErrorCode(ERRCODE_PREVIOUSNOTFINISH);
        return 0;
    }

    if(CDeviceInterface::RecvTone(ch,attribute,time,flags))
    {
        pChannel->SetRecvTone(attribute);
        return 1;
    }
    return 0;
}


int CtiInterface::StopRecvTone(int ch,TONEATTRIBUTE attribute)
{
    VERIFY(0,Addr_StopRecvTone)
    if(pChannel->IsRecvTone(attribute))
    {
        if(CDeviceInterface::StopRecvTone(ch,attribute))
        {
            return 1;
        }
    }
    else
    {
        m_lasterror.SetLastErrorCode(ERRCODE_NOIMPLEMENTEDOPERATE);
    }
    return 0;
}


int  CtiInterface::IsRecvTone(int ch,TONEATTRIBUTE attribute)
{
    VERIFY(0,Addr_IsRecvTone)
    return (pChannel->IsRecvTone(attribute) ? 1 : 0);
}


int  CtiInterface::DelayChEvent(int cmsDelay,BOOL once,int ch,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize)
{
    VERIFY(0,Addr_DelayChEvent)
    // eventid 必须大于EVENT_USEREVENT
    if(eventid<EVENT_USEREVENT)
    {
        m_lasterror.SetLastErrorCode(ERRCODE_PARAMILLEGAL);
        return 0;
    }

    DelayedInfotag delayedmsg;
    delayedmsg.ch=ch;
    delayedmsg.evtid=eventid;
    delayedmsg.evtdata=eventdata;
    delayedmsg.evtsize=eventsize;
    UINT delayid = this->DelayMessage(cmsDelay,once,EVT_CTI_DELAYED,WrapMessageData<DelayedInfotag>(delayedmsg));
	pChannel->AddDelayID(delayid);

	return delayid;
}

int  CtiInterface::DelaySysEvent(int cmsDelay,BOOL once,int identify,LPVOID eventdata,int eventsize)
{
    DelayedInfotag delayedmsg;
    delayedmsg.ch=identify;
    delayedmsg.evtid=EVENT_SYSEVENT;
    delayedmsg.evtdata=eventdata;
    delayedmsg.evtsize=eventsize;
    return this->DelayMessage(cmsDelay,once,EVT_CTI_DELAYED,WrapMessageData<DelayedInfotag>(delayedmsg));

}



int  CtiInterface::InsertChEvent(int ch,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize)
{
	VERIFY(0,Addr_InsertChEvent)
	// eventid 必须大于EVENT_USEREVENT
	if(eventid<EVENT_USEREVENT)
	{
		m_lasterror.SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}


	LPUSEREVENTPARAM lpusereventparam=(LPUSEREVENTPARAM)malloc(sizeof(USEREVENTPARAM));
	lpusereventparam->delayid=0;
	lpusereventparam->once=TRUE;
	lpusereventparam->eventdata=eventdata;
	lpusereventparam->eventsize=eventsize;
	CHANNELEVENT *pMessage=new CHANNELEVENT(ch,eventid,lpusereventparam,sizeof(USEREVENTPARAM),NULL,NULL);
	pChannel->InsertMessage(pMessage);

	return 1;
}


int  CtiInterface::CancelDelay(int ch,int delayid)
{

    if(ch!=-1)
    {
        VERIFY(0,Addr_CancelDelay)

		if(delayid)
		{
			pChannel->DelDelayID(delayid);
		}
		else
		{
			UINT* pdelayid;
			int delaysize=pChannel->GetDelayID(&pdelayid);
			for(int i=0;i<delaysize;++i)
			{
				CancelMessage(pdelayid[i]);
			}
			pChannel->ClearDelayID();
			return 1;
		}

    }

	if(!CMessageHandler::CancelMessage(delayid))
	{
		m_lasterror.SetLastErrorCode(ERRCODE_INVALIDEDELAY);
		return 0;
	}
	return 1;
}


int CtiInterface::BindExclusiveThread(int ch)
{
    VERIFY(0,Addr_BindExclusiveThread)
    m_processormgr.GetSingleProcessor()->AddChannel(pChannel);
	return 1;
}

int CtiInterface::UnBindExclusiveThread(int ch)
{
    VERIFY(0,Addr_UnBindExclusiveThread)
    pChannel->ComebackProcessor();
	return 1;
}



void CtiInterface::PauseCh(int ch)
{
	ChannelInfo*pChannel = m_channelmgr.FindChannel(ch);
	_ASSERT(pChannel);

	pChannel->PauseProcessor();

}

void CtiInterface::ResumeCh(int ch)
{
	ChannelInfo*pChannel = m_channelmgr.FindChannel(ch);
	_ASSERT(pChannel);

	pChannel->ResumeProcessor();
}




int  CtiInterface::GetSelectCh()
{
    return m_eventtrace.GetSelectCh();
}

void  CtiInterface::ResetCh(int ch)
{
    ChannelInfo*pChannel = m_channelmgr.FindChannel(ch);
    _ASSERT(pChannel);


    UINT* pdelayid;
    int delaysize=pChannel->GetDelayID(&pdelayid);

    for(int i=0;i<delaysize;++i)
    {
        CancelMessage(pdelayid[i]);
    }
    pChannel->ClearDelayID();

    std::vector<int> initiative;
    std::vector<int> passively;
    pChannel->GetInitiative(initiative);
    pChannel->GetPassively(passively);

    for(std::vector<int>::iterator Iter=initiative.begin();Iter!=initiative.end();++Iter)
    {
        UnListen(ch,*Iter);
    }

    for(std::vector<int>::iterator Iter=passively.begin();Iter!=passively.end();++Iter)
    {
        UnListen(*Iter,ch);
    }

    if(IsSendTone(ch))
    {
        StopSendTone(ch);
    }

    if(IsSendDtmf(ch))
    {
        StopSendDtmf(ch);
    }

    if(IsRecvDtmf(ch))
    {
        StopRecvDtmf(ch);
    }

    if(IsSendFsk(ch))
    {
        StopSendFsk(ch);
    }

    if(IsRecvFsk(ch))
    {
        StopRecvFsk(ch);
    }

    if(IsPlay(ch))
    {
        StopPlay(ch);
    }

    if(IsRecord(ch))
    {
        StopRecord(ch);
    }

    pChannel->Reset();

	CDeviceInterface::ResetCh(ch);
}






void  CtiInterface::SendNotify(int ch,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize)
{
    CHANNELEVENT *pMessage=new CHANNELEVENT(ch,eventid,eventdata,eventsize,NULL,NULL);

	if(eventid!=EVENT_SYSEVENT)
	{
		ChannelInfo*pChannel = m_channelmgr.FindChannel(pMessage->chid);
		_ASSERT(pChannel);

		switch(eventid)
		{
		case EVENT_ADDCHANNEL:
			{
				pChannel->SetCallback(m_eventfun,m_eventparm);
				HANDLE hEvent=m_processormgr.GetMultipleProcessor()->AddChannel(pChannel);
				_ASSERT(hEvent!=INVALID_HANDLE_VALUE);
			}
			break;
		}
		pChannel->PushMessage(pMessage);
	}
	else
	{
		m_psysprocessor->PushMessage(pMessage);
	}
}
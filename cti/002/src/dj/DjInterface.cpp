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
	created:	14:6:2009   11:38
	filename: 	e:\project\cti\002\src\dj\DjInterface.cpp
	file path:	e:\project\cti\002\src\dj
	file base:	DjInterface
	file ext:	cpp
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/

#include "StdAfx.h"
#include "DjInterface.h"


#include "log/LogMessage.h"


#include "tce1_32.h"

//#define LOG_INIT(...)
//#define TraceLog(...)
//#define EventLog(...)
//#define LEVEL_SENSITIVE
#define TraceLog(LEVEL,...)    TRACE_FULL("CDjInterface",LEVEL)<<__VA_ARGS__
#define EventLog(LEVEL,...)    TRACE_FULL("CDjInterface",LEVEL)<<__VA_ARGS__





#pragma warning(disable:4996)




struct Simulatetag
{
	int              ch;
	CHANNELEVENTTYPE evt;
	DWORD            param;
};






#define		TRANSFCALL(FUNNAME,...)			\
	ICtiBase* pcti=NULL;					\
	int origch=GetCti(ch,&pcti);			\
	_ASSERT(ch!=-1 && pcti!=NULL);			\
	return pcti->FUNNAME(origch,__VA_ARGS__);			



		


CDjInterface::CDjInterface()
:CMessageHandler(new CThreadMessageQueue)
{
	LOG_INIT("CDjInterface",TRUE,LEVEL_SENSITIVE,100*1024*1024);
}

CDjInterface::~CDjInterface()
{

	CMessageQueue*pmsgqueue=this->GetMessageQueue();
	this->SetMessageQueue(NULL);
	delete pmsgqueue;

	TraceLog(LEVEL_WARNING,"CDjInterface 释构");
}


LRESULT CDjInterface::OnSimulateTimer(MessageData* pdata,BOOL& bHandle)
{
	const Simulatetag& simulate= UseMessageData<Simulatetag>(pdata);
	const int& ch=simulate.ch;
	DjChannelInfo* pch=GetChInfo(ch);
	const CHANNELEVENTTYPE&evt=simulate.evt;

	TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" "<<EventDescription(simulate.evt));
	switch(evt)
	{
	case EVENT_RECVDTMF:
		{
			_ASSERT(pch->recvdtmf.timer==pdata->id_);

			if(pch->recvdtmf.timer==pdata->id_)
				pch->recvdtmf.timer=0;

			if(pch->IsRecvDTMF())
			{
				NotifyRecvDtmf(pch,0,0);
			}
		}
		break;

	case EVENT_RECVFSK:
		{
			_ASSERT(pch->recvfsk.timer==pdata->id_);

			if(pch->recvfsk.timer==pdata->id_)
				pch->recvfsk.timer=0;

			if(pch->IsRecvFSK())
			{
				NotifyRecvFsk(pch,0,0,0);
			}
		}

	}

	return 0;
}




int CDjInterface::Init(DeviceDescriptor* pctidevicedescriptor)
{
	int ref=0;

	m_eventfun=pctidevicedescriptor->notifyfun;
	m_eventparm=pctidevicedescriptor->notifyparam;



#ifdef DJISUP
	CDjIsup* pisup=new CDjIsup(this->GetMessageQueue(),this);
	if(pisup->Init(pctidevicedescriptor)!=0)
	{
		ChannelCtiMap ctimap;
		if(!m_ctimap.empty())
		{
			ChannelCtiMap& lastctimap=m_ctimap.back();
			ctimap.offset=lastctimap.offset+lastctimap.total;

		}
		else
		{
			ctimap.offset=0;

		}
		ctimap.pcti=pisup;
		ctimap.total=pisup->GetChTotal();
		m_ctimap.push_back(ctimap);
		++ref;
	}
#endif

#ifdef DJANALOG
	CDjAnalog* panalog=new CDjAnalog(this->GetMessageQueue(),this);
	if(panalog->Init(pctidevicedescriptor)!=0)
	{
		ChannelCtiMap ctimap;
		if(!m_ctimap.empty())
		{
			ChannelCtiMap& lastctimap=m_ctimap.back();
			ctimap.offset=lastctimap.offset+lastctimap.total;
		}
		else
		{
			ctimap.offset=0;
		}

		ctimap.pcti=panalog;
		ctimap.total=panalog->GetChTotal();
		m_ctimap.push_back(ctimap);
		++ref;
	}
#endif

	return ref;
}

void CDjInterface::Term()
{
	for(std::vector<ChannelCtiMap>::iterator iter=m_ctimap.begin();iter!=m_ctimap.end();++iter)
	{
		iter->pcti->Term();
		delete iter->pcti;
	}
	m_ctimap.clear();
	return ;
}


BOOL CDjInterface::Features(FUNCTIONADDRINDEX descriptor)
{
	return FALSE;
}

void* CDjInterface::GetNativeAPI(LPCTSTR funname)
{
	return NULL;
}

int CDjInterface::GetLastErrorCode()
{

	BYTE errtype=m_lasterror.GetLastErrorType();
	for(std::vector<ChannelCtiMap>::iterator iter=m_ctimap.begin();iter!=m_ctimap.end();++iter)
	{
		if(errtype==iter->pcti->m_lasterror.GetLastErrorType())
		{
			return iter->pcti->GetLastErrorCode();
		}
	}
	return 0;
}

LPCTSTR CDjInterface::FormatErrorCode(int code)
{

	BYTE errtype=m_lasterror.GetLastErrorType();
	for(std::vector<ChannelCtiMap>::iterator iter=m_ctimap.begin();iter!=m_ctimap.end();++iter)
	{
		if(errtype==iter->pcti->m_lasterror.GetLastErrorType())
		{
			return iter->pcti->FormatErrorCode(code);
		}
	}
	return _T("");
	
}

CHANNELTYPE CDjInterface::GetChType(int ch)
{
	TRANSFCALL(GetChType)
}

int CDjInterface::GetChTotal()
{
	int total=0;
	for(std::vector<ChannelCtiMap>::iterator iter=m_ctimap.begin();iter!=m_ctimap.end();++iter)
	{
		total+=iter->total;
	}
	return total;
}

int CDjInterface::SearchIdleCh(CHANNELTYPE chtype, int* pChFilter, int FilterLen,int*pChExcept,int ExceptLen)
{
	for(std::vector<ChannelCtiMap>::iterator iter=m_ctimap.begin();iter!=m_ctimap.end();++iter)
	{
		int ret=iter->pcti->SearchIdleCh(chtype,pChFilter,FilterLen,pChExcept,ExceptLen);
		if(ret!=-1)
			return ret;
	}
	return -1;
}




//接续函数


int  CDjInterface::Dialup(int ch,LPCTSTR calling,NUMBERATTRIBUTE callingattribute,LPCTSTR called,LPCTSTR origcalled,NUMBERATTRIBUTE origcalledattribute,BOOL CallerIDBlock,int flags)
{
	TRANSFCALL(Dialup,calling,callingattribute,called,origcalled,origcalledattribute,CallerIDBlock,flags);
}


int  CDjInterface::Pickup(int ch,int flags)
{
	TRANSFCALL(Pickup,flags)
}


int  CDjInterface::Ringback(int ch)
{
	TRANSFCALL(Ringback)
}

int  CDjInterface::Hangup(int ch,RELEASEATTRIBUTE attribute)
{
	TRANSFCALL(Hangup,attribute)
}



//DTMF函数

int  CDjInterface::SendDtmf(int ch,LPCTSTR dtmf,int len,int flags)
{
	TRANSFCALL(SendDtmf,dtmf,len,flags)
}
int  CDjInterface::StopSendDtmf(int ch)
{
	TRANSFCALL(StopSendDtmf)
}

int  CDjInterface::RecvDtmf(int ch,int time,int flags)
{
	//TRANSFCALL(RecvDtmf,time,flags)
	DjChannelInfo* pch=GetChInfo(ch);
	if(time>0)
	{
		Simulatetag simulate;
		simulate.ch=ch;
		simulate.evt=EVENT_RECVDTMF;
		simulate.param=0;
		pch->recvdtmf.timer=CMessageHandler::DelayMessage(time,TRUE,EVT_SIMULATETIMER,WrapMessageData<Simulatetag>(simulate));;
		pch->recvdtmf.elapse=time;

	}
	else
	{
		pch->recvdtmf.timer=0;
	}
	return 1;
}
int  CDjInterface::StopRecvDtmf(int ch)
{
	//TRANSFCALL(StopRecvDtmf)

	DjChannelInfo* pch=GetChInfo(ch);

	if(pch->recvdtmf.timer)
	{
		CMessageHandler::CancelMessage(pch->recvdtmf.timer);
		pch->recvdtmf.timer=0;
	}

	NotifyRecvDtmf(pch,2,0);
	return 1;
}



//FSK函数

int  CDjInterface::SendFsk(int ch,const BYTE* fsk,int len,int flags)
{
	TRANSFCALL(SendFsk,fsk,len,flags)
}
int  CDjInterface::SendFskFull(int ch,int sync,int mark,const BYTE* fsk,int len,int flags)
{
	TRANSFCALL(SendFskFull,sync,mark,fsk,len,flags)
}
int  CDjInterface::StopSendFsk(int ch)
{
	TRANSFCALL(StopSendFsk)
}

int  CDjInterface::RecvFsk(int ch,int time,int flags)
{
//	TRANSFCALL(RecvFsk,time,flags)

	ICtiBase* pcti=NULL;
	int origch=GetCti(ch,&pcti);
	_ASSERT(ch!=-1 && pcti!=NULL);

	DjChannelInfo* pch=GetChInfo(ch);

	if(pcti->RecvFsk(ch,time,flags))
	{
		if(time)
		{
			Simulatetag simulate;
			simulate.ch=ch;
			simulate.evt=EVENT_RECVFSK;
			simulate.param=0;
			pch->recvfsk.timer=CMessageHandler::DelayMessage(time,TRUE,EVT_SIMULATETIMER,WrapMessageData<Simulatetag>(simulate));;
			pch->recvfsk.elapse=time;

		}

		return 1;
	}
	
	return 0;
}

int  CDjInterface::StopRecvFsk(int ch)
{
//	TRANSFCALL(StopRecvFsk)
	
	ICtiBase* pcti=NULL;
	int origch=GetCti(ch,&pcti);
	_ASSERT(ch!=-1 && pcti!=NULL);

	DjChannelInfo* pch=GetChInfo(ch);

	if(pcti->StopRecvFsk(ch))
	{
		if(pch->recvfsk.timer)
		{
			CMessageHandler::CancelMessage(pch->recvfsk.timer);
			pch->recvfsk.timer=0;
		}
		NotifyRecvFsk(pch,2,0,0);
		return 1;
	}

	return 0;
}


//放音函数

int CDjInterface::PlayFile(int ch,LPCTSTR fullpath,LPCTSTR StopstrDtmfCharSet)
{
	TRANSFCALL(PlayFile,fullpath,StopstrDtmfCharSet)
}
int CDjInterface::PlayMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet)
{
	TRANSFCALL(PlayMemory,block1,size1,block2,size2,StopstrDtmfCharSet)
}
int CDjInterface::PlayQueue(int ch,LPCTSTR fullpath[],int size,LPCTSTR StopstrDtmfCharSet)
{
	TRANSFCALL(PlayQueue,fullpath,size,StopstrDtmfCharSet)
}

int CDjInterface::LoadPlayIndex(LPCTSTR fullpath,LPCTSTR Alias)
{
	for(std::vector<ChannelCtiMap>::iterator iter=m_ctimap.begin();iter!=m_ctimap.end();++iter)
	{
		if(iter->pcti->LoadPlayIndex(fullpath,Alias)==0)
		{
			return 0;
		}
	}

	return 1;
}

int CDjInterface::PlayIndex(int ch,LPCTSTR Alias,LPCTSTR StopstrDtmfCharSet)
{
	TRANSFCALL(PlayIndex,Alias,StopstrDtmfCharSet)
}

int CDjInterface::StopPlay(int ch)
{
	TRANSFCALL(StopPlay)
}

int CDjInterface::UpDatePlayMemory(int ch,int index,BYTE* block,int size)
{
	TRANSFCALL(UpDatePlayMemory,index,block,size)
}



//录音函数
int CDjInterface::RecordFile(int ch,LPCTSTR fullpath,int time,LPCTSTR StopstrDtmfCharSet)
{
	TRANSFCALL(RecordFile,fullpath,time,StopstrDtmfCharSet)
}
int CDjInterface::RecordMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet)
{
	TRANSFCALL(RecordMemory,block1,size1,block2,size2,StopstrDtmfCharSet)
}
int CDjInterface::StopRecord(int ch)
{
	TRANSFCALL(StopRecord)
}

int CDjInterface::UpDateRecordMemory(int ch,int index,BYTE* block,int size)
{
	TRANSFCALL(UpDateRecordMemory,index,block,size)
}




//TONE函数

int CDjInterface::SendTone(int ch,TONEATTRIBUTE attribute,int time,int flags)
{
	TRANSFCALL(SendTone,attribute,time,flags)
}
int CDjInterface::SendToneEx(int ch, int nFreq1, int nVolume1, int nFreq2, int nVolume2,DWORD dwOnTime, DWORD dwOffTime,int time,int flags)
{
	TRANSFCALL(SendToneEx,nFreq1,nVolume1,nFreq2,nVolume2,dwOnTime,dwOffTime,time,flags)
}
int CDjInterface::StopSendTone(int ch)
{
	TRANSFCALL(StopSendTone)
}

int CDjInterface::RecvTone(int ch,TONEATTRIBUTE attribute,int time,int flags)
{
	TRANSFCALL(RecvTone,attribute,time,flags)
}
int CDjInterface::StopRecvTone(int ch,TONEATTRIBUTE attribute)
{
	TRANSFCALL(StopRecvTone,attribute)
}


int CDjInterface::Listen(int initiativech,int passivelych)
{
	ICtiBase* pcti1=NULL;
	int originitiativech=GetCti(initiativech,&pcti1);
	_ASSERT(originitiativech!=-1 && pcti1!=NULL);


	ICtiBase* pcti2=NULL;
	int origpassivelych=GetCti(passivelych,&pcti2);
	_ASSERT(origpassivelych!=-1 && pcti2!=NULL);


	if(pcti1==pcti2)
	{
		return pcti1->Listen(originitiativech,origpassivelych);
	}
	else
	{
		DjChannelInfo*pinitiativech=GetChInfo(initiativech);
		DjChannelInfo*ppassivelych=GetChInfo(passivelych);
		if(pinitiativech->IsDigitalTrunkCh() && ppassivelych->IsDigitalTrunkCh())
		{
			return ::DJExg_SetListenTrunkToTrunk(originitiativech,origpassivelych);
		}
		else if(pinitiativech->IsAnalogTrunkCh() && ppassivelych->IsAnalogTrunkCh())
		{
			return ::DJExg_SetListenUserToUser(originitiativech,origpassivelych);
		}
		else if(pinitiativech->IsDigitalTrunkCh() && ppassivelych->IsAnalogTrunkCh())
		{
			return ::DJExg_SetListenTrunkToUser(originitiativech,origpassivelych);
		}
		else if(pinitiativech->IsAnalogTrunkCh() && ppassivelych->IsDigitalTrunkCh())
		{
			return ::DJExg_SetListenUserToTrunk(originitiativech,origpassivelych);
		}
		else
		{
			_ASSERT(0);
		}
	}

	return 0;
}
int CDjInterface::UnListen(int initiativech,int passivelych)
{
	ICtiBase* pcti1=NULL;
	int originitiativech=GetCti(initiativech,&pcti1);
	_ASSERT(originitiativech!=-1 && pcti1!=NULL);


	ICtiBase* pcti2=NULL;
	int origpassivelych=GetCti(passivelych,&pcti2);
	_ASSERT(origpassivelych!=-1 && pcti2!=NULL);


	if(pcti1==pcti2)
	{
		return pcti1->UnListen(originitiativech,origpassivelych);
	}
	else
	{
		DjChannelInfo*pinitiativech=GetChInfo(initiativech);
		DjChannelInfo*ppassivelych=GetChInfo(passivelych);
		if(pinitiativech->IsDigitalTrunkCh() && ppassivelych->IsDigitalTrunkCh())
		{
			return ::DJExg_ClearListenTrunkFromTrunk(originitiativech);
		}
		else if(pinitiativech->IsAnalogTrunkCh() && ppassivelych->IsAnalogTrunkCh())
		{
			return ::DJExg_ClearListenUserFromUser(originitiativech);
		}
		else if(pinitiativech->IsDigitalTrunkCh() && ppassivelych->IsAnalogTrunkCh())
		{
			return ::DJExg_ClearListenTrunkFromUser(originitiativech);
		}
		else if(pinitiativech->IsAnalogTrunkCh() && ppassivelych->IsDigitalTrunkCh())
		{
			return ::DJExg_ClearListenUserFromTrunk(originitiativech);
		}
		else
		{
			_ASSERT(0);
		}
	}

	return 0;
}

void  CDjInterface::ResetCh(int ch)
{
	TRANSFCALL(ResetCh)
}



int CDjInterface::GetCti(int ch,ICtiBase** ppcti)
{
	for(std::vector<ChannelCtiMap>::iterator iter=m_ctimap.begin();iter!=m_ctimap.end();++iter)
	{
		if(ch>=iter->offset && ch<iter->total+iter->offset && ch>=0)
		{
			*ppcti=iter->pcti;
			return ch-iter->offset;
		}
	}
	return -1;
}



int CDjInterface::GetCh(ICtiBase* pcti,int origch)
{
	for(std::vector<ChannelCtiMap>::iterator iter=m_ctimap.begin();iter!=m_ctimap.end();++iter)
	{
		if(iter->pcti==pcti && iter->total>origch && origch>=0)
		{
			return origch+iter->offset;
		}
	}
	return -1;
}


void CDjInterface::UpDateRecvDtmf(DjChannelInfo*pch)
{
	if(pch->recvdtmf.timer)
	{
		CMessageHandler::CancelMessage(pch->recvdtmf.timer);
		Simulatetag simulate;
		simulate.ch=pch->GetID();
		simulate.evt=EVENT_RECVDTMF;
		simulate.param=0;
		pch->recvdtmf.timer=CMessageHandler::DelayMessage(pch->recvdtmf.elapse,TRUE,EVT_SIMULATETIMER,WrapMessageData<Simulatetag>(simulate));
	}
}

DjChannelInfo* CDjInterface::GetChInfo(int ch)
{
	return (DjChannelInfo*)m_channelmgr.FindChannel(ch);
}




#pragma warning(default:4996)
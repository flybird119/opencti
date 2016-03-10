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
	created:	14:6:2009   11:09
	filename: 	e:\project\cti\002\src\dj\DjAnalog.cpp
	file path:	e:\project\cti\002\src\dj
	file base:	DjAnalog
	file ext:	cpp
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "DjAnalog.h"

#include "DjInterface.h"

#include "log/LogMessage.h"




#include "NewSig.h"
#include "Tc08a32.h"
#include "fsk_mix.h"

#pragma comment(lib,"NewSig.lib")
#pragma comment(lib,"Tc08a32.lib")
#pragma comment(lib,"fsk_mix.lib")


//#define LOG_INIT(...)
//#define TraceLog(...)
//#define EventLog(...)
//#define LEVEL_SENSITIVE
#define TraceLog(LEVEL,...)    TRACE_FULL("CDjAnalog",LEVEL)<<__VA_ARGS__
#define EventLog(LEVEL,...)    TRACE_FULL("CDjAnalog",LEVEL)<<__VA_ARGS__




CDjAnalog::CDjAnalog(CMessageQueue* pMessageQueue,CDjInterface* pdjinterface)
:CMessageHandler(pMessageQueue)
,m_totalline(0)
,m_channelmgr(pdjinterface->m_channelmgr)
,m_pdjinterface(pdjinterface)
,m_lasterror(pdjinterface->m_lasterror)
{
	LOG_INIT("CDjAnalog",TRUE,LEVEL_SENSITIVE,100*1024*1024);
}


CDjAnalog::~CDjAnalog()
{
	TraceLog(LEVEL_WARNING,"CDjAnalog 释构");
}


LRESULT CDjAnalog::OnInitial(MessageData* pdata,BOOL& bHandle)
{
	for(int i=0;i<m_totalline;++i)
	{		

		::InitDtmfBuf(i);

		DjAnalogChannelInfo*pch=new DjAnalogChannelInfo(m_pdjinterface->GetCh(this,i));
		m_channelmgr.AddChannel(pch);
		pch->Reset();
		pch->SetType(CDjAnalog::GetChType(i));
		CHANNELTYPE chtype=pch->GetType();
		CHANNELSTATE chstate=STATE_IDLE;
		CHANNELWAY chway=WAY_IDLE;
		NotifyAddChannel(pch,chtype,chstate,chway,m_pdjinterface->GetChTotal());
	}
	DelayMessage(100,FALSE,EVT_POLL,NULL);
	return 0;
}

LRESULT CDjAnalog::OnPoll(MessageData* pdata,BOOL& bHandle)
{
#if !defined(DJDIGITAL)
	::PUSH_PLAY();
	::FeedSigFunc();
#endif
	for(int i=0;i<m_totalline;++i)
	{
		PollJunction(i,GetChInfo(i));
	}
	return 0;
}


void   CDjAnalog::PollJunction(int ch,DjAnalogChannelInfo* pch)
{
	switch(pch->GetState())
	{
	case STATE_IDLE:
			ListenCallin(ch,pch);
		break;
	case STATE_CALLIN:
	case STATE_RINGING:
	case STATE_TALKING:
			ListenHangup(ch,pch);
		break;
	case STATE_DIALUP:
			ListenDialup(ch,pch);
		break;
	case STATE_RINGBACK:
			ListenPick(ch,pch);
		break;
	case STATE_SLEEP:
			ListenRecvCID(ch,pch);
		break;
	case STATE_HANGUP:
			NotifyIdle(pch);
		break;
	case STATE_LOCAL_BLOCK:
	case STATE_REMOTE_BLOCK:
	case STATE_UNKNOWN:
	case STATE_INITIAL:
			_ASSERT(0);
		break;
	}


	CheckDtmf(ch,pch);
	CheckFsk(ch,pch);
	CheckPlay(ch,pch);
	CheckRecord(ch,pch);
	CheckTone(ch,pch);
	CheckFlash(ch,pch);
}





void   CDjAnalog::CheckDtmf(int ch,DjAnalogChannelInfo* pch)
{
	if(::DtmfHit(ch))
	{
		TCHAR DTMF;
		DTMF= this->GetDtmfCode(ch);

		if(DTMF!=0)
		{
			if(pch->IsRecvDTMF())
			{
				m_pdjinterface->UpDateRecvDtmf(pch);
				NotifyRecvDtmf(pch,1,DTMF);
			}

			if(pch->IsPlayVoice(PLAY_NONE))
			{
				for(int i=0;i<pch->play.StopPlayDtmfCharlen;++i)
				{
					if(pch->play.StopPlayDtmfCharSet[i]==DTMF)
					{
						StopPlay(ch);
						pch->play.StopPlayDtmfChar=DTMF;
						break;
					}
				}
			}


			if(pch->IsRecordVoice(RECORD_NONE))
			{
				for(int i=0;i<pch->record.StopRecordDtmfCharlen;++i)
				{
					if(pch->record.StopRecordDtmfCharSet[i]==DTMF)
					{
						StopRecord(ch);
						pch->record.StopRecordDtmfChar=DTMF;
						break;
					}
				}
			}
		}
	}



	if(pch->IsSendDTMF())
	{
		if(::CheckSendEnd(ch))
		{
			if(pch->senddtmf.MtStop)
				NotifySendDtmf(pch,2);
			else
				NotifySendDtmf(pch,1);
		}
	}

}

void   CDjAnalog::CheckFsk(int ch,DjAnalogChannelInfo* pch)
{

	if(pch->IsSendFSK())
	{
		if(::DJFsk_CheckSendFSKEnd(ch,FSK_CH_TYPE_160)==1)
		{
			if(pch->sendfsk.MtStop)
			{
				NotifySendFsk(pch,2);
			}
			else
			{
				::DJFsk_StopSend(ch,FSK_CH_TYPE_160);
				NotifySendFsk(pch,1);
			}
		}
	}


	if(pch->IsRecvFSK())
	{
		static BYTE fskbuf[1024];
		int fsklen=DJFsk_GetFSK(ch,fskbuf,FSK_CH_TYPE_160);
		if(fsklen>0)
			NotifyRecvFsk(pch,1,fskbuf,fsklen);
	}
		
}

void   CDjAnalog::CheckPlay(int ch,DjAnalogChannelInfo* pch)
{
	if(pch->IsPlayVoice(PLAY_NONE))
	{
		BOOL finish=FALSE;
		PLAYATTRIBUTE attribute;
		

		if(pch->IsPlayVoice(PLAY_FILE))
		{
			if(::CheckPlayEnd(ch))
			{
				finish=TRUE;
				attribute=PLAY_FILE;
			}
		}
		else if(pch->IsPlayVoice(PLAY_MEMORY))
		{
			if(::CheckPlayEnd(ch))
			{

				if(pch->play.StopPlayDtmfChar==0 && !pch->play.MtStop)
				{
					BYTE* buf=NULL;
					int   size=0;
					if(pch->playmemory.index%2)
					{
						buf=pch->playmemory.block2;
						size=pch->playmemory.size2;
					}
					else 
					{
						buf=pch->playmemory.block1;
						size=pch->playmemory.size1;
					}

					if(buf!=NULL && size>0)
					{
						::StartPlay(ch,(char*)buf,0,size);


						NotifyUpdatePlayMemory(pch,pch->playmemory.index,pch->playmemory.block1,pch->playmemory.size1,pch->playmemory.block2,pch->playmemory.size2);
						if(pch->playmemory.index%2)
						{
							pch->playmemory.block1=0;
							pch->playmemory.size1=0;
						}
						else 
						{
							pch->playmemory.block2=0;
							pch->playmemory.size2=0;
						}

						++pch->playmemory.index;
					}
					else
					{
						finish=TRUE;
						attribute=PLAY_MEMORY;
					}
					
				
				}
				else
				{
					finish=TRUE;
					attribute=PLAY_MEMORY;
				}
			}
		}
		else if(pch->IsPlayVoice(PLAY_INDEX))
		{
			if(::CheckPlayEnd(ch))
			{
				finish=TRUE;
				attribute=PLAY_INDEX;
			}
		}
		else if(pch->IsPlayVoice(PLAY_QUEUE))
		{
			 if(CheckIndexPlayFile(ch))
			 {
				 finish=TRUE;
				 attribute=PLAY_QUEUE;
			 }
		}

		if(finish)
		{
			if(pch->play.StopPlayDtmfChar)
			{
				NotifyPlay(pch,3,attribute,pch->play.StopPlayDtmfChar);
			}
			else if(pch->play.MtStop)
			{
				NotifyPlay(pch,2,attribute,0);
			}
			else
			{
				StopPlay(ch);
				NotifyPlay(pch,1,attribute,0);
			}
		}

	}
}

void   CDjAnalog::CheckRecord(int ch,DjAnalogChannelInfo* pch)
{
	if(pch->IsRecordVoice(RECORD_NONE))
	{
		BOOL finish=FALSE;
		RECORDATTRIBUTE attribute;

		if(pch->IsRecordVoice(RECORD_FILE))
		{
			if(::CheckRecordEnd(ch))
			{
				finish=TRUE;
				attribute=RECORD_FILE;

			}
		}

		if(finish)
		{
			if(pch->record.StopRecordDtmfChar)
			{
				NotifyRecord(pch,3,attribute,pch->record.StopRecordDtmfChar);
			}
			else if(pch->record.MtStop)
			{
				NotifyRecord(pch,2,attribute,0);
			}
			else
			{
				StopRecord(ch);
				NotifyRecord(pch,1,attribute,0);
			}
		}
	}

}

void   CDjAnalog::CheckTone(int ch,DjAnalogChannelInfo* pch)
{

}

void   CDjAnalog::CheckFlash(int ch,DjAnalogChannelInfo* pch)
{

}

void   CDjAnalog::ListenCallin(int ch,DjAnalogChannelInfo* pch)
{
	if(::RingDetect(ch))
	{
		switch(pch->GetType())
		{
		case TYPE_ANALOG_USER:
			{
				::StartHangUpDetect(ch);
				TCHAR called[128]=_T("\0");
				GetInternalCID(ch,called);
				NotifyCallin(pch,_T("P"),called,_T("P"));
				
			}
			break;
		case TYPE_ANALOG_TRUNK:
			{
				::ResetCallerIDBuffer(ch);
			}
			break;
		default:
			{
				_ASSERT(0);
			}
			break;
		}
	}
}

void   CDjAnalog::ListenHangup(int ch,DjAnalogChannelInfo* pch)
{
	switch(pch->GetType())
	{
	case TYPE_ANALOG_USER:
		{
			switch(::HangUpDetect(ch))
			{
			case HANG_UP_FLAG_FALSE:     //● 没有挂机
				break;
			case HANG_UP_FLAG_TRUE:      //● 已经挂机（从进入 HANG_UP_FLAG_START 状态开始，挂机时间大于0.5秒。）
				NotifyHangup(pch,RELEASE_NORMAL);           
				break;
			case HANG_UP_FLAG_START:     //● 开始挂机
				break;
			case HANG_UP_FLAG_PRESS_R:   //● 拍了一下叉簧
				{

				}
				break;
			}
		}
		break;
	case TYPE_ANALOG_TRUNK:
		{
		
		}
		break;
	}	
}

void   CDjAnalog::ListenDialup(int ch,DjAnalogChannelInfo* pch)
{
	switch(pch->GetType())
	{
	case TYPE_ANALOG_USER:
		{
			if(::OffHookDetect(ch))
			{
				if( pch->sendcid.cidsetup==SENDCID_SEND)
				{
					::DJFsk_StopSend(ch,FSK_CH_TYPE_160);
				}
				::FeedPower(ch);
				::StartHangUpDetect(ch);
				pch->sendcid.cidsetup=SENDCID_IDLE;
				NotifyRingback(pch);
				NotifyTalking(pch);

			}

			switch(pch->sendcid.cidsetup)
			{
			case SENDCID_IDLE:
				{

				}
				break;
			case SENDCID_RING:
				{
					if(pch->GetKeepTime()>=1000)
					{
						::FeedPower(ch);	
						pch->sendcid.cidsetup=SENDCID_WAIT;
					}
				}
				break;
			case SENDCID_WAIT:
				if(pch->GetKeepTime()>=2000)
				{
					char fsk[256];

					int len=FskCIDCoder(pch->sendcid.cidnumber,fsk);
					BYTE* coderbuf=NULL;
					int   coderlen=FskCoder(300,180,(BYTE*)fsk,len,&coderbuf);
					if(coderbuf!=0) 
					{
						::DJFsk_SendFSKBit(ch,(BYTE*)coderbuf,coderlen,FSK_CH_TYPE_160);
						delete []coderbuf;
					}
					pch->sendcid.cidsetup=SENDCID_SEND;
				}
				break;

			case SENDCID_SEND:
				{
					if(::DJFsk_CheckSendFSKEnd (ch,FSK_CH_TYPE_160)==1 || pch->GetKeepTime()>=4000)
					{
						::DJFsk_StopSend(ch,FSK_CH_TYPE_160);
						::FeedRealRing(ch);
						pch->sendcid.cidsetup=SENDCID_IDLE;
						NotifyRingback(pch);
					}
				}
				break;
			}
		}
		break;
	}
}

void   CDjAnalog::ListenRecvCID(int ch,DjAnalogChannelInfo* pch)
{
	
}

void   CDjAnalog::ListenPick(int ch,DjAnalogChannelInfo* pch)
{
	switch(pch->GetType())
	{
	case TYPE_ANALOG_USER:
		{
			if(::OffHookDetect(ch))
			{
				::FeedPower(ch);
				::StartHangUpDetect(ch);
				NotifyTalking(pch);
			}
		}
		break;
	case CHTYPE_TRUNK:
		{
		}
		break;
	}
}




void CDjAnalog::GetInternalCID(int ch,TCHAR* cid)
{
	TCHAR chstr[10];
	_sntprintf(chstr,10,_T("ch%d"),ch);

	TCHAR path[MAX_PATH]=_T("\0");


	COMMON::FileNameConvCurrentFullPath(path,_T("djanalog.ini"));

	::GetPrivateProfileString(_T("Called"),chstr,_T("P"),cid,128,path);

}

DjAnalogChannelInfo* CDjAnalog::GetChInfo(int ch)
{
	return (DjAnalogChannelInfo*)m_channelmgr.FindChannel(m_pdjinterface->GetCh(this,ch));
}


int CDjAnalog::FskCIDCoder(const char* caller,char* fskbuf)
{
	int cidlen= strlen(caller);

	fskbuf[0]=0x4;
	fskbuf[1]=8+cidlen;

	SYSTEMTIME systime;
	::GetLocalTime(&systime);

	fskbuf[2]=systime.wMonth/10+48;
	fskbuf[3]=systime.wMonth%10+48;
	fskbuf[4]=systime.wDay/10+48;
	fskbuf[5]=systime.wDay%10+48;
	fskbuf[6]=systime.wHour/10+48;
	fskbuf[7]=systime.wHour%10+48;
	fskbuf[8]=systime.wMinute/10+48;
	fskbuf[9]=systime.wMinute%10+48;

	memcpy(fskbuf+10,caller,cidlen);

	BYTE crc=0;
	for(int i=0;i<10+cidlen;++i)
	{
		crc+=fskbuf[i];
	}

	crc=~crc;
	crc+=1;
	fskbuf[10+cidlen]=crc;

	return cidlen+11;
}



int CDjAnalog::FskCoder(int sync,int mark,const BYTE* fsk,int len,BYTE** coderbuf)
{

	if(sync<0 || mark<0 || len<0 ) return 0;


	if(sync==0 && mark==0 && len==0) return 0;

	int  codersize=(len*10);
	codersize+=(sync+mark);
	BYTE* coderbits=new BYTE[codersize/8+(codersize%8 ? 1 :0)];



	int e=0,l=0;

	for(int i=0;i<codersize;++i)
	{
		if(i<sync)  //引导串    
		{
			if(i%2==0)
			{
				coderbits[i/8]&= ~(1<<(i%8));       //0
			}
			else
			{
				coderbits[i/8]|= (1<<(i%8));       //1
			}

		}
		else if(i<sync+mark)        //Mrak串
		{
			coderbits[i/8]|= (1<<(i%8));
		}
		else
		{
			if(e%10==0)         //起始位
				coderbits[i/8]&= ~(1<<(i%8));
			else if(e%10==9)    //停止位
				coderbits[i/8]|= (1<<(i%8));
			else
			{


				if ( ( (fsk[l/8] >> l%8) & 1 ) == 1 )
				{
					coderbits[i/8]|= (1<<(i%8));       //1
				}
				else
				{
					coderbits[i/8]&= ~(1<<(i%8));       //0
				}

				++l;
			}

			++e;
		}



	}


	if(coderbuf)
	{
		*coderbuf=coderbits;
	}
	return codersize;
}


TCHAR CDjAnalog::GetDtmfCode(int ch)
{
	SHORT dret=::GetDtmfCode(ch);
	TCHAR dtmf=0;
	if(dret!=-1)
	{
		switch(dret)
		{
		case 1:		dtmf=_T('1');	break;
		case 2:		dtmf=_T('2');	break;
		case 3:		dtmf=_T('3');	break;
		case 4:		dtmf=_T('4');	break;
		case 5:		dtmf=_T('5');	break;
		case 6:		dtmf=_T('6');	break;
		case 7:		dtmf=_T('7');	break;
		case 8:		dtmf=_T('8');	break;
		case 9:		dtmf=_T('9');	break;
		case 10:	dtmf=_T('0');	break;
		case 11:	dtmf=_T('*');	break;
		case 12:	dtmf=_T('#');	break;
		case 13:	dtmf=_T('A');	break;
		case 14 :	dtmf=_T('B');	break;
		case 15:	dtmf=_T('C');	break;
		case 0:		dtmf=_T('D');	break;
		}
	}
	return dtmf;
}



int CDjAnalog::Init(DeviceDescriptor* pctidevicedescriptor)
{
#if !defined(DJDIGITAL)
	long ret=::LoadDRV();
	if(ret!=0)
	{
		TraceLog(LEVEL_WARNING,"LoadDRV Fail:"<<ret);
		return 0;
	}
#endif
	
	m_totalline = ::CheckValidCh();
	if(m_totalline>0)
	{
#if !defined(DJDIGITAL)
		if ( ::EnableCard(m_totalline,1024*32) != 0L) 
		{
			TraceLog(LEVEL_WARNING,"EnableCard分配语音缓冲区失败!");
		}
		else
#endif
		{
			::ResetIndex();
			//注意，这个参数请根据实际环境设置
			::SetDialPara(1000,4000,350,12);
			::SetBusyPara(350);
			if(0==Sig_Init(0))
			{
				TraceLog(LEVEL_WARNING,"Sig_Init  信号音检测初始化失败]");
			}
			int ifskret=0;
			if((ifskret=::DJFsk_InitForFsk(FSK_CH_TYPE_160))!=1)
			{
				TraceLog(LEVEL_WARNING,"DJFsk_InitForFsk FSK初始化失败]:"<<ifskret);
			}
			m_eventfun=pctidevicedescriptor->notifyfun;
			m_eventparm=pctidevicedescriptor->notifyparam;
			CMessageHandler::DelayMessage(0,TRUE,EVT_INITIAL,0);
			return 1;
		}
	}		
	else
	{
		TraceLog(LEVEL_WARNING,"CheckValidCh Fail:"<<m_totalline);
	}

#if !defined(DJDIGITAL)
	::FreeDRV();
#endif
	return 0;
}


void CDjAnalog::Term()
{
	if(m_totalline>0)
	{
		::DJFsk_Release();
#if !defined(DJDIGITAL)
		::DisableCard();
		::FreeDRV();
#endif
		m_totalline=0;
	}
	m_channelmgr.ClearChannel();
}



int CDjAnalog::GetChTotal()
{
	return m_totalline;
}

CHANNELTYPE CDjAnalog::GetChType(int ch)
{
	CHANNELTYPE channeltype=TYPE_UNKNOWN;

	switch(::CheckChTypeNew(ch))
	{
	case  CHTYPE_USER:
		channeltype=TYPE_ANALOG_USER;
		break;
	case CHTYPE_TRUNK:
		channeltype=TYPE_ANALOG_TRUNK;
		break;
	case CHTYPE_EMPTY:
		channeltype=TYPE_ANALOG_EMPTY;
		break;
	case CHTYPE_RECORD:
		channeltype=TYPE_ANALOG_RECORD;
		break;
	}
	return channeltype;
}

int CDjAnalog::Dialup(int ch,LPCTSTR calling,NUMBERATTRIBUTE callingattribute,LPCTSTR called,LPCTSTR origcalled,NUMBERATTRIBUTE origcalledattribute,BOOL CallerIDBlock,int flags)
{
	DjAnalogChannelInfo* pch=GetChInfo(ch);
	if(!(called && origcalled && origcalled))
	{
		m_lasterror.SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}
	const char *szcalling,*szcalled,*szorigcalled;
#if defined(UNICODE) || defined(_UNICODE)
	COMMON::UNICODEChar  wcalled(called);
	COMMON::UNICODEChar  wcalling(calling);
	COMMON::UNICODEChar  worigcalled(origcalled);
	szcalled=wcalled;
	szcalling=wcalling;
	szorigcalled=worigcalled;
#else
	szcalling=calling;
	szcalled=called;
	szorigcalled=origcalled;
#endif


	int iret=0;
	if(pch->GetType()==TYPE_ANALOG_USER)
	{
		strncpy(pch->sendcid.cidnumber,szcalling,127);
		pch->sendcid.cidnumber[127]=0;
		pch->sendcid.cidsetup=SENDCID_RING;
		::FeedRing(ch);
		iret=1;
	}
	else if(pch->GetType()==TYPE_ANALOG_TRUNK)
	{	
	}
	else
	{
		m_lasterror.SetLastErrorCode(ERRCODE_DEVICENONSUPPORT);
	}

	if(iret)
	{
		NotifyDialup(pch,calling,called,origcalled);
	}
	return iret;
}


int CDjAnalog::Pickup(int ch,int flags)
{
	DjAnalogChannelInfo* pch=GetChInfo(ch);
	int iret=0;
	if(pch->GetType()==TYPE_ANALOG_USER)
	{
		iret=1;
		NotifyTalking(pch);
	}
	else if(pch->GetType()==TYPE_ANALOG_USER)
	{	
	}
	else
	{
		m_lasterror.SetLastErrorCode(ERRCODE_DEVICENONSUPPORT);
	}
	if(iret!=0)
	{
		//pch->pickup.flags=flags;
	}
	return iret;
}


int CDjAnalog::Ringback(int ch)
{
	DjAnalogChannelInfo* pch=GetChInfo(ch);
	int iret=0;
	if(pch->GetType()==TYPE_ANALOG_TRUNK)
	{

	}
	else if(pch->GetType()==TYPE_ANALOG_USER)
	{
		NotifyRinging(pch);
		iret=1;
	}
	else
	{
		m_lasterror.SetLastErrorCode(ERRCODE_DEVICENONSUPPORT);
	}
	return iret;
}


int CDjAnalog::Hangup(int ch,RELEASEATTRIBUTE attribute)
{
	DjAnalogChannelInfo* pch=GetChInfo(ch);
	int iret=0;
	if(pch->GetType()==TYPE_ANALOG_TRUNK)
	{
		
	}
	else if(pch->GetType()==TYPE_ANALOG_USER)
	{

		switch(pch->GetState())
		{
		case STATE_DIALUP:
			{

				switch(pch->sendcid.cidsetup)
				{
				case SENDCID_RING:
					::FeedPower(ch);
					break;
				case SENDCID_WAIT:
					break;
				case SENDCID_SEND:
					::DJFsk_StopSend(ch,FSK_CH_TYPE_160);
					break;
				}
				iret=1;
			}
			break;
		case STATE_RINGBACK:
			{
				::FeedPower(ch);
				iret=1;
			}
			break;
		default:
				_ASSERT(0);
			break;
		}
	}
	else
	{
		m_lasterror.SetLastErrorCode(ERRCODE_DEVICENONSUPPORT);
	}
	return iret;
}


int  CDjAnalog::SendDtmf(int ch,LPCTSTR dtmf,int len,int flags)
{

	if(_tcslen(dtmf)!=len)
	{
		m_lasterror.SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}

	DjAnalogChannelInfo* pch=GetChInfo(ch);


	const char * pdtmf=NULL;
#if defined(UNICODE) || defined(_UNICODE)
	COMMON::UNICODEChar wdtmf(dtmf);
	pdtmf=wdtmf;
#else
	pdtmf=dtmf;
#endif
	if(strlen(pdtmf)>64)
	{
		*((char*)pdtmf+64)=0;
	}

	::SendDtmfBuf(ch,(LPSTR)pdtmf);


	pch->senddtmf.MtStop=FALSE;


	return min(64,strlen(pdtmf));
}

int  CDjAnalog::StopSendDtmf(int ch)
{
	DjAnalogChannelInfo* pch=GetChInfo(ch);

	pch->senddtmf.MtStop=TRUE;
	 ::StopPlay(ch);
	 return 1;
}



int  CDjAnalog::SendFsk(int ch,const BYTE* fsk,int len,int flags)
{
	int iret=0;
	if((iret=::DJFsk_SendFSK(ch,(BYTE*)fsk,len,FSK_CH_TYPE_160))==1)
	{
		DjAnalogChannelInfo* pch=GetChInfo(ch);
		pch->sendfsk.MtStop=FALSE;
		return 1;
	}
	else
	{
		switch(iret)
		{				
		case -2:  
			TraceLog(LEVEL_WARNING,"DJFsk_SendFSKBit:"<<iret<<" trunkID不合法");
			break;
		case -1: 
			TraceLog(LEVEL_WARNING,"DJFsk_SendFSKBit:"<<iret<<" Mode不合法");
			break;
		case -3:
			TraceLog(LEVEL_WARNING,"DJFsk_SendFSKBit:"<<iret<<" wSize超过最大值");
			break;
		default:
			TraceLog(LEVEL_WARNING,"DJFsk_SendFSKBit:"<<iret<<" 放音失败");
			break;	
		}
	}
	m_lasterror.SetLastErrorCode(ERRCODE_DERIVERETFAILED);
	return 0;
}

int  CDjAnalog::SendFskFull(int ch,int sync,int mark,const BYTE* fsk,int len,int flags)
{
	BYTE* coderbuf=NULL;
	int   coderlen=FskCoder(sync,mark,fsk,len,&coderbuf);
	if(coderbuf==0) 
	{
		m_lasterror.SetLastErrorCode(ERRCODE_DERIVERETFAILED);
	}
	else
	{
		int iret=0;
		if((iret=::DJFsk_SendFSKBit(ch,(BYTE*)coderbuf,coderlen,FSK_CH_TYPE_160))!=1)
		{
			m_lasterror.SetLastErrorCode(ERRCODE_DERIVERETFAILED);
			switch(iret)
			{				
			case -2:  
					TraceLog(LEVEL_WARNING,"DJFsk_SendFSKBit:"<<iret<<" trunkID不合法");
				break;
			case -1: 
					TraceLog(LEVEL_WARNING,"DJFsk_SendFSKBit:"<<iret<<" Mode不合法");
				break;
			case -3:
					TraceLog(LEVEL_WARNING,"DJFsk_SendFSKBit:"<<iret<<" wSize超过最大值");
				break;
			default:
					TraceLog(LEVEL_WARNING,"DJFsk_SendFSKBit:"<<iret<<" 放音失败");
				break;	
			}
		}
		else
		{
			DjAnalogChannelInfo* pch=GetChInfo(ch);
			pch->sendfsk.MtStop=FALSE;
		}
		
		delete []coderbuf;
		return iret;
	}
	return 0;
}

int  CDjAnalog::StopSendFsk(int ch)
{
	::DJFsk_StopSend(ch,FSK_CH_TYPE_160);
	DjAnalogChannelInfo* pch=GetChInfo(ch);
	pch->sendfsk.MtStop=TRUE;
	 return 1;
}



int  CDjAnalog::RecvFsk(int ch,int time,int flags)
{
	DJFsk_ResetFskBuffer(ch,FSK_CH_TYPE_160);
	return 1;
}
int  CDjAnalog::StopRecvFsk(int ch)
{
	return 1;
}


int CDjAnalog::PlayFile(int ch,LPCTSTR fullpath,LPCTSTR StopstrDtmfCharSet)
{
	DjAnalogChannelInfo* pch=GetChInfo(ch);

	const char * szfullpath=NULL;
#if defined(UNICODE) || defined(_UNICODE)
	COMMON::UNICODEChar wchar(fullpath);
	szfullpath=wchar;
#else
	szfullpath=fullpath;
#endif

	if(1== ::StartPlayFile(ch,(LPSTR)szfullpath,0))
	{
		pch->play.StopPlayDtmfCharlen=_tcslen(StopstrDtmfCharSet);
		_tcsncpy(pch->play.StopPlayDtmfCharSet,StopstrDtmfCharSet,20);
		pch->play.MtStop=FALSE;
		pch->play.StopPlayDtmfChar=0;
		return 1;
	}
	m_lasterror.SetLastErrorCode(ERRCODE_DERIVERETFAILED);
	return 0;
}


int CDjAnalog::PlayMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet)
{
	DjAnalogChannelInfo* pch=GetChInfo(ch);

	::StartPlay(ch,(char*)block1,0,size1);
	pch->playmemory.index=1;
	pch->playmemory.block1=block1;
	pch->playmemory.size1=size1;
	pch->playmemory.block2=block2;
	pch->playmemory.size2=size2;

	pch->play.StopPlayDtmfCharlen=_tcslen(StopstrDtmfCharSet);
	_tcsncpy(pch->play.StopPlayDtmfCharSet,StopstrDtmfCharSet,20);
	pch->play.MtStop=FALSE;
	pch->play.StopPlayDtmfChar=0;

	return 1;
}

int CDjAnalog::PlayQueue(int ch,LPCTSTR fullpath[],int size,LPCTSTR StopstrDtmfCharSet)
{
	if(size>100)
	{
		m_lasterror.SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}
	::RsetIndexPlayFile(ch);

	for(int i=0;i<size;++i)
	{
		const char * szfullpath=NULL;
#if defined(UNICODE) || defined(_UNICODE)
		COMMON::UNICODEChar wchar(fullpath[i]);
		szfullpath=wchar;
#else
		szfullpath=fullpath[i];
#endif
		if(!::AddIndexPlayFile(ch,(char*)szfullpath))
		{
			m_lasterror.SetLastErrorCode(ERRCODE_DERIVERETFAILED);
			return 0;
		}
	}


	if(!::StartIndexPlayFile(ch))
	{
		m_lasterror.SetLastErrorCode(ERRCODE_DERIVERETFAILED);
		return 0;
	}

	DjAnalogChannelInfo* pch=GetChInfo(ch);

	pch->play.StopPlayDtmfCharlen=_tcslen(StopstrDtmfCharSet);
	_tcsncpy(pch->play.StopPlayDtmfCharSet,StopstrDtmfCharSet,20);
	pch->play.MtStop=FALSE;
	pch->play.StopPlayDtmfChar=0;
	return 1;

}

int CDjAnalog::LoadPlayIndex(LPCTSTR fullpath,LPCTSTR Alias)
{
	int   size=0;
	char* buffer=m_playindexmgr.AddIndex(fullpath,Alias,size);
	if(buffer!=NULL)
	{

		if(::SetIndex(buffer,size))
		{
			return 1;
		}
		else
		{
			m_playindexmgr.PopIndex();
			m_lasterror.SetLastErrorCode(ERRCODE_DERIVERETFAILED);
		}
	}
	else
	{
		m_lasterror.SetLastErrorCode(ERRCODE_PARAMILLEGAL);
	}

	return 0;
}

int CDjAnalog::PlayIndex(int ch,LPCTSTR Alias,LPCTSTR StopstrDtmfCharSet)
{
	std::vector<WORD> playarray;
	LPCTSTR pAlias=Alias;
	for(;;)
	{
		int index=m_playindexmgr.GetIndex(pAlias);
		if(index>=0)
		{
			playarray.push_back((WORD)index);
		}

		int size=_tcslen(pAlias);
		if(*(pAlias+size+1) == _T('\0'))
		{
			break;
		}
		else
		{
			pAlias=pAlias+size+1;
		}

	}

	if(playarray.size()==0)
	{
		m_lasterror.SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}

	WORD* p=(WORD*)&(playarray.front());
	int size=playarray.size();

	::StartPlayIndex(ch,p,size);

	DjAnalogChannelInfo* pch=GetChInfo(ch);

	pch->play.StopPlayDtmfCharlen=_tcslen(StopstrDtmfCharSet);
	_tcsncpy(pch->play.StopPlayDtmfCharSet,StopstrDtmfCharSet,20);
	pch->play.MtStop=FALSE;
	pch->play.StopPlayDtmfChar=0;


	return 1;

}



int CDjAnalog::StopPlay(int ch)
{
	DjAnalogChannelInfo* pch=GetChInfo(ch);

	if(pch->IsPlayVoice(PLAY_FILE))
	{
		::StopPlayFile(ch);
	}
	else if(pch->IsPlayVoice(PLAY_QUEUE))
	{
		::RsetIndexPlayFile(ch);
		::StopIndexPlayFile(ch);
	}
	else if(pch->IsPlayVoice(PLAY_MEMORY) || pch->IsPlayVoice(PLAY_INDEX))
	{
		::StopPlay(ch);
	}
	else
	{
		_ASSERT(0);
	}

	pch->play.MtStop=TRUE;

	return 1;
}

int CDjAnalog::UpDatePlayMemory(int ch,int index,BYTE* block,int size)
{
	DjAnalogChannelInfo* pch=GetChInfo(ch);
	if(index+1==pch->playmemory.index)
	{
		if(block!=NULL && size>0)
		{			
			if(pch->playmemory.index%2)
			{
				pch->playmemory.block2=block;
				pch->playmemory.size2=size;
			}
			else 
			{
				pch->playmemory.block1=block;
				pch->playmemory.size1=size;
			}
			return 1;	
		}
	}
	return 0;
}

int CDjAnalog::RecordFile(int ch,LPCTSTR fullpath,int time,LPCTSTR StopstrDtmfCharSet)
{
	const char * szfullpath=NULL;
#if defined(UNICODE) || defined(_UNICODE)
	COMMON::UNICODEChar wchar(fullpath);
	szfullpath=wchar;
#else
	szfullpath=fullpath;
#endif
	int filesize=-1;
	if(time>0)
	{
		filesize=time*8;
	}
	if(!::StartRecordFile_Ex(ch,(LPSTR)szfullpath,filesize,TRUE))
	{
		m_lasterror.SetLastErrorCode(ERRCODE_DERIVERETFAILED);
		return 0;
	}
	
	DjAnalogChannelInfo* pch=GetChInfo(ch);

	pch->record.StopRecordDtmfCharlen=_tcslen(StopstrDtmfCharSet);
	_tcsncpy(pch->record.StopRecordDtmfCharSet,StopstrDtmfCharSet,20);
	pch->record.MtStop=FALSE;
	pch->record.StopRecordDtmfChar=0;

	return 1;
}

int CDjAnalog::RecordMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet)
{
	m_lasterror.SetLastErrorCode(ERRCODE_DEVICENONSUPPORT);
	return 0;
}

int CDjAnalog::StopRecord(int ch)
{
	::StopRecordFile(ch);
	DjAnalogChannelInfo* pch=GetChInfo(ch);
	pch->record.MtStop=TRUE;
	return 1;
}


int CDjAnalog::UpDateRecordMemory(int ch,int index,BYTE* block,int size)
{
	return 0;
}


int CDjAnalog::Listen(int initiativech,int passivelych)
{
	if(::LinkOneToAnother(initiativech,passivelych)==0)
	{
		return 1;
	}
	else
	{
		m_lasterror.SetLastErrorCode(ERRCODE_DERIVERETFAILED);
	}

	return 0;
}

int CDjAnalog::UnListen(int initiativech,int passivelych)
{
	if(::ClearOneFromAnother(initiativech,passivelych)==0)
	{

		return 1;
	}
	else
	{
		m_lasterror.SetLastErrorCode(ERRCODE_DERIVERETFAILED);
	}

	return 0;
}



void CDjAnalog::ResetCh(int ch)
{
	::InitDtmfBuf(ch);
}



void  CDjAnalog::SendNotify(int ch,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize)
{
	m_pdjinterface->SendNotify(ch,eventid,eventdata,eventsize);
}



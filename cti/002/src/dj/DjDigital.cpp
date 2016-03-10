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
	filename: 	e:\project\cti\002\src\dj\DjDigital.cpp
	file path:	e:\project\cti\002\src\dj
	file base:	DjDigital
	file ext:	cpp
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "DjDigital.h"


#include "log/LogMessage.h"

#include "./DjInterface.h"


#include "tce1_32.h"
#pragma comment(lib,"Tce1_32.lib")

#include "fsk_mix.h"
#pragma comment(lib,"fsk_mix.lib")


//#define LOG_INIT(...)
//#define TraceLog(...)
//#define EventLog(...)
//#define LEVEL_SENSITIVE
#define TraceLog(LEVEL,...)    TRACE_FULL("CDjDigital",LEVEL)<<__VA_ARGS__
#define EventLog(LEVEL,...)    TRACE_FULL("CDjDigital",LEVEL)<<__VA_ARGS__



#define CH					   ch/30,ch%30





CDjDigital::CDjDigital(CMessageQueue* pMessageQueue,CDjInterface* pdjinterface)
:CMessageHandler(pMessageQueue)
,m_totalline(0)
,m_pdjinterface(pdjinterface)
,m_channelmgr(pdjinterface->m_channelmgr)
,m_lasterror(pdjinterface->m_lasterror)
,m_errlastcode(0)
{
	LOG_INIT("CDjDigital",TRUE,LEVEL_SENSITIVE,100*1024*1024);
	ICtiBase::m_lasterror.SetLastErrorType(s_errtype);
}


CDjDigital::~CDjDigital()
{
	TraceLog(LEVEL_WARNING,"CDjDigital 释构");
}

LRESULT CDjDigital::OnPoll(MessageData* pdata,BOOL& bHandle)
{
	::DJSys_PushPlay();
	return 0;
}


void   CDjDigital::Poll(int ch,DjDigitalChannelInfo* pch)
{
	
	CheckDtmf(ch,pch);
	CheckFsk(ch,pch);
	CheckPlay(ch,pch);
	CheckRecord(ch,pch);
	CheckTone(ch,pch);
	CheckFlash(ch,pch);


}


void	CDjDigital::CheckDtmf(int ch,DjDigitalChannelInfo* pch)
{
	while(::DJTrk_GetReceiveDtmfNumNew(ch))
	{
		TCHAR DTMF=toupper(::DJTrk_GetDtmfCodeNew(ch));
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
		if(::DJTrk_CheckDtmfSendEnd(ch))
		{
			if(pch->senddtmf.MtStop)
				NotifySendDtmf(pch,2);
			else
				NotifySendDtmf(pch,1);
		}
	}


}

void	CDjDigital::CheckFsk(int ch,DjDigitalChannelInfo* pch)
{
	if(pch->IsSendFSK())
	{
		if(::DJFsk_CheckSendFSKEnd(ch,FSK_CH_TYPE_E1))
		{
			if(pch->sendfsk.MtStop)
			{
				NotifySendFsk(pch,2);
			}
			else
			{
				::DJFsk_StopSend(ch,FSK_CH_TYPE_E1);
				NotifySendFsk(pch,1);
			}
		}
	}


	if(pch->IsRecvFSK())
	{
		static BYTE fskbuf[1024];
		int fsklen=DJFsk_GetFSK(ch,fskbuf,FSK_CH_TYPE_E1);
		if(fsklen>0)
			NotifyRecvFsk(pch,1,fskbuf,fsklen);
	}
}

void	CDjDigital::CheckPlay(int ch,DjDigitalChannelInfo* pch)
{

	if(pch->IsPlayVoice(PLAY_NONE))
	{
		BOOL finish=FALSE;
		PLAYATTRIBUTE attribute;

		int vocch=DJTrk_GetTrunkPlayID(ch);
		if(vocch!=-1)
		{

			if(pch->IsPlayVoice(PLAY_FILE))
			{
				if(::DJVoc_CheckVoiceEnd(vocch))
				{
					finish=TRUE;
					attribute=PLAY_FILE;
				}
			}
			else if(pch->IsPlayVoice(PLAY_MEMORY))
			{

				if(::DJVoc_CheckVoiceEnd(vocch))
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
							::DJVoc_PlayMemory(vocch,(char*)buf,size);

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
				if(::DJTrk_CheckPlayPromptStrEnd(ch))
				{
					finish=TRUE;
					attribute=PLAY_INDEX;
				}
			}
			else if(pch->IsPlayVoice(PLAY_QUEUE))
			{
				if(::DJVoc_CheckIndexPlayEnd(ch))
				{
					finish=TRUE;
					attribute=PLAY_QUEUE;
				}
			}
		}
		else
		{
			finish=TRUE;
			attribute=pch->GetPlayVoice();
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

void	CDjDigital::CheckRecord(int ch,DjDigitalChannelInfo* pch)
{

	if(pch->IsRecordVoice(RECORD_NONE))
	{
		BOOL finish=FALSE;
		RECORDATTRIBUTE attribute;

		int vocch=DJTrk_GetTrunkRecordID(ch);
		if(vocch!=-1)
		{
			if(pch->IsRecordVoice(RECORD_FILE))
			{
				if(::DJVoc_CheckVoiceEnd(vocch))
				{
					finish=TRUE;
					attribute=RECORD_FILE;
				}
			}
		}
		else
		{
			finish=TRUE;
			attribute=pch->GetRecordVoice();
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

void	CDjDigital::CheckTone(int ch,DjDigitalChannelInfo* pch)
{

}

void	CDjDigital::CheckFlash(int ch,DjDigitalChannelInfo* pch)
{

}



DjDigitalChannelInfo* CDjDigital::GetChInfo(int ch)
{
	return (DjDigitalChannelInfo*)m_channelmgr.FindChannel(m_pdjinterface->GetCh(this,ch));
}

int CDjDigital::FskCoder(int sync,int mark,const BYTE* fsk,int len,BYTE** coderbuf)
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








int CDjDigital::Init(DeviceDescriptor* pctidevicedescriptor)
{

#if defined(DJANALOG)
	::DJSys_UserCall_TC08A32(true);
#endif

	int i = ::DJSys_EnableCard( "", "PROMPT.INI" ) ;
	if ( i != 0 ) {
		TraceLog(LEVEL_WARNING,"Load Driver Fail!") ;
		return 0;
	}
	m_totalline = ::DJTrk_GetTotalTrunkNum();
	if(m_totalline>0)
	{

		
		::DJSys_EnableDtmfSend();
		::DJSys_DisableWarn();

		int ifskret=0;

		if((ifskret=::DJFsk_InitForFsk(FSK_CH_TYPE_E1))!=1)
		{
			TraceLog(LEVEL_WARNING,"DJFsk_InitForFsk FSK初始化失败]:"<<ifskret);
		}


		for(int i=0;i<m_totalline;++i)
		{
			::DJTrk_InitDtmfBufNew(i);
		}

		m_eventfun=pctidevicedescriptor->notifyfun;
		m_eventparm=pctidevicedescriptor->notifyparam;


		return 1;
	}		
	else
	{
		::DJSys_DisableCard();
	}
	return 0;
}


void CDjDigital::Term()
{
	::DJFsk_Release();
	::DJSys_DisableCard();
}


int CDjDigital::GetLastErrorCode()
{
	return m_errlastcode;
}

LPCTSTR CDjDigital::FormatErrorCode(int code)
{
	switch(code)
	{	
	case	_ERR_OK 					:	return _T("没有错误");
	case	_ERR_XmsAllocError			:	return _T("系统内存分配失败");
	case	_ERR_MemAllocError			:	return _T("常规内存分配失败");
	case	_ERR_CanNotOpenFile 		:	return _T("不能打开文件");
	case	_ERR_NoTrunkRes 			:	return _T("没有中继资源");
	case	_ERR_NoUserRes				:	return _T("没有用户资源");
	case	_ERR_NoVoiceRes 			:	return _T("没有语音资源");
	case	_ERR_InvalidID				:	return _T("非法ID号");
	case	_ERR_ReConnDifferChannel	:	return _T("重复联接到不同的通道");
	case	_ERR_OperateTypeErr 		:	return _T("操作类型错");
	case	_ERR_InvalidTrunkStep		:	return _T("非法通道状态");
	case	_ERR_NoConnChannel			:	return _T("没有联接的通道");
	case	_ERR_INIsetErr				:	return _T("配置文件内容不正确");
	case	_ERR_NullPtr				:	return _T("使用了空指针");
	case	_ERR_NoMvipRes				:	return _T("没有MVIP资源");
	case	_ERR_InvalidDTMF			:	return _T("非法DTMF码");
	case	_ERR_LastDtmfNotSendEnd     :	return _T("没有MVIP资源");
	case	_ERR_OpenTCE1Device			:	return _T("打开驱动失败");
	case	_ERR_CheckHardware			:	return _T("未能找到硬件资源");
	case	_ERR_PromptFile				:	return _T("从prompt.ini中读取语音文件时发生错语");
	case	_ERR_DirectPlayXms			:	return _T("保留");
	case	_ERR_FilePosition			:	return _T("Position 大于文件的长度");
	}
	return _T("未知");
}


int CDjDigital::GetChTotal()
{
	return m_totalline;
}




int  CDjDigital::SendDtmf(int ch,LPCTSTR dtmf,int len,int flags)
{
	if(_tcslen(dtmf)!=len)
	{
		m_lasterror.SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}

	DjDigitalChannelInfo* pch=GetChInfo(ch);


	const char * pdtmf=NULL;
#if defined(UNICODE) || defined(_UNICODE)
	COMMON::UNICODEChar wdtmf(dtmf);
	pdtmf=wdtmf;
#else
	pdtmf=dtmf;
#endif

	int ret=::DJTrk_SendDtmfStr(ch,(LPSTR)pdtmf);

	if(ret!=1)
	{
		SetErrFlag(ret);
		return 0;
	}

	pch->senddtmf.MtStop=FALSE;
	return len;
}

int  CDjDigital::StopSendDtmf(int ch)
{
	DjDigitalChannelInfo* pch=GetChInfo(ch);
	pch->senddtmf.MtStop=TRUE;
	::DJVoc_StopPlayFile(ch);
	return 1;
	
}

int  CDjDigital::SendFsk(int ch,const BYTE* fsk,int len,int flags)
{
	int iret=0;
	if((iret=::DJFsk_SendFSK(ch,(BYTE*)fsk,len,FSK_CH_TYPE_E1)))
	{
		DjDigitalChannelInfo* pch=GetChInfo(ch);
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

int  CDjDigital::SendFskFull(int ch,int sync,int mark,const BYTE* fsk,int len,int flags)
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
		if((iret=::DJFsk_SendFSKBit(ch,(BYTE*)coderbuf,coderlen,FSK_CH_TYPE_E1))!=1)
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
			DjDigitalChannelInfo* pch=GetChInfo(ch);
			pch->sendfsk.MtStop=FALSE;
		}

		delete []coderbuf;
		return iret;
	}
	return 0;
}

int  CDjDigital::StopSendFsk(int ch)
{
	::DJFsk_StopSend(ch,FSK_CH_TYPE_E1);
	DjDigitalChannelInfo* pch=GetChInfo(ch);
	pch->sendfsk.MtStop=TRUE;
	return 1;
}



int  CDjDigital::RecvFsk(int ch,int time,int flags)
{
	DJFsk_ResetFskBuffer(ch,FSK_CH_TYPE_E1);
	return 1;
}
int  CDjDigital::StopRecvFsk(int ch)
{
	return 1;
}


//放音函数

int CDjDigital::PlayFile(int ch,LPCTSTR fullpath,LPCTSTR StopstrDtmfCharSet)
{
	const char * szfullpath=NULL;
#if defined(UNICODE) || defined(_UNICODE)
	COMMON::UNICODEChar wchar(fullpath);
	szfullpath=wchar;
#else
	szfullpath=fullpath;
#endif

	int ret=::DJVoc_PlayFileNew(ch,(char*)szfullpath,0L,0xFFFFFFFF);
	if(ret!=1)
	{
		SetErrFlag(ret);
		return 0;
	}

	DjDigitalChannelInfo* pch=GetChInfo(ch);
	pch->play.StopPlayDtmfCharlen=_tcslen(StopstrDtmfCharSet);
	_tcsncpy(pch->play.StopPlayDtmfCharSet,StopstrDtmfCharSet,20);
	pch->play.MtStop=FALSE;
	pch->play.StopPlayDtmfChar=0;
	return 1;
}

int CDjDigital::PlayMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet)
{
	DjDigitalChannelInfo* pch=GetChInfo(ch);
	
	int vocch=::DJVoc_SFVC_ForPlay_New(ch,1);

	if(vocch==-1)
	{
		SetErrFlag(_ERR_NoVoiceRes);
		return 0;
	}

	if(::DJVoc_PlayMemory(vocch,(char*)block1,size1)!=0)
	{
		SetErrFlag();
		return 0;
	}

	if( ! DJExg_SetLinkPlayVoiceToTrunk( ch, vocch ) ) 
	{
		::DJExg_ClearLinkPlayVoiceFromTrunk(ch);
		::DJVoc_StopPlayMemory(vocch);
		m_lasterror.SetLastErrorCode(ERRCODE_DERIVERETFAILED);
		return 0;
	}


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

int CDjDigital::PlayQueue(int ch,LPCTSTR fullpath[],int size,LPCTSTR StopstrDtmfCharSet)
{
	if(size>100)
	{
		m_lasterror.SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}
	::DJVoc_InitIndexPlayFile(ch);

	for(int i=0;i<size;++i)
	{
		const char * szfullpath=NULL;
#if defined(UNICODE) || defined(_UNICODE)
		COMMON::UNICODEChar wchar(fullpath[i]);
		szfullpath=wchar;
#else
		szfullpath=fullpath[i];
#endif
		if(!:: DJVoc_AddIndexPlayFile(ch,(char*)szfullpath))
		{
			m_lasterror.SetLastErrorCode(ERRCODE_DERIVERETFAILED);
			return 0;
		}
	}


	if(!::DJVoc_StartIndexPlayFile (ch))
	{
		m_lasterror.SetLastErrorCode(ERRCODE_DERIVERETFAILED);
		return 0;
	}

	DjDigitalChannelInfo* pch=GetChInfo(ch);

	pch->play.StopPlayDtmfCharlen=_tcslen(StopstrDtmfCharSet);
	_tcsncpy(pch->play.StopPlayDtmfCharSet,StopstrDtmfCharSet,20);
	pch->play.MtStop=FALSE;
	pch->play.StopPlayDtmfChar=0;
	return 1;

}

int CDjDigital::LoadPlayIndex(LPCTSTR fullpath,LPCTSTR Alias)
{
	return 0;
}


int CDjDigital::PlayIndex(int ch,LPCTSTR Alias,LPCTSTR StopstrDtmfCharSet)
{
#if defined(UNICODE) || defined(_UNICODE) 
	std::wstring PromptStr;	
#else
	std::string PromptStr;	
#endif
	
	LPCTSTR pAlias=Alias;
	for(;;)
	{

		if(!PromptStr.empty())
		{
			PromptStr.append(_T(","));
		}

		PromptStr.append(pAlias);

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

	const char * szPromptStr=NULL;
#if defined(UNICODE) || defined(_UNICODE)
	COMMON::UNICODEChar wchar(PromptStr.c_str());
	szPromptStr=wchar;
#else
	szPromptStr=PromptStr.c_str();
#endif

	int iret=::DJTrk_PlayPromptStr(ch,szPromptStr);
	if(iret==1)
	{
		DjDigitalChannelInfo* pch=GetChInfo(ch);
		pch->play.StopPlayDtmfCharlen=_tcslen(StopstrDtmfCharSet);
		_tcsncpy(pch->play.StopPlayDtmfCharSet,StopstrDtmfCharSet,20);
		pch->play.MtStop=FALSE;
		pch->play.StopPlayDtmfChar=0;
		return 1;
	}
	else
	{
		SetErrFlag(iret);
	}
	return 0;
}

int CDjDigital::StopPlay(int ch)
{
	DjDigitalChannelInfo* pch=GetChInfo(ch);

	if(pch->IsPlayVoice(PLAY_FILE))
	{
		::DJVoc_StopPlayFile(ch);
	}
	else if(pch->IsPlayVoice(PLAY_QUEUE))
	{
		::DJVoc_InitIndexPlayFile(ch);
		::DJVoc_StopIndexPlayFile(ch);
	}
	else if(pch->IsPlayVoice(PLAY_MEMORY))
	{
		int vocch=::DJTrk_GetTrunkPlayID(ch);
		if(vocch!=-1)
		{
			:: DJVoc_StopPlayMemory (vocch);
		}
	}
	else if(pch->IsPlayVoice(PLAY_INDEX))
	{
		::DJVoc_StopPlayFile(ch);
	}
	else
	{
		_ASSERT(0);
	}

	pch->play.MtStop=TRUE;
	
	return 1;
}


int CDjDigital::UpDatePlayMemory(int ch,int index,BYTE* block,int size)
{
	DjDigitalChannelInfo* pch=GetChInfo(ch);
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




int CDjDigital::RecordFile(int ch,LPCTSTR fullpath,int time,LPCTSTR StopstrDtmfCharSet)
{
	const char * szfullpath=NULL;
#if defined(UNICODE) || defined(_UNICODE)
	COMMON::UNICODEChar wchar(fullpath);
	szfullpath=wchar;
#else
	szfullpath=fullpath;
#endif
	int filesize=0x7FFFFFFFL;//不要修改这个值，东进bug
	if(time>0)
	{
		filesize=time*8;
	}
	int iret=0;
	if((iret=::DJVoc_RecordFileNew(ch,(LPSTR)szfullpath,0,filesize))!=1)
	{
		SetErrFlag(iret);
		return 0;
	}

	DjDigitalChannelInfo* pch=GetChInfo(ch);

	pch->record.StopRecordDtmfCharlen=_tcslen(StopstrDtmfCharSet);
	_tcsncpy(pch->record.StopRecordDtmfCharSet,StopstrDtmfCharSet,20);
	pch->record.MtStop=FALSE;
	pch->record.StopRecordDtmfChar=0;

	return 1;
}

int CDjDigital::RecordMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet)
{
	m_lasterror.SetLastErrorCode(ERRCODE_DEVICENONSUPPORT);
	return 0;
}


int CDjDigital::StopRecord(int ch)
{
	::DJVoc_StopRecordFile(ch);
	DjDigitalChannelInfo* pch=GetChInfo(ch);
	pch->record.MtStop=TRUE;
	return 1;
}

int CDjDigital::Listen(int initiativech,int passivelych)
{
	if(::DJExg_SetListenTrunkToTrunk(initiativech,passivelych)==1)
	{
		return 1;
	}
	else
	{
		m_lasterror.SetLastErrorCode(ERRCODE_DERIVERETFAILED);
	}

	return 0;
}

int CDjDigital::UnListen(int initiativech,int passivelych)
{
	if(::DJExg_ClearListenTrunkFromTrunk(initiativech)==1)
	{

		return 1;
	}
	else
	{
		m_lasterror.SetLastErrorCode(ERRCODE_DERIVERETFAILED);
	}

	return 0;
}



void CDjDigital::ResetCh(int ch)
{
	::DJTrk_InitDtmfBufNew(ch);
}




void  CDjDigital::SendNotify(int ch,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize)
{
	m_pdjinterface->SendNotify(ch,eventid,eventdata,eventsize);
}


void CDjDigital::SetErrFlag(int errcode)
{
	if(errcode)
	{
		m_errlastcode=errcode;
	}
	else
	{
		m_errlastcode=::DJSys_GetErrCode();
	}
	m_lasterror.SetLastErrorType(s_errtype);
}

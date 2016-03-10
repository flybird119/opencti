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
	created:	12:4:2009   11:28
	filename: 	e:\project\opencti.code\trunk\002\src\cti\CtiNotify.cpp
	file path:	e:\project\opencti.code\trunk\002\src\cti
	file base:	CtiNotify
	file ext:	cpp
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/

#include "StdAfx.h"
#include "CtiNotify.h"


#pragma warning(disable:4996)

CtiNotify::CtiNotify(void)
{
}

CtiNotify::~CtiNotify(void)
{
}



void  CtiNotify::NotifyAddChannel(ChannelInfo* pch,CHANNELTYPE chtype,CHANNELSTATE chstate,CHANNELWAY chway,int chtotal)
{
    ADDCHANNELPARAM* paddchannel=(ADDCHANNELPARAM*)malloc(sizeof(ADDCHANNELPARAM));
    paddchannel->chtype=chtype;
    paddchannel->chstate=chstate;
    paddchannel->chway=chway;
	paddchannel->chtotal=chtotal;
    SendNotify(pch->GetID(),EVENT_ADDCHANNEL,paddchannel,sizeof(ADDCHANNELPARAM));

}


void  CtiNotify::NotifyHangup(ChannelInfo* pch,RELEASEATTRIBUTE reason)
{
    LPHANGUPPARAM lphangupparam=(LPHANGUPPARAM)malloc(sizeof(HANGUPPARAM));
    lphangupparam->reason=reason;
    SendNotify(pch->GetID(),EVENT_HANGUP,lphangupparam,sizeof(HANGUPPARAM));
}

void  CtiNotify::NotifyRelease(ChannelInfo* pch)
{
    LPRELEASEPARAM lpcleanparam=(LPRELEASEPARAM)malloc(sizeof(RELEASEPARAM));
    SendNotify(pch->GetID(),EVENT_RELEASE,lpcleanparam,sizeof(RELEASEPARAM));
}

void  CtiNotify::NotifyIdle(ChannelInfo* pch)
{
    LPIDLEPARAM lpcleanparam=(LPIDLEPARAM)malloc(sizeof(IDLEPARAM));
    SendNotify(pch->GetID(),EVENT_IDLE,lpcleanparam,sizeof(IDLEPARAM));
}

void  CtiNotify::NotifyCallin(ChannelInfo* pch,LPCTSTR calling,LPCTSTR called,LPCTSTR origcalled)
{
    LPCALLINPARAM lpcallinparam=(LPCALLINPARAM)malloc(sizeof(CALLINPARAM));
    memset(lpcallinparam,0,sizeof(CALLINPARAM));

    if(calling)
        _tcsncpy(lpcallinparam->calling,calling,128-1);
    if(calling)
        _tcsncpy(lpcallinparam->called,called,128-1);
    if(origcalled)
        _tcsncpy(lpcallinparam->origcalled,origcalled,128-1);

    SendNotify(pch->GetID(),EVENT_CALLIN,lpcallinparam,sizeof(CALLINPARAM));
}

void  CtiNotify::NotifyRinging(ChannelInfo* pch)
{
    LPRINGINGPARAM lpringingparam=(LPRINGINGPARAM)malloc(sizeof(RINGINGPARAM));
    SendNotify(pch->GetID(),EVENT_RINGING,lpringingparam,sizeof(RINGINGPARAM));
}


void  CtiNotify::NotifyTalking(ChannelInfo* pch)
{
    LPTALKINGPARAM lptalkingparam=(LPTALKINGPARAM)malloc(sizeof(TALKINGPARAM));
    SendNotify(pch->GetID(),EVENT_TALKING,lptalkingparam,sizeof(TALKINGPARAM));
}

void  CtiNotify::NotifyRemoteBlock(ChannelInfo* pch)
{
    LPREMOTEBLOCKPARAM lpremoteblockparam=(LPREMOTEBLOCKPARAM)malloc(sizeof(REMOTEBLOCKPARAM));
    SendNotify(pch->GetID(),EVENT_REMOTE_BLOCK,lpremoteblockparam,sizeof(REMOTEBLOCKPARAM));
}

void  CtiNotify::NotifyRingback(ChannelInfo* pch)
{
    LPRINGBACKPARAM lpringbackparam=(LPRINGBACKPARAM)malloc(sizeof(RINGBACKPARAM));
    SendNotify(pch->GetID(),EVENT_RINGBACK,lpringbackparam,sizeof(RINGBACKPARAM));

}

void  CtiNotify::NotifyUnusable(ChannelInfo* pch)
{
	LPUNUSABLEPARAM lpunusableparam=(LPUNUSABLEPARAM )malloc(sizeof(UNUSABLEPARAM ));
	SendNotify(pch->GetID(),EVENT_UNUSABLE,lpunusableparam,sizeof(UNUSABLEPARAM));

}

void  CtiNotify::NotifyDialup(ChannelInfo* pch,LPCTSTR calling,LPCTSTR called,LPCTSTR origcalled)
{
    LPDIALUPPARAM lpdialupparam=(LPDIALUPPARAM)malloc(sizeof(DIALUPPARAM));
    memset(lpdialupparam,0,sizeof(DIALUPPARAM));

    if(calling)
        _tcsncpy(lpdialupparam->calling,calling,128-1);
    if(calling)
        _tcsncpy(lpdialupparam->called,called,128-1);
    if(origcalled)
        _tcsncpy(lpdialupparam->origcalled,origcalled,128-1);

    SendNotify(pch->GetID(),EVENT_DIALUP,lpdialupparam,sizeof(CALLINPARAM));
}

void  CtiNotify::NotifyLocalBlock(ChannelInfo* pch)
{
    LPLOCALBLOCKPARAM lplocalblockparam=(LPLOCALBLOCKPARAM)malloc(sizeof(LOCALBLOCKPARAM));
    SendNotify(pch->GetID(),EVENT_LOCAL_BLOCK,lplocalblockparam,sizeof(LOCALBLOCKPARAM));
}

void  CtiNotify::NotifySleep(ChannelInfo* pch)
{
    LPSLEEPPARAM lpsleepparam=(LPSLEEPPARAM)malloc(sizeof(SLEEPPARAM));
    SendNotify(pch->GetID(),EVENT_SLEEP,lpsleepparam,sizeof(SLEEPPARAM));
}

void  CtiNotify::NotifySendTone(ChannelInfo* pch,int result,TONEATTRIBUTE tone)
{
    LPSENDTONEPARAM lpsendtoneparam=(LPSENDTONEPARAM)malloc(sizeof(SENDTONEPARAM));
    lpsendtoneparam->result=result;
    lpsendtoneparam->tone=tone;
    SendNotify(pch->GetID(),EVENT_SENDTONE,lpsendtoneparam,sizeof(SENDTONEPARAM));
}

void  CtiNotify::NotifyRecvTone(ChannelInfo* pch,int result,TONEATTRIBUTE tone)
{
    LPRECVTONEPARAM lprecvtoneparam=(LPRECVTONEPARAM)malloc(sizeof(SENDTONEPARAM));
    lprecvtoneparam->result=result;
    lprecvtoneparam->tone=tone;
    SendNotify(pch->GetID(),EVENT_RECVTONE,lprecvtoneparam,sizeof(SENDTONEPARAM));
}


void  CtiNotify::NotifySendDtmf(ChannelInfo* pch,int result)
{
    LPSENDDTMFPARAM lpsenddtmfparam=(LPSENDDTMFPARAM)malloc(sizeof(SENDDTMFPARAM));
    lpsenddtmfparam->result=result;
    SendNotify(pch->GetID(),EVENT_SENDDTMF,lpsenddtmfparam,sizeof(SENDDTMFPARAM));
}

void  CtiNotify::NotifyRecvDtmf(ChannelInfo* pch,int result,TCHAR dtmf)
{
    LPRECVDTMFPARAM lprecvdtmfparam=(LPRECVDTMFPARAM)malloc(sizeof(RECVDTMFPARAM));
    lprecvdtmfparam->result=result;
    lprecvdtmfparam->dtmf=dtmf;
    SendNotify(pch->GetID(),EVENT_RECVDTMF,lprecvdtmfparam,sizeof(RECVDTMFPARAM));
}

void  CtiNotify::NotifyPlay(ChannelInfo* pch,int result,PLAYATTRIBUTE attribute,TCHAR dtmf)
{
    LPPLAYPARAM lpplayparam=(LPPLAYPARAM)malloc(sizeof(PLAYPARAM));
    lpplayparam->result=result;
    lpplayparam->attribute=attribute;
    lpplayparam->dtmf=dtmf;
    SendNotify(pch->GetID(),EVENT_PLAY,lpplayparam,sizeof(PLAYPARAM));
}

void  CtiNotify::NotifyUpdatePlayMemory(ChannelInfo* pch,int index,BYTE* block1,int size1,BYTE* block2,int size2)
{
    LPUPDATEPLAYMEMORYPARAM lpupdateplaymemoryparam=(LPUPDATEPLAYMEMORYPARAM)malloc(sizeof(UPDATEPLAYMEMORYPARAM));
    lpupdateplaymemoryparam->index=index;
    lpupdateplaymemoryparam->block1=block1;
    lpupdateplaymemoryparam->size1=size1;
    lpupdateplaymemoryparam->block2=block2;
    lpupdateplaymemoryparam->size2=size2;
    SendNotify(pch->GetID(),EVENT_UPDATEPLAYMEMORY,lpupdateplaymemoryparam,sizeof(UPDATEPLAYMEMORYPARAM));
}






void  CtiNotify::NotifySysEvent(int identify,LPVOID eventdata,int eventsize)
{
    LPSYSEVENTPARAM lpsyseventparam=(LPSYSEVENTPARAM)malloc(sizeof(SYSEVENTPARAM));
    lpsyseventparam->eventdata=eventdata;
    lpsyseventparam->eventsize=eventsize;
    SendNotify(identify,EVENT_SYSEVENT,lpsyseventparam,sizeof(SYSEVENTPARAM));
}

void  CtiNotify::NotifyChEvent(ChannelInfo* pch,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize,UINT delayid,BOOL once)
{
    LPUSEREVENTPARAM lpusereventparam=(LPUSEREVENTPARAM)malloc(sizeof(USEREVENTPARAM));
    lpusereventparam->delayid=delayid;
    lpusereventparam->eventdata=eventdata;
    lpusereventparam->eventsize=eventsize;
	lpusereventparam->once=once;
    SendNotify(pch->GetID(),eventid,lpusereventparam,sizeof(USEREVENTPARAM));
}
void  CtiNotify::NotifyRecord(ChannelInfo* pch,int result,RECORDATTRIBUTE attribute,TCHAR dtmf)
{
    LPRECORDPARAM lprecordparam=(LPRECORDPARAM)malloc(sizeof(RECORDPARAM));
    lprecordparam->result=result;
    lprecordparam->attribute=attribute;
    lprecordparam->dtmf=dtmf;
    SendNotify(pch->GetID(),EVENT_RECORD,lprecordparam,sizeof(RECORDPARAM));
}

void  CtiNotify::NotifyUpdateRecordMemory(ChannelInfo* pch,int index,BYTE* block1,int size1,BYTE* block2,int size2)
{
    LPUPDATERECORDMEMORYPARAM lpupdaterecordmemoryparam=(LPUPDATERECORDMEMORYPARAM)malloc(sizeof(UPDATERECORDMEMORYPARAM));
    lpupdaterecordmemoryparam->index=index;
    lpupdaterecordmemoryparam->block1=block1;
    lpupdaterecordmemoryparam->size1=size1;
    lpupdaterecordmemoryparam->block2=block2;
    lpupdaterecordmemoryparam->size2=size2;
    SendNotify(pch->GetID(),EVENT_UPDATERECORDMEMORY,lpupdaterecordmemoryparam,sizeof(UPDATERECORDMEMORYPARAM));
}

void  CtiNotify::NotifySendFsk(ChannelInfo* pch,int result)
{
    LPSENDFSKPARAM lpsendfskparam=(LPSENDFSKPARAM)malloc(sizeof(SENDFSKPARAM));
    lpsendfskparam->result=result;
    SendNotify(pch->GetID(),EVENT_SENDFSK,lpsendfskparam,sizeof(SENDFSKPARAM));
}

void  CtiNotify::NotifyRecvFsk(ChannelInfo* pch,int result,BYTE* fsk,int len)
{
    LPRECVFSKPARAM lprecvfskparam=(LPRECVFSKPARAM)malloc(sizeof(RECVFSKPARAM)+len);
    lprecvfskparam->result=result;
    lprecvfskparam->size=len;
    memcpy(&lprecvfskparam->buf,fsk,len);
    SendNotify(pch->GetID(),EVENT_RECVFSK,lprecvfskparam,sizeof(RECVFSKPARAM));
}

void  CtiNotify::NotifyState(ChannelInfo* pch,LPCTSTR statetext)
{
	LPSTATEPARAM lpstateparam=(LPSTATEPARAM)malloc(sizeof(STATEPARAM));
	lpstateparam->statetext=statetext;
	SendNotify(pch->GetID(),EVENT_STATE,lpstateparam,sizeof(STATEPARAM));
}


#pragma warning(default:4996)
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
	filename: 	e:\project\opencti.code\trunk\002\src\cti\CtiAPI.cpp
	file path:	e:\project\opencti.code\trunk\002\src\cti
	file base:	CtiAPI
	file ext:	cpp
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/


#include "stdafx.h"

#include "CtiAPI.h"
#include "CtiInterface.h"
#include "./Log/LogMessage.h"


CtiInterface* g_pCtiInterface=NULL;



COMMON::CLock  CTIAPILock(COMMON::CRITICAl);
#define BLOCK_()   LOCKSCOPE(CTIAPILock)



int WINAPI CTI_Init(DeviceDescriptor* pctidevicedescriptor)
{

	//参数限制

	if(pctidevicedescriptor==NULL || pctidevicedescriptor->notifyfun==NULL)
	{
		g_pCtiInterface->SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}

    BLOCK_();
    if(g_pCtiInterface!=NULL)
    {
        ::MessageBox(NULL,_T("设备已启动！"),_T("OpenCTI"),MB_OK|MB_ICONEXCLAMATION);
        return 0;
    }

    if(g_pCtiInterface==NULL)
    {
        LOAD_LOG_SERVER(SW_SHOW);
        g_pCtiInterface=new CtiInterface;
        return g_pCtiInterface->Init(pctidevicedescriptor);
    }
    return 0;
}

void WINAPI CTI_Term()
{
    if(g_pCtiInterface==NULL)
    {
        return ;
    }
    else
    {
        //防死锁
        g_pCtiInterface->GetMessageQueue()->Quit(1,TRUE);
    }
   
    BLOCK_();   
    g_pCtiInterface->Term();
    delete g_pCtiInterface;
    g_pCtiInterface=NULL;
    FREE_LOG_SERVER();
}



BOOL WINAPI CTI_Features(FUNCTIONADDRINDEX descriptor)
{
    BLOCK_();
    return g_pCtiInterface->Features(descriptor);
}

void* WINAPI CTI_GetNativeAPI(LPCTSTR funname)
{
    BLOCK_();
    return g_pCtiInterface->GetNativeAPI(funname);

}
int WINAPI CTI_GetLastErrorCode()
{
    BLOCK_();
    return g_pCtiInterface->GetLastErrorCode();
}


LPCTSTR WINAPI CTI_FormatErrorCode(int code)
{
    BLOCK_();
    return g_pCtiInterface->FormatErrorCode(code);
}

int WINAPI CTI_GetChTotal()
{
    BLOCK_();
    return g_pCtiInterface->GetChTotal();
}


CHANNELTYPE WINAPI CTI_GetChType(int ch)
{
    BLOCK_();
    return g_pCtiInterface->GetChType(ch);
}


int WINAPI CTI_SearchIdleCh(CHANNELTYPE chtype, int* pChFilter, int FilterLen,int*pChExcept,int ExceptLen)
{
    BLOCK_();
    return g_pCtiInterface->SearchIdleCh(chtype,pChFilter,FilterLen,pChExcept,ExceptLen);
}


int WINAPI CTI_SetChOwnerData(int ch,DWORD OwnerData)
{
    BLOCK_();
    return g_pCtiInterface->SetChOwnerData(ch,OwnerData);

}

DWORD WINAPI CTI_GetChOwnerData(int ch)
{
    BLOCK_();
    return g_pCtiInterface->GetChOwnerData(ch);
}



CHANNELSTATE WINAPI CTI_GetChState(int ch)
{
	BLOCK_();
	return g_pCtiInterface->GetChState(ch);
}



//接续函数

int WINAPI CTI_Dialup(int ch,LPCTSTR calling,NUMBERATTRIBUTE callingattribute,LPCTSTR called,LPCTSTR origcalled,NUMBERATTRIBUTE origcalledattribute,BOOL CallerIDBlock,int flags)
{
	if(calling==NULL || called==NULL || origcalled==NULL)
	{
		g_pCtiInterface->SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}

    BLOCK_();
    return g_pCtiInterface->Dialup(ch,calling,callingattribute,called,origcalled,origcalledattribute,CallerIDBlock,flags);
}

int WINAPI CTI_Pickup(int ch,int flags)
{
    BLOCK_();
    return g_pCtiInterface->Pickup(ch,flags);
}

int WINAPI CTI_Ringback(int ch)
{
    BLOCK_();
    return g_pCtiInterface->Ringback(ch);
}

int WINAPI CTI_Hangup(int ch,RELEASEATTRIBUTE attribute)
{
    BLOCK_();
    return g_pCtiInterface->Hangup(ch,attribute);
}



//DTMF函数

int WINAPI CTI_SendDtmf(int ch,LPCTSTR dtmf,int len,int flags)
{
	if(dtmf==NULL || len<=0)
	{
		g_pCtiInterface->SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}

    BLOCK_();
    return g_pCtiInterface->SendDtmf(ch,dtmf,len,flags);
}


int WINAPI CTI_RecvDtmf(int ch,int time,int flags)
{
    BLOCK_();
    return g_pCtiInterface->RecvDtmf(ch,time,flags);
}


int WINAPI  CTI_StopRecvDtmf(int ch)
{
    BLOCK_();
    return g_pCtiInterface->StopRecvDtmf(ch);
}

int WINAPI  CTI_StopSendDtmf(int ch)
{
    BLOCK_();
    return g_pCtiInterface->StopSendDtmf(ch);
}

int WINAPI  CTI_IsSendDtmf(int ch)
{
    BLOCK_();
    return g_pCtiInterface->IsSendDtmf(ch);
}

int WINAPI  CTI_IsRecvDtmf(int ch)
{
    BLOCK_();
    return g_pCtiInterface->IsRecvDtmf(ch);
}



//FSK函数

int WINAPI CTI_SendFsk(int ch,const BYTE* fsk,int len,int flags)
{
	if(fsk==NULL || len<=0)
	{
		g_pCtiInterface->SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}
	
    BLOCK_();
    return g_pCtiInterface->SendFsk(ch,fsk,len,flags);
}
int WINAPI CTI_SendFskFull(int ch,int sync,int mark,const BYTE* fsk,int len,int flags)
{
	if(fsk==NULL || len<=0)
	{
		g_pCtiInterface->SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}

    BLOCK_();
    return g_pCtiInterface->SendFskFull(ch,sync,mark,fsk,len,flags);
}

int WINAPI  CTI_StopSendFsk(int ch)
{
    BLOCK_();
    return g_pCtiInterface->StopSendFsk(ch);
}

int WINAPI  CTI_IsSendFsk(int ch)
{
    BLOCK_();
    return g_pCtiInterface->IsSendFsk(ch);
}

int WINAPI CTI_RecvFsk(int ch,int time,int flags)
{
    BLOCK_();
    return g_pCtiInterface->RecvFsk(ch,time,flags);
}

int WINAPI  CTI_StopRecvFsk(int ch)
{
    BLOCK_();
    return g_pCtiInterface->StopRecvFsk(ch);
}
int WINAPI  CTI_IsRecvFsk(int ch)
{
    BLOCK_();
    return g_pCtiInterface->IsRecvFsk(ch);
}


//media函数

int WINAPI CTI_PlayFile(int ch,LPCTSTR fullpath,LPCTSTR StopstrDtmfCharSet)
{
	if(fullpath==NULL || StopstrDtmfCharSet==NULL)
	{
		g_pCtiInterface->SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}
    BLOCK_();
    return g_pCtiInterface->PlayFile(ch,fullpath,StopstrDtmfCharSet);
}

int WINAPI CTI_PlayMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet)
{
	if(block1==NULL || size1<=0 || StopstrDtmfCharSet==NULL)
	{
		g_pCtiInterface->SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}
    BLOCK_();
    return g_pCtiInterface->PlayMemory(ch,block1,size1,block2,size2,StopstrDtmfCharSet);
}

int WINAPI CTI_PlayQueue(int ch,LPCTSTR fullpath[],int size,LPCTSTR StopstrDtmfCharSet)
{
	if(fullpath==NULL || size<=0 || StopstrDtmfCharSet==NULL)
	{
		g_pCtiInterface->SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}
    BLOCK_();
    return g_pCtiInterface->PlayQueue(ch,fullpath,size,StopstrDtmfCharSet);

}


int WINAPI CTI_LoadPlayIndex(LPCTSTR fullpath,LPCTSTR Alias)
{
	if(fullpath==NULL || Alias==NULL)
	{
		g_pCtiInterface->SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}
    BLOCK_();
	return g_pCtiInterface->LoadPlayIndex(fullpath,Alias);

}

int WINAPI CTI_PlayIndex(int ch,LPCTSTR Alias,LPCTSTR StopstrDtmfCharSet)
{
	if(Alias==NULL || StopstrDtmfCharSet==NULL)
	{
		g_pCtiInterface->SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}
    BLOCK_();
    return g_pCtiInterface->PlayIndex(ch,Alias,StopstrDtmfCharSet);
}


int WINAPI CTI_StopPlay(int ch)
{
    BLOCK_();
    return g_pCtiInterface->StopPlay(ch);
}


int WINAPI CTI_IsPlay(int ch)
{
    BLOCK_();
    return g_pCtiInterface->IsPlay(ch);
}

int WINAPI CTI_UpDatePlayMemory(int ch,int index,BYTE* block,int size)
{

	if(block==NULL || size<=0)
	{
		g_pCtiInterface->SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}
    BLOCK_();
    return g_pCtiInterface->UpDatePlayMemory(ch,index,block,size);
}



int WINAPI CTI_RecordFile(int ch,LPCTSTR fullpath,int time,LPCTSTR StopstrDtmfCharSet)
{
	if(fullpath==NULL || StopstrDtmfCharSet==NULL)
	{
		g_pCtiInterface->SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}
    BLOCK_();
    return g_pCtiInterface->RecordFile(ch,fullpath,time,StopstrDtmfCharSet);
}

int WINAPI CTI_RecordMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet)
{
	if(block1==NULL || size1<=0 || StopstrDtmfCharSet==NULL)
	{
		g_pCtiInterface->SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}
    BLOCK_();
    return g_pCtiInterface->RecordMemory(ch,block1,size1,block2,size2,StopstrDtmfCharSet);
}

int WINAPI CTI_StopRecord(int ch)
{
    BLOCK_();
    return g_pCtiInterface->StopRecord(ch);
}

int WINAPI CTI_IsRecord(int ch)
{
    BLOCK_();
    return g_pCtiInterface->IsRecord(ch);
}

int WINAPI CTI_UpDateRecordMemory(int ch,int index,BYTE* block,int size)
{
	if(block==NULL || size<=0)
	{
		g_pCtiInterface->SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}

    BLOCK_();
    return g_pCtiInterface->UpDateRecordMemory(ch,index,block,size);
}

//Exchange总线

int WINAPI CTI_Listen(int initiativech,int passivelych)
{
    BLOCK_();
    return g_pCtiInterface->Listen(initiativech,passivelych);
}

int WINAPI CTI_UnListen(int initiativech,int passivelych)
{
    BLOCK_();
    return g_pCtiInterface->UnListen(initiativech,passivelych);
}


int WINAPI CTI_SendTone(int ch,TONEATTRIBUTE attribute,int time,int flags)
{
    BLOCK_();
    return g_pCtiInterface->SendTone(ch,attribute,time,flags);
}
int WINAPI CTI_SendToneEx(int ch, int nFreq1, int nVolume1, int nFreq2, int nVolume2,DWORD dwOnTime, DWORD dwOffTime,int time,int flags)
{
    BLOCK_();
    return g_pCtiInterface->SendToneEx(ch,nFreq1,nVolume1,nFreq2,nVolume2,dwOffTime,dwOffTime,time,flags);
}
int WINAPI CTI_StopSendTone(int ch)
{
    BLOCK_();
    return g_pCtiInterface->StopSendTone(ch);
}

int WINAPI CTI_IsSendTone(int ch)
{
    BLOCK_();
    return g_pCtiInterface->IsSendTone(ch);
}


int WINAPI CTI_RecvTone(int ch,TONEATTRIBUTE attribute,int time,int flags)
{
    BLOCK_();
    return g_pCtiInterface->RecvTone(ch,attribute,time,flags);
}

int WINAPI CTI_StopRecvTone(int ch,TONEATTRIBUTE attribute)
{
    BLOCK_();
    return g_pCtiInterface->StopRecvTone(ch,attribute);
}

int WINAPI CTI_IsRecvTone(int ch,TONEATTRIBUTE attribute)
{
    BLOCK_();
    return g_pCtiInterface->IsRecvTone(ch,attribute);
}


//闪断
int WINAPI CTI_SendFlash(int ch,int time)
{
	BLOCK_();
    return g_pCtiInterface->SendFlash(ch,time);
}
int WINAPI CTI_RecvFlash(int ch)
{
    BLOCK_();
    return g_pCtiInterface->IsRecvFlash(ch);
}
int WINAPI CTI_StopRecvFlash(int ch)
{
	BLOCK_();
    return g_pCtiInterface->StopRecvFlash(ch);
}
int WINAPI CTI_IsRecvFlash(int ch)
{
	BLOCK_();
	return g_pCtiInterface->IsRecvFlash(ch);
}
















//辅助函数

int WINAPI CTI_DelayChEvent(int cmsDelay,BOOL once,int chid,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize)
{
    BLOCK_();
    return g_pCtiInterface->DelayChEvent(cmsDelay,once,chid,eventid,eventdata,eventsize);
}

int WINAPI CTI_DelaySysEvent(int cmsDelay,BOOL once,int identify,LPVOID eventdata,int eventsize)
{
    BLOCK_();
    return g_pCtiInterface->DelaySysEvent(cmsDelay,once,identify,eventdata,eventsize);
}

int WINAPI CTI_InsertChEvent(int chid,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize)
{
	BLOCK_();
	return g_pCtiInterface->InsertChEvent(chid,eventid,eventdata,eventsize);
}

int WINAPI CTI_CancelDelay(int ch,int delayid)
{
	BLOCK_();
	return g_pCtiInterface->CancelDelay(ch,delayid);
}

int WINAPI CTI_BindExclusiveThread(int ch)
{
	BLOCK_();
	return g_pCtiInterface->BindExclusiveThread(ch);
}


int WINAPI CTI_UnBindExclusiveThread(int ch)
{
	BLOCK_();
	return g_pCtiInterface->UnBindExclusiveThread(ch);
}



/// \brief  暂停处理
/// \details  
void  WINAPI CTI_PauseCh(int ch)
{
	BLOCK_();
	g_pCtiInterface->PauseCh(ch);

}

/// \brief  恢复处理
/// \details  
void WINAPI CTI_ResumeCh(int ch)
{
	BLOCK_();
	g_pCtiInterface->ResumeCh(ch);

}




int  WINAPI CTI_GetSelectCh()
{
    return g_pCtiInterface->GetSelectCh();
}
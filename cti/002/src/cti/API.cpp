/********************************************************************
	created:	2009/04/12
	created:	12:4:2009   14:01
	filename: 	e:\project\opencti.code\trunk\002\src\cti\API.cpp
	file path:	e:\project\opencti.code\trunk\002\src\cti
	file base:	API
	file ext:	cpp
	author:		xugood@live.cn
	
	purpose:	
*********************************************************************/


#include "StdAfx.h"
#include "API.h"

namespace CTI
{

 tInit                        pInit=NULL;
 tTerm                        pTerm=NULL;

 tFeatures		              pFeatures=NULL;
 tGetNativeAPI                pGetNativeAPI=NULL;
 tGetLastErrorCode            pGetLastErrorCode=NULL;

 tFormatErrorCode             pFormatErrorCode=NULL;
 tGetChTotal                  pGetChTotal=NULL;
 tGetChType					  pGetChType=NULL;
 tSetChOwnerData              pSetChOwnerData=NULL;    
 tGetChOwnerData              pGetChOwnerData=NULL;
 tGetChState				  pGetChState=NULL;
 tSearchIdleCh                pSearchIdleCh=NULL;
 tLoadPlayIndex               pLoadPlayIndex=NULL;




 tDialup			          pDialup=NULL;
 tPickup			          pPickup=NULL;
 tRingback		              pRingback=NULL;
 tHangup			          pHangup=NULL;


 tSendDtmf		              pSendDtmf=NULL;
 tStopSendDtmf		          pStopSendDtmf=NULL;
 tIsSendDtmf		          pIsSendDtmf=NULL;

 tRecvDtmf		              pRecvDtmf=NULL;
 tStopRecvDtmf		          pStopRecvDtmf=NULL;
 tIsRecvDtmf		          pIsRecvDtmf=NULL;


 tSendFsk			          pSendFsk=NULL;
 tSendFskFull                 pSendFskFull=NULL;
 tRecvFsk			          pRecvFsk=NULL;
 tStopRecvFsk			      pStopRecvFsk=NULL;
 tStopSendFsk			      pStopSendFsk=NULL;
 tIsSendFsk			          pIsSendFsk=NULL;
 tIsRecvFsk			          pIsRecvFsk=NULL;



 tSendTone                    pSendTone=NULL;
 tSendToneEx                  pSendToneEx=NULL;
 tStopSendTone                pStopSendTone=NULL;
 tIsSendTone                  pIsSendTone=NULL;

 tRecvTone                    pRecvTone=NULL;
 tStopRecvTone                pStopRecvTone=NULL;
 tIsRecvTone                  pIsRecvTone=NULL;



 tPlayFile		              pPlayFile=NULL;
 tPlayMemory		          pPlayMemory=NULL;
 tPlayQueue	                  pPlayQueue=NULL;
 tPlayIndex                   pPlayIndex=NULL;
 tStopPlay		              pStopPlay=NULL;
 tIsPlay                      pIsPlay=NULL;
 tUpDatePlayMemory            pUpDatePlayMemory=NULL;

 tRecordFile		          pRecordFile=NULL;
 tRecordMemory		          pRecordMemory=NULL;
 tStopRecord		          pStopRecord=NULL;
 tIsRecord                    pIsRecord=NULL;
 tUpDateRecordMemory          pUpDateRecordMemory=NULL;

 tSendFlash                   pSendFlash=NULL;
 tRecvFlash                   pRecvFlash=NULL;
 tStopRecvFlash               pStopRecvFlash=NULL;
 tIsRecvFlash                 pIsRecvFlash=NULL;


 tListen			          pListen=NULL;
 tUnListen		              pUnListen=NULL;


 tDelayChEvent               pDelayChEvent=NULL;
 tDelaySysEvent              pDelaySysEvent=NULL;
 tInsertChEvent              pInsertChEvent=NULL;
 tCancelDelay                pCancelDelay=NULL;
 tBindExclusiveThread		 pBindExclusiveThread=NULL;
 tUnBindExclusiveThread		 pUnBindExclusiveThread=NULL;

 tGetSelectCh                 pGetSelectCh=NULL;

 tPauseCh					  pPauseCh=NULL;
 tResumeCh				      pResumeCh=NULL;


 BOOL LoadInterface(HMODULE hmodule)
 {

	 if((pInit                =   (tInit)              ::GetProcAddress(hmodule,"CTI_Init"))  &&
		 (pTerm               =   (tTerm)              ::GetProcAddress(hmodule,"CTI_Term"))  &&
		 (pGetChType		  =   (tGetChType)		   ::GetProcAddress(hmodule,"CTI_GetChType"))&&
		 (pSetChOwnerData     =   (tSetChOwnerData)    ::GetProcAddress(hmodule,"CTI_SetChOwnerData"))&&
		 (pGetChOwnerData     =   (tGetChOwnerData)    ::GetProcAddress(hmodule,"CTI_GetChOwnerData"))&&
		 (pGetChState		  =   (tGetChState)		   ::GetProcAddress(hmodule,"CTI_GetChState"))&&
		 (pGetChTotal         =   (tGetChTotal)        ::GetProcAddress(hmodule,"CTI_GetChTotal"))&&
		 (pSearchIdleCh       =   (tSearchIdleCh)      ::GetProcAddress(hmodule,"CTI_SearchIdleCh"))&&
		 (pLoadPlayIndex      =   (tLoadPlayIndex)     ::GetProcAddress(hmodule,"CTI_LoadPlayIndex"))&&
		 (pFeatures           =   (tFeatures)          ::GetProcAddress(hmodule,"CTI_Features"))&&
		 (pGetNativeAPI       =   (tGetNativeAPI)      ::GetProcAddress(hmodule,"CTI_GetNativeAPI"))&&
		 (pGetLastErrorCode   =   (tGetLastErrorCode)  ::GetProcAddress(hmodule,"CTI_GetLastErrorCode"))&&
		 (pFormatErrorCode    =   (tFormatErrorCode)   ::GetProcAddress(hmodule,"CTI_FormatErrorCode"))&&
		 (pRingback           =   (tRingback)          ::GetProcAddress(hmodule,"CTI_Ringback"))&&
		 (pDialup             =   (tDialup)            ::GetProcAddress(hmodule,"CTI_Dialup"))&&
		 (pPickup             =   (tPickup)            ::GetProcAddress(hmodule,"CTI_Pickup"))&&
		 (pHangup             =   (tHangup)            ::GetProcAddress(hmodule,"CTI_Hangup"))&&
		 (pSendDtmf           =   (tSendDtmf)          ::GetProcAddress(hmodule,"CTI_SendDtmf"))&&
		 (pStopSendDtmf       =   (tStopSendDtmf)      ::GetProcAddress(hmodule,"CTI_StopSendDtmf"))&&
		 (pIsSendDtmf         =   (tIsSendDtmf)        ::GetProcAddress(hmodule,"CTI_IsSendDtmf"))&&
		 (pRecvDtmf           =   (tRecvDtmf)          ::GetProcAddress(hmodule,"CTI_RecvDtmf"))&&
		 (pStopRecvDtmf       =   (tStopRecvDtmf)      ::GetProcAddress(hmodule,"CTI_StopRecvDtmf"))&&
		 (pIsRecvDtmf         =   (tIsRecvDtmf)        ::GetProcAddress(hmodule,"CTI_IsRecvDtmf"))&&
		 (pSendFsk            =   (tSendFsk)           ::GetProcAddress(hmodule,"CTI_SendFsk"))&&
		 (pSendFskFull        =   (tSendFskFull)       ::GetProcAddress(hmodule,"CTI_SendFskFull"))&&
		 (pStopSendFsk        =   (tStopSendFsk)       ::GetProcAddress(hmodule,"CTI_StopSendFsk"))&&
		 (pIsSendFsk          =   (tIsSendFsk)         ::GetProcAddress(hmodule,"CTI_IsSendFsk"))&&
		 (pRecvFsk            =   (tRecvFsk)           ::GetProcAddress(hmodule,"CTI_RecvFsk"))&&
		 (pStopRecvFsk        =   (tStopRecvFsk)       ::GetProcAddress(hmodule,"CTI_StopRecvFsk"))&&
		 (pIsRecvFsk          =   (tIsRecvFsk)         ::GetProcAddress(hmodule,"CTI_IsRecvFsk"))&&
		 (pSendTone           =   (tSendTone)          ::GetProcAddress(hmodule,"CTI_SendTone"))&&
		 (pSendToneEx         =   (tSendToneEx)        ::GetProcAddress(hmodule,"CTI_SendToneEx"))&&
		 (pStopSendTone       =   (tStopSendTone)      ::GetProcAddress(hmodule,"CTI_StopSendTone"))&&
		 (pIsSendTone         =   (tIsSendTone)        ::GetProcAddress(hmodule,"CTI_IsSendTone"))&&
		 (pRecvTone           =   (tRecvTone)          ::GetProcAddress(hmodule,"CTI_RecvTone"))&&
		 (pStopRecvTone       =   (tStopRecvTone)      ::GetProcAddress(hmodule,"CTI_StopRecvTone"))&&
		 (pIsRecvTone         =   (tIsRecvTone)        ::GetProcAddress(hmodule,"CTI_IsRecvTone"))&&
		 (pPlayFile           =   (tPlayFile)          ::GetProcAddress(hmodule,"CTI_PlayFile"))&&
		 (pPlayMemory         =   (tPlayMemory)        ::GetProcAddress(hmodule,"CTI_PlayMemory"))&&
		 (pPlayQueue          =   (tPlayQueue)         ::GetProcAddress(hmodule,"CTI_PlayQueue"))&&
		 (pPlayIndex          =   (tPlayIndex)         ::GetProcAddress(hmodule,"CTI_PlayIndex"))&&
		 (pStopPlay           =   (tStopPlay)          ::GetProcAddress(hmodule,"CTI_StopPlay"))&&
		 (pIsPlay             =   (tIsPlay)            ::GetProcAddress(hmodule,"CTI_IsPlay"))&&
         (pUpDatePlayMemory   =   (tUpDatePlayMemory)  ::GetProcAddress(hmodule,"CTI_UpDatePlayMemory"))&&
         (pRecordFile         =   (tRecordFile)        ::GetProcAddress(hmodule,"CTI_RecordFile"))&&
		 (pRecordMemory       =   (tRecordMemory)      ::GetProcAddress(hmodule,"CTI_RecordMemory"))&&
		 (pStopRecord         =   (tStopRecord)        ::GetProcAddress(hmodule,"CTI_StopRecord"))&&
		 (pIsRecord           =   (tIsRecord)          ::GetProcAddress(hmodule,"CTI_IsRecord"))&&
         (pUpDateRecordMemory =   (tUpDateRecordMemory)::GetProcAddress(hmodule,"CTI_UpDateRecordMemory"))&&
         (pSendFlash          =   (tSendFlash)         ::GetProcAddress(hmodule,"CTI_SendFlash"))&&
		 (pRecvFlash          =   (tRecvFlash)         ::GetProcAddress(hmodule,"CTI_RecvFlash"))&&
		 (pStopRecvFlash      =   (tStopRecvFlash)     ::GetProcAddress(hmodule,"CTI_StopRecvFlash"))&&
		 (pIsRecvFlash        =   (tIsRecvFlash)       ::GetProcAddress(hmodule,"CTI_IsRecvFlash"))&&
		 (pListen             =   (tListen)            ::GetProcAddress(hmodule,"CTI_Listen"))&&
		 (pUnListen           =   (tUnListen)          ::GetProcAddress(hmodule,"CTI_UnListen"))&&
         (pDelayChEvent       =   (tDelayChEvent)      ::GetProcAddress(hmodule,"CTI_DelayChEvent"))&&
         (pDelaySysEvent      =   (tDelaySysEvent)     ::GetProcAddress(hmodule,"CTI_DelaySysEvent"))&&
		 (pInsertChEvent      =   (tInsertChEvent)     ::GetProcAddress(hmodule,"CTI_InsertChEvent"))&&
		 (pCancelDelay        =   (tCancelDelay)       ::GetProcAddress(hmodule,"CTI_CancelDelay"))&&
		 (pBindExclusiveThread=   (tBindExclusiveThread)::GetProcAddress(hmodule,"CTI_BindExclusiveThread"))&&
		 (pUnBindExclusiveThread= (tUnBindExclusiveThread)::GetProcAddress(hmodule,"CTI_UnBindExclusiveThread"))&&
         (pGetSelectCh        =   (tGetSelectCh)       ::GetProcAddress(hmodule,"CTI_GetSelectCh"))&&
		 (pPauseCh			  =   (tPauseCh)		   ::GetProcAddress(hmodule,"CTI_PauseCh"))&&
		 (pResumeCh			  =   (tResumeCh)          ::GetProcAddress(hmodule,"CTI_ResumeCh")))
	 {

		 return TRUE;
	 }

	 return FALSE;
 }



}
/********************************************************************
	created:	2009/04/12
	created:	12:4:2009   14:01
	filename: 	e:\project\opencti.code\trunk\002\src\cti\API.h
	file path:	e:\project\opencti.code\trunk\002\src\cti
	file base:	API
	file ext:	h
	author:		xugood@live.cn
	
	purpose:	
*********************************************************************/


#pragma once




#include "CtiDefine.h"

namespace CTI
{

//系统服务
typedef int (WINAPI *tInit)(DeviceDescriptor* pctidevicedescriptor);
typedef int (WINAPI *tTerm)();


typedef BOOL (WINAPI *tFeatures)(FUNCTIONADDRINDEX descriptor);
typedef void* (WINAPI *tGetNativeAPI)(LPCTSTR funname);
typedef int (WINAPI *tGetLastErrorCode)();
typedef LPCTSTR (WINAPI *tFormatErrorCode)(int code);
typedef int (WINAPI *tGetChTotal)();
typedef CHANNELTYPE (WINAPI *tGetChType)(int ch);
typedef int (WINAPI *tSearchIdleCh)(CHANNELTYPE chtype, int* pChFilter/* HIWORD=Max  LOWORD=Min */, int FilterLen,int*pChExcept,int ExceptLen);
typedef int (WINAPI *tSetChOwnerData)(int ch,DWORD OwnerData);
typedef DWORD (WINAPI *tGetChOwnerData)(int ch);
typedef CHANNELSTATE (WINAPI *tGetChState)(int ch);




//接续函数
typedef int (WINAPI *tDialup)(int ch,LPCTSTR calling,NUMBERATTRIBUTE callingattribute,LPCTSTR called,LPCTSTR origcalled,NUMBERATTRIBUTE origcalledattribute,BOOL CallerIDBlock,int flags); //CallerIDBlock 去电隐藏
typedef int (WINAPI *tPickup)(int ch,int flags);
typedef int (WINAPI *tRingback)(int ch);
typedef int (WINAPI *tHangup)(int ch,RELEASEATTRIBUTE attribute);



//DTMF函数
typedef int (WINAPI  *tSendDtmf)(int ch,LPCTSTR dtmf,int len,int flags);
typedef int (WINAPI  *tRecvDtmf)(int ch,int time,int flags);
typedef int (WINAPI  *tStopRecvDtmf)(int ch);
typedef int (WINAPI  *tStopSendDtmf)(int ch);
typedef int (WINAPI  *tIsSendDtmf)(int ch);
typedef int (WINAPI  *tIsRecvDtmf)(int ch);




//FSK函数
typedef int (WINAPI  *tSendFsk)(int ch,const BYTE* fsk,int len,int flags);
typedef int (WINAPI  *tSendFskFull)(int ch,int sync,int mark,const BYTE* fsk,int len,int flags);
typedef int (WINAPI  *tStopSendFsk)(int ch);
typedef int (WINAPI  *tIsSendFsk)(int ch);

typedef int (WINAPI  *tRecvFsk)(int ch,int time,int flags);
typedef int (WINAPI  *tStopRecvFsk)(int ch);
typedef int (WINAPI  *tIsRecvFsk)(int ch);




//Tone音
typedef int (WINAPI  *tSendTone)(int ch,TONEATTRIBUTE attribute,int time,int flags);
typedef int (WINAPI  *tSendToneEx)(int ch, int nFreq1, int nVolume1, int nFreq2, int nVolume2,DWORD dwOnTime, DWORD dwOffTime,int time,int flags);
typedef int (WINAPI  *tStopSendTone)(int ch);
typedef int (WINAPI  *tIsSendTone)(int ch);

typedef int (WINAPI  *tRecvTone)(int ch,TONEATTRIBUTE attribute,int time,int flags);
typedef int (WINAPI  *tStopRecvTone)(int ch,TONEATTRIBUTE attribute);
typedef int (WINAPI  *tIsRecvTone)(int ch,TONEATTRIBUTE attribute);


//media函数
typedef int (WINAPI *tPlayFile)(int ch,LPCTSTR fullpath,LPCTSTR StopstrDtmfCharSet);
typedef int (WINAPI *tPlayMemory)(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet);
typedef int (WINAPI *tPlayQueue)(int ch,LPCTSTR fullpath[],int size,LPCTSTR StopstrDtmfCharSet);
typedef int (WINAPI *tStopPlay)(int ch);
typedef int (WINAPI *tIsPlay)(int ch);

typedef int (WINAPI *tLoadPlayIndex)(LPCTSTR fullpath,LPCTSTR Alias);
typedef int (WINAPI *tPlayIndex)(int ch,LPCTSTR Alias,LPCTSTR StopstrDtmfCharSet);

typedef int (WINAPI *tUpDatePlayMemory)(int ch,int index,BYTE* block,int size);


typedef int (WINAPI *tRecordFile)(int ch,LPCTSTR fullpath,int time,LPCTSTR StopstrDtmfCharSet);
typedef int (WINAPI *tRecordMemory)(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet);
typedef int (WINAPI *tStopRecord)(int ch);
typedef int (WINAPI *tIsRecord)(int ch);
typedef int (WINAPI *tUpDateRecordMemory)(int ch,int index,BYTE* block,int size);


//闪断
typedef int (WINAPI *tSendFlash)(int ch,int time);
typedef int (WINAPI *tRecvFlash)(int ch);
typedef int (WINAPI *tStopRecvFlash)(int ch);
typedef int (WINAPI *tIsRecvFlash)(int ch);



//Exchange总线
typedef int (WINAPI *tListen)(int srcch,int desch);
typedef int (WINAPI *tUnListen)(int srch,int desch);


//辅助函数
typedef int (WINAPI *tDelayChEvent)(int cmsDelay,BOOL once,int chid,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize);
typedef int (WINAPI *tDelaySysEvent)(int cmsDelay,BOOL once,int identify,LPVOID eventdata,int eventsize);
typedef int (WINAPI *tInsertChEvent)(int chid,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize);
typedef int (WINAPI *tCancelDelay)(int ch,int delayid);

typedef int (WINAPI *tBindExclusiveThread)(int ch);
typedef int (WINAPI *tUnBindExclusiveThread)(int ch);


typedef int (WINAPI *tGetSelectCh)();

typedef void (WINAPI *tPauseCh)(int ch);
typedef void (WINAPI *tResumeCh)(int ch);





extern tInit                        pInit;
extern tTerm                        pTerm;

extern tFeatures		            pFeatures;
extern tGetNativeAPI                pGetNativeAPI;
extern tGetLastErrorCode            pGetLastErrorCode;

extern tFormatErrorCode             pFormatErrorCode;
extern tGetChTotal                  pGetChTotal;
extern tGetChType				    pGetChType;
extern tSetChOwnerData              pSetChOwnerData;    
extern tGetChOwnerData              pGetChOwnerData;
extern tGetChState	                pGetChState;
extern tSearchIdleCh                pSearchIdleCh;
extern tLoadPlayIndex               pLoadPlayIndex;




extern tDialup			            pDialup;
extern tPickup			            pPickup;
extern tRingback		            pRingback;
extern tHangup			            pHangup;


extern tSendDtmf		            pSendDtmf;
extern tRecvDtmf		            pRecvDtmf;
extern tStopRecvDtmf		        pStopRecvDtmf;
extern tStopSendDtmf		        pStopSendDtmf;
extern tIsSendDtmf		            pIsSendDtmf;
extern tIsRecvDtmf		            pIsRecvDtmf;


extern tSendFsk			            pSendFsk;
extern tSendFskFull                 pSendFskFull;
extern tRecvFsk			            pRecvFsk;
extern tStopRecvFsk			        pStopRecvFsk;
extern tStopSendFsk			        pStopSendFsk;
extern tIsSendFsk			        pIsSendFsk;
extern tIsRecvFsk			        pIsRecvFsk;


extern tSendTone                    pSendTone;
extern tSendToneEx                  pSendToneEx;
extern tStopSendTone                pStopSendTone;
extern tIsSendTone                  pIsSendTone;


extern tRecvTone                    pRecvTone;
extern tStopRecvTone                pStopRecvTone;
extern tIsRecvTone                  pIsRecvTone;

extern tPlayFile		            pPlayFile;
extern tPlayMemory		            pPlayMemory;
extern tPlayQueue	                pPlayQueue;
extern tPlayIndex                   pPlayIndex;


extern tIsPlay		                pIsPlay;
extern tStopPlay		            pStopPlay;
extern tUpDatePlayMemory            pUpDatePlayMemory;

extern tRecordFile		            pRecordFile;
extern tRecordMemory		        pRecordMemory;
extern tStopRecord		            pStopRecord;
extern tIsRecord	                pIsRecord;
extern tUpDateRecordMemory          pUpDateRecordMemory;

extern tSendFlash                   pSendFlash;
extern tRecvFlash                   pRecvFlash;
extern tStopRecvFlash               pStopRecvFlash;
extern tIsRecvFlash                 pIsRecvFlash;

extern tListen			            pListen;
extern tUnListen		            pUnListen;

extern tDelayChEvent                pDelayChEvent;
extern tDelaySysEvent               pDelaySysEvent;
extern tInsertChEvent               pInsertChEvent;
extern tCancelDelay                 pCancelDelay;

extern tBindExclusiveThread			pBindExclusiveThread;
extern tUnBindExclusiveThread		pUnBindExclusiveThread;

extern tGetSelectCh                 pGetSelectCh;

extern tPauseCh						pPauseCh;
extern tResumeCh					pResumeCh;


BOOL LoadInterface(HMODULE hmodule);

}
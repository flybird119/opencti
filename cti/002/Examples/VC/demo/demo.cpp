// demo.cpp : 定义控制台应用程序的入口点。
//

#include "StdAfx.h"

#include "../../../src/cti/API.h"

using namespace CTI;



void WINAPI CtiProc(LPCHANNELEVENT lpchevent)
{
	printf("ThreadID:%d EventID %d  EventName:%S\n",GetCurrentThreadId(),lpchevent->chid,EventDescription(lpchevent->eventid));


	int ch=lpchevent->chid;

	switch(lpchevent->eventid)
	{
	case EVENT_ADDCHANNEL:
		{

		}
		break;
	case EVENT_CALLIN:
		{

			LPCALLERINFO  callininfo=(LPCALLERINFO)lpchevent->eventdata;
			printf("%S %S\n",callininfo->called,callininfo->calling);


			//pBindExclusiveThread(ch);
			pRingback(ch);
		}
		break;
	case EVENT_RINGING:
		{
			pPickup(ch,CHECKFLAG_CHECKHANG|CHECKFLAG_CHECKTALK);
			//pRecvDtmf(ch,0,0);
		}
		break;
	case EVENT_TALKING:
		{


			//int ret=pPlayFile(ch,_T("C:\\DJDBDK\\Voc\\Bank.001"),STOPSOUNDCHARSETNONE);

			int ret=pRecordFile(ch,_T("C:\\fsk.voc"),0,STOPSOUNDCHARSETNONE);


			if(ret==0)
			{
				_tprintf(_T("Errmsg:%s\n"),pFormatErrorCode(pGetLastErrorCode()));
			}
		}
		break;
	case EVENT_IDLE:
		//pUnBindExclusiveThread(ch);
		break;
	}

}


int _tmain(int argc, _TCHAR* argv[])
{
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);


    HMODULE	hmodule=::LoadLibraryEx(_T("ssm.dll"),NULL,0);
    if(hmodule && CTI::LoadInterface(hmodule))
    {
        DeviceDescriptor descriptor;
        descriptor.monitorwnd=NULL;
        descriptor.notifyfun=CtiProc;
        descriptor.notifyparam=0;
		descriptor.threadmode=0;

        pInit(&descriptor);
    }
    else
    {

        printf("载入接口失败");
    }

    getchar();

    if(hmodule)
    {
        pTerm();
        ::FreeLibrary(hmodule);
    }


	return 0;
}


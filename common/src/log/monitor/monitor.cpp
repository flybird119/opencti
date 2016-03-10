
// monitor.cpp : main source file for monitor.exe
//



#include "stdafx.h"

#pragma warning(disable:4996)


#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlmisc.h>

#include <atlwin.h>





#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlctrlx.h>



#include ".\resource.h"

#include <map>
#include <string>

#include ".\ui\TabCtrlXbuttonImpl.h"
#include ".\ui\TrayIcon.h"
#include "..\LogMessage.h"
#include "..\LogServer.h"

#define     WM_PRINTLOG     WM_USER+100
#define     WM_TRAYICON     WM_USER+101
#define     WM_FREECFGDLG   WM_USER+102
#define     WM_SHOWLOG      WM_USER+103

BOOL        g_LogOnOff      =   FALSE;




#include "monitorView.h"
#include "aboutdlg.h"
#include "LogConfigDlg.h"
#include "MainFrm.h"





CAppModule _Module;

HWND       g_NotifyhWnd     =   NULL;
HANDLE     g_UIhThread      =   NULL;
DWORD      g_UiThreadID     =   0;     
HANDLE     g_UiStartEvent   =   NULL;




extern HMODULE g_hDllModule;






int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}
    g_NotifyhWnd = wndMain.m_hWnd;

	wndMain.ShowWindow(nCmdShow);

    g_LogOnOff=wndMain.IsWindowVisible();

    SetEvent(g_UiStartEvent);


	int nRet = theLoop.Run();

    g_NotifyhWnd=NULL;

	_Module.RemoveMessageLoop();
	return nRet;
}

DWORD WINAPI UiMain(LPVOID nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls



    hRes = _Module.Init(NULL, g_hDllModule);
	ATLASSERT(SUCCEEDED(hRes));

	HMODULE hInstRich = ::LoadLibrary(CRichEditCtrl::GetLibraryName());
	ATLASSERT(hInstRich != NULL);

    int nRet = Run(::GetCommandLine(),(int)nCmdShow);

	::FreeLibrary(hInstRich);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}







//

int  Log_StartUI(int nCmdShow)
{
    g_UiStartEvent=::CreateEvent(NULL,FALSE,FALSE,NULL);

    HANDLE hthread=CreateThread(NULL,0,UiMain,(LPVOID)nCmdShow,0,&g_UiThreadID);
    if(hthread)
    {


        g_UIhThread = hthread;

        ::WaitForSingleObject(g_UiStartEvent,1000);
        ::CloseHandle(g_UiStartEvent);
        g_UiStartEvent=NULL;

        return 1;
    }

    return 0;
}

int  Log_StopUI()
{



    if(g_NotifyhWnd)
    {    
        SendMessage(g_NotifyhWnd,WM_COMMAND,MAKEWPARAM(ID_APP_EXIT,1),0);
    }

    if(g_UIhThread!=NULL)
    {
        WaitForSingleObject(g_UIhThread,INFINITE);
        CloseHandle(g_UIhThread);
    }

    g_NotifyhWnd=NULL;
    g_UIhThread=NULL;
    g_UiThreadID=0;

    return 1;
}


int  Log_ShowLog(const char* channel,BOOL bShow)
{
    if(g_NotifyhWnd==NULL) return 0;

    int buflen=strlen(channel);
    char* ptmpbuf=new char[buflen+1];
    strncpy(ptmpbuf,channel,buflen);
    ptmpbuf[buflen]=0;

    return PostMessage(g_NotifyhWnd,WM_SHOWLOG,(WPARAM)ptmpbuf,bShow);
}


int  Log_PrintLog(const char* channel,char* pstrstream,LoggingSeverity__ sev)
{

    if(!g_LogOnOff || g_NotifyhWnd==NULL)  return 0;


    struct LOGINFO 
    {
        const char* channel;
        char* pstrstream;
    };
    LOGINFO* pLogInfo=new LOGINFO;
  
    pLogInfo->channel=channel;
    pLogInfo->pstrstream=pstrstream;

    if(PostMessage(g_NotifyhWnd,WM_PRINTLOG,(WPARAM)pLogInfo,(LPARAM)sev))
    {
        return 1;
    }

    if(pLogInfo->pstrstream)
        free(pLogInfo->pstrstream);

    delete pLogInfo;

    return 0;
}

#pragma warning(default:4996)
#pragma once

#include <windows.h>

typedef struct createdlgtag
{
	HINSTANCE hInstance;
	LPCTSTR lpTemplate;
	HWND hWndParent;
	DLGPROC lpDialogFunc;

	HWND*	phWnd;
	HANDLE  hEvent;

} CREATEDIALOG;


typedef struct destroywindowtag
{
	HWND	hWnd;

	BOOL*   ret;
	HANDLE  hEvent;

} DESTROYWINDOW;



#define		WM_CREATEDIALOG		(WM_USER+100)
#define		WM_DESTROYWINDOW	(WM_USER+101)


class CLoopMessage
{
public:

	CLoopMessage():
		m_threadhandle(NULL),
		m_threadid(0)
	
	{

	}
	BOOL  Start()
	{
		if(m_threadhandle==NULL && m_threadid==0)
		{
			HANDLE hevent=::CreateEvent(NULL,FALSE,FALSE,NULL);
			m_threadhandle=::CreateThread(NULL,0,&CLoopMessage::WorkThread,(LPVOID)hevent,0,&m_threadid);
			if(m_threadhandle)
			{
				::WaitForSingleObject(hevent,INFINITE);
				::CloseHandle(hevent);
				return TRUE;
			}
		}

		return FALSE;
	}


	BOOL Stop()
	{
		if(m_threadhandle!=NULL && m_threadid!=0)
		{
			::PostThreadMessage(m_threadid,WM_QUIT,0,0);
			::WaitForSingleObject(m_threadhandle,INFINITE);
			::CloseHandle(m_threadhandle);
			m_threadhandle=NULL;
			m_threadid=0;
			return TRUE;
		}
		return FALSE;
	}


	HWND CreateDlg(HINSTANCE hInstance,LPCTSTR lpTemplate,HWND hWndParent,DLGPROC lpDialogFunc)
	{
		HWND hwnd=NULL;

		if(::GetCurrentThreadId()==m_threadid)
		{
			hwnd=::CreateDialog(hInstance,lpTemplate,hWndParent,lpDialogFunc);
		}
		else
		{
			HANDLE hEvent=::CreateEvent(NULL,FALSE,FALSE,NULL);
			CREATEDIALOG createdialog={hInstance,lpTemplate,hWndParent,lpDialogFunc,&hwnd,hEvent};
			if(::PostThreadMessage(m_threadid,WM_CREATEDIALOG,(WPARAM)&createdialog,sizeof(CREATEDIALOG)))
			{
				::WaitForSingleObject(hEvent,INFINITE);
			}
			::CloseHandle(hEvent);
		}
		return hwnd;
	}

	BOOL DestroyWindow(HWND hwnd)
	{
		BOOL ret=FALSE;
		if(::GetCurrentThreadId()==m_threadid)
		{
			ret=::DestroyWindow(hwnd);
		}
		else
		{
			HANDLE hEvent=::CreateEvent(NULL,FALSE,FALSE,NULL);
			DESTROYWINDOW destroywindow={hwnd,&ret,hEvent};
			if(::PostThreadMessage(m_threadid,WM_DESTROYWINDOW,(WPARAM)&destroywindow,sizeof(DESTROYWINDOW)))
			{
				::WaitForSingleObject(hEvent,INFINITE);

			}
			::CloseHandle(hEvent);
		}
		return ret;
	}


	static DWORD WINAPI WorkThread(LPVOID param)
	{
		MSG msg;
		::PeekMessage(&msg,0,0,0,PM_NOREMOVE);
		::SetEvent((HANDLE)param);

		while (GetMessage(&msg, NULL, 0, 0))
		{
			if(msg.hwnd==0)
			{
				switch(msg.message)
				{
				case WM_CREATEDIALOG:
					{
						CREATEDIALOG*pcreate=(CREATEDIALOG*)msg.wParam;
						_ASSERT(msg.lParam==sizeof(CREATEDIALOG));
						*pcreate->phWnd=::CreateDialog(pcreate->hInstance,pcreate->lpTemplate,pcreate->hWndParent,pcreate->lpDialogFunc);
						::SetEvent(pcreate->hEvent);
					}
					break;
				case WM_DESTROYWINDOW:
					{
						DESTROYWINDOW*pdestroy=(DESTROYWINDOW*)msg.wParam;
						_ASSERT(msg.lParam==sizeof(DESTROYWINDOW));
						*pdestroy->ret=::DestroyWindow(pdestroy->hWnd);
						::SetEvent(pdestroy->hEvent);
					}
					break;
				}
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}



		return msg.wParam;
	}

private:
	HANDLE		m_threadhandle;
	DWORD		m_threadid;

};

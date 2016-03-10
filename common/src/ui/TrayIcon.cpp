/********************************************************************
	created:	2008/12/08
	created:	8:12:2008   20:41
	filename: 	f:\xugood.comm\src\ui\TrayIcon.cpp
	file path:	f:\xugood.comm\src\ui
	file base:	TrayIcon
	file ext:	cpp
	author:		xugood@live.cn
	
	purpose:	从网上下载的进行改行的托盘类
*********************************************************************/



#include "stdafx.h"
#include "TrayIcon.h"
#include <shellapi.h>
#include <assert.h>
#include <commctrl.h>




const UINT WM_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));


LRESULT CALLBACK HookNotifyHWNDProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CTrayIcon* ptrayicon=(CTrayIcon*)::GetProp(hWnd,_T("CTrayIcon"));
	if(ptrayicon)
	{
		if(message==ptrayicon->m_tnd.uCallbackMessage)
		{
			ptrayicon->OnTrayNotification(wParam,lParam);
		}
		if(message==WM_TASKBARCREATED)
		{
			if(!ptrayicon->m_bHidden)
            {
                ptrayicon->m_bHidden=TRUE;
			    ptrayicon->ShowIcon();
            }
		}

		if(ptrayicon->m_origwndproc)
		{
			return CallWindowProc(ptrayicon->m_origwndproc,hWnd,message,wParam,lParam);
		}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


CTrayIcon::CTrayIcon()
{
	memset(&m_tnd, 0, sizeof(m_tnd));
	m_bEnabled = FALSE;
	m_bHidden  = FALSE;
	m_hInstance = NULL;
	m_origwndproc=NULL;
}

CTrayIcon::CTrayIcon(HWND hWnd, UINT uCallbackMessage, LPCTSTR szToolTip, 
					 HICON icon, UINT uID,HINSTANCE hInstance)
{
	Create(hWnd, uCallbackMessage, szToolTip, icon, uID,hInstance);
	m_bHidden = FALSE;
}

BOOL CTrayIcon::Create(HWND hWnd, UINT uCallbackMessage, LPCTSTR szToolTip, 
					   HICON icon, UINT uID,HINSTANCE hInstance)
{

	if(m_bEnabled) return FALSE;

	if(hInstance) 
	{
		m_hInstance=hInstance;
	}
	else
	{
		m_hInstance=::GetModuleHandle(NULL);
	}

	// this is only for Windows 95 (or higher)
	m_bEnabled = ( GetVersion() & 0xff ) >= 4;
	if (!m_bEnabled) return FALSE;


	m_bEnabled = ::IsWindow(hWnd);
	if (!m_bEnabled) return FALSE;
	
	//Make sure we avoid conflict with other messages
	assert(uCallbackMessage >= WM_USER);

	//Tray only supports tooltip text up to 64 characters
	assert(_tcslen(szToolTip) <= 64);

	// load up the NOTIFYICONDATA structure
	m_tnd.cbSize = sizeof(NOTIFYICONDATA);
	m_tnd.hWnd	 = hWnd;
	m_tnd.uID	 = uID;
	m_tnd.hIcon  = icon;
	m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_tnd.uCallbackMessage = uCallbackMessage;
	_tcsncpy(m_tnd.szTip, szToolTip,(sizeof m_tnd.szTip)/(sizeof m_tnd.szTip[0]));


	//HOOK 窗口函数
	::SetProp(hWnd,_T("CTrayIcon"),this);
	m_origwndproc=(WNDPROC)SetWindowLong(hWnd,GWL_WNDPROC,(int)HookNotifyHWNDProc);


	// Set the tray icon
	m_bEnabled = Shell_NotifyIcon(NIM_ADD, &m_tnd);
	return m_bEnabled;
}




CTrayIcon::~CTrayIcon()
{
	RemoveIcon();
}


/////////////////////////////////////////////////////////////////////////////
// CTrayIcon icon manipulation

void CTrayIcon::MoveToRight()
{
	HideIcon();
	ShowIcon();
}

void CTrayIcon::RemoveIcon()
{
	if (!m_bEnabled) return;


	if(::IsWindow(m_tnd.hWnd))
	{
		int ret=::SetWindowLong(m_tnd.hWnd,GWL_WNDPROC,(int)m_origwndproc);
        assert(ret==(int)HookNotifyHWNDProc);
	}
	
    RemoveProp(m_tnd.hWnd,_T("CTrayIcon"));
	m_origwndproc=NULL;

	m_tnd.uFlags = 0;
    Shell_NotifyIcon(NIM_DELETE, &m_tnd);
    m_bEnabled = FALSE;
}

void CTrayIcon::HideIcon()
{
	if (m_bEnabled && !m_bHidden) {
		m_tnd.uFlags = NIF_ICON;
		Shell_NotifyIcon (NIM_DELETE, &m_tnd);
		m_bHidden = TRUE;
	}
}

void CTrayIcon::ShowIcon()
{
	if (m_bEnabled && m_bHidden) {
		m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		Shell_NotifyIcon(NIM_ADD, &m_tnd);
		m_bHidden = FALSE;
	}
}

BOOL CTrayIcon::SetIcon(HICON hIcon)
{
	if (!m_bEnabled) return FALSE;

	m_tnd.uFlags = NIF_ICON;
	m_tnd.hIcon = hIcon;

	return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

BOOL CTrayIcon::SetIcon(LPCTSTR lpszIconName)
{
	HICON hIcon = LoadIcon(m_hInstance,lpszIconName);

	return SetIcon(hIcon);
}

BOOL CTrayIcon::SetIcon(UINT nIDResource)
{
	HICON hIcon = LoadIcon(m_hInstance,MAKEINTRESOURCE(nIDResource));

	return SetIcon(hIcon);
}

BOOL CTrayIcon::SetStandardIcon(LPCTSTR lpIconName)
{
	HICON hIcon = LoadIcon(m_hInstance, lpIconName);

	return SetIcon(hIcon);
}

BOOL CTrayIcon::SetStandardIcon(UINT nIDResource)
{
	HICON hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(nIDResource));

	return SetIcon(hIcon);
}
 
HICON CTrayIcon::GetIcon() const
{
	HICON hIcon = NULL;
	if (m_bEnabled)
		hIcon = m_tnd.hIcon;

	return hIcon;
}

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon tooltip text manipulation

BOOL CTrayIcon::SetTooltipText(LPCTSTR pszTip)
{
	if (!m_bEnabled) return FALSE;

	m_tnd.uFlags = NIF_TIP;
	_tcsncpy(m_tnd.szTip, pszTip,(sizeof m_tnd.szTip)/(sizeof m_tnd.szTip[0]));

	return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

BOOL CTrayIcon::SetTooltipText(UINT nID)
{
	TCHAR buf[64];
	::LoadString(m_hInstance,nID,buf,(sizeof buf)/(sizeof TCHAR));
	return SetTooltipText(buf);
}

LPCTSTR CTrayIcon::GetTooltipText() const
{
	if (m_bEnabled)
		return  m_tnd.szTip;

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CTrayIcon notification window stuff

BOOL CTrayIcon::SetNotificationWnd(HWND hWnd)
{
	if (!m_bEnabled) return FALSE;

	//Make sure Notification window is valid
	assert(::IsWindow(hWnd));

	::SetWindowLong(m_tnd.hWnd,GWL_WNDPROC,(int)m_origwndproc);

	::SetProp(hWnd,_T("CTrayIcon"),this);
	m_origwndproc=(WNDPROC)SetWindowLong(hWnd,GWL_WNDPROC,(int)HookNotifyHWNDProc);

	m_tnd.hWnd = hWnd;
	m_tnd.uFlags = 0;

	return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

HWND CTrayIcon::GetNotificationWnd() const
{
	return m_tnd.hWnd;
}


BOOL   CTrayIcon::SetInstance(HINSTANCE hInstance)
{
	if(hInstance)
	{
		m_hInstance=hInstance;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

HINSTANCE CTrayIcon::GetInstance()
{
	return m_hInstance;
}


BOOL	CTrayIcon::SetMenuID(UINT nID)
{
	if (!m_bEnabled) return FALSE;
	m_tnd.uFlags = 0;
	Shell_NotifyIcon(NIM_DELETE, &m_tnd);
	m_tnd.uID = nID;
	m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_bEnabled = Shell_NotifyIcon(NIM_ADD, &m_tnd);
	return TRUE;
}

UINT	CTrayIcon::GetMenuID()
{
	if (!m_bEnabled) return NULL;
	return	m_tnd.uID;
}


/////////////////////////////////////////////////////////////////////////////
// CTrayIcon implentation of OnTrayNotification

LRESULT CTrayIcon::OnTrayNotification(UINT wParam, LONG lParam) 
{
	//Return quickly if its not for this tray icon
	if (wParam != m_tnd.uID)
		return 0L;

	HMENU hmenu, hSubMenu;

	switch(LOWORD(lParam))
	{
			
		case WM_RBUTTONUP:
		{	
			if (!(hmenu=::LoadMenu(m_hInstance,MAKEINTRESOURCE(m_tnd.uID)))) return 0;
			if (!(hSubMenu = ::GetSubMenu(hmenu,0))) return 0;

			// Make first menu item the default (bold font)
			::SetMenuDefaultItem(hSubMenu,0,TRUE);

			//Display and track the popup menu
			POINT pos;
			GetCursorPos(&pos);
			::SetForegroundWindow(m_tnd.hWnd);  
			::TrackPopupMenu(hSubMenu, 0, pos.x, pos.y, 0, m_tnd.hWnd, NULL);

			//pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, this, NULL);
			DestroyMenu(hmenu);
		} 
		break;
        case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
		{
		
			if (!(hmenu=LoadMenu(m_hInstance,MAKEINTRESOURCE(m_tnd.uID)))) return 0;
			if (!(hSubMenu = GetSubMenu(hmenu,0))) return 0;

			// double click received, the default action is to execute first menu item
			::SetForegroundWindow(m_tnd.hWnd);
			::SendMessage(m_tnd.hWnd, WM_COMMAND, GetMenuItemID(hSubMenu,0), 0);
			DestroyMenu(hmenu);

			
		}
		break;
		/*

		case NIN_BALLOONSHOW://显示气球提示
			break;
		case NIN_BALLOONTIMEOUT://当由于超时或者用户单击气球上的关闭按钮(X)，使气球消失时发送此
		//	MessageBoxW(NULL,NULL,_T(""),NULL);
			break;
		case NIN_BALLOONUSERCLICK://当用户在气球提示上或托盘图标上单击鼠标（此时气球处于显示状态）时发送此消息
			break;
		case NIN_BALLOONHIDE:	//气球提示消失时发送；例如，当图标被删除，如果因为超时或是用户单击鼠标气球消失，此消息不会被发送
					//	MessageBoxW(NULL,NULL,_T("1"),NULL);
			break;
*/

	}

	return 1;
}


BOOL CTrayIcon::PopInfo(LPCTSTR szInfo,UINT uTimeout,LPCTSTR szInfoTitle,DWORD dwInfoFlags)
{
	if(uTimeout>30000) uTimeout=30000;
	if(uTimeout<10000) uTimeout=10000;
	m_tnd.uTimeout=uTimeout;
	m_tnd.dwInfoFlags=dwInfoFlags;
	m_tnd.uFlags|=NIF_INFO;
	_tcsncpy(m_tnd.szInfo,szInfo,(sizeof m_tnd.szInfo)/(sizeof m_tnd.szInfo[0]));
	_tcsncpy(m_tnd.szInfoTitle,szInfoTitle,(sizeof m_tnd.szInfoTitle)/(sizeof m_tnd.szInfoTitle[0]));
	return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}


void CTrayIcon::MinimizeToTray()
{
   // ::ShowWindow(m_tnd.hWnd,SW_MINIMIZE);

    RECT rc;
    if(GetTrayIconRect(&rc))
	{
		RECT winrc;
		GetWindowRect(m_tnd.hWnd,&winrc);
		DrawAnimatedRects(m_tnd.hWnd,IDANI_CAPTION,&winrc,&rc);
	}
    ::ShowWindow(m_tnd.hWnd,SW_HIDE);

}

BOOL CTrayIcon::GetTrayIconRect(RECT* rc)
{
    HWND  hStatus=::FindWindow(_T("Shell_TrayWnd"),NULL);  //得到任务栏句柄
    if  (hStatus==NULL)  
    {  
       return FALSE;
    }  
    HWND  hNotify=::FindWindowEx(hStatus,NULL,_T("TrayNotifyWnd"),NULL); //右下角区域 
    if  (hNotify==NULL)
    {  
       return FALSE;
    }  
    HWND  hPager=::FindWindowEx(hNotify,NULL,_T("SysPager"),NULL);
    if  (hPager==NULL)  
    {  
       return FALSE;
    } 
    HWND  hBar=FindWindowEx(hPager,NULL,_T("ToolBarWindow32"),NULL);//右下角区域(不包括时间)
    if  (hBar==NULL)   
    {  
        return FALSE;
    }  
    //-------------------以上是得到任务栏右下脚一块地方的句柄
    DWORD  pid = 0;  
    GetWindowThreadProcessId(hBar,&pid);  
    if  (pid==NULL)  
    {  
         return FALSE;
    }   

    HANDLE  hProcess=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_ALL_ACCESS,FALSE,pid);  
    if  (hProcess==NULL)  
    {  
        return FALSE;
    }  

    int  iNum=::SendMessage(hBar,TB_BUTTONCOUNT ,NULL,NULL);  //获取任务栏上图标个数


    TBBUTTON  *pButton = new TBBUTTON;  

    TBBUTTON  *BButton = (TBBUTTON*)VirtualAllocEx(hProcess, NULL, sizeof(TBBUTTON), MEM_COMMIT, PAGE_READWRITE);

    unsigned long n = 0;  
    HWND  whd;

    BOOL bret=FALSE;

    for(int i=0; i<iNum; i++)  
    {  
        ::SendMessage(hBar,TB_GETBUTTON,i,(LPARAM)(BButton));  
        ReadProcessMemory(hProcess,BButton,pButton,sizeof(TBBUTTON),&n);   
        
        try
        {   
            whd=0;   
            ReadProcessMemory(hProcess,(void*)pButton->dwData,&whd,sizeof(whd),&n);   

            if(whd==m_tnd.hWnd)
            {
                RECT  *buttonrc = (RECT*)VirtualAllocEx(hProcess, NULL, sizeof(RECT), MEM_COMMIT, PAGE_READWRITE);
                ::SendMessage(hBar,TB_GETITEMRECT,i,(LPARAM)(buttonrc));
                
                ReadProcessMemory(hProcess,(void*)buttonrc,rc,sizeof(RECT),&n);


                VirtualFreeEx(hProcess, buttonrc, 0, MEM_RELEASE);


                POINT ps={rc->left,rc->top};
                ::ClientToScreen(hBar,&ps);
                ::OffsetRect(rc,ps.x-rc->left,ps.y-rc->top);



                bret = TRUE;

				break;
            }
        }   
        catch(...)
        {   
            break;
        }   
    } 
    delete pButton;
    VirtualFreeEx(hProcess, BButton, 0, MEM_RELEASE);

    return bret;
}


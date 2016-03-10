/********************************************************************
	created:	2008/12/08
	created:	8:12:2008   20:42
	filename: 	f:\xugood.comm\src\ui\TrayIcon.h
	file path:	f:\xugood.comm\src\ui
	file base:	TrayIcon
	file ext:	h
	author:		xugood@live.cn
	
	purpose:	
*********************************************************************/

/////////////////////////////////////////////////////////////////////////////
// TrayIcon.h : header file
//

#ifndef _INCLUDED_TRAYICON_H_
#define _INCLUDED_TRAYICON_H_


#include <windows.h>
#include <tchar.h>


/////////////////////////////////////////////////////////////////////////////
// CTrayIcon window

class CTrayIcon
{
// Construction/destruction
public:
	CTrayIcon();
	CTrayIcon(HWND hWnd, UINT uCallbackMessage, LPCTSTR szTip, HICON icon, UINT uID,HINSTANCE hInstance=NULL);
	virtual ~CTrayIcon();


	friend LRESULT CALLBACK HookNotifyHWNDProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
// Operations
public:
	BOOL Enabled() { return m_bEnabled; }
	BOOL Visible() { return !m_bHidden; }

	//Create the tray icon
	int Create(HWND hWnd, UINT uCallbackMessage, LPCTSTR szTip, HICON icon, UINT uID,HINSTANCE hInstance=NULL);


	BOOL   SetInstance(HINSTANCE hInstance);
	HINSTANCE GetInstance();

	BOOL	SetMenuID(UINT nID);
	UINT	GetMenuID();


	//Change or retrieve the Tooltip text
	BOOL    SetTooltipText(LPCTSTR pszTooltipText);
	BOOL    SetTooltipText(UINT nID);
	LPCTSTR GetTooltipText() const;

	//Change or retrieve the icon displayed
	BOOL  SetIcon(HICON hIcon);
	BOOL  SetIcon(LPCTSTR lpIconName);
	BOOL  SetIcon(UINT nIDResource);
	BOOL  SetStandardIcon(LPCTSTR lpIconName);
	BOOL  SetStandardIcon(UINT nIDResource);
	HICON GetIcon() const;
	void  HideIcon();
	void  ShowIcon();
	void  RemoveIcon();
	void  MoveToRight();

	//Change or retrieve the window to send notification messages to
	BOOL  SetNotificationWnd(HWND pNotifyhWnd);
	HWND  GetNotificationWnd() const;

	//Default handler for tray notification message
	LRESULT OnTrayNotification(WPARAM uID, LPARAM lEvent);


	BOOL PopInfo(LPCTSTR szInfo,UINT uTimeout,LPCTSTR szInfoTitle,DWORD dwInfoFlags=NIIF_NONE);


    void MinimizeToTray();

    BOOL GetTrayIconRect(RECT* rc);

protected:
	BOOL			m_bEnabled;	// does O/S support tray icon?
	BOOL			m_bHidden;	// Has the icon been hidden?
	NOTIFYICONDATA	m_tnd;
	HINSTANCE		m_hInstance;	//资源句柄
	WNDPROC			m_origwndproc;	//原始的窗口函数

};


#endif

/////////////////////////////////////////////////////////////////////////////

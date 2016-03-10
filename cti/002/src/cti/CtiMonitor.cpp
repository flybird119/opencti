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
	created:	2009/01/07
	created:	7:1:2009   9:15
	filename: 	e:\xugood.cti\src\CtiComm\CtiMonitor.cpp
	file path:	e:\xugood.cti\src\CtiComm
	file base:	CtiMonitor
	file ext:	cpp
	author:		xugood@live.cn
	
	purpose:	
*********************************************************************/
#include "StdAfx.h"
#include "CtiMonitor.h"

#include <Windows.h>
#include <Commctrl.h>

#include <process.h>

#include "Common/frequent.h"

CtiMonitor::CtiMonitor(std::vector<ChannelInfo*>& channel,COMMON::CLock& lock):
    m_channel(channel),
    m_MainhWnd(NULL),
    m_ListViewhWnd(NULL),
    m_lock(lock),
    m_hinst(0),
    m_parentwnd(NULL),
    m_iThreadID(0),
    m_hThreadEvent(NULL),
    m_hThread(NULL)
{
}

CtiMonitor::~CtiMonitor(void)
{


}


HWND CtiMonitor::Create(HINSTANCE hinst,HWND hWnd)
{

    if(m_MainhWnd) 
        return m_MainhWnd;

    m_hinst=hinst;
    m_parentwnd=hWnd;

    m_hThread=(HANDLE)_beginthreadex(NULL,0,&CtiMonitor::UIThread,this,0,&m_iThreadID);


    m_hThreadEvent=CreateEvent(NULL,FALSE,FALSE,NULL);

    WaitForSingleObject(m_hThreadEvent,INFINITE);

    CloseHandle(m_hThreadEvent);

    m_hThreadEvent=NULL;

    return m_MainhWnd;
}

void CtiMonitor::Destroy()
{
    if(m_MainhWnd)
    {
        HWND tmpwnd=m_MainhWnd;
        m_MainhWnd=NULL;
        SendMessage(tmpwnd,WM_CLOSE,0,0);

        WaitForSingleObject(m_hThread,INFINITE);
        CloseHandle(m_hThread);
    }
}



void CtiMonitor::SetItemCount(int items)
{
    if(m_ListViewhWnd!=NULL)
    {
        ::PostMessage(m_ListViewhWnd,LVM_SETITEMCOUNT,(WPARAM)items,0);
    }
}

void CtiMonitor::Update(int item,int column)
{
    if(m_ListViewhWnd!=NULL)
    {
        ::PostMessage(m_MainhWnd,WM_USER+200,item,column);
    }


}


int CtiMonitor::GetSelectCh()
{
    if(m_ListViewhWnd!=NULL)
    {
        return (int)::SendMessage(m_ListViewhWnd, LVM_GETNEXTITEM, (WPARAM)-1, MAKELPARAM(LVNI_SELECTED, 0));            
    }

    return -1;
}
unsigned int WINAPI CtiMonitor::UIThread(void* param)
{

    CtiMonitor* pThis=(CtiMonitor*)param;
  
    pThis->StartUI();

     SetEvent(pThis->m_hThreadEvent);

    MSG msg;
    // 主消息循环:
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


LRESULT CALLBACK CtiMonitor::MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   
    BOOL bHandled=FALSE;
    LRESULT result=0;
    switch(message)
    {
    case WM_NOTIFY:
        {

            CtiMonitor* pMonitor = (CtiMonitor*)GetProp(hWnd, _T("CtiMonitor"));
            if(pMonitor!=NULL)
            {
                LPNMHDR lpnmhdr=(LPNMHDR)lParam;
                switch(lpnmhdr->code)
                {
                case LVN_GETDISPINFO:
                    {
                        if(lpnmhdr->hwndFrom==pMonitor->m_ListViewhWnd)
                        {
                            bHandled=TRUE;
                            result=pMonitor->OnGetDispInfo((int)wParam,lpnmhdr,bHandled);
                        }

                    }
                    break;
                }
            }

        }
        break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;

			hdc = BeginPaint(hWnd, &ps);
			// TODO: 在此添加任意绘图代码...
			EndPaint(hWnd, &ps);
		}

    case WM_SIZE:
        {

            CtiMonitor* pMonitor = (CtiMonitor*)GetProp(hWnd, _T("CtiMonitor"));
            if(pMonitor!=NULL)
            {
                RECT rc;
                ::GetClientRect(hWnd,&rc);
                ::SetWindowPos(pMonitor->m_ListViewhWnd,(HWND)HWND_TOP,rc.left,rc.top,rc.right,rc.bottom,SWP_NOZORDER);
            }
        }
        break;

    case WM_USER+200:
        {
            CtiMonitor* pMonitor = (CtiMonitor*)GetProp(hWnd, _T("CtiMonitor"));
            if(pMonitor!=NULL)
            {

              
/*
#if _MSC_VER<1500
#define LVM_ISITEMVISIBLE    (LVM_FIRST + 182)
#define ListView_IsItemVisible(hwnd, index) \
    (UINT)SNDMSG((hwnd), LVM_ISITEMVISIBLE, (WPARAM)(index), (LPARAM)0)
#endif

                    if(::IsWindowVisible(pMonitor->m_ListViewhWnd) && ListView_IsItemVisible(pMonitor->m_ListViewhWnd, wParam))
*/

                    if(::IsWindowVisible(pMonitor->m_ListViewhWnd))
                    {
                        RECT rc={0};

                        ListView_GetSubItemRect(pMonitor->m_ListViewhWnd,wParam,lParam,LVIR_BOUNDS,&rc);

                        if(!::IsRectEmpty(&rc))
                        {
                            ::InvalidateRect(pMonitor->m_ListViewhWnd, &rc,FALSE);
                        }
                    }
                

            }

        }
        break;


    case WM_DESTROY:
        {
            PostQuitMessage(1);
            return 0;
        }
        break;

    }
    if(bHandled)
    {
        return result;
    }
    return DefWindowProc(hWnd,message,wParam,lParam);

}

HWND   CtiMonitor::StartUI()
{
    DWORD x=CW_USEDEFAULT,y=0,width=CW_USEDEFAULT,height=0;

    const TCHAR szTitle[100]        =   _T("CtiMonitor");					// 标题栏文本
    const TCHAR szWindowClass[100]  =   _T("CtiMonitor");			        // 主窗口类名

    WNDCLASSEX wcex;

    DWORD      style=WS_OVERLAPPEDWINDOW;


    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style			= CS_NOCLOSE;
    wcex.lpfnWndProc	= MainWndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= m_hinst;
    wcex.hIcon			= 0;
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= NULL;
    wcex.lpszClassName	= szWindowClass;
    wcex.hIconSm		= 0;

    RegisterClassEx(&wcex);

    if(m_parentwnd!=NULL)
    {
        style=WS_CHILD|WS_VISIBLE;

        RECT rc;
        ::GetClientRect(m_parentwnd,&rc);
        x=y=0;
        width=rc.right-rc.left;
        height=rc.bottom-rc.top;
    }


    m_MainhWnd = CreateWindow(szWindowClass, szTitle, style,
        x, y, width, height, m_parentwnd, NULL, m_hinst, NULL);

    if(m_parentwnd==NULL)
    {
        ShowWindow(m_MainhWnd, SW_SHOW);
        UpdateWindow(m_MainhWnd);
    }


    if(m_MainhWnd!=NULL)
    {

        SetProp(m_MainhWnd,_T("CtiMonitor"),this);
        RECT mainrc;
        GetClientRect(m_MainhWnd,&mainrc);

        m_ListViewhWnd=::CreateWindowEx(WS_EX_CLIENTEDGE,
            WC_LISTVIEW,
            _T("CtiMonitor"),
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_OWNERDATA,
            mainrc.left,mainrc.top,mainrc.right-mainrc.left,mainrc.bottom-mainrc.top,m_MainhWnd,NULL,m_hinst,NULL);

        if(m_ListViewhWnd!=NULL)
        {
            ListView_SetExtendedListViewStyle(m_ListViewhWnd,LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);


            LVCOLUMN column;
            column.mask = LVCF_TEXT|LVCF_FMT|LVCF_WIDTH;
            column.fmt = LVCFMT_LEFT;
            column.pszText=_T("通道");
            column.cx = 10*4;
            ::SendMessage(m_ListViewhWnd, LVM_INSERTCOLUMN, (WPARAM)COLUMN_CHID, (LPARAM)&column);
            column.pszText=_T("类型");
            column.cx = 10*8;
            ::SendMessage(m_ListViewhWnd, LVM_INSERTCOLUMN, (WPARAM)COLUMN_TYPE, (LPARAM)&column);
            column.pszText=_T("状态");
            column.cx = 10*8;
            ::SendMessage(m_ListViewhWnd, LVM_INSERTCOLUMN, (WPARAM)COLUMN_STATE, (LPARAM)&column);
            column.pszText=_T("方向");
            column.cx = 10*4;
            ::SendMessage(m_ListViewhWnd, LVM_INSERTCOLUMN, (WPARAM)COLUMN_WAY, (LPARAM)&column);
            column.pszText=_T("主叫");
            column.cx = 10*12;
            ::SendMessage(m_ListViewhWnd, LVM_INSERTCOLUMN, (WPARAM)COLUMN_CALLING, (LPARAM)&column);
            column.pszText=_T("被叫");
            column.cx = 10*12;
            ::SendMessage(m_ListViewhWnd, LVM_INSERTCOLUMN, (WPARAM)COLUMN_CALLED, (LPARAM)&column);
            column.pszText=_T("原始被叫");
            column.cx = 10*12;
            ::SendMessage(m_ListViewhWnd, LVM_INSERTCOLUMN, (WPARAM)COLUMN_ORIGCALLED, (LPARAM)&column);
            column.pszText=_T("DTMF");
            column.cx = 10*10;
            ::SendMessage(m_ListViewhWnd, LVM_INSERTCOLUMN, (WPARAM)COLUMN_DTMF, (LPARAM)&column);
            column.pszText=_T("备注");
            column.cx = 10*12;
            ::SendMessage(m_ListViewhWnd, LVM_INSERTCOLUMN, (WPARAM)COLUMN_REMARK, (LPARAM)&column);
            column.pszText=_T("内部状态");
            column.cx = 10*12;
            ::SendMessage(m_ListViewhWnd, LVM_INSERTCOLUMN, (WPARAM)COLUMN_INTERNALSTATE, (LPARAM)&column);

        }
    }


    return m_MainhWnd;
}

LRESULT CtiMonitor::OnGetDispInfo(int idCtrl, LPNMHDR pnmh,BOOL& bHandled)
{
    LV_ITEM &lvItem = reinterpret_cast<LV_DISPINFO*>(pnmh)->item;

    {
        LOCKSCOPE(m_lock);
        if(lvItem.iItem>=(int)m_channel.size()) return 0;
    }

    if (lvItem.mask & LVIF_TEXT)
    {
        LPCTSTR sztext=_T("None");

        m_lock.Lock();
        ChannelInfo* pch=m_channel.at(lvItem.iItem);
        m_lock.UnLock();

        if(pch!=NULL)
        {
            switch (lvItem.iSubItem)
            {
            case COLUMN_CHID:
                {
                    sztext=pch->GetChIDStr();
                }
                break;
            case COLUMN_TYPE:
                {
                    sztext=TypeDescription(pch->GetType());
                }
                break;

            case COLUMN_STATE:
                {
                    sztext=StateDescription(pch->GetState());
                }
                break;
            case COLUMN_WAY:
                {
                    sztext=WayDescription(pch->GetWay());
                }
                break;
            case COLUMN_CALLING:
                {
                    sztext=pch->GetCalling();
                }
                break;
            case COLUMN_CALLED:
                {
                    sztext=pch->GetCalled();
                }
                break;
            case COLUMN_ORIGCALLED:
                {
                    sztext=pch->GetOrigCalled();
                }
                break;

            case COLUMN_DTMF:
                {
                    sztext=pch->GetDtmf();
                }
                break;
            case COLUMN_REMARK:
                {
                    sztext=pch->GetRemark();
                }
                break;
            case COLUMN_INTERNALSTATE:
                {
                    sztext=pch->GetInternalstate();
                }
                break;
            default:
                sztext=_T("");
                break;

            }
        }
        lvItem.pszText=(LPTSTR)sztext;
    }

    return 0;
}




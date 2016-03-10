//opencti������һ��ͨ�õ�CTI�����⣬֧�ֶ���������ȳ��̵����������������Ȳ�Ʒ��
//��ַ��http://www.opencti.cn
//QQȺ��21596142
//��ϵ��opencti@qq.com
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
	filename: 	e:\xugood.cti\src\CtiComm\CtiMonitor.h
	file path:	e:\xugood.cti\src\CtiComm
	file base:	CtiMonitor
	file ext:	h
	author:		xugood@live.cn
	
	purpose:	
*********************************************************************/
#pragma once

#include <vector>

#include "CtiChannel.h"




enum
{
    COLUMN_CHID,
    COLUMN_TYPE,
    COLUMN_STATE,
    COLUMN_WAY,
    COLUMN_CALLING,
    COLUMN_CALLED,
    COLUMN_ORIGCALLED,
    COLUMN_DTMF,
    COLUMN_REMARK,
    COLUMN_INTERNALSTATE,
};


class CtiMonitor
{
public:
    CtiMonitor(std::vector<ChannelInfo*>& channel,COMMON::CLock& lock);
    ~CtiMonitor(void);

    HWND Create(HINSTANCE hinst,HWND hWnd);
    void Destroy();

    void SetItemCount(int items);

    void Update(int item,int column);

    int  GetSelectCh();

    HWND StartUI();




private:

    std::vector<ChannelInfo*>& m_channel;
    COMMON::CLock&             m_lock;

    HWND                       m_MainhWnd;
    HWND                       m_ListViewhWnd;

    HINSTANCE                  m_hinst;
    HWND                       m_parentwnd;


    UINT                       m_iThreadID;
    HANDLE                     m_hThreadEvent;
    HANDLE                     m_hThread;

private:
    static unsigned int WINAPI UIThread(void* param);
    static LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);


    LRESULT OnGetDispInfo(int idCtrl, LPNMHDR pnmh,BOOL& bHandled);


};



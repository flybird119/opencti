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
	created:	7:1:2009   9:13
	filename: 	e:\xugood.cti\src\CtiComm\CtiEventer.h
	file path:	e:\xugood.cti\src\CtiComm
	file base:	CtiEventer
	file ext:	h
	author:		xugood@live.cn
	
	purpose:	
*********************************************************************/
#pragma once
#include "CtiChannel.h"
#include "CtiMonitor.h"
#include "CtiEventTranslate.h"
#include "CtiChannel.h"
#include "Common/lock.h"


#include <vector>

class CtiEventTrace
{
public:
    CtiEventTrace(ChannelMgr& channelmgr);
    ~CtiEventTrace(void);
public:


    HWND ShowMonitor(HINSTANCE hinst,HWND hWnd);
    int  GetSelectCh();

    void Clear();


    void EventProc(const LPCHANNELEVENT lpchevent);



public:
    CtiMonitor                  m_monitor;
    CtiEventTranslate           m_translate;
    ChannelMgr&                 m_channelmgr;
    std::vector<ChannelInfo*>   m_channel;
    std::map<int,int>           m_index;

    COMMON::CLock               m_lock;

	UINT						m_channeltotal;
};




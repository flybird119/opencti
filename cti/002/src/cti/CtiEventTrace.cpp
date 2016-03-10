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
	created:	7:1:2009   9:14
	filename: 	e:\xugood.cti\src\CtiComm\CtiEventer.cpp
	file path:	e:\xugood.cti\src\CtiComm
	file base:	CtiEventer
	file ext:	cpp
	author:		xugood@live.cn
	
	purpose:	
*********************************************************************/
#include "StdAfx.h"
#include "CtiEventTrace.h"



#include <algorithm>

CtiEventTrace::CtiEventTrace(ChannelMgr& channelmgr)
:m_lock(COMMON::CRITICAl)
,m_monitor(m_channel,m_lock)
,m_channelmgr(channelmgr)
,m_channeltotal(0)
{

}

CtiEventTrace::~CtiEventTrace(void)
{

}




HWND CtiEventTrace::ShowMonitor(HINSTANCE hinst,HWND hWnd)
{
    return m_monitor.Create(hinst,hWnd);
}

int  CtiEventTrace::GetSelectCh()
{
    int item = m_monitor.GetSelectCh();

    ChannelInfo* pch;
    {
        LOCKSCOPE(m_lock);
        if(item<0 || item>=(int)m_channel.size())
        {
            return -1;
        }
        pch=m_channel.at(item);
    }

    return pch->GetID();
}

void CtiEventTrace::Clear()
{
    m_monitor.Destroy();
    m_channel.clear();
    m_index.clear();
}

bool UDgreater ( ChannelInfo* elem1, ChannelInfo* elem2 )
{
	return elem1->GetID() < elem2->GetID();
}



void CtiEventTrace::EventProc(const LPCHANNELEVENT lpchevent)
{
    m_translate.Translate(lpchevent);

    switch(lpchevent->eventid)
    {
    case EVENT_ADDCHANNEL:
        {
            ChannelInfo* pch=m_channelmgr.FindChannel(lpchevent->chid);
            LOCKSCOPE(m_lock);

			LPADDCHANNELPARAM lpaddchannel=(LPADDCHANNELPARAM)lpchevent->eventdata;

			if(lpaddchannel->chtotal>m_channeltotal)
			{
				m_channeltotal=lpaddchannel->chtotal;
				m_channel.reserve(m_channeltotal);

			}
            m_channel.push_back(pch);
			
            m_index[lpchevent->chid]=m_channel.size()-1;

			if(m_channel.size()>=m_channeltotal)
			{
				std::sort(m_channel.begin(),m_channel.end(),UDgreater);
				m_index.clear();
				int i=0;
				for(std::vector<ChannelInfo*>::iterator iter=m_channel.begin();iter!=m_channel.end();++iter)
				{
					m_index[(*iter)->GetID()]=i;
					++i;
				}

				m_monitor.SetItemCount(m_channeltotal);
			}


			return ;
        }
        break;
    case EVENT_DELCHANNEL:
        {
            _ASSERT(0);
        }
        break;
    }


    LOCKSCOPE(m_lock);

  

    switch(lpchevent->eventid)
    {
	case EVENT_IDLE:
	case EVENT_REMOTE_BLOCK:
	case EVENT_LOCAL_BLOCK:
    case EVENT_DIALUP:
    case EVENT_CALLIN:
        {
			m_monitor.Update(m_index[lpchevent->chid],COLUMN_WAY);
            m_monitor.Update(m_index[lpchevent->chid],COLUMN_CALLING);
            m_monitor.Update(m_index[lpchevent->chid],COLUMN_CALLED);
            m_monitor.Update(m_index[lpchevent->chid],COLUMN_ORIGCALLED);
            m_monitor.Update(m_index[lpchevent->chid],COLUMN_STATE);
            m_monitor.Update(m_index[lpchevent->chid],COLUMN_DTMF);
        }
        break;

	case EVENT_RELEASE:
    case EVENT_RINGING:
    case EVENT_RINGBACK:
    case EVENT_TALKING:
    case EVENT_HANGUP:
    case EVENT_UNUSABLE:
    case EVENT_SLEEP:
        {    
            m_monitor.Update(m_index[lpchevent->chid],COLUMN_STATE);
        }
        break;


    case EVENT_RECVDTMF:
        {
            m_monitor.Update(m_index[lpchevent->chid],COLUMN_DTMF);
        }
        break;

    case EVENT_STATE:
        {
            m_monitor.Update(m_index[lpchevent->chid],COLUMN_INTERNALSTATE);
        }
        break;

    }

}



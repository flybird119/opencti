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
	created:	2009/04/12
	created:	12:4:2009   11:27
	filename: 	e:\project\opencti.code\trunk\002\src\cti\CtiProcessor.cpp
	file path:	e:\project\opencti.code\trunk\002\src\cti
	file base:	CtiProcessor
	file ext:	cpp
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/


#include "StdAfx.h"
#include "CtiProcessor.h"
#include "CtiEventTrace.h"
#include <process.h>






CProcessor::CProcessor(UINT count,BOOL permanence,CtiEventTrace& eventtrace)
:m_total(count)
,m_usable(count)
,m_hThread(INVALID_HANDLE_VALUE)
,m_uiThreadID(0)
,m_permanence(permanence)
,m_lock(COMMON::CRITICAl)
,m_eventrace(eventtrace)
{
    _ASSERT(m_total>0 && m_total<=MAXIMUM_WAIT_OBJECTS);
    m_phEvent=new HANDLE[m_total];
    m_ppChannel=new ChannelProcessor*[m_total];
    for(UINT i=0;i<m_total;++i)
    {
        m_phEvent[i]=::CreateEvent(NULL,FALSE,FALSE,NULL);
        m_ppChannel[i]=NULL;
    }



}

CProcessor::~CProcessor(void)
{
	m_uiThreadID=0;
	::SetEvent(m_phEvent[0]);

	::WaitForSingleObject(m_hThread,INFINITE);
	::CloseHandle(m_hThread);

    for(UINT i=0;i<m_total;++i)
	{
        ::CloseHandle(m_phEvent[i]);
    }

    if(m_phEvent)
    {
        delete []m_phEvent;
    }

    if(m_ppChannel)
    {
        delete []m_ppChannel;
    }



}



HANDLE CProcessor::AddChannel(ChannelProcessor* pchannel)
{
    LOCKSCOPE(m_lock);

    _ASSERT(pchannel);
    if(m_usable==0) 
        return INVALID_HANDLE_VALUE;

    for(UINT i=0;i<m_total;++i)
    {
        if(m_ppChannel[i]==NULL)
        {
            pchannel->SetProcessor(m_phEvent[i],m_uiThreadID);

            --m_usable;
            m_ppChannel[i]=pchannel;
            return m_phEvent[i];
        }
    }

    return INVALID_HANDLE_VALUE;
}

HANDLE CProcessor::DelChannel(ChannelProcessor* pchannel)
{
    LOCKSCOPE(m_lock);

    _ASSERT(pchannel);

    if(m_usable==m_total) 
        return INVALID_HANDLE_VALUE;


    for(UINT i=0;i<m_total;++i)
    {
        if(m_ppChannel[i]==pchannel)
        {
            ++m_usable;
            m_ppChannel[i]=NULL;
            return m_phEvent[i];
        }
    }
    return INVALID_HANDLE_VALUE;
}


UINT   CProcessor::GetUsable()
{
    LOCKSCOPE(m_lock);

    return m_usable;
}


bool   CProcessor::Run()
{
    m_hThread=(HANDLE)_beginthreadex(NULL,0,&CProcessor::workthread,this,0,&m_uiThreadID);

    if((int)m_hThread==-1 || (int)m_hThread==0 )
    {
        m_hThread=INVALID_HANDLE_VALUE;
        return false;
    }

    return true;
}

unsigned int __stdcall CProcessor::workthread(void* param)
{
    CProcessor* pThis=(CProcessor*)param;

    CHANNELEVENT*   pchEvent;
    CtiEventTrace&  EventTrace=pThis->m_eventrace;


    for(;;)
    {
        DWORD dwEvent = ::WaitForMultipleObjects(pThis->m_total,pThis->m_phEvent,FALSE,INFINITE);
		if(pThis->m_uiThreadID==0)
		{
			_endthreadex(1);
			return 1;
		}

        if(dwEvent>=WAIT_OBJECT_0 + 0 && dwEvent<=WAIT_OBJECT_0 + pThis->m_total)
        {
            int index=dwEvent-WAIT_OBJECT_0;
			
			pThis->m_lock.Lock();
            ChannelProcessor* pchannel=pThis->m_ppChannel[index];
            pThis->m_lock.UnLock();

			_ASSERT(pchannel);
            if(pchannel)
            {
                int ret=pchannel->GetMessage(&pchEvent);
                if(ret==1)
                {
                    EventTrace.EventProc(pchEvent);
                    pchannel->GetCallback()(pchEvent);

                    if(pchEvent->eventdata && pchEvent->eventsize)
                    {
                        free(pchEvent->eventdata);
                    }
                    delete pchEvent;

                }
                else if(ret==-1 && !pThis->m_permanence)
                {
                    pThis->DelChannel(pchannel);
                }
            }
        }
        else
        {
            _ASSERT(0);
        }
    }

    return 0;
}

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
	created:	12:4:2009   11:26
	filename: 	e:\project\opencti.code\trunk\002\src\cti\CtiProcessor.h
	file path:	e:\project\opencti.code\trunk\002\src\cti
	file base:	CtiProcessor
	file ext:	h
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/


#pragma once

#include "CtiChannel.h"
#include "CtiEventTrace.h"

#include "common/lock.h"

#include <list>


class CProcessor
{


public:
    CProcessor(UINT count,BOOL permanence,CtiEventTrace& eventtrace);
    ~CProcessor(void);

public:

    HANDLE AddChannel(ChannelProcessor* pchannel);
    HANDLE DelChannel(ChannelProcessor* pchannel);



    UINT   GetUsable();

    bool   Run();

    
    static unsigned int __stdcall workthread(void*);

private:

    HANDLE*         m_phEvent;
    ChannelProcessor**m_ppChannel;
    UINT            m_total;
    UINT            m_usable;



    HANDLE          m_hThread;
    UINT            m_uiThreadID;

    BOOL            m_permanence;       //GetMessage 返回-1清除pChannel


    COMMON::CLock   m_lock;

	CtiEventTrace&	m_eventrace;



};


class CProcessorMgr
{

public:
    CProcessorMgr(CtiEventTrace& eventtrace)
		:m_lock(COMMON::CRITICAl)
		,m_eventrace(eventtrace)
        ,m_threadmode(0)
    {

    }

    ~CProcessorMgr()
    {

    }
public:


    CProcessor* GetMultipleProcessor()
    {
		LOCKSCOPE(m_lock);
        for(std::list<CProcessor*>::iterator processIter=m_ListMultipleProcessor.begin();processIter!=m_ListMultipleProcessor.end();++processIter)
        {
            if((*processIter)->GetUsable()>0)
            {
                return *processIter;
            }
        }

        int Thradcount=MAXIMUM_WAIT_OBJECTS;
        if(m_threadmode==1)
        {
            Thradcount=1;
        }

        CProcessor* pprocessor=new CProcessor(Thradcount,TRUE,m_eventrace);
        bool bret=pprocessor->Run();
        _ASSERT(bret);

        m_ListMultipleProcessor.push_front(pprocessor);

        return pprocessor;
    }

    CProcessor* GetSingleProcessor()
    {
        LOCKSCOPE(m_lock);
        for(std::list<CProcessor*>::iterator processIter=m_ListSingleProcessor.begin();processIter!=m_ListSingleProcessor.end();++processIter)
        {
            if((*processIter)->GetUsable()>0)
            {
                return *processIter;
            }
        }

        CProcessor* pprocessor=new CProcessor(1,FALSE,m_eventrace);
        bool bret=pprocessor->Run();
        _ASSERT(bret);

        m_ListSingleProcessor.push_front(pprocessor);

        return pprocessor;

    }


	void ClearProcessor()
	{
		LOCKSCOPE(m_lock);
		for(std::list<CProcessor*>::iterator processIter=m_ListMultipleProcessor.begin();processIter!=m_ListMultipleProcessor.end();++processIter)
		{
			delete *processIter	;
		}
		m_ListMultipleProcessor.clear();

		for(std::list<CProcessor*>::iterator processIter=m_ListSingleProcessor.begin();processIter!=m_ListSingleProcessor.end();++processIter)
		{
			delete *processIter	;
		}
		m_ListSingleProcessor.clear();
	}


    void SetThreadMode(int threadmode)
    {
        m_threadmode=threadmode;
    }
private:




private:
    std::list<CProcessor*>  m_ListMultipleProcessor;

    std::list<CProcessor*>  m_ListSingleProcessor;

	COMMON::CLock			m_lock;
	CtiEventTrace&			m_eventrace;


    int                     m_threadmode;

};





/********************************************************************
	created:	2008/12/17
	created:	17:12:2008   11:11
	filename: 	e:\xugood.comm\src\Socket\MessageQueue.cpp
	file path:	e:\xugood.comm\src\Socket
	file base:	MessageQueue
	file ext:	cpp
	author:		xugood@live.cn
	
	purpose:	
*********************************************************************/



#include "StdAfx.h"



#include "MessageQueue.h"

#include <algorithm>
#include <sys/timeb.h>


#pragma warning(disable:4996)


CMessageQueue::CMessageQueue(DWORD threadid):
m_msgThreadID(threadid),
m_msgLock(COMMON::CRITICAl)
{
	m_msgEvent[DEQUE_SEND]=::CreateEvent(NULL,TRUE,FALSE,NULL);
	m_msgEvent[DEQUE_POST]=::CreateEvent(NULL,TRUE,FALSE,NULL);
	m_msgEvent[DEQUE_DELAY]=::CreateEvent(NULL,FALSE,FALSE,NULL);


}



CMessageQueue::~CMessageQueue(void)
{
	::CloseHandle(m_msgEvent[DEQUE_SEND]);
	::CloseHandle(m_msgEvent[DEQUE_POST]);
	::CloseHandle(m_msgEvent[DEQUE_DELAY]);



	for(int i=(int)DEQUE_SEND;i<=(int)DEQUE_POST;++i)
	{
		for(std::deque<tagMessage>::iterator iter=m_msgQueue[i].begin();iter!=m_msgQueue[i].end();)
		{
			tagMessage& Msg=*iter;
			if(Msg.pdata!=NULL)
			{
				delete Msg.pdata;
			}
			iter=m_msgQueue[i].erase(iter);
		}
	}


	while (!m_msgDelayQueue.empty()) 
	{
		tagMessage& dmsg = m_msgDelayQueue.top();
		if(dmsg.pdata!=NULL)
		{
			delete dmsg.pdata;
		}
		m_msgDelayQueue.pop();
	}


}

void CMessageQueue::SetWordThread(DWORD threadid)
{
    m_msgThreadID=threadid;
}

DWORD CMessageQueue::GetWordThread()
{
    return m_msgThreadID;
}



LRESULT CMessageQueue::SendMessage(CMessageHandler* pHandler,UINT msg,MessageData* pdata)
{
	LRESULT ret=0;
	DWORD ThreadID=::GetCurrentThreadId();
	if(ThreadID==m_msgThreadID)
	{

		pHandler->OnMessage(msg,pdata,ret);

        if(pdata!=NULL)
        {
            delete pdata;
        }

	}
	else
	{

		HANDLE sendhEvent=::CreateEvent(NULL,TRUE,FALSE,NULL);
		{
			LOCKSCOPE(m_msgLock);
			tagMessage Msg(pHandler,0,msg,pdata,msgsource_send);
			Msg.tagSend.hEvent=sendhEvent;
			Msg.tagSend.lpRet=&ret;
			m_msgQueue[DEQUE_SEND].push_back(Msg);
			::SetEvent(m_msgEvent[DEQUE_SEND]);
		}
		WaitForSingleObject(sendhEvent,INFINITE);
		CloseHandle(sendhEvent);
	}

	return ret;
}

UINT CMessageQueue::PostMessage(CMessageHandler* pHandler,UINT msg,MessageData* pdata)
{
	LOCKSCOPE(m_msgLock);
	tagMessage Msg(pHandler,0,msg,pdata,msgsource_post);
    Msg.tagDelay.elapse=0;
    Msg.tagDelay.id=AllocMsgID(TRUE);

    if(pdata)
    {
        pdata->id_=Msg.tagDelay.id;
    }

	m_msgQueue[DEQUE_POST].push_back(Msg);
	::SetEvent(m_msgEvent[DEQUE_POST]);
	return Msg.tagDelay.id;
}


UINT CMessageQueue::DelayMessage(UINT cmsDelay,BOOL once,CMessageHandler* pHandler,UINT msg,MessageData* pdata)
{
    UINT msgid=AllocMsgID(once);
	PushDelay(cmsDelay,msgid,once,pHandler,msg,pdata);
	return msgid;
}







bool CMessageQueue::CancelMessage(UINT msgid)
{
    LOCKSCOPE(m_msgLock);
    std::map<UINT,bool>::iterator msgiditer = m_msgidmap.find(msgid);
    if(msgiditer!=m_msgidmap.end())
    {
        m_msgidmap.erase(msgiditer);  
        return true;
    }
	return false;
}

bool CMessageQueue::IsMessageOnce(UINT msgid)
{
	LOCKSCOPE(m_msgLock);
	std::map<UINT,bool>::iterator msgiditer = m_msgidmap.find(msgid);
	if(msgiditer!=m_msgidmap.end())
	{
		return msgiditer->second;
	}
	return true;
}



UINT CMessageQueue::Run()
{
	DETAILMESSAGE Msg;
	while(GetMessage(Msg))
	{
		DispatchMessage(Msg);
	}

	return Msg.msg;
}

bool CMessageQueue::Quit(UINT code,BOOL bsync)
{
    bool ret=false;
    if(bsync)
    {
        DWORD ThreadID=::GetCurrentThreadId();
        if(ThreadID==m_msgThreadID)
        {
            return false;
        }
        else
        {
            ret = (SendMessage(0,code,0) ==1 ) ? true : false ;

        }
    }
    else
    {
        ret = (DelayMessage(0,TRUE,NULL,code,0)!=0) ? true : false ;
    }

    return ret;
}


bool CMessageQueue::GetMessage(DETAILMESSAGE& Msg)
{
    UINT ret=0;
    do
    {
        DWORD TimeOut=INFINITE;
        ret=PeekDelayMessage(Msg,TRUE,TimeOut);
        if(ret==0)
        {
            ret=PeekMessage(Msg,TRUE,TimeOut);
        }
    }while(ret==0);

    return (ret==1) ? true : false ;
}

UINT CMessageQueue::PeekMessage(DETAILMESSAGE& Msg,BOOL RemoveMsg,DWORD TimeOut)
{
    Millisecond_ entertime=GetMillisecond();

	for(;;)
	{
		DWORD ret=::WaitForMultipleObjects(DEQUE_TOTAL,m_msgEvent,FALSE,TimeOut);
		if(ret>=WAIT_OBJECT_0 && ret<WAIT_OBJECT_0+DEQUE_TOTAL)
		{
			if(ret==DEQUE_DELAY)
			{
                DWORD delay=INFINITE;
                UINT delayret=PeekDelayMessage(Msg,RemoveMsg,delay);
                if(delayret==0)
                {
                    if(GetLeavings(TimeOut,entertime,delay,GetMillisecond()))
                    {
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    return delayret;
                }
			}
			else
			{
                LOCKSCOPE(m_msgLock);
                if(m_msgQueue[ret].empty())
                {
                    ::ResetEvent(m_msgEvent[ret]);
                   if(GetLeavings(TimeOut,entertime,INFINITE,GetMillisecond()))
                   {
                       continue;
                   }
                   else
                   {
                       break;
                   }
                        
                }
                else
                {
                    Msg=m_msgQueue[ret].front();
                    if(RemoveMsg)
                    {
                        m_msgQueue[ret].pop_front();
                    }

                    //Post消息
                    if(Msg.flag&msgsource_post)
                    {
                        std::map<UINT,bool>::iterator msgiditer= m_msgidmap.find(Msg.tagDelay.id);
                        if(msgiditer!=m_msgidmap.end())
                        {
                            //Once
                            if(msgiditer->second && RemoveMsg)
                            {
                                m_msgidmap.erase(msgiditer);		
                            }

                        }
                        else
                        {
                            //消息已取消
                            if(Msg.pdata!=NULL)
                            {
                                delete Msg.pdata;
                            }
                            

                            if(GetLeavings(TimeOut,entertime,INFINITE,GetMillisecond()))
                            {
                                continue;
                            }
                            else
                            {
                                break;
                            }

                        }
                    }


                    if(m_msgQueue[ret].empty())
                    {
                        ::ResetEvent(m_msgEvent[ret]);
                    }
                    return IsQuit(Msg) ? -1 : 1;
                }
			}

		}
        else if(ret==WAIT_TIMEOUT)
        {
            break;
        }
        else
        {
            _ASSERT(0);
        }

	}

	return 0;
}


UINT CMessageQueue::PeekDelayMessage(DETAILMESSAGE& Msg,BOOL RemoveMsg,DWORD& delay)
{
    LOCKSCOPE(m_msgLock);
    if(!m_msgDelayQueue.empty())
    {
        tagMessage&dmsg=m_msgDelayQueue.top();
        Millisecond_ currenttime=GetMillisecond();

        if(dmsg.delay<=currenttime)
        {
            _ASSERT(dmsg.tagDelay.id!=0);

            std::map<UINT,bool>::iterator msgiditer= m_msgidmap.find(dmsg.tagDelay.id);
            if(msgiditer!=m_msgidmap.end())
            {
                //Once
                if(msgiditer->second && RemoveMsg)
                {
                    m_msgidmap.erase(msgiditer);		
                }

                Msg=dmsg;
                m_msgDelayQueue.pop();
                return IsQuit(Msg) ? -1 : 1;
            }
            else
            {
                //消息已取消
                if(dmsg.pdata!=NULL)
                {
                    delete dmsg.pdata;
                }
                m_msgDelayQueue.pop();
                return PeekDelayMessage(Msg,RemoveMsg,delay);
            }
        }
        else
        {
            delay=(DWORD)(dmsg.delay-currenttime);
        }
    }
    return 0;
}




DWORD CMessageQueue::GetLeavings(DWORD& TimeOut,Millisecond_ entertime,DWORD delay,Millisecond_ currenttime)
{
    DWORD elapse=(DWORD)(currenttime-entertime);
    if(elapse>=TimeOut)
    {
        TimeOut=0;
    }
    else
    {
        DWORD leavings=TimeOut-elapse;
        TimeOut= min(delay,leavings);
    }

    return TimeOut;
}

LRESULT CMessageQueue::DispatchMessage(DETAILMESSAGE& Msg)
{
	LRESULT ret=NULL;
	BOOL clearmsgdata=TRUE; 



	Msg.pHandler->OnMessage(Msg.msg,Msg.pdata,ret);
	if(Msg.flag&msgsource_send)
	{
		*(Msg.tagSend.lpRet)=ret;
		SetEvent(Msg.tagSend.hEvent);
	}
    else
    {
        clearmsgdata=!LoopDelayMessage(Msg);
    }


    if(clearmsgdata && Msg.pdata!=NULL)
    {
        delete Msg.pdata;
    }

	return ret;
}



bool CMessageQueue::LoopDelayMessage(DETAILMESSAGE& Msg)
{
    LOCKSCOPE(m_msgLock);

    std::map<UINT,bool>::iterator msgiditer=m_msgidmap.find(Msg.tagDelay.id);
    if(msgiditer==m_msgidmap.end())
    {

        return FALSE;
    }

    PushDelay(Msg.tagDelay.elapse,Msg.tagDelay.id,msgiditer->second,Msg.pHandler,Msg.msg,Msg.pdata);

    return TRUE;
}


bool CMessageQueue::IsQuit(DETAILMESSAGE& Msg)
{
    if(Msg.pHandler==NULL)
    {
        if(Msg.flag&msgsource_send)
        {
            *(Msg.tagSend.lpRet)=1;
            SetEvent(Msg.tagSend.hEvent);
        }

        return true;
    }

    return false;
}




void CMessageQueue::ClearMessageHandle(CMessageHandler*p)
{
    LOCKSCOPE(m_msgLock);

    for(int i=(int)DEQUE_SEND;i<=(int)DEQUE_POST;++i)
    {
        for(std::deque<tagMessage>::iterator iter=m_msgQueue[i].begin();iter!=m_msgQueue[i].end();)
        {
            if(iter->pHandler==p)
            {
   
                tagMessage& Msg=*iter;
                if(Msg.flag&msgsource_send)
                {
                    *(Msg.tagSend.lpRet)=NULL;
                    SetEvent(Msg.tagSend.hEvent);
                }


                if(Msg.pdata!=NULL)
                {
                    delete Msg.pdata;
                }

                iter=m_msgQueue[i].erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }

    std::queue<tagMessage> dmsgs;
    while (!m_msgDelayQueue.empty()) 
    {
        tagMessage dmsg = m_msgDelayQueue.top();
        m_msgDelayQueue.pop();
        if (dmsg.pHandler==p) 
        {
            if(dmsg.pdata!=NULL)
                delete dmsg.pdata;
        } 
        else 
        {
            dmsgs.push(dmsg);
        }
    }
    while (!dmsgs.empty()) 
    {
        m_msgDelayQueue.push(dmsgs.front());
        dmsgs.pop();
    }
}



Millisecond_ CMessageQueue::GetMillisecond()
{
#if _WIN32_WINNT>=0x0600 || WINVER>=0x0600
	return ::GetTickCount64();
#else
#ifdef RUNLONG
/*
	DWORD dwver=GetVersion();
	WORD wver=((WORD)(BYTE)dwver)*10;
	wver+=(WORD)dwver>>8;

	//DWORD winVer=GetVersion();
	//DWORD dwMajorVersion=(DWORD)(LOBYTE(LOWORD(winVer)));
	//DWORD dwMinorVersion=(DWORD)(HIBYTE(LOWORD(winVer)));  

	if(wver>=60)
	{
		HMODULE hmodule=::LoadLibrary(_T("Kernel32.dll"));
		typedef ULONGLONG (WINAPI *tGetTickCount64)(VOID);
		tGetTickCount64 pGetTickCount64 = (tGetTickCount64)::GetProcAddress(hmodule,"GetTickCount64");
		ULONGLONG  t= pGetTickCount64();
		::FreeLibrary(hmodule);
		return t;
	}
	else
	{
		struct _timeb timebuffer;
		_ftime( &timebuffer );
		return timebuffer.time*1000+timebuffer.millitm;
	}
	*/
	struct _timeb timebuffer;
	_ftime( &timebuffer );
	return timebuffer.time*1000+timebuffer.millitm;
#else
	return ::GetTickCount();
#endif
#endif
}


void CMessageQueue::PushDelay(UINT delay,UINT id,BOOL once,CMessageHandler* pHandler,UINT msg,MessageData* pdata)
{
	LOCKSCOPE(m_msgLock);

	BOOL bSetEvent=FALSE;

	if(m_msgDelayQueue.empty())
	{
		bSetEvent=TRUE;
	}
	else
	{
		tagMessage& topMsg=m_msgDelayQueue.top();
		if(delay+GetMillisecond()<topMsg.delay)
		{
			bSetEvent=TRUE;
		}
	}

	tagMessage Msg(pHandler,delay,msg,pdata,msgsource_delay);
	Msg.tagDelay.id=id;
    Msg.tagDelay.elapse=delay;

    if(pdata)
    {
        pdata->id_=id;
    }

	m_msgDelayQueue.push(Msg);

	if(bSetEvent)
	{
		::SetEvent(m_msgEvent[DEQUE_DELAY]);
	}
}


UINT CMessageQueue::AllocMsgID(BOOL once)
{
    LOCKSCOPE(m_msgLock);
    static UINT  startmsgid =0;
    UINT currentid=0;
    for(;;)
    {
        currentid=(++startmsgid) ? startmsgid : ++startmsgid;
        if(m_msgidmap.find(currentid)==m_msgidmap.end())
        {
            m_msgidmap[currentid]=(once?true:false);
            break;
        }
    }
    return currentid;
}



#pragma warning(default:4996)
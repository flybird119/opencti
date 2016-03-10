/********************************************************************
	created:	2008/12/17
	created:	17:12:2008   11:10
	filename: 	e:\xugood.comm\src\Socket\MessageQueue.h
	file path:	e:\xugood.comm\src\Socket
	file base:	MessageQueue
	file ext:	h
	author:		xugood@live.cn
	
	purpose:	
*********************************************************************/

#pragma once

#include <Windows.h>
#include <process.h>

#include <deque>
#include <queue>
#include <map>
#include "common/Lock.h"
#include <crtdbg.h>


#define RUNLONG		//��ʱ������


#if defined(RUNLONG) || _WIN32_WINNT>=0x0600 || WINVER>=0x0600
typedef ULONGLONG	Millisecond_;
#else
typedef DWORD		Millisecond_;
#endif


/// ��Ϣ�����
class MessageData
{
public:
    MessageData(){}
    virtual ~MessageData(){}
    UINT    id_;
    UINT    size_;
    LPVOID  param_;

};


/// ��Ϣ����������
template <typename T>
class TypeMessageData :public MessageData
{
public:
    TypeMessageData(T& data,UINT size,LPVOID param)
    {
        id_=0;
        size_=size;
        param_=param;
        data_=data;
    }
    virtual ~TypeMessageData(){}

    const T& Data(){return data_;}

private:
    T       data_;

};


/// ������Ϣ������
template <typename T>
inline MessageData* WrapMessageData(T& data,UINT size=0,LPVOID param=NULL)
{
    return new TypeMessageData<T>(data,size,param);
}


/// ʹ����Ϣ������
template <typename T>
inline const T& UseMessageData(MessageData* pmessagedata)
{
    if(pmessagedata==NULL)
    {
        _ASSERT(0=="UseMessageData");

        const static T t;
        return t;
    }

    return static_cast<TypeMessageData<T>*>(pmessagedata)->Data();
}





class CMessageHandler;

/// ��Ϣ�жӵ�ʵ��
///
/// ��Ϣ���ȼ�
/// 1��Delay
/// 2��Send
/// 3��Post

const static DWORD msgsource_post	=	0x01;
const static DWORD msgsource_send	=	0x02;
const static DWORD msgsource_delay	=	0x04;
class CMessageQueue 
{
public:
	explicit CMessageQueue(DWORD threadid);
    virtual ~CMessageQueue(void);


protected:

	/// ���ù����߳�ID
    void SetWordThread(DWORD threadid);
    /// ��ȡ�����߳�ID
    DWORD GetWordThread();
	
	/// ����
	/// 
	/// ѭ��ȡ����Ϣ����ִ����Ϣ������̡�
    UINT Run();

public:
	/// �˳�
	/// 
	///ʹRun�������ء�
    bool Quit(UINT code=1,BOOL bsync=FALSE);

    friend CMessageHandler;

private:



	/// ������Ϣ
	/// 
	/// ͬ����������Ϣ������ִ����ɺ���������ŷ���
	LRESULT SendMessage(CMessageHandler* pHandler,UINT msg,MessageData* pdata);

	/// ������Ϣ
	/// 
	/// �첽����������Ϣ������Ϣ�жӲ�����Ϣ������ִ�оͷ��ء�
	UINT PostMessage(CMessageHandler* pHandler,UINT msg,MessageData* pdata);

	/// ��ʱ��Ϣ
	/// 
	/// �첽����������Ϣ������Ϣ�жӣ��ͷ��أ���ָ����ʱ�䵽���Ϣ�ж������߳���������Ϣ��������
	UINT DelayMessage(UINT cmsDelay,BOOL once,CMessageHandler* pHandler,UINT msg,MessageData* pdata);


	/// �����ָ����Ϣ������������������Ϣ�ж��е���Ϣ
	/// 
	/// ��ָ������Ϣ�������ͷţ����ñ��������������Ϣ�ж������������Ϣ��
    void ClearMessageHandle(CMessageHandler*p);


    /// ȡ���ж��е���Ϣ
    bool CancelMessage(UINT msgid);


	/// Delay��Ϣ��Once��־
	bool IsMessageOnce(UINT msgid);




private:

	const static DWORD msgsource_post	=	0x01;
	const static DWORD msgsource_send	=	0x02;
	const static DWORD msgsource_delay	=	0x04;



	typedef struct tagMessage
	{
		Millisecond_	delay;
		UINT			msg;
		MessageData*	pdata;
		CMessageHandler*pHandler;
		DWORD			flag;



		union
		{
			struct 
			{
				HANDLE		hEvent;
				LRESULT*	lpRet;
			} tagSend;

			struct 
			{
                UINT        elapse;
				UINT		id;
			} tagDelay;
		};

		tagMessage(){}


		tagMessage(CMessageHandler* pHandler_,UINT delay_,UINT msg_,MessageData* pdata_,DWORD flag_):
		pHandler(pHandler_),
		delay(GetMillisecond()+delay_),
		msg(msg_),
		pdata(pdata_),
		flag(flag_)
		{
	
		}

		bool operator< (const tagMessage& dmsg) const {
			return dmsg.delay < delay;
		}

	} DETAILMESSAGE;

	enum QUEUETYPE
	{
		DEQUE_SEND,
		DEQUE_POST,
		DEQUE_DELAY,
		DEQUE_TOTAL
	};

	std::deque<tagMessage>			m_msgQueue[DEQUE_TOTAL];
	HANDLE							m_msgEvent[DEQUE_TOTAL];
	DWORD							m_msgThreadID;
	COMMON::CLock					m_msgLock;
	std::priority_queue<tagMessage> m_msgDelayQueue;

	std::map<UINT,bool>				m_msgidmap;






    private:


        /// ����Ϣ�ж���ȡ����Ϣ
        /// 
        /// ͬ������������Ϣ�ж���û����Ϣ��һֱ�ȴ���������Ϣʱ�ŷ��ء�
        /// \retval true ����Ϣ�ж��гɹ�ȡ����һ����Ϣ
        /// \retval false ������Quit
        bool GetMessage(DETAILMESSAGE& Msg);

        /// ����Ϣ�ж���ȡ����Ϣ
        /// 
        /// �첽����������Ϣ�ж���û����Ϣ���ȴ�TimeOut�ͷ��ء�
        /// \retval 0 ��ʱ
        /// \retval 1 �õ���Ϣ
        /// \retval -1 ������Quit
        UINT PeekMessage(DETAILMESSAGE& Msg,BOOL RemoveMsg,DWORD TimeOut);


        /// ��ȡDelay��Ϣ
        /// \retval 0 û����Ϣ
        /// \retval 1 �õ���Ϣ
        /// \retval -1 ������Quit
        UINT PeekDelayMessage(DETAILMESSAGE& Msg,BOOL RemoveMsg,DWORD& delay);


        /// ����PeekMessage��TimeOutֵ
        inline DWORD GetLeavings(DWORD& TimeOut,Millisecond_ entertime,DWORD delay,Millisecond_ currenttime);


        /// ����Quit������������Ϣ
        bool IsQuit(DETAILMESSAGE& Msg);

        /// ������Ϣ
        LRESULT DispatchMessage(DETAILMESSAGE& Msg);

        /// ����ѭ����Ϣ
        bool LoopDelayMessage(DETAILMESSAGE& Msg);

        /// �õ���ǰʱ��
        static Millisecond_ GetMillisecond();

        /// ����Delay��Ϣ
        void PushDelay(UINT delay,UINT id,BOOL once,CMessageHandler* pHandler,UINT msg,MessageData* pdata);


        /// �õ���ϢID
        UINT AllocMsgID(BOOL once);
};

/// ��Ϣ�ж������߳�
class CThreadMessageQueue :public CMessageQueue
{
public:
    CThreadMessageQueue():
    CMessageQueue(0)
    {
        m_hthread=(HANDLE)_beginthreadex(0,0,MessageQueueThreadProc,this,0,&m_threadid);
        SetWordThread(m_threadid);
    }
    ~CThreadMessageQueue()
    {
        if(m_threadid)
        {
            CMessageQueue::Quit(0xFF,TRUE);
		}
		::WaitForSingleObject(m_hthread,INFINITE);
		CloseHandle((HANDLE)m_hthread);

    }
private:

    static unsigned __stdcall MessageQueueThreadProc( void * param)
    {
        CThreadMessageQueue*pMessageQueue=(CThreadMessageQueue*)param;
        UINT ret= pMessageQueue->Run();
        pMessageQueue->m_threadid=0;
        _endthreadex(ret);
        return ret;
    }

    HANDLE	   m_hthread;
    unsigned   m_threadid;


};



/// ��Ϣ֪ͨ����
class CMessageNotify
{
public:
    virtual BOOL OnMessage(UINT msg,MessageData* pdata,LRESULT& ret)=0;

};




/// ��Ϣ֪ͨ�ӿ���
class CMessageHandler : public CMessageNotify
{
public: 
    CMessageHandler(CMessageQueue* pmessagequeue):
      pMessageQueue_(pmessagequeue)
    {   
        
    }

    virtual ~CMessageHandler()
    {
       if(pMessageQueue_)
           pMessageQueue_->ClearMessageHandle(this);
    }


	LRESULT SendMessage(UINT msg,MessageData* pdata)
	{
		return pMessageQueue_->SendMessage(this,msg,pdata);
	}

    UINT PostMessage(UINT msg,MessageData* pdata)
    {
        return pMessageQueue_->PostMessage(this,msg,pdata);
    }



    UINT DelayMessage(UINT cmsDelay,BOOL once,UINT msg,MessageData* pdata)
    {
        return pMessageQueue_->DelayMessage(cmsDelay,once,this,msg,pdata);
    }

	

    bool CancelMessage(UINT msgid)
    {
        return pMessageQueue_->CancelMessage(msgid);
    }

	bool IsMessageOnce(UINT msgid)
	{
		return pMessageQueue_->IsMessageOnce(msgid);
	}


	CMessageQueue* GetMessageQueue()
	{
		return pMessageQueue_;
	}
	void SetMessageQueue(CMessageQueue*p)
	{

		if(pMessageQueue_)
		{
			pMessageQueue_->ClearMessageHandle(this);
		}
		pMessageQueue_=p;
	}


public:
    CMessageQueue*  pMessageQueue_;
};




/////////////////////////////////////////////////////////////////////////////
/// ��Ϣ֪ͨӳ���
/////////////////////////////////////////////////////////////////////////////

#define BEGIN_YSMESSAGE_MAP()		                            \
public:                                                         \
BOOL  OnMessage(UINT uMsg,MessageData* pdata,LRESULT& ret)		\
{																\
    BOOL bHandled = TRUE;									    \
    switch(1)                                                   \
	{															\
	case 1:														\
		{




#define YSMESSAGE_HANDLER(msg, func)                            \
    if(msg == uMsg)                                             \
	{                                                           \
		ret=func(pdata,bHandled);                               \
		if(bHandled)                                            \
			break;                                              \
	}




#define YSMESSAGE_RANGE_HANDLER(msgFirst, msgLast, func)        \
    if(uMsg >= msgFirst && uMsg <= msgLast)                     \
	{                                                           \
		ret=func(uMsg,pdata, bHandled);                         \
		if(bHandled)                                            \
			break ;                                             \
	}


#define CHAIN_YSMESSAGE_MAP(theChainClass)                      \
    if(theChainClass::OnMessage(uMsg, pdata,ret))               \
		break ;                                                 

#define CHAIN_MSG_YSMESSAGE_MEMBER(theChainMember)              \
    if(theChainMember.OnMessage(uMsg,pdata,ret))                \
		break ;                                                  


#define END_YSMESSAGE_MAP()                                     \
		}                                                       \
		break;                                                  \
	}															\
	return bHandled;                                            \
}


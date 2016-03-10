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


#define RUNLONG		//长时间运行


#if defined(RUNLONG) || _WIN32_WINNT>=0x0600 || WINVER>=0x0600
typedef ULONGLONG	Millisecond_;
#else
typedef DWORD		Millisecond_;
#endif


/// 消息体基类
class MessageData
{
public:
    MessageData(){}
    virtual ~MessageData(){}
    UINT    id_;
    UINT    size_;
    LPVOID  param_;

};


/// 消息体数据类型
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


/// 生成消息体数据
template <typename T>
inline MessageData* WrapMessageData(T& data,UINT size=0,LPVOID param=NULL)
{
    return new TypeMessageData<T>(data,size,param);
}


/// 使用消息体数据
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

/// 消息列队的实现
///
/// 消息优先级
/// 1、Delay
/// 2、Send
/// 3、Post

const static DWORD msgsource_post	=	0x01;
const static DWORD msgsource_send	=	0x02;
const static DWORD msgsource_delay	=	0x04;
class CMessageQueue 
{
public:
	explicit CMessageQueue(DWORD threadid);
    virtual ~CMessageQueue(void);


protected:

	/// 设置工作线程ID
    void SetWordThread(DWORD threadid);
    /// 获取工作线程ID
    DWORD GetWordThread();
	
	/// 运行
	/// 
	/// 循环取出消息，并执行消息处理过程。
    UINT Run();

public:
	/// 退出
	/// 
	///使Run函数返回。
    bool Quit(UINT code=1,BOOL bsync=FALSE);

    friend CMessageHandler;

private:



	/// 发送消息
	/// 
	/// 同步函数，消息处理函数执行完成后，这个函数才返回
	LRESULT SendMessage(CMessageHandler* pHandler,UINT msg,MessageData* pdata);

	/// 寄送消息
	/// 
	/// 异步函数，把消息放入消息列队不等消息处理函数执行就返回。
	UINT PostMessage(CMessageHandler* pHandler,UINT msg,MessageData* pdata);

	/// 定时消息
	/// 
	/// 异步函数，把消息放入消息列队，就返回，当指定的时间到达，消息列队驱动线程来调用消息处理函数。
	UINT DelayMessage(UINT cmsDelay,BOOL once,CMessageHandler* pHandler,UINT msg,MessageData* pdata);


	/// 清除和指定消息处理器关联的所有消息列队中的消息
	/// 
	/// 当指定的消息处理器释放，调用本函数来，清除消息列队中相关联的消息。
    void ClearMessageHandle(CMessageHandler*p);


    /// 取消列队中的消息
    bool CancelMessage(UINT msgid);


	/// Delay消息的Once标志
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


        /// 从消息列队中取出消息
        /// 
        /// 同步函数，当消息列队中没有消息，一直等待到有新消息时才返回。
        /// \retval true 从消息列队中成功取出了一个消息
        /// \retval false 调用了Quit
        bool GetMessage(DETAILMESSAGE& Msg);

        /// 从消息列队中取出消息
        /// 
        /// 异步函数，当消息列队中没有消息，等待TimeOut就返回。
        /// \retval 0 超时
        /// \retval 1 得到消息
        /// \retval -1 调用了Quit
        UINT PeekMessage(DETAILMESSAGE& Msg,BOOL RemoveMsg,DWORD TimeOut);


        /// 获取Delay消息
        /// \retval 0 没有消息
        /// \retval 1 得到消息
        /// \retval -1 调用了Quit
        UINT PeekDelayMessage(DETAILMESSAGE& Msg,BOOL RemoveMsg,DWORD& delay);


        /// 更新PeekMessage的TimeOut值
        inline DWORD GetLeavings(DWORD& TimeOut,Millisecond_ entertime,DWORD delay,Millisecond_ currenttime);


        /// 处理Quit函数产生的消息
        bool IsQuit(DETAILMESSAGE& Msg);

        /// 分派消息
        LRESULT DispatchMessage(DETAILMESSAGE& Msg);

        /// 继续循环消息
        bool LoopDelayMessage(DETAILMESSAGE& Msg);

        /// 得到当前时间
        static Millisecond_ GetMillisecond();

        /// 推入Delay消息
        void PushDelay(UINT delay,UINT id,BOOL once,CMessageHandler* pHandler,UINT msg,MessageData* pdata);


        /// 得到消息ID
        UINT AllocMsgID(BOOL once);
};

/// 消息列队驱动线程
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



/// 消息通知基类
class CMessageNotify
{
public:
    virtual BOOL OnMessage(UINT msg,MessageData* pdata,LRESULT& ret)=0;

};




/// 消息通知接口类
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
/// 消息通知映射表
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


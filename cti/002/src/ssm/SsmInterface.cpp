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
	created:	12:4:2009   11:28
	filename: 	e:\project\opencti.code\trunk\002\src\ssm\SsmInterface.cpp
	file path:	e:\project\opencti.code\trunk\002\src\ssm
	file base:	SsmInterface
	file ext:	cpp
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/

#include "StdAfx.h"
#include "SsmInterface.h"




#include "log/LogMessage.h"

#include "common/frequent.h"

#include <vector>
#include <string>



#pragma comment(lib,"SHP_A3.lib")
#pragma comment(lib,"shinitpci.lib")
#pragma comment(lib,"LogModule.lib")


//#define LOG_INIT(...)
//#define TraceLog(...)
//#define EventLog(...)
//#define LEVEL_SENSITIVE
#define TraceLog(LEVEL,...)    TRACE_FULL("SsmInterface",LEVEL)<<__VA_ARGS__
#define EventLog(LEVEL,...)    TRACE_FULL("SsmEventTrace",LEVEL)<<__VA_ARGS__




#pragma warning(disable:4996)




//  包装的结构
struct EventNotifytag
{
    int     nReference;
    DWORD   dwParam;
};

struct Simulatetag
{
    int              ch;
    CHANNELEVENTTYPE evt;
    DWORD            param;
};




CSsmInterface::CSsmInterface()
:CMessageHandler(new CThreadMessageQueue)
,m_totalline(0)
{

	LOG_INIT("SsmInterface",TRUE,LEVEL_SENSITIVE,100*1024*1024);
	LOG_INIT("SsmEventTrace",TRUE,LEVEL_SENSITIVE,100*1024*1024);

}

CSsmInterface::~CSsmInterface(void)
{
	CMessageQueue*pmsgqueue=this->GetMessageQueue();
	this->SetMessageQueue(NULL);
	delete pmsgqueue;
	TraceLog(LEVEL_WARNING,"CSsmInterface 释构");
}




LRESULT CSsmInterface::OnInitial(MessageData* pdata,BOOL& bHandle)
{
    for(int i=0;i<m_totalline;++i)
    {
        SsmChannelInfo*pch=new SsmChannelInfo(i);
        m_channelmgr.AddChannel(pch);
        pch->Reset();


		pch->SetType(CSsmInterface::GetChType(i));

        if(pch->IsDigitalTrunkCh())
        {
          ::SsmEnableAutoSendKB(i,FALSE);         //关闭＂自动应答来话呼叫＂
        }

        CHANNELTYPE chtype=pch->GetType();
        CHANNELSTATE chstate=STATE_INITIAL;
        CHANNELWAY chway=WAY_UNKNOWN;

       
        pch->internalstate=::SsmGetChState(i);
        
        if(pch->internalstate==S_CALL_STANDBY)
        {
            chstate=STATE_IDLE;
            chway=WAY_IDLE;
        }
 
        NotifyAddChannel(pch,chtype,chstate,chway,m_totalline);
    }

    SsmSetInterEventType(0);//表示使用MESSAGE_INFO结构
    _ASSERT(SsmGetInterEventType()==0);

    //设置驱动程序抛出事件的模式
    EVENT_SET_INFO EventMode={0};
    EventMode.dwWorkMode = EVENT_CALLBACK;                          //事件回调模式
    EventMode.lpHandlerParam = &CSsmInterface::EventCallBackProc;   //注册回调函数
    EventMode.dwUser = (DWORD)this;

    SsmSetEvent(0xffff, -1, TRUE, &EventMode);


    //禁止部分事件
    SsmSetEvent(E_SYS_BargeIn,-1,FALSE,NULL);
    SsmSetEvent(E_SYS_NoSound,-1,FALSE,NULL);
    
    SsmSetEvent(E_CHG_OvrlEnrgLevel,-1,FALSE,NULL);
//    SsmSetEvent(E_CHG_PeakFrq,-1,TRUE,NULL);
//    SsmSetEvent(E_OverallEnergy,-1,TRUE,NULL);

    SsmSetEvent(E_CHG_ToneAnalyze,-1,FALSE,NULL);
    SsmSetEvent(E_CHG_AMD,-1,FALSE,NULL);
    SsmSetEvent(E_CHG_BusyTone,-1,FALSE,NULL);

    return 0;
}



LRESULT CSsmInterface::OnSimulateTimer(MessageData* pdata,BOOL& bHandle)
{
    const Simulatetag& simulate= UseMessageData<Simulatetag>(pdata);
    const int& ch=simulate.ch;
    SsmChannelInfo* pch=GetChInfo(ch);
    const CHANNELEVENTTYPE&evt=simulate.evt;

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" "<<EventDescription(simulate.evt));


    switch(evt)
    {
  
    case EVENT_RELEASE:
        {
            _ASSERT(pch->recvcid.remoteblocktimer==pdata->id_);

            _ASSERT(pch->GetState()==STATE_SLEEP || pch->GetState()==STATE_CALLIN || pch->GetState()==STATE_RINGING);
            switch(pch->GetState())
            {
            case STATE_CALLIN:
            case STATE_RINGING:
                {
                    NotifyHangup(pch,RELEASE_NORMAL);
                }
                break;
            }
            NotifyIdle(pch);
        }
        break;

    case EVENT_SENDTONE:
        {
            _ASSERT(pch->sendtone.timer==pdata->id_);

            if(pch->sendtone.timer==pdata->id_)
                pch->sendtone.timer=0;

            if(pch->IsSendTone(TONE_NONE))
            {
                ::SsmStopSendTone(ch);
                NotifySendTone(pch,1,pch->GetSendTone());
            }
        }
        break;

    case EVENT_RECVDTMF:
        {
            _ASSERT(pch->recvdtmf.timer==pdata->id_);

            if(pch->recvdtmf.timer==pdata->id_)
                pch->recvdtmf.timer=0;

            if(pch->IsRecvDTMF())
            {
              NotifyRecvDtmf(pch,0,0);
            }
        }
        break;
/*

    case EVENT_RECVTONE:
        {
            _ASSERT( m_pline[ch].tone.recv.busy.timer==notify.timerid);

            if(m_pline[ch].tone.recv.busy.timer==notify.timerid)
                m_pline[ch].tone.recv.busy.timer=0;

            if(m_pline[ch].toneflag&TONERECV_BUSY)
            {
                m_pline[ch].toneflag&=~TONERECV_BUSY;
                SsmSetEvent(E_CHG_BusyTone,-1,FALSE,NULL);
                RECVTONEPARAM recvtoneparam;
                recvtoneparam.result=0;
                recvtoneparam.tone=TONE_BUSY;
                SendNotify(ch,EVENT_RECVTONE,&recvtoneparam,sizeof(recvtoneparam));
            }
        }
        break;
        */
    }

    return 0;
}


LRESULT CSsmInterface::OnUpdatePlay(MessageData* pdata,BOOL& bHandle)
{
    const EventNotifytag &peventnotify=UseMessageData<EventNotifytag>(pdata);
    int ch=peventnotify.nReference;


    LPCTSTR lpdescription=NULL;
    switch(peventnotify.dwParam)
    {
    case 1: 
        {
            lpdescription=_T("缓冲区播放完毕");
        }
        break;
    case 2:
        {
            lpdescription=_T("放音任务因检测到DTMF 按键字符而终止");
        }
        break;
    case 3:
        {
            lpdescription=_T("放音任务因检测到bargein而终止");
        }
        break;
    case 4:
        {
            lpdescription=_T("放音任务被应用程序终止");
        }
        break;
    case 5:
        {
            lpdescription=_T("放音任务因检测到对端挂机而终止");
        }
        break;
    default:
        {
            lpdescription=_T("未知");
        }

    }

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" "<<lpdescription);

    SsmChannelInfo* pch=GetChInfo(ch);

    if(peventnotify.dwParam==1)
    {

        BYTE* buf=NULL;
        int   size=0;
        if(pch->playmemory.index%2)
        {
            buf=pch->playmemory.block2;
            size=pch->playmemory.size2;
        }
        else 
        {
            buf=pch->playmemory.block1;
            size=pch->playmemory.size1;
        }

        if(buf!=NULL && size>0)
        {
            NotifyUpdatePlayMemory(pch,pch->playmemory.index,pch->playmemory.block1,pch->playmemory.size1,pch->playmemory.block2,pch->playmemory.size2);
            if(pch->playmemory.index%2)
            {
                pch->playmemory.block1=0;
                pch->playmemory.size1=0;
            }
            else 
            {
                pch->playmemory.block2=0;
                pch->playmemory.size2=0;
            }
        }
        ++pch->playmemory.index;

        

    }

    return 0;
}

LRESULT CSsmInterface::OnUpdateRecord(MessageData* pdata,BOOL& bHandle)
{
    const EventNotifytag &peventnotify=UseMessageData<EventNotifytag>(pdata);
    int ch=peventnotify.nReference;



    LPCTSTR lpdescription=NULL;
    switch(peventnotify.dwParam)
    {
    case 1: 
        {
            lpdescription=_T("录音任务被应用程序终止");
        }
        break;
    case 2:
        {
            lpdescription=_T("录音任务因检测到DTMF 按键字符而结束");
        }
        break;
    case 3:
        {
            lpdescription=_T("录音任务因检测到对端挂机而结束");
        }
        break;
    case 4:
        {
            lpdescription=_T("缓冲区录音完毕");
        }
        break;
    default:
        {
            lpdescription=_T("未知");
        }

    }

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" "<<lpdescription);

    SsmChannelInfo* pch=GetChInfo(ch);

    if(peventnotify.dwParam==4)
    {

        BYTE* buf=NULL;
        int   size=0;
        if(pch->recordmemory.index%2)
        {
            buf=pch->recordmemory.block2;
            size=pch->recordmemory.size2;
        }
        else 
        {
            buf=pch->recordmemory.block1;
            size=pch->recordmemory.size1;
        }

        if(buf!=NULL && size>0)
        {
            NotifyUpdateRecordMemory(pch,pch->recordmemory.index,pch->recordmemory.block1,pch->recordmemory.size1,pch->recordmemory.block2,pch->recordmemory.size2);
            if(pch->recordmemory.index%2)
            {
                pch->recordmemory.block1=0;
                pch->recordmemory.size1=0;
            }
            else 
            {
                pch->recordmemory.block2=0;
                pch->recordmemory.size2=0;
            }
        }
        ++pch->recordmemory.index;
    }

    return 0;
}


LRESULT CSsmInterface::OnChState(MessageData* pdata,BOOL& bHandle)
{
    const EventNotifytag &peventnotify=UseMessageData<EventNotifytag>(pdata);

    int ch=peventnotify.nReference;

    int oldstatue=HIWORD(peventnotify.dwParam);
    int newstate=LOWORD(peventnotify.dwParam);

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" "<<ChStateToDescription(ch,peventnotify.dwParam));


    SsmChannelInfo* pch=GetChInfo(ch);
    

    _ASSERT(pch->internalstate==oldstatue);
    _ASSERT(oldstatue!=newstate);

    if(pch->internalstate==newstate) return 0;
    pch->internalstate=newstate;
	NotifyState(pch,StateToText(newstate));


    switch(newstate)
    {

        //TUP[Standby]
		//ISUP[Standby]
        //模拟中继[Standby]
    case S_CALL_STANDBY:                        //空闲
            NotifyIdle(pch);;
        break;
        //TUP[Pending]
		//ISUP[Pending]
        //模拟中继[Standby]
	case S_CALL_PENDING:                        //挂起
		    OnStatePending(pch);    
		break;

        //TUP[OffHook]
		//ISUP[OffHook] “摘机”状态。应用程序可以在本状态调用SsmAutoDial函数发起去话呼叫业务。通道进入OffHook状态时，驱动程序会启动定时器T1。如果应用程序在T1溢出之前没有调用SsmAutoDial函数，驱动程序就会将此通道进行回收
        //模拟中继[OffHook] 
	case S_CALL_PICKUPED:						//摘机
		//ISUP 呼出时不调用SsmPckup
        //模拟中继 不处理
		break;


        //TUP[Ringing]
		//ISUP[Ringing]
        //模拟中继[Ringing]
	case S_CALL_RINGING:                        //振铃
		{
			
			if(pch->GetType()==TYPE_ANALOG_TRUNK)
			{

				if(pch->recvcid.remoteblocktimer!=0)
				{
					CMessageHandler::CancelMessage(pch->recvcid.remoteblocktimer);
					pch->recvcid.remoteblocktimer=0;
				}

				if(pch->GetState()==STATE_SLEEP)
				{
					pch->recvcid.cidsetup=RECVCID_WAIT;
					TCHAR called[128]=_T("\0");
					GetInternalCID(ch,called);
					NotifyCallin(pch,_T("P"),called,_T("P"));
				}

				if(pch->GetState()==STATE_CALLIN && pch->recvcid.cidsetup==RECVCID_RING)
				{
					pch->recvcid.cidsetup=RECVCID_FINISH;
					NotifyRinging(pch);
				}
			}

			if(pch->IsDigitalTrunkCh())
			{
				NotifyRinging(pch);
			}
			
		}
		break;

        //TUP[Connected]
		//ISUP[Connected]
        //模拟中继[Connected]
	case S_CALL_TALKING:                        //通话
		{
			if(pch->GetWay()==WAY_CALLIN)
            {
                _ASSERT(pch->GetState()==STATE_RINGING || pch->GetState()==STATE_RINGBACK);
                NotifyTalking(pch);
            }
		}
		break;




	case S_CALL_OFFLINE:                        //“线路断开”状态
		break;


        //TUP[WaitAnser]
		//ISUP[WaitAnswer]“等待被叫用户摘机”状态，只适用于去话呼叫。此时，被叫方应该听到振铃音。如果本端是坐席通道通过数字中继通道进行电话呼出，则应向坐席通道发送回铃音信号
        //模拟中继[WaitAnswer]
	case S_CALL_WAIT_REMOTE_PICKUP:				//去话呼叫，“等候被叫用户摘机”
			//不处理
		break;


        //TUP[Unusable]
		//ISUP[Unusable]“不可用”状态
	case S_CALL_UNAVAILABLE:					//通道不可用
			NotifyUnusable(pch);
		break;

        //TUP[Locked]
		//ISUP[Locked] “呼出锁定”状态。应用程序可以在本状态直接调用SsmAutoDial函数发起去话呼叫业务。通道迁移到本状态时，驱动程序会启动定时器T1。如果应用程序在T1溢出之前没有调用SsmPickup或SsmAutoDial函数，驱动程序就会将此通道进行回收。T1定时器的设定值为60秒
	case S_CALL_LOCKED:							//呼出锁定
			//ISUP 呼出时调用SsmSearchIdleCallOutCh
            //TUP 呼出时调用SsmSearchIdleCallOutCh
		break;





        //TUP[BlockedByPBX]  对端闭塞状态。对端交换机进行维护时，会向本端发送闭塞消息，以阻止本端对它发起电话呼叫。当通道处于对端闭塞状态时，驱动程序不支持应用程序发出的去话呼叫指令，但仍然可以接受和处理来话呼叫
	case S_CALL_RemoteBlock:                    //“对端闭塞”
			NotifyRemoteBlock(pch);
		break;

        //TUP[BlockedByApp] 本端闭塞状态。此时，应用程序不能进行电话呼出操作。本端闭塞通常用于系统维护功能。应用程序可以通过调用函数SsmBlockLocalCh或SsmBlockLocalPCM将通道闭塞，以通知驱动程序阻止电话呼出。注意：在本端闭塞状态下，通道仍然可以接受和处理来话呼叫
	case S_CALL_LocalBlock:                     //“本地闭塞”
			 NotifyLocalBlock(pch);
		break;



        //TUP[Reseting]
    case S_TUP_WaitPcmReset:                    //TUP 通道：“电路复原”
           NotifyUnusable(pch);
        break;


        //TUP[RecvPhoNum]
    case S_TUP_WaitSAM:                         //TUP 通道：“等待对端交换机的后续地址消息”
    case S_TUP_WaitGSM:                         //TUP 通道：“等待对端交换机的GSM消息”
    case S_TUP_WaitPrefix:                      //TUP 通道：“接收入局字冠”
            //不处理
        break;

        //TUP[Dialing]
    case S_TUP_WaitDialAnswer:                  //TUP 通道：“等待对端交换机的应答消息”
    case S_TUP_WaitSetCallerID:                 //TUP 通道："等待应用程序设置主叫号码"
            //不处理
        break;

        //TUP[Release]
    case S_TUP_WaitCLF:                         //TUP 通道：“等待对端交换机的拆线消息”
    case S_TUP_WaitRLG:                         //TUP 通道：“等待对端交换机的RLG消息”
            //不处理
        break;
  



		//ISUP[RecvPhoNum]
	case S_ISUP_WaitSAM:						//ISUP通道：“等待对端交换机的SAM”
			//不处理
		break;

		//ISUP[Release]“拆线”状态。在Release状态，驱动程序会等待对方发送RLC消息（释放完成消息）
	case S_ISUP_WaitRLC:                        //ISUP通道：等待对端交换机的释放监护信号RLC
			//不处理
		break;

		//ISUP[Reseting] “电路复位”状态
    case S_ISUP_WaitReset:                      //ISUP：电路复原
			NotifyUnusable(pch);
		break;

		//ISUP[BlockingRemote] “闭塞对端交换机”状态。在BlockingRemote状态，对端交换机不能发起去话呼叫，但本端可以发起去话呼叫
    case S_ISUP_LocallyBlocked:                 //ISUP通道：本地闭塞 
			NotifyLocalBlock(pch);
        break;
		//ISUP[BlockedByPBX] “被对端交换机闭塞”状态。对端交换机进行维护时，会向本端发送闭塞消息，以阻止本端对它发起电话呼叫。当通道处于对端闭塞状态时，驱动程序不支持应用程序发出的去话呼叫指令，但仍然可以接受和处理来话呼叫
    case S_ISUP_RemotelyBlocked:                //ISUP通道：远端闭塞
			NotifyRemoteBlock(pch);
        break;


		//ISUP[AutoDial]
	case S_ISUP_WaitDialAnswer:					//ISUP通道：“等待对端交换机的应答消息”
			//不处理
		break;
		//ISUP[RecvPhoNum]
	case S_ISUP_WaitINF:						//ISUP通道：“等待对端交换机的INF消息”
			//不处理
		break;
		//ISUP[AutoDial] 
	case S_ISUP_WaitSetCallerID:				//ISUP通道：“等待应用程序设置主叫号码”
			//ISUP 呼出时 已调用SsmSetTxCallerId函数
		break;
		//ISUP[Paused]
	case S_ISUP_Suspend :						//ISUP通道：“暂停“
			//不处理
		break;


        //模拟中继[WaitDialtone] 
    case S_CALL_ANALOG_WAITDIALTONE:            //模拟中继线通道：去话呼叫，“等待拨号音”
            //不处理
        break;

        //模拟中继[Dialing]
    case S_CALL_ANALOG_TXPHONUM:                //模拟中继线通道：去话呼叫，“拨号”
        {
            NotifyRingback(pch);
        }
        break;

        //模拟中继[WaitRingBackTone]            
    case S_CALL_ANALOG_WAITDIALRESULT:          //模拟中继线通道：去话呼叫，“等待拨号结果”
            //不处理
        break;





	default:
		_ASSERT(0);
		
    }
    
    return 0;
}


LRESULT CSsmInterface::OnHookState(MessageData* pdata,BOOL& bHandle)
{
    const EventNotifytag &peventnotify=UseMessageData<EventNotifytag>(pdata);
    int ch=peventnotify.nReference;
    SsmChannelInfo* pch=GetChInfo(ch);

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" "<<HookStateToDescription(ch,peventnotify.dwParam));

    if(peventnotify.dwParam==0)//挂机
    {
        //防止启动前已搞机
        if(pch->GetState()==STATE_INITIAL || pch->internalstate==S_CALL_STANDBY)
        {

        }
        else
        {
            NotifyHangup(pch,RELEASE_NORMAL);
        }
        
    }
    else if(peventnotify.dwParam==1)//摘机
    {   

        switch(pch->GetState())
        {
        case STATE_IDLE:
            {
                TCHAR called[128]=_T("\0");
                GetInternalCID(ch,called);
				NotifyCallin(pch,_T("P"),called,_T("P"));

            }
            break;
        case STATE_DIALUP:
            {
               pch->sendcid.bpick=true;

               if(::SsmCheckSendFsk(ch)==1)
               {
                   ::SsmStopSendFsk(ch);
               }

               ::SsmStopRing(ch);
               NotifyRingback(pch);
               NotifyTalking(pch);
            }
            break;

        case STATE_RINGBACK:
            {
                ::SsmStopRing(ch);
				NotifyTalking(pch);
            }
            break;
        }

    }

    return 0;
}





LRESULT CSsmInterface::OnCIDExBuf(MessageData* pdata,BOOL& bHandle)
{   

    const EventNotifytag &peventnotify=UseMessageData<EventNotifytag>(pdata);
    int ch=peventnotify.nReference;
    SsmChannelInfo* pch=GetChInfo(ch);

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" "<<CIDExBufToDescription(ch,peventnotify.dwParam));

    if(pch->recvcid.cidsetup!=RECVCID_RECV)
    {
        return 0;
    }
    else
    {
        pch->recvcid.cidsetup=RECVCID_WAIT;
    }

    char* pcalling=::SsmGetCallerIdA(ch);
    if(pcalling==NULL)
    {
        pcalling="P";
    }
  
    TCHAR called[128]=_T("\0");
    GetInternalCID(ch,called);

    TCHAR calling[128]=_T("\0");

#if defined(_UNICODE) || defined(UNICODE)
    COMMON::ASCIIChar achar(pcalling);
    _tcsncpy(calling,achar,128-1);
#else
    _tcsncpy(calling,pcalling,128-1);
#endif

    NotifyCallin(pch,calling,called,_T("P"));

    return 0;

}

LRESULT CSsmInterface::OnRingCount(MessageData* pdata,BOOL& bHandle)
{
    const EventNotifytag &peventnotify=UseMessageData<EventNotifytag>(pdata);
    int ch=peventnotify.nReference;

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" "<<RingCountToDescription(ch,peventnotify.dwParam));

    SsmChannelInfo* pch=GetChInfo(ch);

    pch->recvcid.ringcount=peventnotify.dwParam;

    if(pch->recvcid.cidsetup==RECVCID_IDLE)
    {
        NotifySleep(pch);

        DetectRingdown(ch);
        pch->recvcid.cidsetup=RECVCID_RECV;     
    }
    else if(pch->recvcid.cidsetup==RECVCID_RING)
    { 
        if(pch->GetState()==STATE_CALLIN)
        {
            NotifyRinging(pch);
            pch->recvcid.cidsetup=RECVCID_FINISH;
        }
    }

    return 0;
}


LRESULT CSsmInterface::OnSendFsk(MessageData* pdata,BOOL& bHandle)
{
    const EventNotifytag &peventnotify=UseMessageData<EventNotifytag>(pdata);
    int ch=peventnotify.nReference;

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" "<<SendFSKToDescription(ch,peventnotify.dwParam));

    SsmChannelInfo* pch=GetChInfo(ch);

    if(pch->GetState()==STATE_DIALUP && pch->GetType()==TYPE_ANALOG_USER && !pch->sendcid.bpick)
    {
        NotifyRingback(pch);
    }
    else if(pch->IsSendFSK())
    {
        NotifySendFsk(pch,1);
    }

    return 0;
}


LRESULT CSsmInterface::OnAutoDial(MessageData* pdata,BOOL& bHandle)
{
    const EventNotifytag &peventnotify=UseMessageData<EventNotifytag>(pdata);
    int ch=peventnotify.nReference;
	SsmChannelInfo* pch=GetChInfo(ch);

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" "<<AutoDialToDescription(ch,peventnotify.dwParam));

	
	if(!pch->IsCalloutStates())
	{
		return 0;
	}

	switch(peventnotify.dwParam)
	{
		//通道空闲，没有执行AutoDial任务
	case DIAL_STANDBY:
		break;

		//正在发送被叫号码
	case DIAL_DIALING:
		break;

		//回铃
		//SS1通道：表明去话呼叫时，驱动程序收到对端交换机的后向KB=1或KB=6信号，表明被叫空闲
		//TUP/ISUP通道：表明驱动程序收到对端交换机的地址齐消息(ACM)
	case DIAL_ECHOTONE:
            if(pch->GetType()!=TYPE_ANALOG_TRUNK)
            {
			    NotifyRingback(pch);
            }
		break;

		//没有在线路上检测到拨号音，AutoDial任务失败。只适用于模拟中继线通道
	case DIAL_NO_DIALTONE:
        {
               NotifyHangup(pch,RELEASE_LINEFAULT);
        }
		break;

		//被叫用户忙，AutoDial任务失败。
		//对于模拟中继线通道，表明线路上检测到了忙音信号
	case DIAL_BUSYTONE:
        if(pch->GetType()==TYPE_ANALOG_TRUNK)
        {
             if(pch->dialup.flags&CHECKFLAG_CHECKHANG)
             {
                NotifyHangup(pch,RELEASE_UNUSABLE);
             }
        }
        else
        {
			NotifyHangup(pch,RELEASE_USERBUSY);
        }
		break;

		//拨号完成后，线路上先是出现了回铃音，然后保持静默。AutoDial任务终止。只适用于模拟中继线通道
	case DIAL_ECHO_NOVOICE:
        {
            if(pch->dialup.flags&CHECKFLAG_CHECKTALK && pch->GetState()==STATE_RINGBACK)
            {
                NotifyTalking(pch);
            }
            
        }
        break;
		//拨号完成后，线路上没有检测到回铃音，一直保持静默。AutoDial任务终止。只适用于模拟中继线通道
	case DIAL_NOVOICE:
        {
            if(pch->dialup.flags&CHECKFLAG_CHECKHANG)
            {
                NotifyHangup(pch,RELEASE_LINEFAULT);
            }
        }
		break;


		//被叫用户摘机，AutoDial任务完成
	case DIAL_VOICE:
        {
            _ASSERT(pch->GetState()==STATE_RINGBACK);

            BOOL Talk=FALSE;

            if(pch->GetType()==TYPE_ANALOG_TRUNK)
            {
                if(pch->dialup.flags&CHECKFLAG_CHECKTALK)
                {
                    Talk=TRUE;
                }

            }
            else
            {
                Talk=TRUE;
            }
          
            if(Talk)
            {
                NotifyTalking(pch);
            }
        }
		break;

		//被叫用户摘机（检测到F1频率的应答信号），AutoDial任务完成。只适用于模拟中继线通道
	case DIAL_VOICEF1:
		break;

		//被叫用户摘机（检测到F2频率的应答信号），AutoDial任务完成。只适用于模拟中继线通道
	case DIAL_VOICEF2:
		break;

		//被叫用户在指定时间内没有摘机，AutoDial失败
	case DIAL_NOANSWER:
        if(pch->GetType()==TYPE_ANALOG_TRUNK)
        {
            if(pch->dialup.flags&CHECKFLAG_CHECKHANG)
            {
                NotifyHangup(pch,RELEASE_NOANSWER);
            }
        }
        else
        {
			NotifyHangup(pch,RELEASE_NOANSWER);
        }
		break;

		//AutoDial任务失败。失败原因可以通过函数SsmGetAutoDialFailureReason获得
	case DIAL_FAILURE:
		{
			int Failedreason=::SsmGetAutoDialFailureReason(ch);
			TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" SsmGetAutoDialFailureReason:"<<AutoDialFailedToDescription(ch,Failedreason));

			RELEASEATTRIBUTE releaseattribute=DialFailedToReleaseAttribute(Failedreason);
			if(releaseattribute!=RELEASE_NONE)
            {
                if(pch->GetType()==TYPE_ANALOG_TRUNK)
                {
                    if(pch->dialup.flags&CHECKFLAG_CHECKHANG)
                    {
                        NotifyHangup(pch,releaseattribute);
                    }
                }
                else
                {
				    NotifyHangup(pch,releaseattribute);
                }
            }
		}
		break;

		//被叫用户号码为空号，AutoDial任务失败
	case DIAL_INVALID_PHONUM:
        if(pch->GetType()==TYPE_ANALOG_TRUNK)
        {
            if(pch->dialup.flags&CHECKFLAG_CHECKHANG)
            {
                NotifyHangup(pch,RELEASE_VACANT);
            }
        }
        else
        {
			NotifyHangup(pch,RELEASE_VACANT);
        }
		break;

	default:
		_ASSERT(0);
	}


    return 0;
}


LRESULT CSsmInterface::OnToneAnalyze(MessageData* pdata,BOOL& bHandle)
{
    const EventNotifytag &peventnotify=UseMessageData<EventNotifytag>(pdata);
    int ch=peventnotify.nReference;

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" "<<ToneAnalyzeToDescription(ch,peventnotify.dwParam));

    return 0;
}

LRESULT CSsmInterface::OnRecvDTMF(MessageData* pdata,BOOL& bHandle)
{
    const EventNotifytag &peventnotify=UseMessageData<EventNotifytag>(pdata);
    int ch=peventnotify.nReference;

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" "<<RecvDTMFToDescription(ch,peventnotify.dwParam));

    SsmChannelInfo* pch=GetChInfo(ch);

    if(pch->IsRecvDTMF())
    {
        WORD high = HIWORD(peventnotify.dwParam);
        TCHAR low  = LOWORD(peventnotify.dwParam);
        
 
        TCHAR dtmf=toupper(low);

        if(pch->recvdtmf.timer)
        {
            CMessageHandler::CancelMessage(pch->recvdtmf.timer);
            Simulatetag simulate;
            simulate.ch=ch;
            simulate.evt=EVENT_RECVDTMF;
            simulate.param=0;
            pch->recvdtmf.timer=CMessageHandler::DelayMessage(pch->recvdtmf.elapse,TRUE,EVT_SIMULATETIMER,WrapMessageData<Simulatetag>(simulate));




        }

        NotifyRecvDtmf(pch,1,dtmf);
    }

    return 0;
}


LRESULT CSsmInterface::OnSendDTMF(MessageData* pdata,BOOL& bHandle)
{
    //=0：完成缓冲区全部DTMF字符的发送
    //=1：发送任务被应用程序终止

    const EventNotifytag &peventnotify=UseMessageData<EventNotifytag>(pdata);
    int ch=peventnotify.nReference;

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" "<<SendDTMFToDescription(ch,peventnotify.dwParam));

    SsmChannelInfo* pch=GetChInfo(ch);

     if(pch->IsSendDTMF())
     {
        int result=1;

        //ssm卡，有个bug, 暂时用这个方法
        if(pch->stopdtmf.bstop)
        {
            result=2;
        }

        NotifySendDtmf(pch,result);
     }
     return 0;
}

LRESULT CSsmInterface::OnRecvFSK(MessageData* pdata,BOOL& bHandle)
{
    const EventNotifytag &peventnotify=UseMessageData<EventNotifytag>(pdata);
    int ch=peventnotify.nReference;

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" "<<RecvFSKToDescription(ch,peventnotify.dwParam));

    SsmChannelInfo* pch=GetChInfo(ch);

    if(pch->IsRecvFSK())
    {

        //1：超时，接收失败
        //2：收到指定的结束特征字节而结束
        //3：收到指定长度FSK数据而结束
        //4：收到指定格式的数据而结束
        //5：应用程序调用函数SsmStopRcvFSK中断

        int result=0;
        static UCHAR recvfskbuf[1024];
        int len=0;
        switch(peventnotify.dwParam)
        {
        case 5:
            result=2;
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            {

                len=::SsmGetRcvFSK(ch,recvfskbuf);
                if(len>0)
                {
                    result=3;
                }
                else
                {
                    result=0;
                }
     
                
            }
            break;
        }
       
        NotifyRecvFsk(pch,result,recvfskbuf,len);
    }

    return 0;
}


LRESULT CSsmInterface::OnPlayEnd(MessageData* pdata,BOOL& bHandle)
{
    const EventNotifytag &peventnotify=UseMessageData<EventNotifytag>(pdata);
    int ch=peventnotify.nReference;

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" "<<PlayEndToDescription(ch,peventnotify.dwParam));

    SsmChannelInfo* pch=GetChInfo(ch);

    if(pch->IsPlayVoice(PLAY_NONE))
    {
        int result=0;
        TCHAR dtmf=0;;
      
        if(peventnotify.dwParam==1)         //全部语音数据播放完毕
        {
            result=1;
            
        }
        else if(peventnotify.dwParam==2)   //因收到DTMF字符而终止
        {
           
            result=3;
            char lastdtmf;
            if(::SsmGetLastDtmf(ch,&lastdtmf)==1)
            {
                dtmf=toupper(lastdtmf);
            }
        }
        else if(peventnotify.dwParam==5)
        {
            result=2;
        }
 
        NotifyPlay(pch,result,pch->GetPlayVoice(),dtmf);
        
    }

    return 0;
}



LRESULT CSsmInterface::OnRecordEnd(MessageData* pdata,BOOL& bHandle)
{
    const EventNotifytag &peventnotify=UseMessageData<EventNotifytag>(pdata);
    int ch=peventnotify.nReference;

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" "<<RecordEndToDescription(ch,peventnotify.dwParam));

    SsmChannelInfo* pch=GetChInfo(ch);

    if(pch->IsRecordVoice(RECORD_NONE))
    {
        //1：被应用程序终止
        //2：因检测到DTMF字符而终止
        //3：因检测到对端用户的挂机动作而终止
        //4：因录制的数据到达指定长度或时间而终止
        //5：文件录音被暂停
        //6：将录音数据写入到文件失败

       
        int result=0;
        TCHAR dtmf=0;;

        switch(peventnotify.dwParam)
        {
        case 1:
            result=2;
            break;
        case 2:
            {
                result=3;
                char lastdtmf;
                if(::SsmGetLastDtmf(ch,&lastdtmf)==1)
                {
                    dtmf=toupper(lastdtmf);
                }
            }
            break;
        case 3:
        case 4:
            result=1;
            break;

        }
        
        NotifyRecord(pch,result,pch->GetRecordVoice(),dtmf);

       }

    return 0;
}
/*

LRESULT CSsmInterface::OnBusyTone(MessageData* pdata,BOOL& bHandle)
{
    const EventNotifytag &peventnotify=UseMessageData<EventNotifytag>(pdata);
    int ch=peventnotify.nReference;

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" "<<BusyToneToDescription(ch,peventnotify.dwParam));

    if(m_pline[ch].toneflag&TONERECV_BUSY)
    {


        WORD high = HIWORD(peventnotify.dwParam);
        TCHAR low  = LOWORD(peventnotify.dwParam);
        RECVTONEPARAM recvtoneparam;
        recvtoneparam.result=1;
        recvtoneparam.tone=TONE_BUSY;


    }
    return 0;
}
*/





LRESULT CSsmInterface::OnRxPhoNumBuf(MessageData* pdata,BOOL& bHandle)
{
	const EventNotifytag &peventnotify=UseMessageData<EventNotifytag>(pdata);
	int ch=peventnotify.nReference;
    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" "<<RxPhoNumBufToDescription(ch,peventnotify.dwParam));
    SsmChannelInfo* pch=GetChInfo(ch);
    CHANNELSTATE chstate=pch->GetState();

	//注意这里 
	//_ASSERT(0);
    NotifySleep(pch);  
	return 0;
}


//系统服务函数
int CSsmInterface::Init(DeviceDescriptor* pctidevicedescriptor)
{
    int iret=0;
    iret=::SsmStartCti("ShConfig.ini","ShIndex.ini");
    if(iret!=0)
    {
        if(iret==-1)
        {
            TraceLog(LEVEL_WARNING,"Init Fail SsmStartCti Ret:"<<iret<<" Reasion:"<<::SsmGetLastErrMsgA());
        }
        else if(iret==-2)
        {
            TraceLog(LEVEL_WARNING,"Init Fail SsmStartCti Ret:"<<iret<<" Reasion:"<<"驱动程序已经装载");
        }
        else
        {
            TraceLog(LEVEL_WARNING,"Init Fail SsmStartCti Ret:"<<iret<<" Reasion:"<<"未知原因");
        }
        iret=0;
        ShowWarn(_T("SsmStartCti失败！"));
    }
    else
    {
        //int CfgBoard=SsmGetMaxCfgBoard();
        //int UsableBoard=SsmGetMaxUsableBoard();
        //int PciBoard=0;
        //GetTotalPciBoard(&PciBoard);
        //TraceLog(LEVEL_SENSITIVE,"PciBoard:"<<PciBoard<<" SsmGetMaxCfgBoard:"<<CfgBoard<<" SsmGetMaxUsableBoard:"<<UsableBoard);
        m_totalline=::SsmGetMaxCh();
        if(m_totalline>0)
        {
            iret=1;
            m_eventfun=pctidevicedescriptor->notifyfun;
            m_eventparm=pctidevicedescriptor->notifyparam;
            CMessageHandler::DelayMessage(0,TRUE,EVT_INITIAL,0);
        }
        else
        {
            ::SsmCloseCti();
        }
    }
    return iret;
}

void CSsmInterface::Term()
{
    if(m_totalline>0)
    {
        int ret=::SsmCloseCti();
        if(ret!=1)
        {
            TraceLog(LEVEL_WARNING,"Term Fail SsmCloseCti Ret:"<<ret);
        }
        m_totalline=0;
    }
}

/*

BOOL CSsmInterface::Features(FUNCTIONADDRINDEX descriptor)
{
    return FALSE;
}

void* CSsmInterface::GetNativeAPI(LPCTSTR funname)
{
    return NULL;
}





*/

int CSsmInterface::GetLastErrorCode()
{
    return ::SsmGetLastErrCode();
}

LPCTSTR CSsmInterface::FormatErrorCode(int code)
{
    char buf[300];
    ::SsmGetLastErrMsg(buf);
    LPCTSTR ptbuf=NULL;


#if defined(_UNICODE) || defined(UNICODE)
    COMMON::ASCIIChar achar(buf);
    ptbuf=achar;
#else
    ptbuf=buf;
#endif
    static TCHAR ErrorMsg[300];
    _tcsncpy(ErrorMsg,ptbuf,300);
    return ErrorMsg;

}


CHANNELTYPE CSsmInterface::GetChType(int ch)
{
    CHANNELTYPE channeltype=TYPE_UNKNOWN;
    switch(::SsmGetChType(ch))
    {
    case 12:        //数字电话线录音通道
    case 10:        // 磁石通道
    case 15:        // H.323通道
    case 16:        //SIP通道
        channeltype=TYPE_UNKNOWN;
        break;
    case 4:         // SS1通道
        channeltype=TYPE_SS1;
        break;
    case 7:         // ISDN通道（用户侧）
        channeltype=TYPE_DSS1_USER;
        break;
    case 8:         // ISDN通道（网络侧）
        channeltype=TYPE_DSS1_NET;
        break;
    case 20:        //SHT系列板卡未安装业务模块的通道
        channeltype=TYPE_ANALOG_EMPTY;
        break;
    case 0:         // 模拟中继线通道
        channeltype=TYPE_ANALOG_TRUNK;
        break;
    case 2:         // 坐席通道
        channeltype=TYPE_ANALOG_USER;
        break;
    case 3:         // 模拟中继线录音通道
        channeltype=TYPE_ANALOG_RECORD;
        break;
    case 6:         // TUP通道
        channeltype=TYPE_SS7_TUP;
        break;
    case 11:        //ISUP通道（中国SS7信令ISUP）
        channeltype=TYPE_SS7_ISUP;
        break;
    case 9:         // 传真资源通道
        channeltype=TYPE_FAX;
        break;
    case -1:        // 调用失败
    default:
        channeltype=TYPE_UNKNOWN;

    }
    return channeltype;
}

int CSsmInterface::GetChTotal()
{
    return m_totalline;
}


int CSsmInterface::SearchIdleCh(CHANNELTYPE chtype, int* pChFilter, int FilterLen,int*pChExcept,int ExceptLen)
{
    int ch=-1;


    for(int i=0;i<FilterLen;++i)
    {
        int maxfilter=HIWORD(pChFilter[i]);
        int minfilter=LOWORD(pChFilter[i]);

        ch=FindIdleCh(chtype,minfilter,maxfilter,pChExcept,ExceptLen);
        if(ch!=-1)
        {
            return ch;
        }
    }
 
    return ch;
}




//接续函数

int CSsmInterface::Dialup(int ch,LPCTSTR calling,NUMBERATTRIBUTE callingattribute,LPCTSTR called,LPCTSTR origcalled,NUMBERATTRIBUTE origcalledattribute,BOOL CallerIDBlock,int flags)
{
    SsmChannelInfo* pch=GetChInfo(ch);


    if(!(called && origcalled && origcalled))
    {
        m_lasterror.SetLastErrorCode(ERRCODE_PARAMILLEGAL);
        return 0;
    }

    const char *szcalling,*szcalled,*szorigcalled;

#if defined(UNICODE) || defined(_UNICODE)
    COMMON::UNICODEChar  wcalled(called);
    COMMON::UNICODEChar  wcalling(calling);
    COMMON::UNICODEChar  worigcalled(origcalled);

    szcalled=wcalled;
    szcalling=wcalling;
    szorigcalled=worigcalled;

#else
    szcalling=calling;
    szcalled=called;
    szorigcalled=origcalled;

#endif

    int iret=0;


    if(pch->GetType()==TYPE_ANALOG_USER)
    {
        pch->sendcid.bpick=false;
        iret=(::SsmStartRingWithCIDStr(ch,(LPSTR)szcalling,strlen(szcalling),1000)==0 ?  1 : 0);
    }
    else if(pch->GetType()==TYPE_ANALOG_TRUNK)
    {
        if(::SsmPickup(ch)==0)
        {
            iret=::SsmAutoDial(ch,(LPSTR)szcalled) ==-1 ? 0 : 1;
            if(iret==0)
            {
                ::SsmHangup(ch);
            }
        }
    }
    else if(pch->IsDigitalTrunkCh())
    {
        /*
        WORD wParam=0;
        if(CallerIDBlock)
        {
        wParam |= (1<<2);
        }

        switch(attribute)
        {
        case DIAL_CITY_NUM:
        break;
        case DIAL_COUNTRY_NUM:
        wParam |= (1<<1);
        break;
        case DIAL_INTERNATIONAL_NUM:
        wParam |= 3;
        break;
        }

        SsmSetTxCallerId(ch,(LPSTR)szcalling);
        SsmSetTxOriginalCallerID(ch,(BYTE*)szorigcalled);

        return    ::SsmAutoDialEx(ch,(LPSTR)szcalled,wParam) ==-1 ? 0 : 1;
        */

        SsmSetTxCallerId(ch,(LPSTR)szcalling);
        SsmSetTxOriginalCallerID(ch,(BYTE*)szorigcalled);

        iret=::SsmAutoDial(ch,(LPSTR)szcalled) ==-1 ? 0 : 1;


    }

    if(iret==0)
    {
        m_lasterror.SetLastErrorType(1);
    }
    else
    {
        pch->dialup.flags=flags;
       
        
        NotifyDialup(pch,calling,called,origcalled);

    }





    return iret;
}


int CSsmInterface::Pickup(int ch,int flags)
{
    SsmChannelInfo* pch=GetChInfo(ch);
    
    int ret=0;

    if(pch->GetType()==TYPE_ANALOG_USER)
    {
        ret=1;
        NotifyTalking(pch);
    }
    else
    {
        ret= ::SsmPickup(ch)==-1 ? 0 : 1;
    }

    if(ret==0)
    {
        m_lasterror.SetLastErrorType(1);
    }
    else
    {
        pch->pickup.flags=flags;
    }

    return ret;
}



int CSsmInterface::Ringback(int ch)
{
    SsmChannelInfo* pch=GetChInfo(ch);

    int iret=0;

    if(pch->IsDigitalTrunkCh()) 
    {
		//5030版本可能存在一个bug，这里返回1
        if(::SsmSetKB(ch,1)!=-1)
        {
            iret=1;
        }
        else
        {
            m_lasterror.SetLastErrorType(1);
        }
    }
    else if(pch->GetType()==TYPE_ANALOG_TRUNK)
    {
        pch->recvcid.cidsetup=RECVCID_RING;
        iret = 1;
    }
    else if(pch->GetType()==TYPE_ANALOG_USER)
    {
        NotifyRinging(pch);
        iret=1;
    }
    else
    {
        m_lasterror.SetLastErrorCode(ERRCODE_DEVICENONSUPPORT);
    }
    return iret;
}


int CSsmInterface::Hangup(int ch,RELEASEATTRIBUTE attribute)
{
    SsmChannelInfo* pch=GetChInfo(ch);

    int iret=0;


    if(pch->GetType()==TYPE_ANALOG_USER)
    {
        CHANNELSTATE chstate=pch->GetState();
        if(chstate==STATE_DIALUP || chstate==STATE_RINGBACK)
        {
            if(::SsmStopRing(ch)==0)
            {
                iret=1;
            }


        }
        else
        {
            m_lasterror.SetLastErrorCode(ERRCODE_STATEACCORD);
            return 0;
        }

    }
    else if(pch->IsTrunkCh() && (attribute==RELEASE_NORMAL || pch->GetState()==STATE_TALKING || pch->GetType()==TYPE_ANALOG_TRUNK))
    {
        iret=::SsmHangup(ch)==-1 ? 0 : 1;
    }
    else if(pch->IsDigitalTrunkCh())
    {
        if(pch->GetType()==TYPE_SS7_ISUP)
        {
            UCHAR ucCauseVal=0x90;//正常呼叫拆线
            switch(attribute)
            {
            case RELEASE_USERBUSY:
                ucCauseVal=0x91;
                break;
            case RELEASE_VACANT:
                ucCauseVal=0x81;
                break;
            case RELEASE_REJECTED:
                ucCauseVal=0x95;
                break;
            case RELEASE_NOANSWER:
                ucCauseVal=0x93;
                break;
            }

			//SsmHangupEx还可以向驱动程序传递挂机原因等信息，但只适用于ISUP通道。
            iret=::SsmHangupEx(ch,ucCauseVal)==-1 ? 0 : 1;
        }
		else
		{
			//ISDN看 SsmISDNSetHangupRzn
			iret=::SsmHangup(ch)==-1 ? 0 : 1;
		}
    }
	else
	{
		m_lasterror.SetLastErrorCode(ERRCODE_DEVICENONSUPPORT);
		return 0;
	}

    if(iret==0)
    {
        m_lasterror.SetLastErrorType(1);
    }

    return iret;
}



//DTMF函数

int  CSsmInterface::SendDtmf(int ch,LPCTSTR dtmf,int len,int flags)
{
	if(_tcslen(dtmf)!=len)
	{
		m_lasterror.SetLastErrorCode(ERRCODE_PARAMILLEGAL);
		return 0;
	}

    SsmChannelInfo* pch=GetChInfo(ch);

    const char * pdtmf=NULL;
#if defined(UNICODE) || defined(_UNICODE)
    COMMON::UNICODEChar wdtmf(dtmf);
    pdtmf=wdtmf;
#else
    pdtmf=dtmf;
#endif
    if(strlen(pdtmf)>50)
    {
        *((char*)pdtmf+50)=0;
    }

    if(::SsmTxDtmf(ch,(LPSTR)pdtmf)==0)
    {
        pch->stopdtmf.bstop=FALSE;

        return min(50,strlen(pdtmf));
    }

    m_lasterror.SetLastErrorType(1);
    return 0;

}
int  CSsmInterface::StopSendDtmf(int ch)
{    
    SsmChannelInfo* pch=GetChInfo(ch);

    if(::SsmChkTxDtmf(ch)==1)
    {
        pch->stopdtmf.bstop=TRUE;
        ::SsmStopTxDtmf(ch);
    }
    return 1;
}




int  CSsmInterface::RecvDtmf(int ch,int time,int flags)
{
    SsmChannelInfo* pch=GetChInfo(ch);

    if(time>0)
    {
        Simulatetag simulate;
        simulate.ch=ch;
        simulate.evt=EVENT_RECVDTMF;
        simulate.param=0;
		pch->recvdtmf.timer=CMessageHandler::DelayMessage(time,TRUE,EVT_SIMULATETIMER,WrapMessageData<Simulatetag>(simulate));;
        pch->recvdtmf.elapse=time;

    }
    else
    {
        pch->recvdtmf.timer=0;
    }
    return 1;
}
int  CSsmInterface::StopRecvDtmf(int ch)
{
    SsmChannelInfo* pch=GetChInfo(ch);

    if(pch->recvdtmf.timer)
    {
        CMessageHandler::CancelMessage(pch->recvdtmf.timer);
        pch->recvdtmf.timer=0;
    }

     NotifyRecvDtmf(pch,2,0);

    return 1;
}



//FSK函数

int  CSsmInterface::SendFsk(int ch,const BYTE* fsk,int len,int flags)
{
    return SendFskFull(ch,80,20,fsk,len,flags);
}
int  CSsmInterface::SendFskFull(int ch,int sync,int mark,const BYTE* fsk,int len,int flags)
{
  
    int iret=0;
    BYTE *fskbuf =new BYTE[(sync+mark+len*10)/8+1];
    int bitlen= ::SsmTransFskData((BYTE*)fsk,len,sync,mark,fskbuf);
    if(bitlen>0)
    {
        iret= ::SsmStartSendFSK(ch,(char*)fskbuf,bitlen) == -1 ? 0 : 1;
    }
    if(iret==0)
    {
        m_lasterror.SetLastErrorType(1);
    }

    delete []fskbuf;
    return iret;
}
int  CSsmInterface::StopSendFsk(int ch)
{
    SsmChannelInfo* pch=GetChInfo(ch);

    if(::SsmCheckSendFsk(ch)==1)
    {
        :: SsmStopSendFsk(ch);
    }

    //三汇有bug, 不会产生E_PROC_SendFSK事件,模拟一个
    NotifySendFsk(pch,2);

    return 1;
}



int  CSsmInterface::RecvFsk(int ch,int time,int flags)
{
   if((WORD)time<=0) 
       time=65535;

   UCHAR markcode[]={0x87};
   if(::SsmStartRcvFSK_II(ch,time,260,markcode,0)==0)
   {
        return 1;
   }
   m_lasterror.SetLastErrorType(1);
    return 0;
}



int  CSsmInterface::StopRecvFsk(int ch)
{
    if(::SsmCheckRcvFSK(ch)==0)
    {
        ::SsmStopRcvFSK(ch);
    }
    return 1;
}







//放音函数

int CSsmInterface::PlayFile(int ch,LPCTSTR fullpath,LPCTSTR StopstrDtmfCharSet)
{
    const char * szfullpath=NULL;
#if defined(UNICODE) || defined(_UNICODE)
    COMMON::UNICODEChar wchar(fullpath);
    szfullpath=wchar;
#else
    szfullpath=fullpath;
#endif

    if(::SsmPlayFile(ch,(LPSTR)szfullpath,-1,0,-1)==0)
    {
        DtmfStopPlay(ch,StopstrDtmfCharSet);
        return 1;
    }
    m_lasterror.SetLastErrorType(1);
    return 0;

}


int CSsmInterface::PlayMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet)
{
    SsmChannelInfo* pch=GetChInfo(ch);


    BOOL ret=FALSE;


 
    if(::SsmPlayMemBlock(ch,6,block1,size1,&CSsmInterface::playmemblockhangler,this)==0)
    {
        if(block2!=NULL && size2>0)
        {
            if(::SsmPlayMemBlock(ch,6,block2,size2,&CSsmInterface::playmemblockhangler,this)==0)
            {
                DtmfStopPlay(ch,StopstrDtmfCharSet);
                ret=TRUE;
            }
        }
        else
        {
            DtmfStopPlay(ch,StopstrDtmfCharSet);
            ret=TRUE;
        }
    }






    if(ret)
    {
        pch->playmemory.index=1;
        pch->playmemory.block1=block1;
        pch->playmemory.size1=size1;
        pch->playmemory.block2=block2;
        pch->playmemory.size2=size2;

        return 1;
    }
    else
    {
        if(::SsmCheckPlay(ch)==0)
        {
            ::SsmStopPlay(ch);
        }

        m_lasterror.SetLastErrorType(1);
    }



    return 0;
}


int CSsmInterface::PlayQueue(int ch,LPCTSTR fullpath[],int size,LPCTSTR StopstrDtmfCharSet)
{
    if(size>256)
    {
        m_lasterror.SetLastErrorCode(ERRCODE_PARAMILLEGAL);
        return 0;
    }

    ::SsmClearFileList(ch);

    for(int i=0;i<size;++i)
    {
        const char * szfullpath=NULL;
#if defined(UNICODE) || defined(_UNICODE)
        COMMON::UNICODEChar wchar(fullpath[i]);
        szfullpath=wchar;
#else
        szfullpath=fullpath[i];
#endif
        if(::SsmAddToFileList(ch,(char*)szfullpath,-1,0,-1)==-1)
        {
            m_lasterror.SetLastErrorType(1);
            return 0;
        }
    }


    if(::SsmPlayFileList(ch)==0)
    {
        DtmfStopPlay(ch,StopstrDtmfCharSet);
        return 1;
    }

    m_lasterror.SetLastErrorType(1);
    return 0;
}

int CSsmInterface::LoadPlayIndex(LPCTSTR fullpath,LPCTSTR Alias)
{
    static int beginindex=0;

    if(_tcslen(Alias)<20)
    {

        const char * szfullpath=NULL;
        char  szalias[24];
        const char * palias=NULL;

#if defined(UNICODE) || defined(_UNICODE)
        COMMON::UNICODEChar wchar(fullpath);
        szfullpath=wchar;

        COMMON::UNICODEChar walias(Alias);
        palias=walias;
#else
        szfullpath=fullpath;
        palias=Alias;
#endif

        sprintf(szalias,"SSM%s",palias);


        if(SsmLoadIndexData(beginindex,szalias,6,(LPSTR)szfullpath,0,-1)==0)
        {
            ++beginindex;
            return 1;
        }
    }

    m_lasterror.SetLastErrorType(1);

    return 0;
}
int CSsmInterface::PlayIndex(int ch,LPCTSTR Alias,LPCTSTR StopstrDtmfCharSet)
{
    LPCTSTR pAlias=Alias;
    std::string IdxStr;
    for(;;)
    {
       
        const char* alias=NULL;
#if defined(_UNICODE) || defined(UNICODE)

        COMMON::UNICODEChar wchar(pAlias);
        alias=wchar;

#else
        alias=pAlias;
#endif
        IdxStr.append("SSM");
        IdxStr.append(alias);
        IdxStr.append(",");



        int size=_tcslen(pAlias);
        if(*(pAlias+size+1) == _T('\0'))
        {
            break;
        }
        else
        {
            pAlias=pAlias+size+1;
        }

    }

    
    if(SsmPlayIndexString(ch,(LPSTR)IdxStr.c_str())==0)
    {
        DtmfStopPlay(ch,StopstrDtmfCharSet);
        return 1;
    }

    m_lasterror.SetLastErrorType(1);

    return 0;
}


int CSsmInterface::StopPlay(int ch)
{
    if(::SsmCheckPlay(ch)==0)
        ::SsmStopPlay(ch);

    return 1;
    
}

int CSsmInterface::UpDatePlayMemory(int ch,int index,BYTE* block,int size)
{
    SsmChannelInfo* pch=GetChInfo(ch);
    if(index+1==pch->playmemory.index)
    {
        if(block!=NULL && size>0)
        {
            if(::SsmPlayMemBlock(ch,6,block,size,&CSsmInterface::playmemblockhangler,this)==0)
            {
                if(pch->playmemory.index%2)
                {
                    pch->playmemory.block2=block;
                    pch->playmemory.size2=size;
                }
                else 
                {
                    pch->playmemory.block1=block;
                    pch->playmemory.size1=size;
                }
                return 1;
            }
        }
    }

    return 0;
}



//录音函数
int CSsmInterface::RecordFile(int ch,LPCTSTR fullpath,int time,LPCTSTR StopstrDtmfCharSet)
{
    const char * szfullpath=NULL;
#if defined(UNICODE) || defined(_UNICODE)
    COMMON::UNICODEChar wchar(fullpath);
    szfullpath=wchar;
#else
    szfullpath=fullpath;
#endif
    if(time<=0)
    {
       time=-1;
    }
    SsmSetRecMixer(ch,TRUE,0);
    if(::SsmRecToFile(ch,(LPSTR)szfullpath,-1,0,-1,time,1)==0)
    {
        DtmfStopRecord(ch,StopstrDtmfCharSet);
        return 1;
    }
    else
    {
        m_lasterror.SetLastErrorType(1);
    }
    return 0;
}
int CSsmInterface::RecordMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet)
{
    SsmChannelInfo* pch=GetChInfo(ch);

    ::SsmStopRecordMemBlock(ch);

    BOOL ret=FALSE;
    if(::SsmRecordMemBlock(ch,6,block1,size1,&CSsmInterface::recordmemblockhangler,this)==0)
    {
        if(block2!=NULL && size2>0)
        {
            if(::SsmRecordMemBlock(ch,6,block2,size2,&CSsmInterface::recordmemblockhangler,this)==0)
            {
                ret=TRUE;
            }
        }
        else
        {
            ret=TRUE;
        }
    }

    if(ret)
    {
        pch->recordmemory.index=1;
        pch->recordmemory.block1=block1;
        pch->recordmemory.size1=size1;
        pch->recordmemory.block2=block2;
        pch->recordmemory.size2=size2;
        DtmfStopRecord(ch,StopstrDtmfCharSet);
        return 1;
    }
    else
    {
        if(::SsmCheckRecord(ch)==0)
        {
            ::SsmStopRecordMemBlock(ch);
        }
        m_lasterror.SetLastErrorType(1);
    }   
    return 0;
}

int CSsmInterface::StopRecord(int ch)
{
   SsmChannelInfo* pch=GetChInfo(ch);
   BOOL ret=FALSE;
    if(pch->IsRecordVoice(RECORD_FILE))
    {
        if(::SsmChkRecToFile(ch)!=0)
        {
            ::SsmStopRecToFile(ch);
        }
        ret=TRUE;
    }
    else if(pch->IsRecordVoice(RECORD_MEMORY))
    {
        if(:: SsmCheckRecord(ch)==0)
        {
            ::SsmStopRecordMemBlock(ch);
        }
        ret=TRUE;
    }
    else
    {
        m_lasterror.SetLastErrorCode(ERRCODE_NOPRECOGNITION);
    }

    if(ret)
    {
         return 1;
    }

    m_lasterror.SetLastErrorType(1);
    return 0;
}

int CSsmInterface::UpDateRecordMemory(int ch,int index,BYTE* block,int size)
{
    SsmChannelInfo* pch=GetChInfo(ch);
    if(index+1==pch->recordmemory.index)
    {
        if(block!=NULL && size>0)
        {
            if(::SsmRecordMemBlock(ch,6,block,size,&CSsmInterface::recordmemblockhangler,this)==0)
            {
                if(pch->recordmemory.index%2)
                {
                    pch->recordmemory.block2=block;
                    pch->recordmemory.size2=size;
                }
                else 
                {
                    pch->recordmemory.block1=block;
                    pch->recordmemory.size1=size;
                }
                return 1;
            }
        }
    }
    return 0;
}







//TONE函数

int CSsmInterface::SendTone(int ch,TONEATTRIBUTE attribute,int time,int flags)
{
    SsmChannelInfo* pch=GetChInfo(ch);
    int sigtype=-1;
    switch(attribute)
    {
    case TONE_DIAL:      //拨号音        450HZ
        sigtype=0;
        break;

    case TONE_RINGBACK:  //回铃音        450HZ， 响1秒、停4秒
        sigtype=2;
        break;

    case TONE_BUSY:      //忙音          450HZ， 响350毫秒、停350毫秒
        sigtype=1;
        break;

    case TONE_BLOCK:     //拥塞音        450HZ， 响700毫秒、停700毫秒
        {
            if(SendToneEx(ch,450,0,0,0,700,700,time,flags)==1)
            {
                return 1;
            }
            return 0;
        }
        break;
    }

    if(sigtype!=-1)
    {        
        if(::SsmSendTone(ch,sigtype)==0)
        {
            if(time>0)
            {
                Simulatetag simulate;
                simulate.ch=ch;
                simulate.evt=EVENT_SENDTONE;
                simulate.param=0;
                pch->sendtone.timer=CMessageHandler::DelayMessage(time,TRUE,EVT_SIMULATETIMER,WrapMessageData<Simulatetag>(simulate));;
            }
            else
            {
                pch->sendtone.timer=0;
            }
            return 1;
        }
        else
        {
            m_lasterror.SetLastErrorType(1);
        }
    }
    else
    {
        m_lasterror.SetLastErrorCode(ERRCODE_DEVICENONSUPPORT);
    }
    
    return 0;
}

int CSsmInterface::SendToneEx(int ch, int nFreq1, int nVolume1, int nFreq2, int nVolume2,DWORD dwOnTime, DWORD dwOffTime,int time,int flags)
{
    SsmChannelInfo* pch=GetChInfo(ch);
    if(::SsmSetTxTonePara(ch,nFreq1,nVolume1,nFreq2,nVolume2)==0)
    {
        if(dwOffTime%8)
            dwOffTime=(dwOffTime/8)*8;

        if(dwOnTime%8)
            dwOnTime=(dwOnTime/8)*8;

        if(::SsmSendToneEx(ch,dwOnTime,dwOffTime)==0)
        {
            if(time)
            {
                Simulatetag simulate;
                simulate.ch=ch;
                simulate.evt=EVENT_SENDTONE;
                simulate.param=0;
                pch->sendtone.timer=CMessageHandler::DelayMessage(time,TRUE,EVT_SIMULATETIMER,WrapMessageData<Simulatetag>(simulate));
            }
            else
            {
                pch->sendtone.timer=0;
            }
            return 1;
        }
    }
    m_lasterror.SetLastErrorType(1);
    return 0;
}

int CSsmInterface::StopSendTone(int ch)
{
    SsmChannelInfo* pch=GetChInfo(ch);

    int type;
    if(::SsmChkSendTone(ch,&type)==1)
    {
        ::SsmStopSendTone(ch);
    }

    if(pch->sendtone.timer)
    {
        CMessageHandler::CancelMessage(pch->sendtone.timer);
        pch->sendtone.timer=0;
    }

    NotifySendTone(pch,2,pch->GetSendTone());
    return 1;
}

int  CSsmInterface::RecvTone(int ch,TONEATTRIBUTE attribute,int time,int flags)
{
    SsmChannelInfo* pch=GetChInfo(ch);

    return 0;
    switch(attribute)
    {
    case TONE_BUSY:
        {
            SsmSetEvent(E_CHG_BusyTone,-1,TRUE,NULL);
            if(time>0)
            {
                Simulatetag simulate;
                simulate.ch=ch;
                simulate.evt=EVENT_RECVTONE;
                simulate.param=0;
                pch->recvtone.timer=CMessageHandler::DelayMessage(time,TRUE,EVT_SIMULATETIMER,WrapMessageData<Simulatetag>(simulate));;

            }
            else
            {
               pch->recvtone.timer=0;
            }
            return 1;
        }
        break;
    case TONE_SILENCE:
    case TONE_RINGBACK:
    case TONE_BLOCK:
    case TONE_DIAL:
    default:
        m_lasterror.SetLastErrorCode(ERRCODE_DEVICENONSUPPORT);

    }


    return 0;


}
int  CSsmInterface::StopRecvTone(int ch,TONEATTRIBUTE attribute)
{
    SsmChannelInfo* pch=GetChInfo(ch);

    switch(attribute)
    {
    case TONE_BUSY:
         SsmSetEvent(E_CHG_BusyTone,-1,FALSE,NULL);
         return 1;
        break;
    case TONE_SILENCE:
    case TONE_DIAL:
    case TONE_RINGBACK:
    case TONE_BLOCK:
    default:
        {
            m_lasterror.SetLastErrorCode(ERRCODE_DEVICENONSUPPORT);
        }
    }
    return 0;
}



//交换函数
int CSsmInterface::Listen(int initiativech,int passivelych)
{
    if(::SsmListenTo(passivelych,initiativech)==0)
    {
        return 1;
    }
    m_lasterror.SetLastErrorType(1);
    return 0;

}

int CSsmInterface::UnListen(int initiativech,int passivelych)
{
    if(SsmStopListenTo(passivelych,initiativech)==0)
    {
        return 1;
    }
    m_lasterror.SetLastErrorType(1);
    return 0;
}

void CSsmInterface::ResetCh(int ch)
{

}

int CALLBACK CSsmInterface::EventCallBackProc(WORD wEvent, int nReference, DWORD dwParam, DWORD dwUser)
{

    int ch=nReference;


    CSsmInterface* pssmwrap=(CSsmInterface*)dwUser;
    EventLog(LEVEL_SENSITIVE,"CSsmWrap::EventCallBackProc 通道:"<<ch<<" wEvent:"<<(LPVOID)wEvent<<" nReference:"<<nReference<<" dwParam:"<<dwParam<<" dwUser:"<<dwUser<<
        "\n"<<" Name:"<<pssmwrap->EventToName(wEvent)<<" Description:"<<pssmwrap->EventToDescription(wEvent)<<" Detail:"<<pssmwrap->EventToDetail(ch,wEvent,dwParam));




    EventNotifytag eventnotify;
    eventnotify.dwParam=dwParam;
    eventnotify.nReference=nReference;
    pssmwrap->PostMessage(EVT_BOARDBEGIN+wEvent,WrapMessageData<EventNotifytag>(eventnotify));
 
    return 1;
    
    
}


BOOL  CSsmInterface::playmemblockhangler(int ch, int nEndReason, PUCHAR pucBuf, DWORD dwStopOffset, PVOID pV)
{
    CSsmInterface* pssmwrap=(CSsmInterface*)pV;
    EventNotifytag eventnotify;
    eventnotify.dwParam=nEndReason;
    eventnotify.nReference=ch;
    pssmwrap->PostMessage(EVT_UPDATEPLAY,WrapMessageData<EventNotifytag>(eventnotify));
    return TRUE;
}

BOOL  CSsmInterface::recordmemblockhangler(int ch, int nEndReason, PUCHAR pucBuf, DWORD dwStopOffset, PVOID pV)
{
    CSsmInterface* pssmwrap=(CSsmInterface*)pV;
    EventNotifytag eventnotify;
    eventnotify.dwParam=nEndReason;
    eventnotify.nReference=ch;
    pssmwrap->PostMessage(EVT_UPDATERECORD,WrapMessageData<EventNotifytag>(eventnotify));
    return TRUE;
}







void CSsmInterface::OnStatePending(SsmChannelInfo* pch)
{
    int ch=pch->GetID();
    int reason = ::SsmGetPendingReason(ch);

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" "<<CallPendingToDescription(reason));
    switch(reason)
	{
	case PEND_WaitBckStpMsg:			        //ISUP TUP ISDN SS1 来话呼叫：驱动程序按照预设的收号规则完成被叫号码等信息的接收后，“自动应答来话呼叫”功能未开启，需要应用程序自行决定是否接受此呼叫
		{

			char* pcalling=::SsmGetCallerIdA(ch);
			if(pcalling==NULL)
			{
				pcalling="P";
			}

			char* pcalled=::SsmGetPhoNumStrA(ch);
			if(pcalled==NULL)
			{
				pcalled="P";
			}

			char* porigcalled=::SsmGet1stPhoNumStrA(ch);
			if(porigcalled==NULL)
			{
				porigcalled="P";
			}

			TCHAR called[128]=_T("\0");
			TCHAR calling[128]=_T("\0");
			TCHAR origcalled[128]=_T("\0");

#if defined(_UNICODE) || defined(UNICODE)
			COMMON::ASCIIChar acharcalling(pcalling);
			_tcsncpy(calling,acharcalling,128-1);
			COMMON::ASCIIChar acharcalled(pcalled);
			_tcsncpy(called,acharcalled,128-1);
			COMMON::ASCIIChar acharorigcalled(porigcalled);
			_tcsncpy(origcalled,acharorigcalled,128-1);

#else
			_tcsncpy(calling,pcalling,128-1);
			_tcsncpy(called,pcalled,128-1);
			_tcsncpy(origcalled,porigcalled,128-1);

#endif

			NotifyCallin(pch,calling,called,origcalled);
		}
		break;


	case PEND_RemoteHangupOnTalking:			//ISUP TUP SS1 来话呼叫：双方通话时，对端挂机
		//SS1OUT_TALKING_REMOTE_HANGUPED
	case 54://PEND_RemoteHangupOnRinging:		//ISUP TUP 来话呼叫：通道处于“振铃”状态时，对端交换机取消了呼叫
			::SsmHangup(ch);
			NotifyHangup(pch,RELEASE_NORMAL);
		break;


	case PEND_AutoDialFailed:                   //ISUP TUP 去话呼叫失败。具体的自动拨号失败原因，可调用SsmGetAutoDialFailureReason()获得
		{
			int Failedreason=::SsmGetAutoDialFailureReason(ch);
			TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" 通道:"<<ch<<" SsmGetAutoDialFailureReason:"<<AutoDialFailedToDescription(ch,Failedreason));
			::SsmHangup(ch);
		}
		break;
	case PEND_CalleeHangupOnTalking:			//ISUP TUP SS1 去话呼叫：双方通话时，对端挂机
		{
			::SsmHangup(ch);
			NotifyHangup(pch,RELEASE_NORMAL);
		}
		break;
	case PEND_SsxUnusable:                      //SS7信令不可用
	case PEND_CircuitReset:                     //ISUP TUP 发生电路复原事件
	case PEND_PcmSyncLos:                       //ISUP TUP SS1 数字中继线的基本帧（0时隙）同步信号丢失
	case ISUP_HardCircuitBlock:                 //ISUP 收到对端交换机的硬件闭塞消息
	case ISUP_RemoteSuspend:                    //ISUP T6定时器溢出。有关T6定时器的更多信息请参见第1章中“ISUP通道的状态转移”部分内容
		{
			::SsmHangup(ch);
			if(pch->GetWay()==WAY_CALLOUT)
			{
				
				if(pch->IsCallStates())
				{
					NotifyHangup(pch,RELEASE_LINEFAULT);
				}
			}	
		}
		break;

	case 63://PEND_RemoteHangupOnSuspend:       //ISUP“暂停”状态时，对端用户挂机
	case 64://PEND_CalleeHangupOnSuspend:       //ISUP 暂停”状态时，被叫用户挂机
			//不处理
		break;




    case PEND_CalleeHangupOnWaitRemotePickUp:   //TUP 去话呼叫失败：等待被叫用户摘机时，收到对端交换机的拆线消息
        {
           	::SsmHangup(ch);
        }
        break;

    case PEND_RcvHGBOrSGB:                      //收到对端交换机闭塞消息（SGB/HGB）
        {
            ::SsmHangup(ch);
            if(pch->GetWay()==WAY_CALLOUT)
            {

                if(pch->IsCallStates())
                {
                    NotifyHangup(pch,RELEASE_LINEFAULT);
                }
            }	
        }
        break;









    case ANALOGOUT_NO_DIALTONE:                 //模拟中继线通道 去话呼叫失败：未检测到拨号音
        {
             ::SsmHangup(ch);
        }
        break;
    case ANALOGOUT_ECHO_NOVOICE:                //模拟中继线通道 去话呼叫：检测到回铃音后，线路上保持静默，驱动程序无法判别被叫是否摘机
        //不处理
        break;

    case ANALOGOUT_NOANSWER:                    //模拟中继线通道 去话呼叫：检测到回铃信后，被叫用户未在配置项MaxWaitAutoDialAnswerTime指定的时间内应答
    case ANALOGOUT_BUSYTONE:                    //模拟中继线通道 去话呼叫失败：检测到忙音
    case ANALOGOUT_NOVOICE:                     //模拟中继线通道 去话呼叫：拨号过程已经完成，未在线路上检测到回铃音和任何其它的语音信号，驱动程序无法判断被叫用户是否摘机
        {
            if(pch->dialup.flags&CHECKFLAG_CHECKHANG)
            {
                ::SsmHangup(ch);
            }
        }
        break;
    case ANALOGOUT_TALKING_REMOTE_HANGUPED:     //模拟中继线通道 通道在“连接”状态时检测到对端用户挂机
        {
            if(pch->dialup.flags&CHECKFLAG_CHECKHANG || pch->pickup.flags&CHECKFLAG_CHECKHANG)
            {
                ::SsmHangup(ch);
                NotifyHangup(pch,RELEASE_NORMAL);
            }
        }
        break;


    case -1:
        break;

    default:
		_ASSERT(0);
        break;
    }

}




LPCTSTR CSsmInterface::StateToText(int state)
{
    switch(state)
    {
    case S_CALL_STANDBY:                    return _T("空闲");
    case S_CALL_PICKUPED:                 	return _T("摘机");
    case S_CALL_RINGING:                 	return _T("振铃");
    case S_CALL_TALKING:                 	return _T("通话");
    case S_CALL_ANALOG_WAITDIALTONE:        return _T("模拟中继线通道：去话呼叫 等待拨号音");
    case S_CALL_ANALOG_TXPHONUM:            return _T("模拟中继线通道：去话呼叫 拨号");
    case S_CALL_ANALOG_WAITDIALRESULT:      return _T("模拟中继线通道：去话呼叫 等待拨号结果");
    case S_CALL_PENDING:                 	return _T("挂起");
    case S_CALL_OFFLINE:                 	return _T("线路断开 状态");
    case S_CALL_WAIT_REMOTE_PICKUP:         return _T("去话呼叫 等候被叫用户摘机");
    case S_CALL_ANALOG_CLEAR:               return _T("模拟中继线通道：内部状态");
    case S_CALL_UNAVAILABLE:                return _T("通道不可用");
    case S_CALL_LOCKED:                 	return _T("呼出锁定");
    case S_CALL_RemoteBlock:                return _T("对端闭塞");
    case S_CALL_LocalBlock:                 return _T("本地闭塞");
    case S_CALL_Ss1InWaitPhoNum:            return _T("SS1通道：接收被叫号码");
    case S_CALL_Ss1InWaitFwdStop:           return _T("SS1通道：等待对端交换机停发前向信号");
    case S_CALL_Ss1InWaitCallerID:          return _T("SS1通道：接收Caller ID");
    case S_CALL_Ss1InWaitKD:                return _T("SS1通道：接收KD信号");
    case S_CALL_Ss1InWaitKDStop:            return _T("SS1通道：等待对端交换机停发KD信号");
    case S_CALL_SS1_SAYIDLE:                return _T("SS1通道：向对端交换机发送示闲信号");
    case S_CALL_SS1WaitIdleCAS:             return _T("SS1通道：等待对端交换机的示闲信令");
    case S_CALL_SS1PhoNumHoldup:            return _T("SS1通道：号码拦截 状态");
    case S_CALL_Ss1InWaitStopSendA3p:       return _T("SS1通道：等待对端交换机停发脉冲方式的A3信号");
    case S_CALL_Ss1OutWaitBwdAck:           return _T("SS1通道：等待对端交换机应答占用证实信号");
    case S_CALL_Ss1OutTxPhoNum:             return _T("SS1通道：发送被叫号码");
    case S_CALL_Ss1OutWaitAppendPhoNum:     return _T("SS1通道：等待应用程序追加电话号码");
    case S_CALL_Ss1OutTxCallerID:           return _T("SS1通道：发送主叫号码");
    case S_CALL_Ss1OutWaitKB:               return _T("SS1通道：等待对端交换机的KB信号");
    case S_CALL_Ss1OutDetectA3p:            return _T("SS1通道：等待对端交换机的A3脉冲信号");
    case S_FAX_ROUND:                 	    return _T("FAX 通道：状态转移过程中");
    case S_FAX_PhaseA:                   	return _T("FAX 通道：传真呼叫建立（PhaseA）");
    case S_FAX_PhaseB:                   	return _T("FAX 通道：传真报文前处理（PhaseB）");
    case S_FAX_SendDCS:                  	return _T("FAX 通道：传真发送中向接收方发送DCS信号”");
    case S_FAX_Train:                    	return _T("FAX 通道：传真报文传输前传输训练");
    case S_FAX_PhaseC:                   	return _T("FAX 通道：传真报文传输中（PhaseC）");
    case S_FAX_PhaseD:                   	return _T("FAX 通道：传真报文后处理(PhaseD)");
    case S_FAX_NextPage:                 	return _T("FAX 通道：传真报文传输下一页");
    case S_FAX_AllSent:                  	return _T("FAX 通道：传真发送中报文传输结束");
    case S_FAX_PhaseE:                   	return _T("FAX 通道：传真呼叫释放(PhaseE)");
    case S_FAX_Reset:                    	return _T("FAX 通道：复位MODEM");
    case S_FAX_Init:                     	return _T("FAX 通道：初始化MODEM");
    case S_FAX_RcvDCS:                   	return _T("FAX 通道：传真接收 接收发方的DCS信号");
    case S_FAX_SendFTT:                  	return _T("FAX 通道：传真接收 发送训练失败信号FTT");
    case S_FAX_SendCFR:                 	return _T("FAX 通道：传真接收 发送可接受的证实信号CFR");
    case S_TUP_WaitPcmReset:                return _T("TUP 通道：电路复原");
    case S_TUP_WaitSAM:                     return _T("TUP 通道：等待对端交换机的后续地址消息");
    case S_TUP_WaitGSM:                     return _T("TUP 通道：等待对端交换机的GSM消息");
    case S_TUP_WaitCLF:                     return _T("TUP 通道：等待对端交换机的拆线消息");
    case S_TUP_WaitPrefix:                  return _T("TUP 通道：接收入局字冠");
    case S_TUP_WaitDialAnswer:              return _T("TUP 通道：等待对端交换机的应答消息");
    case S_TUP_WaitRLG:                     return _T("TUP 通道：等待对端交换机的RLG消息");
    case S_TUP_WaitSetCallerID:             return _T("TUP 通道：等待应用程序设置主叫号码");
    case S_ISDN_OUT_WAIT_NET_RESPONSE:     	return _T("ISDN 通道：等待网络响应");
    case S_ISDN_OUT_PLS_APPEND_NO:          return _T("ISDN 通道：等待应用程序追加号码");
    case S_ISDN_IN_CHK_CALL_IN:             return _T("ISDN 通道：检测到呼入");
    case S_ISDN_IN_RCVING_NO:               return _T("ISDN 通道：正在接收号码");
    case S_ISDN_IN_WAIT_TALK:               return _T("ISDN 通道：准备进入通话");
    case S_ISDN_OUT_WAIT_ALERT:             return _T("ISDN 通道：等待对方发提醒信号");
    case S_ISDN_CALL_BEGIN:                 return _T("ISDN 通道：发起呼叫(去话)或检测到呼入(来话)");
    case S_ISDN_WAIT_HUANGUP:               return _T("ISDN 通道：等待释放完成");
    case S_CALL_SENDRING:                   return _T("磁石通道：发送振铃");
    case S_ISUP_WaitSAM:                 	return _T("ISUP通道：等待对端交换机的SAM");
    case S_ISUP_WaitRLC:                 	return _T("ISUP通道：等待对端交换机的释放监护信号RLC");
    case S_ISUP_WaitReset:                 	return _T("ISUP通道：电路复原");
    case S_ISUP_LocallyBlocked:            	return _T("ISUP通道：本地闭塞");
    case S_ISUP_RemotelyBlocked:           	return _T("ISUP通道：远端闭塞");
     case S_ISUP_WaitDialAnswer:            return _T("ISUP通道：等待对端交换机的应答消息");
    case S_ISUP_WaitINF:                  	return _T("ISUP通道：等待对端交换机的INF消息");
    case S_ISUP_WaitSetCallerID:            return _T("ISUP通道：等待应用程序设置主叫号码");
    case S_DTRC_ACTIVE:                 	return _T("DTR 通道：被监控话路处于非空闲状态");
    case S_ISUP_Suspend:                 	return _T("ISUP通道：暂停");
    case 130:                       	    return _T("E/M通道：拨号");                         //S_CALL_EM_TXPHONUM
    case 131:                    	        return _T("E/M通道：等待对端的示闲信号");           //S_CALL_EM_WaitIdleCAS
    case S_CALL_VOIP_DIALING:               return _T("IP通道：VoIP主叫拨号状态");
    case S_CALL_VOIP_WAIT_CONNECTED:        return _T("IP通道：VoIP被叫摘机等待进入通话状态");
    case S_CALL_VOIP_CHANNEL_UNUSABLE:      return _T("IP通道：VoIP通道目前不可用");
    default:
        {
            static TCHAR buf[16];
            _sntprintf(buf,16,_T("保留%d"),state);
            return buf;
        }
    }
 
    return _T("\0");
}



LPCTSTR CSsmInterface::EventToDescription(int weventevent)
{
    switch(weventevent)
    {
    case E_PROC_Recognize:	return _T("语音识别结束");
    case E_CHG_ISDNStatus:	return _T("ISDN：ISDN协议的LAPD层状态发生变化");
    case E_RCV_Ss7Msu:	    return _T("SS7：从SS7服务器收到新的消息（MSU）");
    case E_CHG_Mtp3State:	return _T("SS7：SS7服务器Mtp3层的状态发生变化，通常用于指示到某个DPC的路由是否可用");
    case 0x0004:            return _T("保留[0x0004]");
    case E_CHG_FaxPages:	return _T("传真通道：驱动程序完成一页传真的接收或发送");
    case E_PROC_FaxEnd:	    return _T("传真通道：驱动程序完成传真的发送或接收任务");
    case E_CHG_PcmLinkStatus:return _T("数字中继线的同步状态发生变化");
    case E_CHG_LineVoltage:	return _T("模拟电话线路上的电压发生变化");
    case E_RCV_CAS:	        return _T("SS1通道：对端交换机的ABCD信令码发生变化");
    case E_RCV_R2:	        return _T("SS1通道：收到对端交换机的R2信号");
    case E_PROC_WaitDTMF:	return _T("WaitDTMF任务完成，任务通过函数SsmSetWaitDtmf、SsmSetWaitDtmfEx或SsmSetWaitDtmfExA提交");
    case E_CHG_RcvDTMF:	    return _T("DTMF检测器：收到一个DTMF字符");
    case E_PROC_SendDTMF:	return _T("DTMF发生器：发送DTMF的任务完成，发送DTMF任务由函数SsmTxDtmf启动");
    case E_PROC_SendFlash:	return _T("发送闪断信号的任务完成");
    case E_PROC_PlayEnd:	return _T("放音操作：放音任务结束");
    case E_PROC_PlayFile:	return _T("放音操作：文件放音进程指示");
    case E_PROC_PlayFileList:return _T("放音操作：驱动程序完成文件序列中一个文件的播放");
    case E_PROC_PlayMem:	return _T("放音操作：单个内存播放任务的进展指示");
    case E_PROC_RecordEnd:	return _T("录音操作：录音任务终止");
    case E_PROC_RecordFile:	return _T("录音操作：文件录音任务的进展指示");
    case E_PROC_RecordMem:	return _T("录音操作：内存录音任务的进展指示");
    case E_PROC_SendFSK:	return _T("FSK数据发送器完成全部数据的发送");
    case E_PROC_RcvFSK:	    return _T("RcvFSK任务结束");
    case E_CHG_ChState:	    return _T("状态机：通道状态发生变化");
    case E_PROC_AutoDial:	return _T("状态机：AutoDial任务有进展");
    case E_CHG_RemoteChBlock:return _T("TUP/ISUP通道：闭塞对端通道的操作完成");
    case E_CHG_RemotePCMBlock:return _T("TUP/ISUP通道：闭塞对端PCM的操作完成");
    case E_SYS_ActualPickup:return _T("模拟中继线通道：摘机命令执行完毕");
    case E_CHG_RingFlag:	return _T("模拟中继线通道/模拟电话线录音通道：铃流信号的电平发生变化");
    case E_CHG_RingCount:	return _T("模拟中继线通道：铃流信号检测器中信号周期的计数器发生变化");
    case E_CHG_CIDExBuf:	return _T("DTMF检测器：主叫号码扩展缓冲区的长度发生变化");
    case E_CHG_RxPhoNumBuf:	return _T("DTMF检测器：收到新的被叫号码");
    case E_CHG_PolarRvrsCount:return _T("模拟中继线通道：在线路上检测到一次极性反转");
    case E_SYS_RemotePickup:return _T("模拟中继线通道：增强的远端摘机检测器检测到被叫用户摘机");
    case E_CHG_FlashCount:	return _T("坐席通道：在电话机上检测到一次闪断操作");
    case E_CHG_HookState:	return _T("坐席通道：话机上检测到摘机或挂机动作");
    case E_CHG_ToneAnalyze:	return _T("信号音检测器：分析结果发生变化");
    case E_OverallEnergy:	return _T("信号音检测器：线路上的全频能量发生变化");
    case E_CHG_OvrlEnrgLevel:return _T("信号音检测器：全频能量标识输出事件");
    case E_CHG_BusyTone:	return _T("信号音检测器：呼叫进程音检测器检测到的忙音周期的个数发生变化");
    case E_CHG_BusyToneEx:	return _T("信号音检测器：采用背靠背忙音检测算法检测到的忙音信号");
    case E_CHG_VocFxFlag:	return _T("信号音检测器：单音频信号音的电平发生变化，通常用于传真信号音的检测");
    case E_CHG_ToneValue:	return _T("信号音检测器：信号音的电平发生变化");
    case E_CHG_RingEchoToneTime:	return _T("信号音检测器：回铃音计数器的值发生变化");
    case E_CHG_PeakFrq:	    return _T("信号音检测器：峰值频率发生变化");
    case E_SYS_BargeIn:	    return _T("Barge In检测器：检测结果发生变化");
    case E_SYS_NoSound:	    return _T("信号音检测器：线路上保持静默");
    case E_SYS_TIMEOUT:	    return _T("全局定时器：由函数SsmStartTimer启动的定时器发生溢出");
    case E_CHG_SpyState:	return _T("DTP系列：监控电路的状态发生变化");
    case 0x0032:            return _T("保留[0x0032]");
    case E_CHG_CICRxPhoNumBuf:	return _T("SS7虚电路：收到新的被叫号码");
    case E_CHG_CICState:	return _T("SS7虚电路：电路状态发生变化");
    case E_PROC_CICAutoDial:return _T("SS7虚电路：ShgAutoDial任务有进展");
    case E_RCV_Ss7IsupUtuinf: return _T("SS7：收到USR消息事件");
    case E_CHG_Mtp2Status:	return _T("SS7信令链路：信令链路的状态发生变化");
    case E_RCV_DSTDChannel:	return _T("DST系列：D通道事件");
    case E_RCV_Ss7SpyMsu:	return _T("SS7：从SS7服务器收到新的监控消息（MSU）");
    case E_CHG_ToneDetector:return _T("信号音检测器：新检测模式结果输出事件");
    case E_CHG_ToneDetectorItem:return _T("信号音检测器：新检测模式信号音周期计数事件");
    case E_PROC_FaxDcnTag:	return _T("传真通道：传真接收成功结束时，判断对方传真是否强制停止过");
    case E_CHG_AMD:	        return _T("信号音检测器，分析是否有真人摘机");
    case E_RCV_Ss7IsupCpg:	return _T("SS7：收到CPG消息事件");
    //case E_CHG_CbChStatus:	return _T("大容量坐席：监控板卡通道与大容量坐席模块连接状态发生的变化");
    default:                return _T("未知");

    }

    return _T("\0");
}


LPCTSTR CSsmInterface::EventToName(int wevent)
{
    switch(wevent)
    {
    case E_PROC_Recognize:	return _T("E_PROC_Recognize");
    case E_CHG_ISDNStatus:	return _T("IE_CHG_ISDNStatus");
    case E_RCV_Ss7Msu:	    return _T("E_RCV_Ss7Msu");
    case E_CHG_Mtp3State:	return _T("E_CHG_Mtp3State");
    case 0x0004:            return _T("0x0004");
    case E_CHG_FaxPages:	return _T("E_CHG_FaxPages");
    case E_PROC_FaxEnd:	    return _T("E_PROC_FaxEnd");
    case E_CHG_PcmLinkStatus:return _T("E_CHG_PcmLinkStatus");
    case E_CHG_LineVoltage:	return _T("E_CHG_LineVoltage");
    case E_RCV_CAS:	        return _T("E_RCV_CAS");
    case E_RCV_R2:	        return _T("E_RCV_R2");
    case E_PROC_WaitDTMF:	return _T("E_PROC_WaitDTMF");
    case E_CHG_RcvDTMF:	    return _T("E_CHG_RcvDTMF");
    case E_PROC_SendDTMF:	return _T("E_PROC_SendDTMF");
    case E_PROC_SendFlash:	return _T("E_PROC_SendFlash");
    case E_PROC_PlayEnd:	return _T("E_PROC_PlayEnd");
    case E_PROC_PlayFile:	return _T("E_PROC_PlayFile");
    case E_PROC_PlayFileList:return _T("E_PROC_PlayFileList");
    case E_PROC_PlayMem:	return _T("E_PROC_PlayMem");
    case E_PROC_RecordEnd:	return _T("E_PROC_RecordEnd");
    case E_PROC_RecordFile:	return _T("E_PROC_RecordFile");
    case E_PROC_RecordMem:	return _T("E_PROC_RecordMem");
    case E_PROC_SendFSK:	return _T("E_PROC_SendFSK");
    case E_PROC_RcvFSK:	    return _T("E_PROC_RcvFSK");
    case E_CHG_ChState:	    return _T("E_CHG_ChState");
    case E_PROC_AutoDial:	return _T("E_PROC_AutoDial");
    case E_CHG_RemoteChBlock:return _T("E_CHG_RemoteChBlock");
    case E_CHG_RemotePCMBlock:return _T("E_CHG_RemotePCMBlock");
    case E_SYS_ActualPickup:return _T("E_SYS_ActualPickup");
    case E_CHG_RingFlag:	return _T("E_CHG_RingFlag");
    case E_CHG_RingCount:	return _T("E_CHG_RingCount");
    case E_CHG_CIDExBuf:	return _T("E_CHG_CIDExBuf");
    case E_CHG_RxPhoNumBuf:	return _T("E_CHG_RxPhoNumBuf");
    case E_CHG_PolarRvrsCount:return _T("E_CHG_PolarRvrsCount");
    case E_SYS_RemotePickup:return _T("E_SYS_RemotePickup");
    case E_CHG_FlashCount:	return _T("E_CHG_FlashCount");
    case E_CHG_HookState:	return _T("E_CHG_HookState");
    case E_CHG_ToneAnalyze:	return _T("E_CHG_ToneAnalyze");
    case E_OverallEnergy:	return _T("E_OverallEnergy");
    case E_CHG_OvrlEnrgLevel:return _T("E_CHG_OvrlEnrgLevel");
    case E_CHG_BusyTone:	return _T("E_CHG_BusyTone");
    case E_CHG_BusyToneEx:	return _T("E_CHG_BusyToneEx");
    case E_CHG_VocFxFlag:	return _T("E_CHG_VocFxFlag");
    case E_CHG_ToneValue:	return _T("E_CHG_ToneValue");
    case E_CHG_RingEchoToneTime:	return _T("E_CHG_RingEchoToneTime");
    case E_CHG_PeakFrq:	    return _T("E_CHG_PeakFrq");
    case E_SYS_BargeIn:	    return _T("E_SYS_BargeIn");
    case E_SYS_NoSound:	    return _T("E_SYS_NoSound");
    case E_SYS_TIMEOUT:	    return _T("E_SYS_TIMEOUT");
    case E_CHG_SpyState:	return _T("E_CHG_SpyState");
    case 0x0032:            return _T("x0032");
    case E_CHG_CICRxPhoNumBuf:	return _T("E_CHG_CICRxPhoNumBuf");
    case E_CHG_CICState:	return _T("E_CHG_CICState");
    case E_PROC_CICAutoDial:return _T("E_PROC_CICAutoDial");
    case E_RCV_Ss7IsupUtuinf: return _T("E_RCV_Ss7IsupUtuinf");
    case E_CHG_Mtp2Status:	return _T("E_CHG_Mtp2Status:");
    case E_RCV_DSTDChannel:	return _T("E_RCV_DSTDChannel");
    case E_RCV_Ss7SpyMsu:	return _T("E_RCV_Ss7SpyMsu");
    case E_CHG_ToneDetector:return _T("E_CHG_ToneDetector");
    case E_CHG_ToneDetectorItem:return _T("E_CHG_ToneDetectorItem");
    case E_PROC_FaxDcnTag:	return _T("E_PROC_FaxDcnTag");
    case E_CHG_AMD:	        return _T("E_CHG_AMD");
    case E_RCV_Ss7IsupCpg:	return _T("E_RCV_Ss7IsupCpg");
    //case E_CHG_CbChStatus:	return _T("E_CHG_CbChStatus");
    default:                
        {
            static TCHAR buf[8];
            _sntprintf(buf,8,_T("0x%04x"),wevent);
            return buf;
        }
    }

    return _T("\0");
}

LPCTSTR CSsmInterface::CallPendingToDescription(DWORD reason)
{
    switch(reason)
    {
    case ANALOGOUT_NO_DIALTONE:             return _T("模拟中继线通道去话呼叫失败：未检测到拨号音");
    case ANALOGOUT_BUSYTONE:                return _T("模拟中继线通道去话呼叫失败：检测到忙音");
    case ANALOGOUT_ECHO_NOVOICE:            return _T("模拟中继线通道去话呼叫：检测到回铃音后，线路上保持静默，驱动程序无法判别被叫是否摘机");
    case ANALOGOUT_NOANSWER:                return _T("模拟中继线通道去话呼叫：检测到回铃信后，被叫用户未在配置项MaxWaitAutoDialAnswerTime指定的时间内应答");
    case ANALOGOUT_TALKING_REMOTE_HANGUPED: return _T("模拟中继线通道通道在“连接”状态时检测到对端用户挂机");
    case ANALOGOUT_NOVOICE:                 return _T("模拟中继线通道去话呼叫：拨号过程已经完成，未在线路上检测到回铃音和任何其它的语音信号，驱动程序无法判断被叫用户是否摘机");
    case PEND_WaitBckStpMsg:                return _T("ISUP/TUP ISDN SS1来话呼叫：驱动程序按照预设的收号规则完成被叫号码等信息的接收后，“自动应答来话呼叫”功能未开启，需要应用程序自行决定是否接受此呼叫");
    case SS1IN_BWD_KB5:                     return _T("SS1来话呼叫：收到对端交换机的KD信号后，应用程序设置KB=5（空号），等待主叫方的拆线信号");
    case PEND_RemoteHangupOnTalking:        return _T("ISUP/TUP SS1来话呼叫：双方通话时，对端挂机");
    case PEND_AutoDialFailed:               return _T("ISUP/TUP 去话呼叫失败。具体的自动拨号失败原因，可调用SsmGetAutoDialFailureReason()获得");
    case PEND_SsxUnusable:                  return _T("ISUP/TUP SS7信令不可用");
    case PEND_CircuitReset:                 return _T("ISUP/TUP 发生电路复原事件");
    case PEND_PcmSyncLos:                   return _T("ISUP/TUP SS1数字中继线的基本帧（0时隙）同步信号丢失");
     //   SS1OUT_TALKING_REMOTE_HANGUPED
    case PEND_CalleeHangupOnTalking:        return _T("ISUP/TUP/SS1 去话呼叫：双方通话时，对端挂机");
    case SS1OUT_NOANSWER:                   return _T("SS1去话呼叫失败：被叫用户未在配置项MaxWaitAutoDialAnswerTime指定的时间内应答");
    case SS1OUT_NOBWDACK:                   return _T("SS1去话呼叫失败：等待对端交换机的“占用证实”信号超时");
    case SS1OUT_DIALING_BWD_HANGUP:         return _T("SS1去话呼叫失败：对端交换机取消呼叫");
    case SS1OUT_BWD_A5:                     return _T("SS1去话呼叫失败：收到对端交换机的A5信号（空号）");
    case SS1OUT_BWD_KB5:                    return _T("SS1去话呼叫失败：收到对端交换机的KB=5（空号）");
    case SS1OUT_BWD_KB2:                    return _T("SS1去话呼叫失败：收到对端交换机的KB=2（被叫用户“市忙”）");
    case SS1OUT_BWD_KB3:                    return _T("SS1去话呼叫失败：收到对端交换机的KB=3（被叫用户“长忙”）");
    case SS1OUT_BWD_A4:                     return _T("SS1去话呼叫失败：收到对端交换机的A4信号（机键拥塞）");
    case SS1OUT_BWD_KB4:                    return _T("SS1去话呼叫失败：收到对端交换机的KB=4信号（机键拥塞）");
    case SS1OUT_TIMEOUT_BWD_A:              return _T(" SS1去话呼叫失败：等待对端交换机的后向A组信号超时");
    case SS1OUT_TIMEOUT_BWD_A_STOP:         return _T("SS1去话呼叫失败：等待对端交换机停发后向A组信号超时");
    case SS1OUT_TIMEOUT_BWD_KB:             return _T("SS1去话呼叫失败：等待对端交换机的KB信号超时");
    case SS1OUT_TIMEOUT_BWD_KB_STOP:        return _T("SS1去话呼叫失败：等待对端交换机停发KB信号超时");
    case SS1OUT_TIMEOUT_CALLERID_BWD_A1:    return _T(" SS1去话呼叫失败：向对端交换机发送主叫号码时，等待对端交换机后向A组信号超时。");
    case SS1OUT_TIMEOUT_CALLERID_BWD_A1_STOP:return _T("SS1去话呼叫失败：向对端交换机发送主叫号码时，等待对端交换机停发后向A组信号超时，自动拨号失败");
    case SS1OUT_UNDEFINED_CALLERID_BWD_A:   return _T("SS1去话呼叫失败：向对端交换机发送主叫号码时，收到未定义的后向A组信号");
    case SS1OUT_UNDEFINED_BWD_A:            return _T("去话呼叫失败：收到未定义的后向A组信号");
    case SS1OUT_UNDEFINED_BWD_KB:           return _T("去话呼叫失败：收到未定义的KB信号");
    case ISDN_CALLOVER:                     return _T("ISDN呼叫结束，对方先挂机");
    case ISDN_WAIT_RELEASE:                 return _T("ISDN收到对方的“拆线”消息，等待释放本端链路");
    case ISDN_HANGING:                      return _T("ISDN本端先挂机，正在拆线");
    case ISDN_RELEASING:                    return _T("ISDN正在释放呼叫");
    case ISDN_UNALLOCATED_NUMBER:           return _T("ISDN未分配的号码");
    case ISDN_NETWORK_BUSY:                 return _T("ISDN网络忙");
    case ISDN_CIRCUIT_NOT_AVAILABLE:        return _T("ISDN指定的电路不可用");
    case PEND_CalleeHangupOnWaitRemotePickUp:return _T("TUP去话呼叫失败：等待被叫用户摘机时，收到对端交换机的拆线消息");
    case ISUP_HardCircuitBlock:             return _T("ISUP收到对端交换机的硬件闭塞消息");
    case ISUP_RemoteSuspend:                return _T("ISUPT6定时器溢出。有关T6定时器的更多信息请参见第1章中“ISUP通道的状态转移”部分内容。");
    case PEND_RcvHGBOrSGB:                  return _T("TUP收到对端交换机闭塞消息（SGB/HGB）");
    case ISDN_NO_ANSWER:                    return _T("ISDN无应答");
    case ISDN_CALL_REJ:                     return _T("ISDN呼叫拒绝");
    case 54://PEND_RemoteHangupOnRinging:
        return _T("ISUP/TUP 来话呼叫：通道处于“振铃”状态时，对端交换机取消了呼叫");
    case 55://ISDN_NO_ROUTE:
        return _T("ISDN无法路由，原因可能为“移动电话不在服务区”");
    case 56://ISDN_NO_ROUTE_TO_DEST:
        return _T("ISDN无法路由，原因可能为“移动电话关机”");
    case 57://EM_USER_BUSY:
        return _T("E/M用户忙");
    case 58://EM_CH_ERROR:
        return _T("E/M通道故障");
    case 59://EM_LOCAL_HANGUP:
        return _T("E/M本地先挂机");
    case 60://EM_LOCAL_NOANSWER:
        return _T("E/M本地无应答");
    case 61://EM_REMOTE_HANGUP:
        return _T("E/M对端先挂机");
    case 62://EM_REMOTE_NOANSWER:
        return _T("E/M对端无应答");
    case 63://PEND_RemoteHangupOnSuspend:
        return _T("ISUP“暂停”状态时，对端用户挂机");
    case 64://PEND_CalleeHangupOnSuspend:
        return _T("ISUP“暂停”状态时，被叫用户挂机");
    case 65://ISDN_NORMAL_UNSPEC:
        return _T("ISDN呼叫正常结束");
    case -1:
        return _T("SsmGetPendingReason 返回－1");
    default:
        return _T("未知");
    }
    return _T("\0");
}

LPCTSTR CSsmInterface::EventToDetail(int ch,int wevent,DWORD dwParam)
{
    switch(wevent)
    {
    case E_CHG_ChState:
        return ChStateToDescription(ch,dwParam);
    case E_PROC_AutoDial:
        return AutoDialToDescription(ch,dwParam);
    case E_CHG_RemoteChBlock:
        return RemoteChBlockToDescription(ch,dwParam);
    case E_CHG_RemotePCMBlock:
        return RemotePCMBlockToDescription(ch,dwParam);
    case E_CHG_CIDExBuf:
        return CIDExBufToDescription(ch,dwParam);
    case E_CHG_HookState:
        return HookStateToDescription(ch,dwParam);
    case E_PROC_SendFSK:
        return SendFSKToDescription(ch,dwParam);
    case E_CHG_RingCount:
        return RingCountToDescription(ch,dwParam);
    case E_CHG_ToneAnalyze:
        return ToneAnalyzeToDescription(ch,dwParam);
    case E_CHG_AMD:
        return AmdToDescription(ch,dwParam);
    case E_CHG_RcvDTMF:
        return RecvDTMFToDescription(ch,dwParam);
    case E_PROC_SendDTMF:
        return SendDTMFToDescription(ch,dwParam);
    case E_PROC_RcvFSK:
        return RecvFSKToDescription(ch,dwParam);
    case E_PROC_PlayEnd:
        return PlayEndToDescription(ch,dwParam);
    case E_PROC_PlayFileList:
        return PlayFileListToDescription(ch,dwParam);
    case E_PROC_PlayMem:
        return PlayMemoDescription(ch,dwParam);
    case E_PROC_PlayFile:
        return PlayFileToDescription(ch,dwParam);
    case E_PROC_RecordFile:
        return RecordFileToDescription(ch,dwParam);
    case E_PROC_RecordEnd:
        return RecordEndToDescription(ch,dwParam);
    case E_SYS_BargeIn:
        return BargeInToDescription(ch,dwParam);
    case E_CHG_BusyTone:
        return BusyToneToDescription(ch,dwParam);
	case E_CHG_RxPhoNumBuf:
		return RxPhoNumBufToDescription(ch,dwParam);
	case E_CHG_Mtp2Status:
		return Mtp2StatusToDescription(ch,dwParam);
    case E_CHG_OvrlEnrgLevel:
        return OvrlEnrgLevelToDescription(ch,dwParam);
    case E_OverallEnergy:
        return OverallEnergyToDescription(ch,dwParam);
    case E_CHG_PeakFrq:
        return PeakFrqToDescription(ch,dwParam);
    }

    return _T("\0");
}

LPCTSTR CSsmInterface::ChStateToDescription(int ch,DWORD dwParam)
{
	int oldstate=HIWORD(dwParam);
	int newstate=LOWORD(dwParam);

#if defined(UNICODE) || defined(_UNICODE)
	static std::wstring  DescriptionStr;
#else
	static std::string  DescriptionStr;
#endif

	DescriptionStr.erase(DescriptionStr.begin(),DescriptionStr.end());

	DescriptionStr.append(StateToText(oldstate));
	DescriptionStr.append(_T("to"));
	DescriptionStr.append(StateToText(newstate));


	return DescriptionStr.c_str();


}

LPCTSTR CSsmInterface::AutoDialToDescription(int ch,DWORD dwParam)
{
	switch(dwParam)
	{ 
	case DIAL_STANDBY:      return _T("通道空闲，没有执行AutoDial任务");
	case DIAL_DIALING:      return _T("正在发送被叫号码");
	case DIAL_ECHOTONE:     return _T("回铃。模拟中继线通道：拨号完成后，线路上检测到了回铃音 SS1通道：表明去话呼叫时，驱动程序收到对端交换机的后向KB=1或KB=6信号，表明被叫空闲 TUP/ISUP通道：表明驱动程序收到对端交换机的地址齐消息(ACM)");
	case DIAL_NO_DIALTONE:  return _T("没有在线路上检测到拨号音，AutoDial任务失败。只适用于模拟中继线通道");
	case DIAL_BUSYTONE:     return _T("被叫用户忙，AutoDial任务失败。对于模拟中继线通道，表明线路上检测到了忙音信号");
	case DIAL_ECHO_NOVOICE: return _T("拨号完成后，线路上先是出现了回铃音，然后保持静默。AutoDial任务终止。只适用于模拟中继线通道");
	case DIAL_NOVOICE:      return _T("拨号完成后，线路上没有检测到回铃音，一直保持静默。AutoDial任务终止。只适用于模拟中继线通道");
	case DIAL_VOICE:        return _T("被叫用户摘机，AutoDial任务完成");
	case DIAL_VOICEF1:      return _T("被叫用户摘机（检测到F1频率的应答信号），AutoDial任务完成。只适用于模拟中继线通道");
	case DIAL_VOICEF2:      return _T("被叫用户摘机（检测到F2频率的应答信号），AutoDial任务完成。只适用于模拟中继线通道");
	case DIAL_NOANSWER:     return _T("被叫用户在指定时间内没有摘机，AutoDial失败");
	case DIAL_FAILURE:      return _T("AutoDial任务失败。失败原因可以通过函数SsmGetAutoDialFailureReason获得");
	case DIAL_INVALID_PHONUM:return _T("被叫用户号码为空号，AutoDial任务失败");
	default:                return _T("未知");
	}

	return _T("\0");
}

LPCTSTR CSsmInterface::RemoteChBlockToDescription(int ch,DWORD dwParam)
{
	switch(dwParam)
	{
	case 0:	return _T("已经成功解除由本端导致的闭塞");
	case 1: return _T("已经成功闭塞对端");
	case 2: return _T("正在等待对端交换机的闭塞证实信号");
	case 3: return _T("正在等待对端交换机的解除闭塞证实信号");
	default:return _T("未知");
	}
	return _T("\0");
}

LPCTSTR CSsmInterface::RemotePCMBlockToDescription(int ch,DWORD dwParam)
{
	DWORD blockstate=LOWORD(dwParam);
	DWORD blockmode=HIWORD(dwParam);

	switch(blockstate)
	{
		case 0: return _T("已经成功解除由本端导致的闭塞");
		case 1: return _T("已经成功闭塞对端");
		case 2: return _T("正在等待对端交换机的闭塞证实信号");
		case 3: return _T("正在等待对端交换机的解除闭塞证实信号");
		default:return _T("未知");
	}

	return _T("\0");
}

LPCTSTR CSsmInterface::CIDExBufToDescription(int ch,DWORD dwParam)
{
    if(dwParam>255)
        return _T("\0");

	static CHAR cidexbuf[256];
	::SsmGetCallerId(ch,cidexbuf);


    LPCTSTR outptr=NULL;

#if defined(_UNICODE) || defined(UNICODE)
    static TCHAR wcidexbuf[256];
    ::MultiByteToWideChar(CP_ACP, 0, cidexbuf, min(256,strlen(cidexbuf)), wcidexbuf, 256 );
    outptr=wcidexbuf;
#else
    outptr=cidexbuf;
#endif

	return outptr;
}


LPCTSTR CSsmInterface::HookStateToDescription(int ch,DWORD dwParam)
{
  switch(dwParam)
  {
  case 0: return _T("挂机");
  case 1: return _T("摘机");
  }
  return _T("\0");
}

LPCTSTR CSsmInterface::SendFSKToDescription(int ch,DWORD dwParam)
{
    return _T("0-保留未使用");
}

LPCTSTR CSsmInterface::RingCountToDescription(int ch,DWORD dwParam)
{
    static TCHAR ringcount[64];
    _sntprintf(ringcount,64,_T("铃流信号周期个数:%d"),dwParam);
    return ringcount;
}

LPCTSTR CSsmInterface::ToneAnalyzeToDescription(int ch,DWORD dwParam)
{

    WORD high = HIWORD(dwParam);
    WORD low  = LOWORD(dwParam);


    switch(low)
    {
    case 1:    return _T("检测到拨号音");
    case 2:    return _T("检测到忙音");
    case 3:    return _T("检测到回铃音");
    case 4:    return _T("检测到回铃音后，线路保持静默");
    case 5:    return _T("检测到无声");
    case 6:    return _T("检测到说话声，用于自动拨号时检测被叫用户是否应答（摘机）");
    case 7:    return _T("检测到F1频率的信号音，用于自动拨号时检测被叫用户的应答类型");
    case 8:    return _T("检测到F2频率的信号音，用于自动拨号时检测被叫用户的应答类型");
    case 9:    return _T("检测到用户指定的信号音类型");
    default:   return _T("未知");
    }

    return _T("\0");
}



LPCTSTR CSsmInterface::AmdToDescription(int ch,DWORD dwParam)
{
    switch(dwParam)
    {
    case 0: return _T("检测到真人摘机");
    case 1: return _T("检测到信号音");
    case 2: return _T("检测到彩铃或提示音");
    case 3: return _T("超时");
    case 4: return _T("检测到信号音或提示音后线路无声");
    case 5: return _T("检测到拨号后线路无声");
    case 6: return _T("检测到忙音");
    default:return _T("未知");
    }
    return _T("\0");
}

LPCTSTR CSsmInterface::SendDTMFToDescription(int ch,DWORD dwParam)
{
    switch(dwParam)
    {
        case 0: return _T("完成缓冲区全部DTMF字符的发送");
        case 1: return _T("发送任务被应用程序终止");
        default:return _T("未知");
    }

    return _T("\0");
}

LPCTSTR CSsmInterface::RecvDTMFToDescription(int ch,DWORD dwParam)
{
    //高16比特为收到的DTMF字符的总数，低16比特为最新收到的字符，相当于连续调用SsmGetRxDtmfLen和SsmGetLastDtmf函数
    static TCHAR description[16];
    WORD high = HIWORD(dwParam);
    WORD low  = LOWORD(dwParam);
    _sntprintf(description,16,_T("%d %C"),high,low);
    return description;

}

LPCTSTR CSsmInterface::RecvFSKToDescription(int ch,DWORD dwParam)
{
    switch(dwParam)
    {
    case 1:     return _T("超时接收失败");
    case 2:     return _T("收到指定的结束特征字节而结束");
    case 3:     return _T("收到指定长度FSK数据而结束");
    case 4:     return _T("收到指定格式的数据而结束");
    case 5:     return _T("应用程序调用函数SsmStopRcvFSK中断");
    default:    return _T("未知");
    }

    return _T("\0");

}


LPCTSTR CSsmInterface::PlayEndToDescription(int ch,DWORD dwParam)
{
    switch(dwParam)
    {
    case 1: return _T("全部语音数据播放完毕");
    case 2: return _T("因收到DTMF字符而终止");
    case 3: return _T("因检测到Barge in 而终止");
    case 4: return _T("因检测到对端用户的挂机动作而终止");
    case 5: return _T("被应用程序终止");
    case 6: return _T("文件播放任务被暂停");
    case 7: return _T("因操作总线而终止");
    case 8: return _T("文件播放任务因网络故障而终止");
    default:return _T("未知");
    }

    return _T("\0");

}

LPCTSTR CSsmInterface::PlayFileListToDescription(int ch,DWORD dwParam)
{
    static TCHAR description[64];
    _stprintf(description,_T("文件索引值:%d"),dwParam);
    return description;
}

LPCTSTR CSsmInterface::PlayMemoDescription(int ch,DWORD dwParam)
{
    switch(dwParam)
    {
    case -1:
        return _T("驱动程序的放音指针越过缓冲区的中间位置");
        break;
    case -2:
        return _T("驱动程序的放音指针越过缓冲区尾部后，返回到缓冲区头部");
        break;
    default:
        {
            static TCHAR description[64];
            _stprintf(description,_T(" 驱动程序的放音指针在缓冲区中的偏移量:%d"),dwParam);
            return description;  
        }
        break;
    }

    return _T("\0");
}

LPCTSTR CSsmInterface::PlayFileToDescription(int ch,DWORD dwParam)
{
    static TCHAR description[64];
    _stprintf(description,_T("放音时间:%d"),dwParam);
    return description;
}

LPCTSTR CSsmInterface::RecordFileToDescription(int ch,DWORD dwParam)
{
    static TCHAR description[64];
    _stprintf(description,_T("录音时间:%d"),dwParam);
    return description;
}

LPCTSTR CSsmInterface::RecordEndToDescription(int ch,DWORD dwParam)
{
    switch(dwParam)
    {
    case 1: return _T("被应用程序终止");
    case 2: return _T("因检测到DTMF字符而终止");
    case 3: return _T("因检测到对端用户的挂机动作而终止");
    case 4: return _T("因录制的数据到达指定长度或时间而终止");
    case 5: return _T("文件录音被暂停");
    case 6: return _T("将录音数据写入到文件失败");
    default:return _T("未知");
    }

    return _T("\0");

}

LPCTSTR CSsmInterface::BargeInToDescription(int ch,DWORD dwParam)
{
    switch(dwParam)
    {
    case 0: return _T("Barge In消失");
    case 1: return _T("检测到BargeIn");
    default:return _T("\0");
    }
    return _T("\0");
}

LPCTSTR CSsmInterface::BusyToneToDescription(int ch,DWORD dwParam)
{
//    信号音检测器的忙音检测结果：
//    高16比特：指示信号音分析器，0第一组，1第二组
//    低16比特：忙音周期的个数

    static TCHAR description[128];
    _stprintf(description,_T("组：:%hd 忙音周期的个数：%hd"),HIWORD(dwParam),LOWORD(dwParam));
    return description;

}

LPCTSTR CSsmInterface::RxPhoNumBufToDescription(int ch,DWORD dwParam)
{
	if(dwParam>255)
		return _T("\0");

	static CHAR cidexbuf[256];
	::SsmGetPhoNumStr(ch,cidexbuf);


	LPCTSTR outptr=NULL;

#if defined(_UNICODE) || defined(UNICODE)
	static TCHAR wcidexbuf[256];
	::MultiByteToWideChar(CP_ACP, 0, cidexbuf, min(256,strlen(cidexbuf)), wcidexbuf, 256 );
	outptr=wcidexbuf;
#else
	outptr=cidexbuf;
#endif

	return outptr;
}


LPCTSTR CSsmInterface::AutoDialFailedToDescription(int ch,DWORD dwParam)
{
    
    switch(dwParam)
    {
    case -1:                        return _T("操作失败");
    case ATDL_NULL:                 return _T("没有呼出任务");
    case ATDL_Cancel:               return _T("AutoDial任务被应用程序取消");
    case ATDL_WaitDialAnsTimeout:   return _T("等待被叫应答超时");
    case ATDL_WaitRemotePickupTimeout:return _T("等待被叫摘机超时");
    case ATDL_Mtp3Unusable:         return _T("SS7信令：信令不可用");
    case ATDL_RcvSSB:               return _T("SS7信令：收到对端交换机的SSB消息");
    case ATDL_RcvSLB:               return _T("SS7信令：收到对端交换机的SLB消息");
    case ATDL_RcvUNN:               return _T("SS7信令：收到对端交换机的UNN消息");
    case ATDL_RcvSEC:               return _T("SS7信令：收到对端交换机的SEC消息");
    case ATDL_RcvCGC:               return _T("SS7信令：收到对端交换机的CGC消息");
    case ATDL_RcvNNC:               return _T("SS7信令：收到对端交换机的NNC消息");
    case ATDL_RcvCFL:               return _T("SS7信令：收到对端交换机的CFL消息");
    case ATDL_RcvLOS:               return _T("SS7信令：收到对端交换机的LOS消息");
    case ATDL_RcvSST:               return _T("SS7信令：收到对端交换机的SST消息");
    case ATDL_RcvACB:               return _T("SS7信令：收到对端交换机的ACB消息");
    case ATDL_RcvDPN:               return _T("SS7信令：收到对端交换机的DPN消息");
    case ATDL_RcvEUM:               return _T("SS7信令：收到对端交换机的EUM消息");
    case ATDL_RcvADI:               return _T("SS7信令：收到对端交换机的ADI消息");
    case ATDL_RcvBLO:               return _T("SS7信令：收到对端交换机的BLO消息");
    case ATDL_DoubleOccupy:         return _T("SS7信令：检出同抢");
    case ATDL_CircuitReset:         return _T("SS7信令：收到对端交换机的电路/群复原信号");
    case ATDL_BlockedByRemote:      return _T("SS7信令：电路被对端交换机闭塞");
    case ATDL_SS1WaitOccupyAckTimeout:return _T("No.1信令：等待占用应答信号超时");
    case ATDL_SS1RcvCAS_HANGUP:     return _T("No.1信令：收到后向拆线信号");
    case ATDL_SS1RcvA4:             return _T("No.1信令：收到A4信号（机键拥塞）");
    case ATDL_SS1RcvA5:             return _T("No.1信令：收到A5信号（空号）");
    case ATDL_SS1RcvUndefinedAx:    return _T("No.1信令：收到未定义后向A信号");
    case ATDL_SS1RcvUndefinedAxOnTxCallerId:return _T("No.1信令：送主叫收到未定义A");
    case ATDL_SS1WaitAxTimeout:     return _T("No.1信令：等候接收后向A组信号超时");
    case ATDL_SS1WaitAxStopTimeout: return _T("No.1信令：等候后向A组信号停发超时");
    case ATDL_SS1WaitAxTimeoutOnTxCallerId:return _T("No.1信令：送主叫时等候A信号超时");
    case ATDL_SS1WaitAxStopTimeoutOnTxCallerId:return _T("No.1信令：送主叫A信号停发超时");
    case ATDL_SS1RcvKB2:            return _T("No.1信令：收到KB2信号(用户“市忙”)");
    case ATDL_SS1RcvKB3:            return _T("No.1信令：收到KB3信号(用户“长忙”)");
    case ATDL_SS1RcvKB4:            return _T("No.1信令：收到KB4信号（机键拥塞）");
    case ATDL_SS1RcvKB5:            return _T("No.1信令：收到KB5信号（空号）");
    case ATDL_SS1RcvUndefinedKB:    return _T("No.1信令：收到未定义的KB信号");
    case ATDL_SS1WaitKBTimeout:     return _T("No.1信令：接收后向KB信号超时");
    case ATDL_SS1WaitKBStopTimeout: return _T("No.1信令：等候对方方停发KB信号超时");
    case ATDL_ISDNNETISBUS:         return _T("ISDN：网络忙");
    case ATDL_ISDNEMPTYNO:          return _T("ISDN：空号");
    case ATDL_IllegalMessage:       return _T("SS7信令：收到对端交换机的非法消息");
    case ATDL_RcvREL:               return _T("ISUP：收到对端交换机的释放消息（REL）");
    case ATDL_RcvCBK:               return _T("TUP：收到对端交换机的CBK消息");
    case ATDL_IPInvalidPhonum:      return _T("IP：所拨号码无效");
    case ATDL_IPRemoteBusy:         return _T("IP：对端忙");
    case ATDL_IPBeenRefused:        return _T("IP：被拒绝");
    case ATDL_IPDnsFail:            return _T("IP：DNS无效");
    case ATDL_IPCodecUnSupport:     return _T("IP：不支持的CODEC类型");
    case ATDL_IPOutOfResources:     return _T("IP：没有可用的资源");
    case ATDL_IPLocalNetworkErr:    return _T("IP：本端网络出现错误");
    case ATDL_IPRemoteNetworkErr:   return _T("IP：远端网络出现错误");
    default:                        return _T("保留");
    }

    return _T("\0");
}


LPCTSTR CSsmInterface::Mtp2StatusToDescription(int ch,DWORD dwParam)
{
	//SS7信令链路状态
	switch(dwParam)
	{
	case 1:			return _T("业务中断");
	case 2:			return _T("初始定位");
	case 3:			return _T("已定位/准备好");
	case 4:			return _T("已定位/未准备好");
	case 5:			return _T("业务开通");
	case 6:			return _T("处理机故障");
	default:		return _T("未知");
	}
	return _T("\0");
}

LPCTSTR CSsmInterface::OvrlEnrgLevelToDescription(int ch,DWORD dwParam)
{
//信号音分析器全频能量标识值，低16比特：
//Bit15 (信号音标志位)：0/1       无/有信号音
//Bit14 (函数操作状态位)：0       保留
//Bit13-0 (持续时间)：有/无信号音的持续时间
    static TCHAR buf[256];
    _sntprintf(buf,256,_T("信号音标志位:%d 持续时间:%d"),((WORD)dwParam)>>15,dwParam&0x3FFF);
    return buf;
}

LPCTSTR CSsmInterface::OverallEnergyToDescription(int ch,DWORD dwParam)
{
    //信号音分析器全频能量值
    static TCHAR buf[256];
    _sntprintf(buf,256,_T("全频能量值:%d"),dwParam);
    return buf;
}


LPCTSTR CSsmInterface::PeakFrqToDescription(int ch,DWORD dwParam)
{
    //信号音分析器：峰值频率
    static TCHAR buf[256];
    _sntprintf(buf,256,_T("峰值频率:%d"),dwParam);
    return buf;
}




void CSsmInterface::ShowWarn(LPCTSTR Text)
{
    ::MessageBox(NULL,Text,_T("CTISSM警告"),MB_ICONEXCLAMATION);
}



SsmChannelInfo* CSsmInterface::GetChInfo(int ch)
{
    return (SsmChannelInfo*)m_channelmgr.FindChannel(ch);
}



void CSsmInterface::GetInternalCID(int ch,TCHAR* cid)
{
	TCHAR chstr[10];
	_sntprintf(chstr,10,_T("ch%d"),ch);
	TCHAR path[MAX_PATH]=_T("\0");
	COMMON::FileNameConvCurrentFullPath(path,_T("Ssm.ini"));
	::GetPrivateProfileString(_T("Called"),chstr,_T("P"),cid,128,path);

}

void  CSsmInterface::DetectRingdown(int ch)
{
    SsmChannelInfo* pch=GetChInfo(ch);
    if(pch)
    {
        Simulatetag simulate;
        simulate.ch=ch;
        simulate.evt=EVENT_RELEASE;
        simulate.param=0;

        //7s没有铃流[主叫挂机]
        pch->recvcid.remoteblocktimer=CMessageHandler::DelayMessage(7000,TRUE,EVT_SIMULATETIMER,WrapMessageData<Simulatetag>(simulate));
    }
}




RELEASEATTRIBUTE CSsmInterface::DialFailedToReleaseAttribute(int Failedreason)
{
	switch(Failedreason)
	{
	case ATDL_NULL:                 //_T("没有呼出任务");
	case ATDL_Cancel:               //_T("AutoDial任务被应用程序取消");
			return RELEASE_NONE;
		break;
	case ATDL_WaitDialAnsTimeout:   //_T("等待被叫应答超时");
	case ATDL_WaitRemotePickupTimeout://_T("等待被叫摘机超时");
			return RELEASE_NOANSWER;
		break;
	case ATDL_Mtp3Unusable:         //_T("SS7信令：信令不可用");
	case ATDL_RcvSSB:               //_T("SS7信令：收到对端交换机的SSB消息");
	case ATDL_RcvSLB:               //_T("SS7信令：收到对端交换机的SLB消息");
	case ATDL_RcvUNN:               //_T("SS7信令：收到对端交换机的UNN消息");
	case ATDL_RcvSEC:               //_T("SS7信令：收到对端交换机的SEC消息");
	case ATDL_RcvCGC:               //_T("SS7信令：收到对端交换机的CGC消息");
	case ATDL_RcvNNC:               //_T("SS7信令：收到对端交换机的NNC消息");
	case ATDL_RcvCFL:               //_T("SS7信令：收到对端交换机的CFL消息");
	case ATDL_RcvLOS:               //_T("SS7信令：收到对端交换机的LOS消息");
	case ATDL_RcvSST:               //_T("SS7信令：收到对端交换机的SST消息");
	case ATDL_RcvACB:               //_T("SS7信令：收到对端交换机的ACB消息");
	case ATDL_RcvDPN:               //_T("SS7信令：收到对端交换机的DPN消息");
	case ATDL_RcvEUM:               //_T("SS7信令：收到对端交换机的EUM消息");
	case ATDL_RcvADI:               //_T("SS7信令：收到对端交换机的ADI消息");
	case ATDL_RcvBLO:               //_T("SS7信令：收到对端交换机的BLO消息");
	case ATDL_DoubleOccupy:         //_T("SS7信令：检出同抢");
	case ATDL_CircuitReset:         //_T("SS7信令：收到对端交换机的电路/群复原信号");
	case ATDL_BlockedByRemote:      //_T("SS7信令：电路被对端交换机闭塞");
	case ATDL_SS1WaitOccupyAckTimeout://_T("No.1信令：等待占用应答信号超时");
	case ATDL_SS1RcvCAS_HANGUP:     //_T("No.1信令：收到后向拆线信号");
	case ATDL_SS1RcvA4:             //_T("No.1信令：收到A4信号（机键拥塞）");
			return RELEASE_LINEFAULT;
		break;
	case ATDL_SS1RcvA5:             //_T("No.1信令：收到A5信号（空号）");
			return RELEASE_VACANT;
		break;
	case ATDL_SS1RcvUndefinedAx:    //_T("No.1信令：收到未定义后向A信号");
	case ATDL_SS1RcvUndefinedAxOnTxCallerId://_T("No.1信令：送主叫收到未定义A");
	case ATDL_SS1WaitAxTimeout:     //_T("No.1信令：等候接收后向A组信号超时");
	case ATDL_SS1WaitAxStopTimeout: //_T("No.1信令：等候后向A组信号停发超时");
	case ATDL_SS1WaitAxTimeoutOnTxCallerId://_T("No.1信令：送主叫时等候A信号超时");
	case ATDL_SS1WaitAxStopTimeoutOnTxCallerId://_T("No.1信令：送主叫A信号停发超时");
			return RELEASE_LINEFAULT;
		break;
	case ATDL_SS1RcvKB2:            //_T("No.1信令：收到KB2信号(用户“市忙”)");
	case ATDL_SS1RcvKB3:            //_T("No.1信令：收到KB3信号(用户“长忙”)");
			return RELEASE_USERBUSY;
		break;
	case ATDL_SS1RcvKB4:            //_T("No.1信令：收到KB4信号（机键拥塞）");
			return RELEASE_LINEFAULT;
		break;
	case ATDL_SS1RcvKB5:            //_T("No.1信令：收到KB5信号（空号）");
			return RELEASE_VACANT;
		break;
	case ATDL_SS1RcvUndefinedKB:    //_T("No.1信令：收到未定义的KB信号");
	case ATDL_SS1WaitKBTimeout:     //_T("No.1信令：接收后向KB信号超时");
	case ATDL_SS1WaitKBStopTimeout: //_T("No.1信令：等候对方方停发KB信号超时");
	case ATDL_ISDNNETISBUS:         //_T("ISDN：网络忙");
			return RELEASE_LINEFAULT;
		break;
	case ATDL_ISDNEMPTYNO:          //_T("ISDN：空号");
			return RELEASE_VACANT;
		break;
	case ATDL_IllegalMessage:       //_T("SS7信令：收到对端交换机的非法消息");
	case ATDL_RcvREL:               //_T("ISUP：收到对端交换机的释放消息（REL）");
	case ATDL_RcvCBK:               //_T("TUP：收到对端交换机的CBK消息");
			return RELEASE_REJECTED;
		break;
	case ATDL_IPInvalidPhonum:      //_T("IP：所拨号码无效");
			return RELEASE_VACANT;
		break;
	case ATDL_IPRemoteBusy:         //_T("IP：对端忙");
			return RELEASE_USERBUSY;
		break;
	case ATDL_IPBeenRefused:        //_T("IP：被拒绝");
			return RELEASE_REJECTED;
		break;
	case ATDL_IPDnsFail:            //_T("IP：DNS无效");
	case ATDL_IPCodecUnSupport:     //_T("IP：不支持的CODEC类型");
	case ATDL_IPOutOfResources:     //_T("IP：没有可用的资源");
	case ATDL_IPLocalNetworkErr:    //_T("IP：本端网络出现错误");
	case ATDL_IPRemoteNetworkErr:   //_T("IP：远端网络出现错误");
		return RELEASE_LINEFAULT;
		break;
	default:
		return RELEASE_UNUSABLE;
	}

	return RELEASE_UNUSABLE;
}



void  CSsmInterface::DtmfStopPlay(int ch,LPCTSTR StopstrDtmfCharSet)
{
    int currentstate=::SsmGetDtmfStopPlayFlag(ch);

    if(StopstrDtmfCharSet==NULL)
    {
        if(currentstate==1)
        {
            ::SsmSetDtmfStopPlay(ch,FALSE);
        }
    }
    else
    {

       

        if(currentstate==0)
        {
             ::SsmSetDtmfStopPlay(ch,TRUE);
        }

        TCHAR NewStopstrDtmfCharSet[20]=_T("\0");
        _tcsncpy(NewStopstrDtmfCharSet,StopstrDtmfCharSet,16);
      
        _tcslwr(NewStopstrDtmfCharSet);
      

        const char * aStopstrDtmfCharSet=NULL;

#if defined(UNICODE) || defined(_UNICODE)
        COMMON::UNICODEChar wchar(NewStopstrDtmfCharSet);
        aStopstrDtmfCharSet=wchar;
#else
        aStopstrDtmfCharSet=NewStopstrDtmfCharSet;
#endif
        ::SsmSetDTMFStopPlayCharSet(ch,(LPSTR)aStopstrDtmfCharSet);
    }
}


void  CSsmInterface::DtmfStopRecord(int ch,LPCTSTR StopstrDtmfCharSet)
{

    if(StopstrDtmfCharSet==NULL)
    {
        StopstrDtmfCharSet=_T("\0");
    }

    TCHAR NewStopstrDtmfCharSet[20]=_T("\0");
    _tcsncpy(NewStopstrDtmfCharSet,StopstrDtmfCharSet,16);

    _tcslwr(NewStopstrDtmfCharSet);


    const char * aStopstrDtmfCharSet=NULL;

#if defined(UNICODE) || defined(_UNICODE)
    COMMON::UNICODEChar wchar(NewStopstrDtmfCharSet);
    aStopstrDtmfCharSet=wchar;
#else
    aStopstrDtmfCharSet=NewStopstrDtmfCharSet;
#endif
    ::SsmSetDTMFStopRecCharSet(ch,(LPSTR)aStopstrDtmfCharSet);
    
}


int   CSsmInterface::FindIdleCh(CHANNELTYPE chtype,int filtermin,int filtermax,int*pChExcept,int ExceptLen)
{
    

    for(int i=filtermin;i<=(filtermax ? filtermax : m_totalline);++i)
    {
        
        for(int e=0;i<ExceptLen;++e)
        {
            if(i==pChExcept[e])
            {
                continue;
            }
        }

        SsmChannelInfo* pch=GetChInfo(i);
		if(pch)
		{
			int chstate=::SsmGetChState(i);
			if(pch->GetType()&chtype && (chstate==S_CALL_STANDBY || chstate==S_ISUP_LocallyBlocked))
			{
				if(pch->GetState()==STATE_IDLE || pch->GetState()==STATE_LOCAL_BLOCK)
				{
					return i;
				}
	       
			}
		}
    }

    return -1;
}


#pragma warning(default:4996)

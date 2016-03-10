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




//  ��װ�Ľṹ
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
	TraceLog(LEVEL_WARNING,"CSsmInterface �͹�");
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
          ::SsmEnableAutoSendKB(i,FALSE);         //�رգ��Զ�Ӧ���������У�
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

    SsmSetInterEventType(0);//��ʾʹ��MESSAGE_INFO�ṹ
    _ASSERT(SsmGetInterEventType()==0);

    //�������������׳��¼���ģʽ
    EVENT_SET_INFO EventMode={0};
    EventMode.dwWorkMode = EVENT_CALLBACK;                          //�¼��ص�ģʽ
    EventMode.lpHandlerParam = &CSsmInterface::EventCallBackProc;   //ע��ص�����
    EventMode.dwUser = (DWORD)this;

    SsmSetEvent(0xffff, -1, TRUE, &EventMode);


    //��ֹ�����¼�
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

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" ͨ��:"<<ch<<" "<<EventDescription(simulate.evt));


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
            lpdescription=_T("�������������");
        }
        break;
    case 2:
        {
            lpdescription=_T("�����������⵽DTMF �����ַ�����ֹ");
        }
        break;
    case 3:
        {
            lpdescription=_T("�����������⵽bargein����ֹ");
        }
        break;
    case 4:
        {
            lpdescription=_T("��������Ӧ�ó�����ֹ");
        }
        break;
    case 5:
        {
            lpdescription=_T("�����������⵽�Զ˹һ�����ֹ");
        }
        break;
    default:
        {
            lpdescription=_T("δ֪");
        }

    }

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" ͨ��:"<<ch<<" "<<lpdescription);

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
            lpdescription=_T("¼������Ӧ�ó�����ֹ");
        }
        break;
    case 2:
        {
            lpdescription=_T("¼���������⵽DTMF �����ַ�������");
        }
        break;
    case 3:
        {
            lpdescription=_T("¼���������⵽�Զ˹һ�������");
        }
        break;
    case 4:
        {
            lpdescription=_T("������¼�����");
        }
        break;
    default:
        {
            lpdescription=_T("δ֪");
        }

    }

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" ͨ��:"<<ch<<" "<<lpdescription);

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

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" ͨ��:"<<ch<<" "<<ChStateToDescription(ch,peventnotify.dwParam));


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
        //ģ���м�[Standby]
    case S_CALL_STANDBY:                        //����
            NotifyIdle(pch);;
        break;
        //TUP[Pending]
		//ISUP[Pending]
        //ģ���м�[Standby]
	case S_CALL_PENDING:                        //����
		    OnStatePending(pch);    
		break;

        //TUP[OffHook]
		//ISUP[OffHook] ��ժ����״̬��Ӧ�ó�������ڱ�״̬����SsmAutoDial��������ȥ������ҵ��ͨ������OffHook״̬ʱ�����������������ʱ��T1�����Ӧ�ó�����T1���֮ǰû�е���SsmAutoDial��������������ͻὫ��ͨ�����л���
        //ģ���м�[OffHook] 
	case S_CALL_PICKUPED:						//ժ��
		//ISUP ����ʱ������SsmPckup
        //ģ���м� ������
		break;


        //TUP[Ringing]
		//ISUP[Ringing]
        //ģ���м�[Ringing]
	case S_CALL_RINGING:                        //����
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
        //ģ���м�[Connected]
	case S_CALL_TALKING:                        //ͨ��
		{
			if(pch->GetWay()==WAY_CALLIN)
            {
                _ASSERT(pch->GetState()==STATE_RINGING || pch->GetState()==STATE_RINGBACK);
                NotifyTalking(pch);
            }
		}
		break;




	case S_CALL_OFFLINE:                        //����·�Ͽ���״̬
		break;


        //TUP[WaitAnser]
		//ISUP[WaitAnswer]���ȴ������û�ժ����״̬��ֻ������ȥ�����С���ʱ�����з�Ӧ�������������������������ϯͨ��ͨ�������м�ͨ�����е绰��������Ӧ����ϯͨ�����ͻ������ź�
        //ģ���м�[WaitAnswer]
	case S_CALL_WAIT_REMOTE_PICKUP:				//ȥ�����У����Ⱥ򱻽��û�ժ����
			//������
		break;


        //TUP[Unusable]
		//ISUP[Unusable]�������á�״̬
	case S_CALL_UNAVAILABLE:					//ͨ��������
			NotifyUnusable(pch);
		break;

        //TUP[Locked]
		//ISUP[Locked] ������������״̬��Ӧ�ó�������ڱ�״ֱ̬�ӵ���SsmAutoDial��������ȥ������ҵ��ͨ��Ǩ�Ƶ���״̬ʱ�����������������ʱ��T1�����Ӧ�ó�����T1���֮ǰû�е���SsmPickup��SsmAutoDial��������������ͻὫ��ͨ�����л��ա�T1��ʱ�����趨ֵΪ60��
	case S_CALL_LOCKED:							//��������
			//ISUP ����ʱ����SsmSearchIdleCallOutCh
            //TUP ����ʱ����SsmSearchIdleCallOutCh
		break;





        //TUP[BlockedByPBX]  �Զ˱���״̬���Զ˽���������ά��ʱ�����򱾶˷��ͱ�����Ϣ������ֹ���˶�������绰���С���ͨ�����ڶԶ˱���״̬ʱ����������֧��Ӧ�ó��򷢳���ȥ������ָ�����Ȼ���Խ��ܺʹ�����������
	case S_CALL_RemoteBlock:                    //���Զ˱�����
			NotifyRemoteBlock(pch);
		break;

        //TUP[BlockedByApp] ���˱���״̬����ʱ��Ӧ�ó����ܽ��е绰�������������˱���ͨ������ϵͳά�����ܡ�Ӧ�ó������ͨ�����ú���SsmBlockLocalCh��SsmBlockLocalPCM��ͨ����������֪ͨ����������ֹ�绰������ע�⣺�ڱ��˱���״̬�£�ͨ����Ȼ���Խ��ܺʹ�����������
	case S_CALL_LocalBlock:                     //�����ر�����
			 NotifyLocalBlock(pch);
		break;



        //TUP[Reseting]
    case S_TUP_WaitPcmReset:                    //TUP ͨ��������·��ԭ��
           NotifyUnusable(pch);
        break;


        //TUP[RecvPhoNum]
    case S_TUP_WaitSAM:                         //TUP ͨ�������ȴ��Զ˽������ĺ�����ַ��Ϣ��
    case S_TUP_WaitGSM:                         //TUP ͨ�������ȴ��Զ˽�������GSM��Ϣ��
    case S_TUP_WaitPrefix:                      //TUP ͨ��������������ֹڡ�
            //������
        break;

        //TUP[Dialing]
    case S_TUP_WaitDialAnswer:                  //TUP ͨ�������ȴ��Զ˽�������Ӧ����Ϣ��
    case S_TUP_WaitSetCallerID:                 //TUP ͨ����"�ȴ�Ӧ�ó����������к���"
            //������
        break;

        //TUP[Release]
    case S_TUP_WaitCLF:                         //TUP ͨ�������ȴ��Զ˽������Ĳ�����Ϣ��
    case S_TUP_WaitRLG:                         //TUP ͨ�������ȴ��Զ˽�������RLG��Ϣ��
            //������
        break;
  



		//ISUP[RecvPhoNum]
	case S_ISUP_WaitSAM:						//ISUPͨ�������ȴ��Զ˽�������SAM��
			//������
		break;

		//ISUP[Release]�����ߡ�״̬����Release״̬�����������ȴ��Է�����RLC��Ϣ���ͷ������Ϣ��
	case S_ISUP_WaitRLC:                        //ISUPͨ�����ȴ��Զ˽��������ͷż໤�ź�RLC
			//������
		break;

		//ISUP[Reseting] ����·��λ��״̬
    case S_ISUP_WaitReset:                      //ISUP����·��ԭ
			NotifyUnusable(pch);
		break;

		//ISUP[BlockingRemote] �������Զ˽�������״̬����BlockingRemote״̬���Զ˽��������ܷ���ȥ�����У������˿��Է���ȥ������
    case S_ISUP_LocallyBlocked:                 //ISUPͨ�������ر��� 
			NotifyLocalBlock(pch);
        break;
		//ISUP[BlockedByPBX] �����Զ˽�����������״̬���Զ˽���������ά��ʱ�����򱾶˷��ͱ�����Ϣ������ֹ���˶�������绰���С���ͨ�����ڶԶ˱���״̬ʱ����������֧��Ӧ�ó��򷢳���ȥ������ָ�����Ȼ���Խ��ܺʹ�����������
    case S_ISUP_RemotelyBlocked:                //ISUPͨ����Զ�˱���
			NotifyRemoteBlock(pch);
        break;


		//ISUP[AutoDial]
	case S_ISUP_WaitDialAnswer:					//ISUPͨ�������ȴ��Զ˽�������Ӧ����Ϣ��
			//������
		break;
		//ISUP[RecvPhoNum]
	case S_ISUP_WaitINF:						//ISUPͨ�������ȴ��Զ˽�������INF��Ϣ��
			//������
		break;
		//ISUP[AutoDial] 
	case S_ISUP_WaitSetCallerID:				//ISUPͨ�������ȴ�Ӧ�ó����������к��롱
			//ISUP ����ʱ �ѵ���SsmSetTxCallerId����
		break;
		//ISUP[Paused]
	case S_ISUP_Suspend :						//ISUPͨ��������ͣ��
			//������
		break;


        //ģ���м�[WaitDialtone] 
    case S_CALL_ANALOG_WAITDIALTONE:            //ģ���м���ͨ����ȥ�����У����ȴ���������
            //������
        break;

        //ģ���м�[Dialing]
    case S_CALL_ANALOG_TXPHONUM:                //ģ���м���ͨ����ȥ�����У������š�
        {
            NotifyRingback(pch);
        }
        break;

        //ģ���м�[WaitRingBackTone]            
    case S_CALL_ANALOG_WAITDIALRESULT:          //ģ���м���ͨ����ȥ�����У����ȴ����Ž����
            //������
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

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" ͨ��:"<<ch<<" "<<HookStateToDescription(ch,peventnotify.dwParam));

    if(peventnotify.dwParam==0)//�һ�
    {
        //��ֹ����ǰ�Ѹ��
        if(pch->GetState()==STATE_INITIAL || pch->internalstate==S_CALL_STANDBY)
        {

        }
        else
        {
            NotifyHangup(pch,RELEASE_NORMAL);
        }
        
    }
    else if(peventnotify.dwParam==1)//ժ��
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

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" ͨ��:"<<ch<<" "<<CIDExBufToDescription(ch,peventnotify.dwParam));

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

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" ͨ��:"<<ch<<" "<<RingCountToDescription(ch,peventnotify.dwParam));

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

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" ͨ��:"<<ch<<" "<<SendFSKToDescription(ch,peventnotify.dwParam));

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

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" ͨ��:"<<ch<<" "<<AutoDialToDescription(ch,peventnotify.dwParam));

	
	if(!pch->IsCalloutStates())
	{
		return 0;
	}

	switch(peventnotify.dwParam)
	{
		//ͨ�����У�û��ִ��AutoDial����
	case DIAL_STANDBY:
		break;

		//���ڷ��ͱ��к���
	case DIAL_DIALING:
		break;

		//����
		//SS1ͨ��������ȥ������ʱ�����������յ��Զ˽������ĺ���KB=1��KB=6�źţ��������п���
		//TUP/ISUPͨ�����������������յ��Զ˽������ĵ�ַ����Ϣ(ACM)
	case DIAL_ECHOTONE:
            if(pch->GetType()!=TYPE_ANALOG_TRUNK)
            {
			    NotifyRingback(pch);
            }
		break;

		//û������·�ϼ�⵽��������AutoDial����ʧ�ܡ�ֻ������ģ���м���ͨ��
	case DIAL_NO_DIALTONE:
        {
               NotifyHangup(pch,RELEASE_LINEFAULT);
        }
		break;

		//�����û�æ��AutoDial����ʧ�ܡ�
		//����ģ���м���ͨ����������·�ϼ�⵽��æ���ź�
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

		//������ɺ���·�����ǳ����˻�������Ȼ�󱣳־�Ĭ��AutoDial������ֹ��ֻ������ģ���м���ͨ��
	case DIAL_ECHO_NOVOICE:
        {
            if(pch->dialup.flags&CHECKFLAG_CHECKTALK && pch->GetState()==STATE_RINGBACK)
            {
                NotifyTalking(pch);
            }
            
        }
        break;
		//������ɺ���·��û�м�⵽��������һֱ���־�Ĭ��AutoDial������ֹ��ֻ������ģ���м���ͨ��
	case DIAL_NOVOICE:
        {
            if(pch->dialup.flags&CHECKFLAG_CHECKHANG)
            {
                NotifyHangup(pch,RELEASE_LINEFAULT);
            }
        }
		break;


		//�����û�ժ����AutoDial�������
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

		//�����û�ժ������⵽F1Ƶ�ʵ�Ӧ���źţ���AutoDial������ɡ�ֻ������ģ���м���ͨ��
	case DIAL_VOICEF1:
		break;

		//�����û�ժ������⵽F2Ƶ�ʵ�Ӧ���źţ���AutoDial������ɡ�ֻ������ģ���м���ͨ��
	case DIAL_VOICEF2:
		break;

		//�����û���ָ��ʱ����û��ժ����AutoDialʧ��
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

		//AutoDial����ʧ�ܡ�ʧ��ԭ�����ͨ������SsmGetAutoDialFailureReason���
	case DIAL_FAILURE:
		{
			int Failedreason=::SsmGetAutoDialFailureReason(ch);
			TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" ͨ��:"<<ch<<" SsmGetAutoDialFailureReason:"<<AutoDialFailedToDescription(ch,Failedreason));

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

		//�����û�����Ϊ�պţ�AutoDial����ʧ��
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

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" ͨ��:"<<ch<<" "<<ToneAnalyzeToDescription(ch,peventnotify.dwParam));

    return 0;
}

LRESULT CSsmInterface::OnRecvDTMF(MessageData* pdata,BOOL& bHandle)
{
    const EventNotifytag &peventnotify=UseMessageData<EventNotifytag>(pdata);
    int ch=peventnotify.nReference;

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" ͨ��:"<<ch<<" "<<RecvDTMFToDescription(ch,peventnotify.dwParam));

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
    //=0����ɻ�����ȫ��DTMF�ַ��ķ���
    //=1����������Ӧ�ó�����ֹ

    const EventNotifytag &peventnotify=UseMessageData<EventNotifytag>(pdata);
    int ch=peventnotify.nReference;

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" ͨ��:"<<ch<<" "<<SendDTMFToDescription(ch,peventnotify.dwParam));

    SsmChannelInfo* pch=GetChInfo(ch);

     if(pch->IsSendDTMF())
     {
        int result=1;

        //ssm�����и�bug, ��ʱ���������
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

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" ͨ��:"<<ch<<" "<<RecvFSKToDescription(ch,peventnotify.dwParam));

    SsmChannelInfo* pch=GetChInfo(ch);

    if(pch->IsRecvFSK())
    {

        //1����ʱ������ʧ��
        //2���յ�ָ���Ľ��������ֽڶ�����
        //3���յ�ָ������FSK���ݶ�����
        //4���յ�ָ����ʽ�����ݶ�����
        //5��Ӧ�ó�����ú���SsmStopRcvFSK�ж�

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

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" ͨ��:"<<ch<<" "<<PlayEndToDescription(ch,peventnotify.dwParam));

    SsmChannelInfo* pch=GetChInfo(ch);

    if(pch->IsPlayVoice(PLAY_NONE))
    {
        int result=0;
        TCHAR dtmf=0;;
      
        if(peventnotify.dwParam==1)         //ȫ���������ݲ������
        {
            result=1;
            
        }
        else if(peventnotify.dwParam==2)   //���յ�DTMF�ַ�����ֹ
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

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" ͨ��:"<<ch<<" "<<RecordEndToDescription(ch,peventnotify.dwParam));

    SsmChannelInfo* pch=GetChInfo(ch);

    if(pch->IsRecordVoice(RECORD_NONE))
    {
        //1����Ӧ�ó�����ֹ
        //2�����⵽DTMF�ַ�����ֹ
        //3�����⵽�Զ��û��Ĺһ���������ֹ
        //4����¼�Ƶ����ݵ���ָ�����Ȼ�ʱ�����ֹ
        //5���ļ�¼������ͣ
        //6����¼������д�뵽�ļ�ʧ��

       
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

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" ͨ��:"<<ch<<" "<<BusyToneToDescription(ch,peventnotify.dwParam));

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
    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" ͨ��:"<<ch<<" "<<RxPhoNumBufToDescription(ch,peventnotify.dwParam));
    SsmChannelInfo* pch=GetChInfo(ch);
    CHANNELSTATE chstate=pch->GetState();

	//ע������ 
	//_ASSERT(0);
    NotifySleep(pch);  
	return 0;
}


//ϵͳ������
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
            TraceLog(LEVEL_WARNING,"Init Fail SsmStartCti Ret:"<<iret<<" Reasion:"<<"���������Ѿ�װ��");
        }
        else
        {
            TraceLog(LEVEL_WARNING,"Init Fail SsmStartCti Ret:"<<iret<<" Reasion:"<<"δ֪ԭ��");
        }
        iret=0;
        ShowWarn(_T("SsmStartCtiʧ�ܣ�"));
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
    case 12:        //���ֵ绰��¼��ͨ��
    case 10:        // ��ʯͨ��
    case 15:        // H.323ͨ��
    case 16:        //SIPͨ��
        channeltype=TYPE_UNKNOWN;
        break;
    case 4:         // SS1ͨ��
        channeltype=TYPE_SS1;
        break;
    case 7:         // ISDNͨ�����û��ࣩ
        channeltype=TYPE_DSS1_USER;
        break;
    case 8:         // ISDNͨ��������ࣩ
        channeltype=TYPE_DSS1_NET;
        break;
    case 20:        //SHTϵ�а忨δ��װҵ��ģ���ͨ��
        channeltype=TYPE_ANALOG_EMPTY;
        break;
    case 0:         // ģ���м���ͨ��
        channeltype=TYPE_ANALOG_TRUNK;
        break;
    case 2:         // ��ϯͨ��
        channeltype=TYPE_ANALOG_USER;
        break;
    case 3:         // ģ���м���¼��ͨ��
        channeltype=TYPE_ANALOG_RECORD;
        break;
    case 6:         // TUPͨ��
        channeltype=TYPE_SS7_TUP;
        break;
    case 11:        //ISUPͨ�����й�SS7����ISUP��
        channeltype=TYPE_SS7_ISUP;
        break;
    case 9:         // ������Դͨ��
        channeltype=TYPE_FAX;
        break;
    case -1:        // ����ʧ��
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




//��������

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
		//5030�汾���ܴ���һ��bug�����ﷵ��1
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
            UCHAR ucCauseVal=0x90;//�������в���
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

			//SsmHangupEx���������������򴫵ݹһ�ԭ�����Ϣ����ֻ������ISUPͨ����
            iret=::SsmHangupEx(ch,ucCauseVal)==-1 ? 0 : 1;
        }
		else
		{
			//ISDN�� SsmISDNSetHangupRzn
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



//DTMF����

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



//FSK����

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

    //������bug, �������E_PROC_SendFSK�¼�,ģ��һ��
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







//��������

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



//¼������
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







//TONE����

int CSsmInterface::SendTone(int ch,TONEATTRIBUTE attribute,int time,int flags)
{
    SsmChannelInfo* pch=GetChInfo(ch);
    int sigtype=-1;
    switch(attribute)
    {
    case TONE_DIAL:      //������        450HZ
        sigtype=0;
        break;

    case TONE_RINGBACK:  //������        450HZ�� ��1�롢ͣ4��
        sigtype=2;
        break;

    case TONE_BUSY:      //æ��          450HZ�� ��350���롢ͣ350����
        sigtype=1;
        break;

    case TONE_BLOCK:     //ӵ����        450HZ�� ��700���롢ͣ700����
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



//��������
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
    EventLog(LEVEL_SENSITIVE,"CSsmWrap::EventCallBackProc ͨ��:"<<ch<<" wEvent:"<<(LPVOID)wEvent<<" nReference:"<<nReference<<" dwParam:"<<dwParam<<" dwUser:"<<dwUser<<
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

    TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" ͨ��:"<<ch<<" "<<CallPendingToDescription(reason));
    switch(reason)
	{
	case PEND_WaitBckStpMsg:			        //ISUP TUP ISDN SS1 �������У�����������Ԥ����պŹ�����ɱ��к������Ϣ�Ľ��պ󣬡��Զ�Ӧ���������С�����δ��������ҪӦ�ó������о����Ƿ���ܴ˺���
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


	case PEND_RemoteHangupOnTalking:			//ISUP TUP SS1 �������У�˫��ͨ��ʱ���Զ˹һ�
		//SS1OUT_TALKING_REMOTE_HANGUPED
	case 54://PEND_RemoteHangupOnRinging:		//ISUP TUP �������У�ͨ�����ڡ����塱״̬ʱ���Զ˽�����ȡ���˺���
			::SsmHangup(ch);
			NotifyHangup(pch,RELEASE_NORMAL);
		break;


	case PEND_AutoDialFailed:                   //ISUP TUP ȥ������ʧ�ܡ�������Զ�����ʧ��ԭ�򣬿ɵ���SsmGetAutoDialFailureReason()���
		{
			int Failedreason=::SsmGetAutoDialFailureReason(ch);
			TraceLog(LEVEL_SENSITIVE,__FUNCTION__<<" ͨ��:"<<ch<<" SsmGetAutoDialFailureReason:"<<AutoDialFailedToDescription(ch,Failedreason));
			::SsmHangup(ch);
		}
		break;
	case PEND_CalleeHangupOnTalking:			//ISUP TUP SS1 ȥ�����У�˫��ͨ��ʱ���Զ˹һ�
		{
			::SsmHangup(ch);
			NotifyHangup(pch,RELEASE_NORMAL);
		}
		break;
	case PEND_SsxUnusable:                      //SS7�������
	case PEND_CircuitReset:                     //ISUP TUP ������·��ԭ�¼�
	case PEND_PcmSyncLos:                       //ISUP TUP SS1 �����м��ߵĻ���֡��0ʱ϶��ͬ���źŶ�ʧ
	case ISUP_HardCircuitBlock:                 //ISUP �յ��Զ˽�������Ӳ��������Ϣ
	case ISUP_RemoteSuspend:                    //ISUP T6��ʱ��������й�T6��ʱ���ĸ�����Ϣ��μ���1���С�ISUPͨ����״̬ת�ơ���������
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

	case 63://PEND_RemoteHangupOnSuspend:       //ISUP����ͣ��״̬ʱ���Զ��û��һ�
	case 64://PEND_CalleeHangupOnSuspend:       //ISUP ��ͣ��״̬ʱ�������û��һ�
			//������
		break;




    case PEND_CalleeHangupOnWaitRemotePickUp:   //TUP ȥ������ʧ�ܣ��ȴ������û�ժ��ʱ���յ��Զ˽������Ĳ�����Ϣ
        {
           	::SsmHangup(ch);
        }
        break;

    case PEND_RcvHGBOrSGB:                      //�յ��Զ˽�����������Ϣ��SGB/HGB��
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









    case ANALOGOUT_NO_DIALTONE:                 //ģ���м���ͨ�� ȥ������ʧ�ܣ�δ��⵽������
        {
             ::SsmHangup(ch);
        }
        break;
    case ANALOGOUT_ECHO_NOVOICE:                //ģ���м���ͨ�� ȥ�����У���⵽����������·�ϱ��־�Ĭ�����������޷��б𱻽��Ƿ�ժ��
        //������
        break;

    case ANALOGOUT_NOANSWER:                    //ģ���м���ͨ�� ȥ�����У���⵽�����ź󣬱����û�δ��������MaxWaitAutoDialAnswerTimeָ����ʱ����Ӧ��
    case ANALOGOUT_BUSYTONE:                    //ģ���м���ͨ�� ȥ������ʧ�ܣ���⵽æ��
    case ANALOGOUT_NOVOICE:                     //ģ���м���ͨ�� ȥ�����У����Ź����Ѿ���ɣ�δ����·�ϼ�⵽���������κ������������źţ����������޷��жϱ����û��Ƿ�ժ��
        {
            if(pch->dialup.flags&CHECKFLAG_CHECKHANG)
            {
                ::SsmHangup(ch);
            }
        }
        break;
    case ANALOGOUT_TALKING_REMOTE_HANGUPED:     //ģ���м���ͨ�� ͨ���ڡ����ӡ�״̬ʱ��⵽�Զ��û��һ�
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
    case S_CALL_STANDBY:                    return _T("����");
    case S_CALL_PICKUPED:                 	return _T("ժ��");
    case S_CALL_RINGING:                 	return _T("����");
    case S_CALL_TALKING:                 	return _T("ͨ��");
    case S_CALL_ANALOG_WAITDIALTONE:        return _T("ģ���м���ͨ����ȥ������ �ȴ�������");
    case S_CALL_ANALOG_TXPHONUM:            return _T("ģ���м���ͨ����ȥ������ ����");
    case S_CALL_ANALOG_WAITDIALRESULT:      return _T("ģ���м���ͨ����ȥ������ �ȴ����Ž��");
    case S_CALL_PENDING:                 	return _T("����");
    case S_CALL_OFFLINE:                 	return _T("��·�Ͽ� ״̬");
    case S_CALL_WAIT_REMOTE_PICKUP:         return _T("ȥ������ �Ⱥ򱻽��û�ժ��");
    case S_CALL_ANALOG_CLEAR:               return _T("ģ���м���ͨ�����ڲ�״̬");
    case S_CALL_UNAVAILABLE:                return _T("ͨ��������");
    case S_CALL_LOCKED:                 	return _T("��������");
    case S_CALL_RemoteBlock:                return _T("�Զ˱���");
    case S_CALL_LocalBlock:                 return _T("���ر���");
    case S_CALL_Ss1InWaitPhoNum:            return _T("SS1ͨ�������ձ��к���");
    case S_CALL_Ss1InWaitFwdStop:           return _T("SS1ͨ�����ȴ��Զ˽�����ͣ��ǰ���ź�");
    case S_CALL_Ss1InWaitCallerID:          return _T("SS1ͨ��������Caller ID");
    case S_CALL_Ss1InWaitKD:                return _T("SS1ͨ��������KD�ź�");
    case S_CALL_Ss1InWaitKDStop:            return _T("SS1ͨ�����ȴ��Զ˽�����ͣ��KD�ź�");
    case S_CALL_SS1_SAYIDLE:                return _T("SS1ͨ������Զ˽���������ʾ���ź�");
    case S_CALL_SS1WaitIdleCAS:             return _T("SS1ͨ�����ȴ��Զ˽�������ʾ������");
    case S_CALL_SS1PhoNumHoldup:            return _T("SS1ͨ������������ ״̬");
    case S_CALL_Ss1InWaitStopSendA3p:       return _T("SS1ͨ�����ȴ��Զ˽�����ͣ�����巽ʽ��A3�ź�");
    case S_CALL_Ss1OutWaitBwdAck:           return _T("SS1ͨ�����ȴ��Զ˽�����Ӧ��ռ��֤ʵ�ź�");
    case S_CALL_Ss1OutTxPhoNum:             return _T("SS1ͨ�������ͱ��к���");
    case S_CALL_Ss1OutWaitAppendPhoNum:     return _T("SS1ͨ�����ȴ�Ӧ�ó���׷�ӵ绰����");
    case S_CALL_Ss1OutTxCallerID:           return _T("SS1ͨ�����������к���");
    case S_CALL_Ss1OutWaitKB:               return _T("SS1ͨ�����ȴ��Զ˽�������KB�ź�");
    case S_CALL_Ss1OutDetectA3p:            return _T("SS1ͨ�����ȴ��Զ˽�������A3�����ź�");
    case S_FAX_ROUND:                 	    return _T("FAX ͨ����״̬ת�ƹ�����");
    case S_FAX_PhaseA:                   	return _T("FAX ͨ����������н�����PhaseA��");
    case S_FAX_PhaseB:                   	return _T("FAX ͨ�������汨��ǰ����PhaseB��");
    case S_FAX_SendDCS:                  	return _T("FAX ͨ�������淢��������շ�����DCS�źš�");
    case S_FAX_Train:                    	return _T("FAX ͨ�������汨�Ĵ���ǰ����ѵ��");
    case S_FAX_PhaseC:                   	return _T("FAX ͨ�������汨�Ĵ����У�PhaseC��");
    case S_FAX_PhaseD:                   	return _T("FAX ͨ�������汨�ĺ���(PhaseD)");
    case S_FAX_NextPage:                 	return _T("FAX ͨ�������汨�Ĵ�����һҳ");
    case S_FAX_AllSent:                  	return _T("FAX ͨ�������淢���б��Ĵ������");
    case S_FAX_PhaseE:                   	return _T("FAX ͨ������������ͷ�(PhaseE)");
    case S_FAX_Reset:                    	return _T("FAX ͨ������λMODEM");
    case S_FAX_Init:                     	return _T("FAX ͨ������ʼ��MODEM");
    case S_FAX_RcvDCS:                   	return _T("FAX ͨ����������� ���շ�����DCS�ź�");
    case S_FAX_SendFTT:                  	return _T("FAX ͨ����������� ����ѵ��ʧ���ź�FTT");
    case S_FAX_SendCFR:                 	return _T("FAX ͨ����������� ���Ϳɽ��ܵ�֤ʵ�ź�CFR");
    case S_TUP_WaitPcmReset:                return _T("TUP ͨ������·��ԭ");
    case S_TUP_WaitSAM:                     return _T("TUP ͨ�����ȴ��Զ˽������ĺ�����ַ��Ϣ");
    case S_TUP_WaitGSM:                     return _T("TUP ͨ�����ȴ��Զ˽�������GSM��Ϣ");
    case S_TUP_WaitCLF:                     return _T("TUP ͨ�����ȴ��Զ˽������Ĳ�����Ϣ");
    case S_TUP_WaitPrefix:                  return _T("TUP ͨ������������ֹ�");
    case S_TUP_WaitDialAnswer:              return _T("TUP ͨ�����ȴ��Զ˽�������Ӧ����Ϣ");
    case S_TUP_WaitRLG:                     return _T("TUP ͨ�����ȴ��Զ˽�������RLG��Ϣ");
    case S_TUP_WaitSetCallerID:             return _T("TUP ͨ�����ȴ�Ӧ�ó����������к���");
    case S_ISDN_OUT_WAIT_NET_RESPONSE:     	return _T("ISDN ͨ�����ȴ�������Ӧ");
    case S_ISDN_OUT_PLS_APPEND_NO:          return _T("ISDN ͨ�����ȴ�Ӧ�ó���׷�Ӻ���");
    case S_ISDN_IN_CHK_CALL_IN:             return _T("ISDN ͨ������⵽����");
    case S_ISDN_IN_RCVING_NO:               return _T("ISDN ͨ�������ڽ��պ���");
    case S_ISDN_IN_WAIT_TALK:               return _T("ISDN ͨ����׼������ͨ��");
    case S_ISDN_OUT_WAIT_ALERT:             return _T("ISDN ͨ�����ȴ��Է��������ź�");
    case S_ISDN_CALL_BEGIN:                 return _T("ISDN ͨ�����������(ȥ��)���⵽����(����)");
    case S_ISDN_WAIT_HUANGUP:               return _T("ISDN ͨ�����ȴ��ͷ����");
    case S_CALL_SENDRING:                   return _T("��ʯͨ������������");
    case S_ISUP_WaitSAM:                 	return _T("ISUPͨ�����ȴ��Զ˽�������SAM");
    case S_ISUP_WaitRLC:                 	return _T("ISUPͨ�����ȴ��Զ˽��������ͷż໤�ź�RLC");
    case S_ISUP_WaitReset:                 	return _T("ISUPͨ������·��ԭ");
    case S_ISUP_LocallyBlocked:            	return _T("ISUPͨ�������ر���");
    case S_ISUP_RemotelyBlocked:           	return _T("ISUPͨ����Զ�˱���");
     case S_ISUP_WaitDialAnswer:            return _T("ISUPͨ�����ȴ��Զ˽�������Ӧ����Ϣ");
    case S_ISUP_WaitINF:                  	return _T("ISUPͨ�����ȴ��Զ˽�������INF��Ϣ");
    case S_ISUP_WaitSetCallerID:            return _T("ISUPͨ�����ȴ�Ӧ�ó����������к���");
    case S_DTRC_ACTIVE:                 	return _T("DTR ͨ��������ػ�·���ڷǿ���״̬");
    case S_ISUP_Suspend:                 	return _T("ISUPͨ������ͣ");
    case 130:                       	    return _T("E/Mͨ��������");                         //S_CALL_EM_TXPHONUM
    case 131:                    	        return _T("E/Mͨ�����ȴ��Զ˵�ʾ���ź�");           //S_CALL_EM_WaitIdleCAS
    case S_CALL_VOIP_DIALING:               return _T("IPͨ����VoIP���в���״̬");
    case S_CALL_VOIP_WAIT_CONNECTED:        return _T("IPͨ����VoIP����ժ���ȴ�����ͨ��״̬");
    case S_CALL_VOIP_CHANNEL_UNUSABLE:      return _T("IPͨ����VoIPͨ��Ŀǰ������");
    default:
        {
            static TCHAR buf[16];
            _sntprintf(buf,16,_T("����%d"),state);
            return buf;
        }
    }
 
    return _T("\0");
}



LPCTSTR CSsmInterface::EventToDescription(int weventevent)
{
    switch(weventevent)
    {
    case E_PROC_Recognize:	return _T("����ʶ�����");
    case E_CHG_ISDNStatus:	return _T("ISDN��ISDNЭ���LAPD��״̬�����仯");
    case E_RCV_Ss7Msu:	    return _T("SS7����SS7�������յ��µ���Ϣ��MSU��");
    case E_CHG_Mtp3State:	return _T("SS7��SS7������Mtp3���״̬�����仯��ͨ������ָʾ��ĳ��DPC��·���Ƿ����");
    case 0x0004:            return _T("����[0x0004]");
    case E_CHG_FaxPages:	return _T("����ͨ���������������һҳ����Ľ��ջ���");
    case E_PROC_FaxEnd:	    return _T("����ͨ��������������ɴ���ķ��ͻ��������");
    case E_CHG_PcmLinkStatus:return _T("�����м��ߵ�ͬ��״̬�����仯");
    case E_CHG_LineVoltage:	return _T("ģ��绰��·�ϵĵ�ѹ�����仯");
    case E_RCV_CAS:	        return _T("SS1ͨ�����Զ˽�������ABCD�����뷢���仯");
    case E_RCV_R2:	        return _T("SS1ͨ�����յ��Զ˽�������R2�ź�");
    case E_PROC_WaitDTMF:	return _T("WaitDTMF������ɣ�����ͨ������SsmSetWaitDtmf��SsmSetWaitDtmfEx��SsmSetWaitDtmfExA�ύ");
    case E_CHG_RcvDTMF:	    return _T("DTMF��������յ�һ��DTMF�ַ�");
    case E_PROC_SendDTMF:	return _T("DTMF������������DTMF��������ɣ�����DTMF�����ɺ���SsmTxDtmf����");
    case E_PROC_SendFlash:	return _T("���������źŵ��������");
    case E_PROC_PlayEnd:	return _T("���������������������");
    case E_PROC_PlayFile:	return _T("�����������ļ���������ָʾ");
    case E_PROC_PlayFileList:return _T("����������������������ļ�������һ���ļ��Ĳ���");
    case E_PROC_PlayMem:	return _T("���������������ڴ沥������Ľ�չָʾ");
    case E_PROC_RecordEnd:	return _T("¼��������¼��������ֹ");
    case E_PROC_RecordFile:	return _T("¼���������ļ�¼������Ľ�չָʾ");
    case E_PROC_RecordMem:	return _T("¼���������ڴ�¼������Ľ�չָʾ");
    case E_PROC_SendFSK:	return _T("FSK���ݷ��������ȫ�����ݵķ���");
    case E_PROC_RcvFSK:	    return _T("RcvFSK�������");
    case E_CHG_ChState:	    return _T("״̬����ͨ��״̬�����仯");
    case E_PROC_AutoDial:	return _T("״̬����AutoDial�����н�չ");
    case E_CHG_RemoteChBlock:return _T("TUP/ISUPͨ���������Զ�ͨ���Ĳ������");
    case E_CHG_RemotePCMBlock:return _T("TUP/ISUPͨ���������Զ�PCM�Ĳ������");
    case E_SYS_ActualPickup:return _T("ģ���м���ͨ����ժ������ִ�����");
    case E_CHG_RingFlag:	return _T("ģ���м���ͨ��/ģ��绰��¼��ͨ���������źŵĵ�ƽ�����仯");
    case E_CHG_RingCount:	return _T("ģ���м���ͨ���������źż�������ź����ڵļ����������仯");
    case E_CHG_CIDExBuf:	return _T("DTMF����������к�����չ�������ĳ��ȷ����仯");
    case E_CHG_RxPhoNumBuf:	return _T("DTMF��������յ��µı��к���");
    case E_CHG_PolarRvrsCount:return _T("ģ���м���ͨ��������·�ϼ�⵽һ�μ��Է�ת");
    case E_SYS_RemotePickup:return _T("ģ���м���ͨ������ǿ��Զ��ժ���������⵽�����û�ժ��");
    case E_CHG_FlashCount:	return _T("��ϯͨ�����ڵ绰���ϼ�⵽һ�����ϲ���");
    case E_CHG_HookState:	return _T("��ϯͨ���������ϼ�⵽ժ����һ�����");
    case E_CHG_ToneAnalyze:	return _T("�ź����������������������仯");
    case E_OverallEnergy:	return _T("�ź������������·�ϵ�ȫƵ���������仯");
    case E_CHG_OvrlEnrgLevel:return _T("�ź����������ȫƵ������ʶ����¼�");
    case E_CHG_BusyTone:	return _T("�ź�������������н������������⵽��æ�����ڵĸ��������仯");
    case E_CHG_BusyToneEx:	return _T("�ź�������������ñ�����æ������㷨��⵽��æ���ź�");
    case E_CHG_VocFxFlag:	return _T("�ź��������������Ƶ�ź����ĵ�ƽ�����仯��ͨ�����ڴ����ź����ļ��");
    case E_CHG_ToneValue:	return _T("�ź�����������ź����ĵ�ƽ�����仯");
    case E_CHG_RingEchoToneTime:	return _T("�ź������������������������ֵ�����仯");
    case E_CHG_PeakFrq:	    return _T("�ź������������ֵƵ�ʷ����仯");
    case E_SYS_BargeIn:	    return _T("Barge In�����������������仯");
    case E_SYS_NoSound:	    return _T("�ź������������·�ϱ��־�Ĭ");
    case E_SYS_TIMEOUT:	    return _T("ȫ�ֶ�ʱ�����ɺ���SsmStartTimer�����Ķ�ʱ���������");
    case E_CHG_SpyState:	return _T("DTPϵ�У���ص�·��״̬�����仯");
    case 0x0032:            return _T("����[0x0032]");
    case E_CHG_CICRxPhoNumBuf:	return _T("SS7���·���յ��µı��к���");
    case E_CHG_CICState:	return _T("SS7���·����·״̬�����仯");
    case E_PROC_CICAutoDial:return _T("SS7���·��ShgAutoDial�����н�չ");
    case E_RCV_Ss7IsupUtuinf: return _T("SS7���յ�USR��Ϣ�¼�");
    case E_CHG_Mtp2Status:	return _T("SS7������·��������·��״̬�����仯");
    case E_RCV_DSTDChannel:	return _T("DSTϵ�У�Dͨ���¼�");
    case E_RCV_Ss7SpyMsu:	return _T("SS7����SS7�������յ��µļ����Ϣ��MSU��");
    case E_CHG_ToneDetector:return _T("�ź�����������¼��ģʽ�������¼�");
    case E_CHG_ToneDetectorItem:return _T("�ź�����������¼��ģʽ�ź������ڼ����¼�");
    case E_PROC_FaxDcnTag:	return _T("����ͨ����������ճɹ�����ʱ���ж϶Է������Ƿ�ǿ��ֹͣ��");
    case E_CHG_AMD:	        return _T("�ź���������������Ƿ�������ժ��");
    case E_RCV_Ss7IsupCpg:	return _T("SS7���յ�CPG��Ϣ�¼�");
    //case E_CHG_CbChStatus:	return _T("��������ϯ����ذ忨ͨ�����������ϯģ������״̬�����ı仯");
    default:                return _T("δ֪");

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
    case ANALOGOUT_NO_DIALTONE:             return _T("ģ���м���ͨ��ȥ������ʧ�ܣ�δ��⵽������");
    case ANALOGOUT_BUSYTONE:                return _T("ģ���м���ͨ��ȥ������ʧ�ܣ���⵽æ��");
    case ANALOGOUT_ECHO_NOVOICE:            return _T("ģ���м���ͨ��ȥ�����У���⵽����������·�ϱ��־�Ĭ�����������޷��б𱻽��Ƿ�ժ��");
    case ANALOGOUT_NOANSWER:                return _T("ģ���м���ͨ��ȥ�����У���⵽�����ź󣬱����û�δ��������MaxWaitAutoDialAnswerTimeָ����ʱ����Ӧ��");
    case ANALOGOUT_TALKING_REMOTE_HANGUPED: return _T("ģ���м���ͨ��ͨ���ڡ����ӡ�״̬ʱ��⵽�Զ��û��һ�");
    case ANALOGOUT_NOVOICE:                 return _T("ģ���м���ͨ��ȥ�����У����Ź����Ѿ���ɣ�δ����·�ϼ�⵽���������κ������������źţ����������޷��жϱ����û��Ƿ�ժ��");
    case PEND_WaitBckStpMsg:                return _T("ISUP/TUP ISDN SS1�������У�����������Ԥ����պŹ�����ɱ��к������Ϣ�Ľ��պ󣬡��Զ�Ӧ���������С�����δ��������ҪӦ�ó������о����Ƿ���ܴ˺���");
    case SS1IN_BWD_KB5:                     return _T("SS1�������У��յ��Զ˽�������KD�źź�Ӧ�ó�������KB=5���պţ����ȴ����з��Ĳ����ź�");
    case PEND_RemoteHangupOnTalking:        return _T("ISUP/TUP SS1�������У�˫��ͨ��ʱ���Զ˹һ�");
    case PEND_AutoDialFailed:               return _T("ISUP/TUP ȥ������ʧ�ܡ�������Զ�����ʧ��ԭ�򣬿ɵ���SsmGetAutoDialFailureReason()���");
    case PEND_SsxUnusable:                  return _T("ISUP/TUP SS7�������");
    case PEND_CircuitReset:                 return _T("ISUP/TUP ������·��ԭ�¼�");
    case PEND_PcmSyncLos:                   return _T("ISUP/TUP SS1�����м��ߵĻ���֡��0ʱ϶��ͬ���źŶ�ʧ");
     //   SS1OUT_TALKING_REMOTE_HANGUPED
    case PEND_CalleeHangupOnTalking:        return _T("ISUP/TUP/SS1 ȥ�����У�˫��ͨ��ʱ���Զ˹һ�");
    case SS1OUT_NOANSWER:                   return _T("SS1ȥ������ʧ�ܣ������û�δ��������MaxWaitAutoDialAnswerTimeָ����ʱ����Ӧ��");
    case SS1OUT_NOBWDACK:                   return _T("SS1ȥ������ʧ�ܣ��ȴ��Զ˽������ġ�ռ��֤ʵ���źų�ʱ");
    case SS1OUT_DIALING_BWD_HANGUP:         return _T("SS1ȥ������ʧ�ܣ��Զ˽�����ȡ������");
    case SS1OUT_BWD_A5:                     return _T("SS1ȥ������ʧ�ܣ��յ��Զ˽�������A5�źţ��պţ�");
    case SS1OUT_BWD_KB5:                    return _T("SS1ȥ������ʧ�ܣ��յ��Զ˽�������KB=5���պţ�");
    case SS1OUT_BWD_KB2:                    return _T("SS1ȥ������ʧ�ܣ��յ��Զ˽�������KB=2�������û�����æ����");
    case SS1OUT_BWD_KB3:                    return _T("SS1ȥ������ʧ�ܣ��յ��Զ˽�������KB=3�������û�����æ����");
    case SS1OUT_BWD_A4:                     return _T("SS1ȥ������ʧ�ܣ��յ��Զ˽�������A4�źţ�����ӵ����");
    case SS1OUT_BWD_KB4:                    return _T("SS1ȥ������ʧ�ܣ��յ��Զ˽�������KB=4�źţ�����ӵ����");
    case SS1OUT_TIMEOUT_BWD_A:              return _T(" SS1ȥ������ʧ�ܣ��ȴ��Զ˽������ĺ���A���źų�ʱ");
    case SS1OUT_TIMEOUT_BWD_A_STOP:         return _T("SS1ȥ������ʧ�ܣ��ȴ��Զ˽�����ͣ������A���źų�ʱ");
    case SS1OUT_TIMEOUT_BWD_KB:             return _T("SS1ȥ������ʧ�ܣ��ȴ��Զ˽�������KB�źų�ʱ");
    case SS1OUT_TIMEOUT_BWD_KB_STOP:        return _T("SS1ȥ������ʧ�ܣ��ȴ��Զ˽�����ͣ��KB�źų�ʱ");
    case SS1OUT_TIMEOUT_CALLERID_BWD_A1:    return _T(" SS1ȥ������ʧ�ܣ���Զ˽������������к���ʱ���ȴ��Զ˽���������A���źų�ʱ��");
    case SS1OUT_TIMEOUT_CALLERID_BWD_A1_STOP:return _T("SS1ȥ������ʧ�ܣ���Զ˽������������к���ʱ���ȴ��Զ˽�����ͣ������A���źų�ʱ���Զ�����ʧ��");
    case SS1OUT_UNDEFINED_CALLERID_BWD_A:   return _T("SS1ȥ������ʧ�ܣ���Զ˽������������к���ʱ���յ�δ����ĺ���A���ź�");
    case SS1OUT_UNDEFINED_BWD_A:            return _T("ȥ������ʧ�ܣ��յ�δ����ĺ���A���ź�");
    case SS1OUT_UNDEFINED_BWD_KB:           return _T("ȥ������ʧ�ܣ��յ�δ�����KB�ź�");
    case ISDN_CALLOVER:                     return _T("ISDN���н������Է��ȹһ�");
    case ISDN_WAIT_RELEASE:                 return _T("ISDN�յ��Է��ġ����ߡ���Ϣ���ȴ��ͷű�����·");
    case ISDN_HANGING:                      return _T("ISDN�����ȹһ������ڲ���");
    case ISDN_RELEASING:                    return _T("ISDN�����ͷź���");
    case ISDN_UNALLOCATED_NUMBER:           return _T("ISDNδ����ĺ���");
    case ISDN_NETWORK_BUSY:                 return _T("ISDN����æ");
    case ISDN_CIRCUIT_NOT_AVAILABLE:        return _T("ISDNָ���ĵ�·������");
    case PEND_CalleeHangupOnWaitRemotePickUp:return _T("TUPȥ������ʧ�ܣ��ȴ������û�ժ��ʱ���յ��Զ˽������Ĳ�����Ϣ");
    case ISUP_HardCircuitBlock:             return _T("ISUP�յ��Զ˽�������Ӳ��������Ϣ");
    case ISUP_RemoteSuspend:                return _T("ISUPT6��ʱ��������й�T6��ʱ���ĸ�����Ϣ��μ���1���С�ISUPͨ����״̬ת�ơ��������ݡ�");
    case PEND_RcvHGBOrSGB:                  return _T("TUP�յ��Զ˽�����������Ϣ��SGB/HGB��");
    case ISDN_NO_ANSWER:                    return _T("ISDN��Ӧ��");
    case ISDN_CALL_REJ:                     return _T("ISDN���оܾ�");
    case 54://PEND_RemoteHangupOnRinging:
        return _T("ISUP/TUP �������У�ͨ�����ڡ����塱״̬ʱ���Զ˽�����ȡ���˺���");
    case 55://ISDN_NO_ROUTE:
        return _T("ISDN�޷�·�ɣ�ԭ�����Ϊ���ƶ��绰���ڷ�������");
    case 56://ISDN_NO_ROUTE_TO_DEST:
        return _T("ISDN�޷�·�ɣ�ԭ�����Ϊ���ƶ��绰�ػ���");
    case 57://EM_USER_BUSY:
        return _T("E/M�û�æ");
    case 58://EM_CH_ERROR:
        return _T("E/Mͨ������");
    case 59://EM_LOCAL_HANGUP:
        return _T("E/M�����ȹһ�");
    case 60://EM_LOCAL_NOANSWER:
        return _T("E/M������Ӧ��");
    case 61://EM_REMOTE_HANGUP:
        return _T("E/M�Զ��ȹһ�");
    case 62://EM_REMOTE_NOANSWER:
        return _T("E/M�Զ���Ӧ��");
    case 63://PEND_RemoteHangupOnSuspend:
        return _T("ISUP����ͣ��״̬ʱ���Զ��û��һ�");
    case 64://PEND_CalleeHangupOnSuspend:
        return _T("ISUP����ͣ��״̬ʱ�������û��һ�");
    case 65://ISDN_NORMAL_UNSPEC:
        return _T("ISDN������������");
    case -1:
        return _T("SsmGetPendingReason ���أ�1");
    default:
        return _T("δ֪");
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
	case DIAL_STANDBY:      return _T("ͨ�����У�û��ִ��AutoDial����");
	case DIAL_DIALING:      return _T("���ڷ��ͱ��к���");
	case DIAL_ECHOTONE:     return _T("���塣ģ���м���ͨ����������ɺ���·�ϼ�⵽�˻����� SS1ͨ��������ȥ������ʱ�����������յ��Զ˽������ĺ���KB=1��KB=6�źţ��������п��� TUP/ISUPͨ�����������������յ��Զ˽������ĵ�ַ����Ϣ(ACM)");
	case DIAL_NO_DIALTONE:  return _T("û������·�ϼ�⵽��������AutoDial����ʧ�ܡ�ֻ������ģ���м���ͨ��");
	case DIAL_BUSYTONE:     return _T("�����û�æ��AutoDial����ʧ�ܡ�����ģ���м���ͨ����������·�ϼ�⵽��æ���ź�");
	case DIAL_ECHO_NOVOICE: return _T("������ɺ���·�����ǳ����˻�������Ȼ�󱣳־�Ĭ��AutoDial������ֹ��ֻ������ģ���м���ͨ��");
	case DIAL_NOVOICE:      return _T("������ɺ���·��û�м�⵽��������һֱ���־�Ĭ��AutoDial������ֹ��ֻ������ģ���м���ͨ��");
	case DIAL_VOICE:        return _T("�����û�ժ����AutoDial�������");
	case DIAL_VOICEF1:      return _T("�����û�ժ������⵽F1Ƶ�ʵ�Ӧ���źţ���AutoDial������ɡ�ֻ������ģ���м���ͨ��");
	case DIAL_VOICEF2:      return _T("�����û�ժ������⵽F2Ƶ�ʵ�Ӧ���źţ���AutoDial������ɡ�ֻ������ģ���м���ͨ��");
	case DIAL_NOANSWER:     return _T("�����û���ָ��ʱ����û��ժ����AutoDialʧ��");
	case DIAL_FAILURE:      return _T("AutoDial����ʧ�ܡ�ʧ��ԭ�����ͨ������SsmGetAutoDialFailureReason���");
	case DIAL_INVALID_PHONUM:return _T("�����û�����Ϊ�պţ�AutoDial����ʧ��");
	default:                return _T("δ֪");
	}

	return _T("\0");
}

LPCTSTR CSsmInterface::RemoteChBlockToDescription(int ch,DWORD dwParam)
{
	switch(dwParam)
	{
	case 0:	return _T("�Ѿ��ɹ�����ɱ��˵��µı���");
	case 1: return _T("�Ѿ��ɹ������Զ�");
	case 2: return _T("���ڵȴ��Զ˽������ı���֤ʵ�ź�");
	case 3: return _T("���ڵȴ��Զ˽������Ľ������֤ʵ�ź�");
	default:return _T("δ֪");
	}
	return _T("\0");
}

LPCTSTR CSsmInterface::RemotePCMBlockToDescription(int ch,DWORD dwParam)
{
	DWORD blockstate=LOWORD(dwParam);
	DWORD blockmode=HIWORD(dwParam);

	switch(blockstate)
	{
		case 0: return _T("�Ѿ��ɹ�����ɱ��˵��µı���");
		case 1: return _T("�Ѿ��ɹ������Զ�");
		case 2: return _T("���ڵȴ��Զ˽������ı���֤ʵ�ź�");
		case 3: return _T("���ڵȴ��Զ˽������Ľ������֤ʵ�ź�");
		default:return _T("δ֪");
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
  case 0: return _T("�һ�");
  case 1: return _T("ժ��");
  }
  return _T("\0");
}

LPCTSTR CSsmInterface::SendFSKToDescription(int ch,DWORD dwParam)
{
    return _T("0-����δʹ��");
}

LPCTSTR CSsmInterface::RingCountToDescription(int ch,DWORD dwParam)
{
    static TCHAR ringcount[64];
    _sntprintf(ringcount,64,_T("�����ź����ڸ���:%d"),dwParam);
    return ringcount;
}

LPCTSTR CSsmInterface::ToneAnalyzeToDescription(int ch,DWORD dwParam)
{

    WORD high = HIWORD(dwParam);
    WORD low  = LOWORD(dwParam);


    switch(low)
    {
    case 1:    return _T("��⵽������");
    case 2:    return _T("��⵽æ��");
    case 3:    return _T("��⵽������");
    case 4:    return _T("��⵽����������·���־�Ĭ");
    case 5:    return _T("��⵽����");
    case 6:    return _T("��⵽˵�����������Զ�����ʱ��ⱻ���û��Ƿ�Ӧ��ժ����");
    case 7:    return _T("��⵽F1Ƶ�ʵ��ź����������Զ�����ʱ��ⱻ���û���Ӧ������");
    case 8:    return _T("��⵽F2Ƶ�ʵ��ź����������Զ�����ʱ��ⱻ���û���Ӧ������");
    case 9:    return _T("��⵽�û�ָ�����ź�������");
    default:   return _T("δ֪");
    }

    return _T("\0");
}



LPCTSTR CSsmInterface::AmdToDescription(int ch,DWORD dwParam)
{
    switch(dwParam)
    {
    case 0: return _T("��⵽����ժ��");
    case 1: return _T("��⵽�ź���");
    case 2: return _T("��⵽�������ʾ��");
    case 3: return _T("��ʱ");
    case 4: return _T("��⵽�ź�������ʾ������·����");
    case 5: return _T("��⵽���ź���·����");
    case 6: return _T("��⵽æ��");
    default:return _T("δ֪");
    }
    return _T("\0");
}

LPCTSTR CSsmInterface::SendDTMFToDescription(int ch,DWORD dwParam)
{
    switch(dwParam)
    {
        case 0: return _T("��ɻ�����ȫ��DTMF�ַ��ķ���");
        case 1: return _T("��������Ӧ�ó�����ֹ");
        default:return _T("δ֪");
    }

    return _T("\0");
}

LPCTSTR CSsmInterface::RecvDTMFToDescription(int ch,DWORD dwParam)
{
    //��16����Ϊ�յ���DTMF�ַ�����������16����Ϊ�����յ����ַ����൱����������SsmGetRxDtmfLen��SsmGetLastDtmf����
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
    case 1:     return _T("��ʱ����ʧ��");
    case 2:     return _T("�յ�ָ���Ľ��������ֽڶ�����");
    case 3:     return _T("�յ�ָ������FSK���ݶ�����");
    case 4:     return _T("�յ�ָ����ʽ�����ݶ�����");
    case 5:     return _T("Ӧ�ó�����ú���SsmStopRcvFSK�ж�");
    default:    return _T("δ֪");
    }

    return _T("\0");

}


LPCTSTR CSsmInterface::PlayEndToDescription(int ch,DWORD dwParam)
{
    switch(dwParam)
    {
    case 1: return _T("ȫ���������ݲ������");
    case 2: return _T("���յ�DTMF�ַ�����ֹ");
    case 3: return _T("���⵽Barge in ����ֹ");
    case 4: return _T("���⵽�Զ��û��Ĺһ���������ֹ");
    case 5: return _T("��Ӧ�ó�����ֹ");
    case 6: return _T("�ļ�����������ͣ");
    case 7: return _T("��������߶���ֹ");
    case 8: return _T("�ļ�����������������϶���ֹ");
    default:return _T("δ֪");
    }

    return _T("\0");

}

LPCTSTR CSsmInterface::PlayFileListToDescription(int ch,DWORD dwParam)
{
    static TCHAR description[64];
    _stprintf(description,_T("�ļ�����ֵ:%d"),dwParam);
    return description;
}

LPCTSTR CSsmInterface::PlayMemoDescription(int ch,DWORD dwParam)
{
    switch(dwParam)
    {
    case -1:
        return _T("��������ķ���ָ��Խ�����������м�λ��");
        break;
    case -2:
        return _T("��������ķ���ָ��Խ��������β���󣬷��ص�������ͷ��");
        break;
    default:
        {
            static TCHAR description[64];
            _stprintf(description,_T(" ��������ķ���ָ���ڻ������е�ƫ����:%d"),dwParam);
            return description;  
        }
        break;
    }

    return _T("\0");
}

LPCTSTR CSsmInterface::PlayFileToDescription(int ch,DWORD dwParam)
{
    static TCHAR description[64];
    _stprintf(description,_T("����ʱ��:%d"),dwParam);
    return description;
}

LPCTSTR CSsmInterface::RecordFileToDescription(int ch,DWORD dwParam)
{
    static TCHAR description[64];
    _stprintf(description,_T("¼��ʱ��:%d"),dwParam);
    return description;
}

LPCTSTR CSsmInterface::RecordEndToDescription(int ch,DWORD dwParam)
{
    switch(dwParam)
    {
    case 1: return _T("��Ӧ�ó�����ֹ");
    case 2: return _T("���⵽DTMF�ַ�����ֹ");
    case 3: return _T("���⵽�Զ��û��Ĺһ���������ֹ");
    case 4: return _T("��¼�Ƶ����ݵ���ָ�����Ȼ�ʱ�����ֹ");
    case 5: return _T("�ļ�¼������ͣ");
    case 6: return _T("��¼������д�뵽�ļ�ʧ��");
    default:return _T("δ֪");
    }

    return _T("\0");

}

LPCTSTR CSsmInterface::BargeInToDescription(int ch,DWORD dwParam)
{
    switch(dwParam)
    {
    case 0: return _T("Barge In��ʧ");
    case 1: return _T("��⵽BargeIn");
    default:return _T("\0");
    }
    return _T("\0");
}

LPCTSTR CSsmInterface::BusyToneToDescription(int ch,DWORD dwParam)
{
//    �ź����������æ���������
//    ��16���أ�ָʾ�ź�����������0��һ�飬1�ڶ���
//    ��16���أ�æ�����ڵĸ���

    static TCHAR description[128];
    _stprintf(description,_T("�飺:%hd æ�����ڵĸ�����%hd"),HIWORD(dwParam),LOWORD(dwParam));
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
    case -1:                        return _T("����ʧ��");
    case ATDL_NULL:                 return _T("û�к�������");
    case ATDL_Cancel:               return _T("AutoDial����Ӧ�ó���ȡ��");
    case ATDL_WaitDialAnsTimeout:   return _T("�ȴ�����Ӧ��ʱ");
    case ATDL_WaitRemotePickupTimeout:return _T("�ȴ�����ժ����ʱ");
    case ATDL_Mtp3Unusable:         return _T("SS7����������");
    case ATDL_RcvSSB:               return _T("SS7����յ��Զ˽�������SSB��Ϣ");
    case ATDL_RcvSLB:               return _T("SS7����յ��Զ˽�������SLB��Ϣ");
    case ATDL_RcvUNN:               return _T("SS7����յ��Զ˽�������UNN��Ϣ");
    case ATDL_RcvSEC:               return _T("SS7����յ��Զ˽�������SEC��Ϣ");
    case ATDL_RcvCGC:               return _T("SS7����յ��Զ˽�������CGC��Ϣ");
    case ATDL_RcvNNC:               return _T("SS7����յ��Զ˽�������NNC��Ϣ");
    case ATDL_RcvCFL:               return _T("SS7����յ��Զ˽�������CFL��Ϣ");
    case ATDL_RcvLOS:               return _T("SS7����յ��Զ˽�������LOS��Ϣ");
    case ATDL_RcvSST:               return _T("SS7����յ��Զ˽�������SST��Ϣ");
    case ATDL_RcvACB:               return _T("SS7����յ��Զ˽�������ACB��Ϣ");
    case ATDL_RcvDPN:               return _T("SS7����յ��Զ˽�������DPN��Ϣ");
    case ATDL_RcvEUM:               return _T("SS7����յ��Զ˽�������EUM��Ϣ");
    case ATDL_RcvADI:               return _T("SS7����յ��Զ˽�������ADI��Ϣ");
    case ATDL_RcvBLO:               return _T("SS7����յ��Զ˽�������BLO��Ϣ");
    case ATDL_DoubleOccupy:         return _T("SS7������ͬ��");
    case ATDL_CircuitReset:         return _T("SS7����յ��Զ˽������ĵ�·/Ⱥ��ԭ�ź�");
    case ATDL_BlockedByRemote:      return _T("SS7�����·���Զ˽���������");
    case ATDL_SS1WaitOccupyAckTimeout:return _T("No.1����ȴ�ռ��Ӧ���źų�ʱ");
    case ATDL_SS1RcvCAS_HANGUP:     return _T("No.1����յ���������ź�");
    case ATDL_SS1RcvA4:             return _T("No.1����յ�A4�źţ�����ӵ����");
    case ATDL_SS1RcvA5:             return _T("No.1����յ�A5�źţ��պţ�");
    case ATDL_SS1RcvUndefinedAx:    return _T("No.1����յ�δ�������A�ź�");
    case ATDL_SS1RcvUndefinedAxOnTxCallerId:return _T("No.1����������յ�δ����A");
    case ATDL_SS1WaitAxTimeout:     return _T("No.1����Ⱥ���պ���A���źų�ʱ");
    case ATDL_SS1WaitAxStopTimeout: return _T("No.1����Ⱥ����A���ź�ͣ����ʱ");
    case ATDL_SS1WaitAxTimeoutOnTxCallerId:return _T("No.1���������ʱ�Ⱥ�A�źų�ʱ");
    case ATDL_SS1WaitAxStopTimeoutOnTxCallerId:return _T("No.1���������A�ź�ͣ����ʱ");
    case ATDL_SS1RcvKB2:            return _T("No.1����յ�KB2�ź�(�û�����æ��)");
    case ATDL_SS1RcvKB3:            return _T("No.1����յ�KB3�ź�(�û�����æ��)");
    case ATDL_SS1RcvKB4:            return _T("No.1����յ�KB4�źţ�����ӵ����");
    case ATDL_SS1RcvKB5:            return _T("No.1����յ�KB5�źţ��պţ�");
    case ATDL_SS1RcvUndefinedKB:    return _T("No.1����յ�δ�����KB�ź�");
    case ATDL_SS1WaitKBTimeout:     return _T("No.1������պ���KB�źų�ʱ");
    case ATDL_SS1WaitKBStopTimeout: return _T("No.1����Ⱥ�Է���ͣ��KB�źų�ʱ");
    case ATDL_ISDNNETISBUS:         return _T("ISDN������æ");
    case ATDL_ISDNEMPTYNO:          return _T("ISDN���պ�");
    case ATDL_IllegalMessage:       return _T("SS7����յ��Զ˽������ķǷ���Ϣ");
    case ATDL_RcvREL:               return _T("ISUP���յ��Զ˽��������ͷ���Ϣ��REL��");
    case ATDL_RcvCBK:               return _T("TUP���յ��Զ˽�������CBK��Ϣ");
    case ATDL_IPInvalidPhonum:      return _T("IP������������Ч");
    case ATDL_IPRemoteBusy:         return _T("IP���Զ�æ");
    case ATDL_IPBeenRefused:        return _T("IP�����ܾ�");
    case ATDL_IPDnsFail:            return _T("IP��DNS��Ч");
    case ATDL_IPCodecUnSupport:     return _T("IP����֧�ֵ�CODEC����");
    case ATDL_IPOutOfResources:     return _T("IP��û�п��õ���Դ");
    case ATDL_IPLocalNetworkErr:    return _T("IP������������ִ���");
    case ATDL_IPRemoteNetworkErr:   return _T("IP��Զ��������ִ���");
    default:                        return _T("����");
    }

    return _T("\0");
}


LPCTSTR CSsmInterface::Mtp2StatusToDescription(int ch,DWORD dwParam)
{
	//SS7������·״̬
	switch(dwParam)
	{
	case 1:			return _T("ҵ���ж�");
	case 2:			return _T("��ʼ��λ");
	case 3:			return _T("�Ѷ�λ/׼����");
	case 4:			return _T("�Ѷ�λ/δ׼����");
	case 5:			return _T("ҵ��ͨ");
	case 6:			return _T("���������");
	default:		return _T("δ֪");
	}
	return _T("\0");
}

LPCTSTR CSsmInterface::OvrlEnrgLevelToDescription(int ch,DWORD dwParam)
{
//�ź���������ȫƵ������ʶֵ����16���أ�
//Bit15 (�ź�����־λ)��0/1       ��/���ź���
//Bit14 (��������״̬λ)��0       ����
//Bit13-0 (����ʱ��)����/���ź����ĳ���ʱ��
    static TCHAR buf[256];
    _sntprintf(buf,256,_T("�ź�����־λ:%d ����ʱ��:%d"),((WORD)dwParam)>>15,dwParam&0x3FFF);
    return buf;
}

LPCTSTR CSsmInterface::OverallEnergyToDescription(int ch,DWORD dwParam)
{
    //�ź���������ȫƵ����ֵ
    static TCHAR buf[256];
    _sntprintf(buf,256,_T("ȫƵ����ֵ:%d"),dwParam);
    return buf;
}


LPCTSTR CSsmInterface::PeakFrqToDescription(int ch,DWORD dwParam)
{
    //�ź�������������ֵƵ��
    static TCHAR buf[256];
    _sntprintf(buf,256,_T("��ֵƵ��:%d"),dwParam);
    return buf;
}




void CSsmInterface::ShowWarn(LPCTSTR Text)
{
    ::MessageBox(NULL,Text,_T("CTISSM����"),MB_ICONEXCLAMATION);
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

        //7sû������[���йһ�]
        pch->recvcid.remoteblocktimer=CMessageHandler::DelayMessage(7000,TRUE,EVT_SIMULATETIMER,WrapMessageData<Simulatetag>(simulate));
    }
}




RELEASEATTRIBUTE CSsmInterface::DialFailedToReleaseAttribute(int Failedreason)
{
	switch(Failedreason)
	{
	case ATDL_NULL:                 //_T("û�к�������");
	case ATDL_Cancel:               //_T("AutoDial����Ӧ�ó���ȡ��");
			return RELEASE_NONE;
		break;
	case ATDL_WaitDialAnsTimeout:   //_T("�ȴ�����Ӧ��ʱ");
	case ATDL_WaitRemotePickupTimeout://_T("�ȴ�����ժ����ʱ");
			return RELEASE_NOANSWER;
		break;
	case ATDL_Mtp3Unusable:         //_T("SS7����������");
	case ATDL_RcvSSB:               //_T("SS7����յ��Զ˽�������SSB��Ϣ");
	case ATDL_RcvSLB:               //_T("SS7����յ��Զ˽�������SLB��Ϣ");
	case ATDL_RcvUNN:               //_T("SS7����յ��Զ˽�������UNN��Ϣ");
	case ATDL_RcvSEC:               //_T("SS7����յ��Զ˽�������SEC��Ϣ");
	case ATDL_RcvCGC:               //_T("SS7����յ��Զ˽�������CGC��Ϣ");
	case ATDL_RcvNNC:               //_T("SS7����յ��Զ˽�������NNC��Ϣ");
	case ATDL_RcvCFL:               //_T("SS7����յ��Զ˽�������CFL��Ϣ");
	case ATDL_RcvLOS:               //_T("SS7����յ��Զ˽�������LOS��Ϣ");
	case ATDL_RcvSST:               //_T("SS7����յ��Զ˽�������SST��Ϣ");
	case ATDL_RcvACB:               //_T("SS7����յ��Զ˽�������ACB��Ϣ");
	case ATDL_RcvDPN:               //_T("SS7����յ��Զ˽�������DPN��Ϣ");
	case ATDL_RcvEUM:               //_T("SS7����յ��Զ˽�������EUM��Ϣ");
	case ATDL_RcvADI:               //_T("SS7����յ��Զ˽�������ADI��Ϣ");
	case ATDL_RcvBLO:               //_T("SS7����յ��Զ˽�������BLO��Ϣ");
	case ATDL_DoubleOccupy:         //_T("SS7������ͬ��");
	case ATDL_CircuitReset:         //_T("SS7����յ��Զ˽������ĵ�·/Ⱥ��ԭ�ź�");
	case ATDL_BlockedByRemote:      //_T("SS7�����·���Զ˽���������");
	case ATDL_SS1WaitOccupyAckTimeout://_T("No.1����ȴ�ռ��Ӧ���źų�ʱ");
	case ATDL_SS1RcvCAS_HANGUP:     //_T("No.1����յ���������ź�");
	case ATDL_SS1RcvA4:             //_T("No.1����յ�A4�źţ�����ӵ����");
			return RELEASE_LINEFAULT;
		break;
	case ATDL_SS1RcvA5:             //_T("No.1����յ�A5�źţ��պţ�");
			return RELEASE_VACANT;
		break;
	case ATDL_SS1RcvUndefinedAx:    //_T("No.1����յ�δ�������A�ź�");
	case ATDL_SS1RcvUndefinedAxOnTxCallerId://_T("No.1����������յ�δ����A");
	case ATDL_SS1WaitAxTimeout:     //_T("No.1����Ⱥ���պ���A���źų�ʱ");
	case ATDL_SS1WaitAxStopTimeout: //_T("No.1����Ⱥ����A���ź�ͣ����ʱ");
	case ATDL_SS1WaitAxTimeoutOnTxCallerId://_T("No.1���������ʱ�Ⱥ�A�źų�ʱ");
	case ATDL_SS1WaitAxStopTimeoutOnTxCallerId://_T("No.1���������A�ź�ͣ����ʱ");
			return RELEASE_LINEFAULT;
		break;
	case ATDL_SS1RcvKB2:            //_T("No.1����յ�KB2�ź�(�û�����æ��)");
	case ATDL_SS1RcvKB3:            //_T("No.1����յ�KB3�ź�(�û�����æ��)");
			return RELEASE_USERBUSY;
		break;
	case ATDL_SS1RcvKB4:            //_T("No.1����յ�KB4�źţ�����ӵ����");
			return RELEASE_LINEFAULT;
		break;
	case ATDL_SS1RcvKB5:            //_T("No.1����յ�KB5�źţ��պţ�");
			return RELEASE_VACANT;
		break;
	case ATDL_SS1RcvUndefinedKB:    //_T("No.1����յ�δ�����KB�ź�");
	case ATDL_SS1WaitKBTimeout:     //_T("No.1������պ���KB�źų�ʱ");
	case ATDL_SS1WaitKBStopTimeout: //_T("No.1����Ⱥ�Է���ͣ��KB�źų�ʱ");
	case ATDL_ISDNNETISBUS:         //_T("ISDN������æ");
			return RELEASE_LINEFAULT;
		break;
	case ATDL_ISDNEMPTYNO:          //_T("ISDN���պ�");
			return RELEASE_VACANT;
		break;
	case ATDL_IllegalMessage:       //_T("SS7����յ��Զ˽������ķǷ���Ϣ");
	case ATDL_RcvREL:               //_T("ISUP���յ��Զ˽��������ͷ���Ϣ��REL��");
	case ATDL_RcvCBK:               //_T("TUP���յ��Զ˽�������CBK��Ϣ");
			return RELEASE_REJECTED;
		break;
	case ATDL_IPInvalidPhonum:      //_T("IP������������Ч");
			return RELEASE_VACANT;
		break;
	case ATDL_IPRemoteBusy:         //_T("IP���Զ�æ");
			return RELEASE_USERBUSY;
		break;
	case ATDL_IPBeenRefused:        //_T("IP�����ܾ�");
			return RELEASE_REJECTED;
		break;
	case ATDL_IPDnsFail:            //_T("IP��DNS��Ч");
	case ATDL_IPCodecUnSupport:     //_T("IP����֧�ֵ�CODEC����");
	case ATDL_IPOutOfResources:     //_T("IP��û�п��õ���Դ");
	case ATDL_IPLocalNetworkErr:    //_T("IP������������ִ���");
	case ATDL_IPRemoteNetworkErr:   //_T("IP��Զ��������ִ���");
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

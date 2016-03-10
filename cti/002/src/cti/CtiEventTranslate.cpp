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
	created:	2009/01/06
	created:	6:1:2009   16:13
	filename: 	e:\xugood.cti\src\CtiComm\TranslateEvent.cpp
	file path:	e:\xugood.cti\src\CtiComm
	file base:	TranslateEvent
	file ext:	cpp
	author:		xugood@live.cn
	
	purpose:	
*********************************************************************/
#include "stdafx.h"

#include "CtiEventTranslate.h"
#include "./Log/LogServer.h"




#define PrintLog(...)      TRACE_TEXT("TranslateEvent",LEVEL_SENSITIVE)<<__VA_ARGS__


CtiEventTranslate::CtiEventTranslate()
{
    LOG_INIT("TranslateEvent",TRUE,LEVEL_SENSITIVE,1024*1024);
}



void  CtiEventTranslate::OutNormal(const LPCHANNELEVENT lpchevent,std::ostringstream& outstream)
{
    if(lpchevent->eventid>=EVENT_USEREVENT)
    {
        outstream<<"通道:"<<lpchevent->chid<<"    "<<"EventID:"<<lpchevent->eventid<<"    "<<"EventName:"<<EventDescription(lpchevent->eventid)<<"    "<<"time:"<<lpchevent->time;
    }
    else
    {
        switch(lpchevent->eventid)
        {
        case EVENT_SYSEVENT:
            outstream<<"标识:"<<lpchevent->chid<<"    "<<"EventID:"<<lpchevent->eventid<<"    "<<"EventName:"<<EventDescription(lpchevent->eventid)<<"    "<<"time:"<<lpchevent->time;
            break;
        default:
            outstream<<"通道:"<<lpchevent->chid<<"    "<<"EventID:"<<lpchevent->eventid<<"    "<<"EventName:"<<EventDescription(lpchevent->eventid)<<"    "<<"time:"<<lpchevent->time;
        }
    }


}







void CtiEventTranslate::Translate(const LPCHANNELEVENT lpchevent)
{

	std::ostringstream outstream;
	
    OutNormal(lpchevent,outstream);

    if(lpchevent->eventid>EVENT_USEREVENT)
    {
        LPUSEREVENTPARAM lpusereventparam=(LPUSEREVENTPARAM)lpchevent->eventdata;
        _ASSERT(sizeof(USEREVENTPARAM)==lpchevent->eventsize);
        outstream<<"    EventParam    "<<"eventdata:"<<lpusereventparam->eventdata<<" eventsize:"<<lpusereventparam->eventsize<<" delayid:"<<lpusereventparam->delayid;

    }
    else
    {
        switch(lpchevent->eventid)
        {




        case EVENT_DIALUP:
        case EVENT_CALLIN:
            {

                LPCALLERINFO pcallid=(LPCALLERINFO)lpchevent->eventdata;
                _ASSERT(sizeof(CALLERINFO)==lpchevent->eventsize);
                outstream<<"    EventParam    "<<"Calling:"<<pcallid->calling<<" Called:"<<pcallid->called<<" OrigCalled:"<<pcallid->origcalled;
            }
            break;


        case EVENT_RINGING:
            {
                LPRINGINGPARAM pringing=(LPRINGINGPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(RINGINGPARAM)==lpchevent->eventsize);
            }
            break;

        case EVENT_HANGUP:
            {
                LPHANGUPPARAM phang=(LPHANGUPPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(HANGUPPARAM)==lpchevent->eventsize);

                outstream<<"    EventParam    "<<"Hang:"<<ReleaseAttributeDescription(phang->reason);

            }
            break;

        case EVENT_RELEASE:
            {
                LPRELEASEPARAM lpreleaseparam=(LPRELEASEPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(RELEASEPARAM)==lpchevent->eventsize);

            }
            break;
        case EVENT_IDLE:
            {
                LPIDLEPARAM lpidleparam=(LPIDLEPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(IDLEPARAM)==lpchevent->eventsize);

            }
            break;
        case EVENT_RINGBACK:
            {
                LPRINGBACKPARAM lpringbackaram=(LPRINGBACKPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(RINGBACKPARAM)==lpchevent->eventsize);

            }
            break;
        case EVENT_TALKING:
            {
                LPTALKINGPARAM lptalkingparam=(LPTALKINGPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(TALKINGPARAM)==lpchevent->eventsize);


            }
            break;


        case EVENT_SENDFLASH:
            {
                LPSENDFLASHPARAM lpsendflashparam=(LPSENDFLASHPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(SENDFLASHPARAM)==lpchevent->eventsize);
            }
            break;
        case EVENT_RECVFLASH:
            {
                LPRECVFLASHPARAM lprecvflashparam=(LPRECVFLASHPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(RECVFLASHPARAM)==lpchevent->eventsize);

            }
            break;

        case EVENT_SENDTONE:
            {
                LPSENDTONEPARAM psendtone=(LPSENDTONEPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(SENDTONEPARAM)==lpchevent->eventsize);
                outstream<<"    EventParam    "<<"reuslt:"<<psendtone->result<<" Tone:"<<psendtone->tone;

            }
            break;

        case EVENT_RECVTONE:
            {
                LPRECVTONEPARAM precvtone=(LPRECVTONEPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(RECVTONEPARAM)==lpchevent->eventsize);
                outstream<<"    EventParam    "<<"reuslt:"<<precvtone->result<<" Tone:"<<precvtone->tone;

            }
            break;

        case EVENT_ADDCHANNEL:
            {
                LPADDCHANNELPARAM paddchannel=(LPADDCHANNELPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(ADDCHANNELPARAM)==lpchevent->eventsize);
                outstream<<"    EventParam    "<<"Type:"<<TypeDescription((CHANNELTYPE)paddchannel->chtype)<<" State:"<<StateDescription((CHANNELSTATE)paddchannel->chstate)<<" Way:"<<WayDescription((CHANNELWAY)paddchannel->chway);
            }
            break;


        case EVENT_DELCHANNEL:
            {
                LPDELCHANNELPARAM lpdelchannelparam=(LPDELCHANNELPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(DELCHANNELPARAM)==lpchevent->eventsize);

            }
            break;



        case EVENT_RECVDTMF:
            {
                LPRECVDTMFPARAM precvdtmf=(LPRECVDTMFPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(RECVDTMFPARAM)==lpchevent->eventsize);
                outstream<<"    EventParam    "<<"reuslt:"<<precvdtmf->result;
                if(precvdtmf->dtmf)
                    outstream<<" DTMF:"<<(char)precvdtmf->dtmf;
            }
            break;
        case EVENT_SENDDTMF:
            {
                LPSENDDTMFPARAM psenddtmf=(LPSENDDTMFPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(SENDDTMFPARAM)==lpchevent->eventsize);
                outstream<<"    EventParam    "<<"reuslt:"<<psenddtmf->result;
            }
            break;
        case EVENT_RECVFSK:
            {
                LPRECVFSKPARAM precvfsk=(LPRECVFSKPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(RECVFSKPARAM)==lpchevent->eventsize);
                outstream<<"    EventParam    "<<"reuslt:"<<precvfsk->result<<"    "<<"FSK:\n"<<HexFormat((const char*)&precvfsk->buf,precvfsk->size,16);

            }
            break;
        case EVENT_SENDFSK:
            {
                LPSENDFSKPARAM psenddtmf=(LPSENDFSKPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(SENDFSKPARAM)==lpchevent->eventsize);
                outstream<<"    EventParam    "<<"reuslt:"<<psenddtmf->result;


            }
            break;

        case EVENT_PLAY:
            {
                LPPLAYPARAM pplay=(LPPLAYPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(PLAYPARAM)==lpchevent->eventsize);

                outstream<<"    EventParam    "<<"result:"<<pplay->result<<" attribute:"<<pplay->attribute;
                if(pplay->dtmf)
                    outstream<<" DTMF:"<<(char)pplay->dtmf;



            }
            break;

        case EVENT_RECORD:
            {
                LPRECORDPARAM precord=(LPRECORDPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(RECORDPARAM)==lpchevent->eventsize);
                outstream<<"    EventParam    "<<"result:"<<precord->result<<" attribute:"<<precord->attribute;
                if(precord->dtmf)
                    outstream<<" DTMF:"<<(char)precord->dtmf;

            }
            break;

        case EVENT_USEREVENT:
            {
                LPUSEREVENTPARAM lpusereventparam=(LPUSEREVENTPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(USEREVENTPARAM)==lpchevent->eventsize);
                outstream<<"    EventParam    "<<"eventdata:"<<lpusereventparam->eventdata<<" eventsize:"<<lpusereventparam->eventsize<<" delayid:"<<lpusereventparam->delayid;

            }
            break;
        case EVENT_SYSEVENT:
            {
                LPSYSEVENTPARAM lpsyseventparam=(LPSYSEVENTPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(SYSEVENTPARAM)==lpchevent->eventsize);
                outstream<<"    EventParam    "<<"eventdata:"<<lpsyseventparam->eventdata<<" eventsize:"<<lpsyseventparam->eventsize;

            }
            break;

        case EVENT_UNUSABLE:
            {
                LPUNUSABLEPARAM lpunusableparam=(LPUNUSABLEPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(UNUSABLEPARAM)==lpchevent->eventsize);

            }
            break;
        case EVENT_STATE:
            {
                LPSTATEPARAM lpstateparam=(LPSTATEPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(STATEPARAM)==lpchevent->eventsize);
                outstream<<"    EventParam    "<<"statetext:"<<lpstateparam->statetext;

            }
            break;

        case EVENT_REMOTE_BLOCK:
            {
                LPREMOTEBLOCKPARAM lpremoteblockparam=(LPREMOTEBLOCKPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(REMOTEBLOCKPARAM)==lpchevent->eventsize);

            }
            break;

        case EVENT_LOCAL_BLOCK:
            {
                LPLOCALBLOCKPARAM lplocalblockparam=(LPLOCALBLOCKPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(LOCALBLOCKPARAM)==lpchevent->eventsize);

            }
            break;


        case STATE_SLEEP:
            {
                LPSLEEPPARAM lpsleepparam=(LPSLEEPPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(SLEEPPARAM)==lpchevent->eventsize);

            }
            break;

        case EVENT_UPDATEPLAYMEMORY:
            {
                LPUPDATEPLAYMEMORYPARAM lpupdateplaymem=(LPUPDATEPLAYMEMORYPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(UPDATEPLAYMEMORYPARAM)==lpchevent->eventsize);
                outstream<<"    EventParam    "<<"Index:"<<lpupdateplaymem->index;
            }
            break;
        case EVENT_UPDATERECORDMEMORY:
            {
                LPUPDATERECORDMEMORYPARAM lpupdaterecordmem=(LPUPDATERECORDMEMORYPARAM)lpchevent->eventdata;
                _ASSERT(sizeof(UPDATERECORDMEMORYPARAM)==lpchevent->eventsize);
                outstream<<"    EventParam    "<<"Index:"<<lpupdaterecordmem->index;
            }
            break;
        default:
            break;

        }


    }

    PrintLog(outstream.str()<<LOG_ENDL);
};



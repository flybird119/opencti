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
	filename: 	e:\project\opencti.code\trunk\002\src\cti\CtiVerify.cpp
	file path:	e:\project\opencti.code\trunk\002\src\cti
	file base:	CtiVerify
	file ext:	cpp
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/


#include "StdAfx.h"
#include "CtiVerify.h"

CtiVerify::CtiVerify(ChannelMgr& channelmgr,CLastError& lasterror)
:m_channelmgr(channelmgr)
,m_lasterror(lasterror)
{
}

CtiVerify::~CtiVerify(void)
{
}

ChannelInfo* CtiVerify::Verify(int ch,FUNCTIONADDRINDEX addrindex,int& errcode)
{
    ChannelInfo*pChannel = m_channelmgr.FindChannel(ch);
    if(pChannel)
    {
        switch(addrindex)
        {
        case Addr_None:
        case Addr_Init:
        case Addr_Term:
            break;
		case Addr_GetChState:
        case Addr_GetChType:
            break;
        case Addr_SetChOwnerData:
        case Addr_GetChOwnerData:
            break;
        case Addr_GetChTotal:
            break;

        case Addr_Listen:
        case Addr_DelayChEvent:
		case Addr_InsertChEvent:
            {
                if(!pChannel->IsWorkStates())
                {
                    errcode=ERRCODE_STATEACCORD;
                }
            }
            break;

        case Addr_SearchIdleCh:
        case Addr_LoadPlayIndex:
        case Addr_Features:
        case Addr_GetNativeAPI:
        case Addr_GetLastErrorCode:
        case Addr_FormatErrorCode:
            break;
        case Addr_Ringback:
            {
                if(pChannel->GetState()!=STATE_CALLIN)
                {
                    errcode=ERRCODE_STATEACCORD;
                }
            }
            break;
        case Addr_Pickup:
            {
                if(!pChannel->IsTrunkCh())
                {
                    errcode=ERRCODE_DEVICENONSUPPORT;
                }
                else
                {
                    if(pChannel->GetState()!=STATE_RINGING)
                    {
                        errcode=ERRCODE_STATEACCORD;
                    }
                }
            }
            break;
        case Addr_Dialup:
            {
                if(pChannel->IsTrunkCh())
                {
                    CHANNELSTATE chstate=pChannel->GetState();
                    if(chstate!=STATE_IDLE && chstate!=STATE_LOCAL_BLOCK)
                    {
                        errcode=ERRCODE_STATEACCORD;
                    }
                }
                else
                {
                    errcode=ERRCODE_DEVICENONSUPPORT;
                }
            }
            break;
        case Addr_Hangup:
            {
                if(!pChannel->IsTrunkCh())
                {
                    errcode=ERRCODE_DEVICENONSUPPORT;
                }
                else
                {
                    CHANNELTYPE     chtype=pChannel->GetType();
                    CHANNELSTATE    chstate=pChannel->GetState();
                    if(chtype==TYPE_ANALOG_USER)
                    {
                        if(chstate!=STATE_DIALUP && chstate!=STATE_RINGBACK)
                        {
                            errcode=ERRCODE_STATEACCORD;
                        }
                    }
                    else if(chtype==TYPE_ANALOG_TRUNK)
                    {
                        if(chstate!=STATE_DIALUP && chstate!=STATE_RINGBACK && chstate!=STATE_TALKING)
                        {
                            errcode=ERRCODE_STATEACCORD;
                        }
                    }
					else if(pChannel->IsDigitalTrunkCh())
					{
                        if(chstate!=STATE_DIALUP && chstate!=STATE_RINGBACK && chstate!=STATE_TALKING && chstate!=STATE_CALLIN && chstate!=STATE_RINGING)
                        {
                            errcode=ERRCODE_STATEACCORD;
                        }
					}
					else
					{
						errcode=ERRCODE_DEVICENONSUPPORT;
					}
                }
            }
            break;


        case Addr_SendDtmf:
        case Addr_SendFsk:
        case Addr_SendFskFull:
        case Addr_SendTone:
        case Addr_SendToneEx:
        case Addr_PlayFile:
        case Addr_PlayMemory:
        case Addr_PlayQueue:
        case Addr_PlayIndex:
        case Addr_RecvFsk:
        case Addr_RecvDtmf:
        case Addr_RecvTone:
        case Addr_RecordFile:
        case Addr_RecordMemory:
            {
                if(pChannel->IsTrunkCh())
                {
                    CHANNELSTATE    chstate=pChannel->GetState();
                    if(chstate!=STATE_RINGBACK && chstate!=STATE_TALKING && chstate!=STATE_RINGING)
                    {
                        errcode=ERRCODE_STATEACCORD;
                    }
                    else
                    {

                        bool same = false;
                        switch(addrindex)
                        {
                        case Addr_SendDtmf:
                                same=pChannel->IsSendDTMF();
                            break;
                        case Addr_RecvDtmf:
                                same=pChannel->IsRecvDTMF();
                            break;
                        case Addr_SendFsk:
                        case Addr_SendFskFull:
                                same=pChannel->IsSendFSK();
                            break;
                        case Addr_RecvFsk:
                                same=pChannel->IsRecvFSK();
                            break;
                        case Addr_SendTone:
                        case Addr_SendToneEx:
                                same=pChannel->IsSendTone(TONE_NONE);
                            break;
                        case Addr_RecvTone:
                            break;
                        case Addr_PlayFile:
                        case Addr_PlayMemory:
                        case Addr_PlayQueue:
                        case Addr_PlayIndex:
                                same=pChannel->IsPlayVoice(PLAY_NONE);
                            break;
                        case Addr_RecordFile:
                        case Addr_RecordMemory:
                                same=pChannel->IsRecordVoice(RECORD_NONE);
                            break;
                        }
                        if(same)
                        {
                            errcode=ERRCODE_PREVIOUSNOTFINISH;
                        }
                        else
                        {
                            bool confict = false ;
                            switch(addrindex)
                            {
                            case Addr_SendFskFull:
                            case Addr_SendFsk:
                                    confict=pChannel->IsRecvFSK() || pChannel->IsSound();
                                break;
                            case Addr_RecvFsk:
                                    confict=pChannel->IsSendFSK() || pChannel->IsSound();
                                break;
                            case Addr_SendDtmf:
                            case Addr_SendTone:
                            case Addr_SendToneEx:
                            case Addr_PlayFile:
                            case Addr_PlayMemory:
                            case Addr_PlayQueue:
                            case Addr_PlayIndex:
                                    confict=pChannel->IsSound();
                                break;
                            }


                            if(confict)
                            {
                                errcode=ERRCODE_OPERATIONALCONFICT;
                            }
                        }
                    }
                }
                else
                {
                    errcode=ERRCODE_DEVICENONSUPPORT;
                }
            }
            break;

        case Addr_StopSendDtmf:
        case Addr_StopRecvDtmf:
        case Addr_StopSendFsk:
        case Addr_StopRecvFsk:
        case Addr_StopSendTone:
        case Addr_StopRecvTone:
        case Addr_StopPlay:
        case Addr_StopRecord:
        case Addr_UpDatePlayMemory:
        case Addr_UpDateRecordMemory:
            {
                bool same = false;
                switch(addrindex)
                {
                case Addr_StopSendDtmf:
                    same=pChannel->IsSendDTMF();
                    break;
                case Addr_StopRecvDtmf:
                    same=pChannel->IsRecvDTMF();
                    break;
                case Addr_StopSendFsk:
                    same=pChannel->IsSendFSK();
                    break;
                case Addr_StopRecvFsk:
                    same=pChannel->IsRecvFSK();
                    break;
                case Addr_StopSendTone:
                    same=pChannel->IsSendTone(TONE_NONE);
                    break;
                case Addr_StopRecvTone:
                    break;
                case Addr_StopPlay:
                    same=pChannel->IsPlayVoice(PLAY_NONE);
                    break;
                case Addr_UpDatePlayMemory:
                    same=pChannel->IsPlayVoice(PLAY_MEMORY);
                    break;
                case Addr_StopRecord:
                    same=pChannel->IsRecordVoice(RECORD_NONE);
                    break;
                case Addr_UpDateRecordMemory:
                    same=pChannel->IsRecordVoice(RECORD_MEMORY);
                    break;

                }
                if(!same)
                {
                    errcode=ERRCODE_NOIMPLEMENTEDOPERATE;
                }
            }
            break;

        case Addr_IsSendDtmf:
        case Addr_IsRecvDtmf:
        case Addr_IsSendFsk:
        case Addr_IsRecvFsk:
        case Addr_IsSendTone:
        case Addr_IsRecvTone:
        case Addr_IsPlay:
        case Addr_IsRecord:
            break;

        case Addr_SendFlash:
        case Addr_RecvFlash:
        case Addr_StopRecvFlash:
        case Addr_IsRecvFlash:
            break;


        case Addr_UnListen:
        case Addr_DelaySysEvent:
        case Addr_GetSelectCh:
            break;


        case Addr_UnBindExclusiveThread:
        case Addr_BindExclusiveThread:

            break;

        }


    }
    else
    {
        errcode=ERRCODE_CHNOTEXIST;
    }



    return pChannel;
}



ChannelInfo* CtiVerify::VerifyCh(int ch,FUNCTIONADDRINDEX funaddrindex)
{
    int errorcode=ERRCODE_NOERROR;
    ChannelInfo*pChannel = Verify(ch,funaddrindex,errorcode);
    if(!pChannel || errorcode!=ERRCODE_NOERROR)
    {
        m_lasterror.SetLastErrorCode(errorcode);
        return NULL;
    }
    else
    {
        m_lasterror.SetLastErrorCode(ERRCODE_NOERROR);
    }

    return pChannel;
}
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
	filename: 	e:\project\opencti.code\trunk\002\src\cti\CtiNotify.h
	file path:	e:\project\opencti.code\trunk\002\src\cti
	file base:	CtiNotify
	file ext:	h
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/

#pragma once



#include "CtiChannel.h"


class CtiNotify
{
public:
    CtiNotify(void);
    virtual ~CtiNotify(void);

protected:
    virtual void  SendNotify(int ch,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize)=0;

   
    void  NotifyAddChannel(ChannelInfo* pch,CHANNELTYPE chtype,CHANNELSTATE chstate,CHANNELWAY chway,int chtotal);
    void  NotifyHangup(ChannelInfo* pch,RELEASEATTRIBUTE reason);
    void  NotifyRelease(ChannelInfo* pch);
    void  NotifyIdle(ChannelInfo* pch);
    void  NotifyCallin(ChannelInfo* pch,LPCTSTR calling,LPCTSTR called,LPCTSTR origcalled);
    void  NotifyRinging(ChannelInfo* pch);
    void  NotifyTalking(ChannelInfo* pch);
    void  NotifyRemoteBlock(ChannelInfo* pch);
    void  NotifyRingback(ChannelInfo* pch);
	void  NotifyUnusable(ChannelInfo* pch);
    void  NotifyDialup(ChannelInfo* pch,LPCTSTR calling,LPCTSTR called,LPCTSTR origcalled);
    void  NotifyLocalBlock(ChannelInfo* pch);
    void  NotifySleep(ChannelInfo* pch);

    void  NotifySendTone(ChannelInfo* pch,int result,TONEATTRIBUTE tone);
    void  NotifyRecvTone(ChannelInfo* pch,int result,TONEATTRIBUTE tone);


    void  NotifySendDtmf(ChannelInfo* pch,int result);
    void  NotifyRecvDtmf(ChannelInfo* pch,int result,TCHAR dtmf);



    void  NotifyPlay(ChannelInfo* pch,int result,PLAYATTRIBUTE attribute,TCHAR dtmf);
    void  NotifyUpdatePlayMemory(ChannelInfo* pch,int index,BYTE* block1,int size1,BYTE* block2,int size2);



    void  NotifySysEvent(int identify,LPVOID eventdata,int eventsize);
    void  NotifyChEvent(ChannelInfo* pch,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize,UINT delayid,BOOL once);

    void  NotifyRecord(ChannelInfo* pch,int result,RECORDATTRIBUTE attribute,TCHAR dtmf);

    void  NotifyUpdateRecordMemory(ChannelInfo* pch,int index,BYTE* block1,int size1,BYTE* block2,int size2);

    void  NotifySendFsk(ChannelInfo* pch,int result);
    void  NotifyRecvFsk(ChannelInfo* pch,int result,BYTE* fsk,int len);

	void  NotifyState(ChannelInfo* pch,LPCTSTR statetext);



};

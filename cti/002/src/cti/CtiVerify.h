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
	created:	12:4:2009   11:25
	filename: 	e:\project\opencti.code\trunk\002\src\cti\CtiVerify.h
	file path:	e:\project\opencti.code\trunk\002\src\cti
	file base:	CtiVerify
	file ext:	h
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/


#pragma once

#include "CtiDefine.h"
#include "CtiChannel.h"
#include "CtiError.h"



#define VERIFY(FAILED,FUNINDEX)                                         \
    int errorcode=ERRCODE_NOERROR;                                      \
    ChannelInfo*pChannel = m_verify.Verify(ch,FUNINDEX,errorcode);      \
    if(!pChannel || errorcode!=ERRCODE_NOERROR)                         \
    {                                                                   \
        m_lasterror.SetLastErrorCode(errorcode);                        \
        return FAILED;                                                  \
    }                                                                   \
    else                                                                \
    {                                                                   \
        m_lasterror.SetLastErrorCode(ERRCODE_NOERROR);                  \
    }                                                               


class CtiVerify
{
public:
    CtiVerify(ChannelMgr& channelmgr,CLastError& lasterror);
    ~CtiVerify(void);

    ChannelInfo* Verify(int ch,FUNCTIONADDRINDEX addrindex,int& errcode);

    ChannelInfo* VerifyCh(int ch,FUNCTIONADDRINDEX funaddrindex);

private:
    ChannelMgr& m_channelmgr;
    CLastError& m_lasterror;
};

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
	created:	2009/06/14
	created:	14:6:2009   11:39
	filename: 	e:\project\cti\002\src\dj\SwapInterface.h
	file path:	e:\project\cti\002\src\dj
	file base:	SwapInterface
	file ext:	h
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/

#pragma once


class DjChannelInfo : public ChannelInfo
{
public:
	DjChannelInfo(UINT chid)
		:ChannelInfo(chid)
	{

	}

	struct  
	{
		UINT        timer;
		UINT        elapse;
	} recvdtmf;


	struct  
	{
		UINT        timer;
		UINT        elapse;
	} recvfsk;


	virtual void Reset()
	{
		ChannelInfo::Reset();
	}
};

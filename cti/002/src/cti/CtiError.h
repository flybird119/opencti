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
	created:	12:4:2009   11:29
	filename: 	e:\project\opencti.code\trunk\002\src\cti\CtiError.h
	file path:	e:\project\opencti.code\trunk\002\src\cti
	file base:	CtiError
	file ext:	h
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/

#pragma once

#include <windows.h>

enum ErrorCode
{
    ERRCODE_NOERROR,                    //没有错误
    ERRCODE_OPENFILEFAILED,             //打开文件失败
    ERRCODE_FILENOTEXIST,               //文件不存在
    ERRCODE_DEVICEFAILED,               //设备返回失败
    ERRCODE_CHNOTEXIST,                 //道道不存在
    ERRCODE_NOTINIT,                    //没有初始化
    ERRCODE_STATEACCORD,                //状态不符合
    ERRCODE_PREVIOUSNOTFINISH,          //上一个相同操作未结束
    ERRCODE_OPERATIONALCONFICT,         //操作存在冲突
	ERRCODE_DEVICENONSUPPORT,			//设备不支持这个操作
	ERRCODE_NOIMPLEMENTEDAPI,			//接口没有实现该API
	ERRCODE_NOIMPLEMENTEDOPERATE,		//未启动这个操作
	ERRCODE_PARAMILLEGAL,				//参数不合法
	ERRCODE_NOPRECOGNITION,				//不可预知的异常
	ERRCODE_INVALIDEDELAY,				//无效的DELAYID
	ERRCODE_DERIVERETFAILED,			//驱动返回失败
	ERRCODE_TOTAL

};

static LPCTSTR s_ErrorMsg[]=
{
    _T("没有错误"),
    _T("打开文件失败"),
    _T("文件不存在"),
    _T("设备返回失败"),
    _T("道道不存在"),
    _T("没有初始化"),
    _T("状态不符合"),
    _T("上一个相同操作未结束"),
    _T("操作存在冲突"),
	_T("设备不支持这个操作"),
	_T("接口没有实现该API"),
	_T("未启动这个操作"),
	_T("参数不合法"),
	_T("不可预知的异常"),
	_T("无效的DELAYID"),
	_T("驱动返回失败"),
};



// 说明 最8高位用于表示错误类型，
// 如果最高8位是0，GetLastErrorCode可以得到错语码，
// 如果最高8位大于0，错误码由驱动管理。
// 错语代码不能大于0x00FFFFFF

class CLastError
{
public:

    CLastError()
        :m_tlsindex(::TlsAlloc())
    {
        _ASSERT(m_tlsindex!=0xFFFFFFFF);
        DWORD tlsvalue=ERRCODE_NOERROR;
        ::TlsSetValue(m_tlsindex,(LPVOID)tlsvalue);
    }
    virtual ~CLastError()
    {
        ::TlsFree(m_tlsindex);
    }
    void  SetLastErrorCode(int errorcode)
    {
        ::TlsSetValue(m_tlsindex,(LPVOID)(errorcode&0x00FFFFFF));
    }

    int GetLastErrorCode()
    {
        return (int)((DWORD)::TlsGetValue(m_tlsindex)&0x00FFFFFF);
    }

    BYTE GetLastErrorType()
    {
        return (int)((DWORD)::TlsGetValue(m_tlsindex)>>24);
    }

    void SetLastErrorType(BYTE errtype)
    {
        DWORD errcode=((DWORD)::TlsGetValue(m_tlsindex)&0x00FFFFFF);
        errcode|=((DWORD)errtype)<<24;
        ::TlsSetValue(m_tlsindex,(LPVOID)errcode);

    }
    LPCTSTR FormatErrorCode(int code)
    {
		if(code<ERRCODE_TOTAL)
		{
			return s_ErrorMsg[code];
		}
		return _T("未知错误代码");
    }




private:
    DWORD   m_tlsindex;
};





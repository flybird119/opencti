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
    ERRCODE_NOERROR,                    //û�д���
    ERRCODE_OPENFILEFAILED,             //���ļ�ʧ��
    ERRCODE_FILENOTEXIST,               //�ļ�������
    ERRCODE_DEVICEFAILED,               //�豸����ʧ��
    ERRCODE_CHNOTEXIST,                 //����������
    ERRCODE_NOTINIT,                    //û�г�ʼ��
    ERRCODE_STATEACCORD,                //״̬������
    ERRCODE_PREVIOUSNOTFINISH,          //��һ����ͬ����δ����
    ERRCODE_OPERATIONALCONFICT,         //�������ڳ�ͻ
	ERRCODE_DEVICENONSUPPORT,			//�豸��֧���������
	ERRCODE_NOIMPLEMENTEDAPI,			//�ӿ�û��ʵ�ָ�API
	ERRCODE_NOIMPLEMENTEDOPERATE,		//δ�����������
	ERRCODE_PARAMILLEGAL,				//�������Ϸ�
	ERRCODE_NOPRECOGNITION,				//����Ԥ֪���쳣
	ERRCODE_INVALIDEDELAY,				//��Ч��DELAYID
	ERRCODE_DERIVERETFAILED,			//��������ʧ��
	ERRCODE_TOTAL

};

static LPCTSTR s_ErrorMsg[]=
{
    _T("û�д���"),
    _T("���ļ�ʧ��"),
    _T("�ļ�������"),
    _T("�豸����ʧ��"),
    _T("����������"),
    _T("û�г�ʼ��"),
    _T("״̬������"),
    _T("��һ����ͬ����δ����"),
    _T("�������ڳ�ͻ"),
	_T("�豸��֧���������"),
	_T("�ӿ�û��ʵ�ָ�API"),
	_T("δ�����������"),
	_T("�������Ϸ�"),
	_T("����Ԥ֪���쳣"),
	_T("��Ч��DELAYID"),
	_T("��������ʧ��"),
};



// ˵�� ��8��λ���ڱ�ʾ�������ͣ�
// ������8λ��0��GetLastErrorCode���Եõ������룬
// ������8λ����0������������������
// ������벻�ܴ���0x00FFFFFF

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
		return _T("δ֪�������");
    }




private:
    DWORD   m_tlsindex;
};





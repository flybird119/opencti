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
	created:	2009/01/06
	created:	6:1:2009   16:08
	filename: 	e:\xugood.cti\src\CtiComm\TranslateEvent.h
	file path:	e:\xugood.cti\src\CtiComm
	file base:	TranslateEvent
	file ext:	h
	author:		xugood@live.cn
	
	purpose:	
*********************************************************************/

#pragma once
#include "CtiDefine.h"
#include <sstream>

class CtiEventTranslate
{
public:
    CtiEventTranslate();

    void Translate(const LPCHANNELEVENT lpchevent);

private:

    void  OutNormal(const LPCHANNELEVENT lpchevent,std::ostringstream& outstream);


};



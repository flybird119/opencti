#pragma once

#include <Windows.h>

//��������Դ
//���ɵ�ר��
//http://blog.csdn.net/jiangsheng/archive/2004/06/24/25563.aspx
//




class CShareRestrictedSD  
{
private:
	CShareRestrictedSD();
	virtual ~CShareRestrictedSD();

public:
    static CShareRestrictedSD*  GetInstance();
	SECURITY_ATTRIBUTES* GetSA();

protected:
	PVOID  ptr;
	SECURITY_ATTRIBUTES sa;
	SECURITY_DESCRIPTOR sd;
};
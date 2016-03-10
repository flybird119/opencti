#pragma once

#include "LoopMessage.h"
#include "./Common/CallBackTemplate.h"

template<typename T>
class CBaseWnd
{
public:
	CBaseWnd(CLoopMessage* pLoopMessage):m_pLoopMessage(pLoopMessage),m_hwnd(NULL)
	{
		m_farproc=ALLOCTHUNK(CBaseWnd,WndProc);
	}
	~CBaseWnd(void)
	{
		if(m_hwnd)
		{
			m_pLoopMessage->DestroyWindow(m_hwnd);
		}

		if(m_farproc)
		{
			FREETHUNK(CBaseWnd,m_farproc);
		}
	}

public:
	HWND CreateDlg(HINSTANCE hInstance,LPCTSTR lpTemplate,HWND hWndParent)
	{
		if(m_hwnd==NULL)
		{
			m_hwnd = m_pLoopMessage->CreateDlg(hInstance,lpTemplate,hWndParent,(DLGPROC)m_farproc);
		}
		return m_hwnd;
	}


	LRESULT  CALLBACK WndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		T* pT = static_cast<T*>(this);   
		LRESULT ret;
		if(pT->OnWindowMessage(hwnd,uMsg,wParam,lParam,ret))
		{
			return pT->DefWindowProc(hwnd,uMsg,wParam,lParam);
		}
		else
		{
			return ret;
		}
	
	}


public:
	HWND				m_hwnd;

private:

	CLoopMessage*		m_pLoopMessage;
	FARPROC				m_farproc;


};





#define BEGIN_WINMESSAGE_MAP()																	\
public:																							\
	BOOL  OnWindowMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam,LRESULT& lResult)		\
{																								\
	BOOL bHandled = TRUE;																		\
	switch(1)																					\
{																								\
	case 1:																						\
{




#define WINMESSAGE_HANDLER(msg, func)															\
	if(msg == uMsg)																				\
{																								\
	lResult=func(hwnd,uMsg,wParam,lParam,bHandled);												\
	if(bHandled)																				\
	break;																						\
}




#define WINMESSAGE_RANGE_HANDLER(msgFirst, msgLast, func)										\
	if(uMsg >= msgFirst && uMsg <= msgLast)														\
{																								\
	lResult=func(hwnd,uMsg,wParam,lParam,bHandled);												\
	if(bHandled)																				\
	break ;																						\
}


#define CHAIN_WINMESSAGE_MAP(theChainClass)														\
	if(theChainClass::OnWindowMessage(hwnd,uMsg,wParam,lParam,bHandled,lResult))				\
	break ;                                                 

#define CHAIN_MSG_WINMESSAGE_MEMBER(theChainMember)												\
	if(theChainMember.OnWindowMessage(hwnd,uMsg,wParam,lParam,bHandled,lResult))				\
	break ;                                                  


#define END_WINMESSAGE_MAP()																	\
}																								\
	break;																						\
}																								\
	return bHandled;																			\
}












//Ê¹ÓÃÀý×Ó
/*

class Dlg : public CBaseWnd<Dlg>
{
public:
Dlg(CLoopMessage* pLoopMessage):CBaseWnd<Dlg>(pLoopMessage)
{

}

BEGIN_WINMESSAGE_MAP()
WINMESSAGE_HANDLER(WM_INITDIALOG,OnInitDialog)
END_WINMESSAGE_MAP()

LRESULT DefWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
return FALSE;
}

LRESULT OnInitDialog(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,BOOL& bHandled)
{
ShowWindow(hWnd,SW_SHOW);



return 1;
}




};

*/
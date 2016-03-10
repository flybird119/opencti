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
	created:	12:4:2009   11:30
	filename: 	e:\project\opencti.code\trunk\002\src\cti\CtiDefine.h
	file path:	e:\project\opencti.code\trunk\002\src\cti
	file base:	CtiDefine
	file ext:	h
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/

/// \file cticonst.h
/// \brief opencti�¼��Ͳ���

#pragma once

#include <windows.h>
#include <tchar.h>


#pragma warning(disable:4996)


/// \defgroup EventandParam �¼��Ͳ��� 
/// \brief 
/// \details 
/// \{ 







/// \defgroup EventParam �¼����� 
/// \brief 
/// \details 
/// \{ 






enum CHANNELSTATE;
enum CHANNELTYPE;
enum CHANNELWAY;

/// \brief ����ͨ����Ϣ
/// \see \link CHANNELEVENTTYPE::EVENT_ADDCHANNEL EVENT_ADDCHANNEL \endlink  
typedef struct AddChannelInfotag
{
    CHANNELTYPE    chtype;	///< ͨ������ �μ�::CHANNELTYPE
    CHANNELSTATE   chstate; ///< ͨ��״̬ �μ�::CHANNELSTATE
    CHANNELWAY     chway;   ///< ͨ������ �μ�::CHANNELWAY
	UINT		   chtotal;	///< ϵͳ��ͨ������

} ADDCHANNELPARAM,*LPADDCHANNELPARAM;


/// \brief �Ƴ�ͨ����Ϣ
/// \see \link CHANNELEVENTTYPE::EVENT_DELCHANNEL EVENT_DELCHANNEL \endlink  
typedef struct DelChannelInfotag
{
	DWORD reason;  ///< ԭ��

} DELCHANNELPARAM,*LPDELCHANNELPARAM;


/// \brief ͨ���ͷ���Ϣ
/// \see \link CHANNELEVENTTYPE::EVENT_RELEASE EVENT_RELEASAE \endlink  
typedef struct ReleaseInfotag
{

} RELEASEPARAM,*LPRELEASEPARAM;



/// \brief ͨ��������Ϣ
/// \see \link CHANNELEVENTTYPE::EVENT_IDLE EVENT_IDLE \endlink  
typedef struct IdleInfotag
{

} IDLEPARAM,*LPIDLEPARAM;



/// \brief ͨ����������Ϣ
/// \see \link CHANNELEVENTTYPE::EVENT_UNUSABLE EVENT_UNUSABLE \endlink  
typedef struct UnusableInfotag
{
	

} UNUSABLEPARAM,*LPUNUSABLEPARAM;



/// \brief ͨ��״̬��Ϣ
/// \see \link CHANNELEVENTTYPE::EVENT_STATE EVENT_STATE \endlink  
typedef struct StateInfotag
{
	LPCTSTR statetext; ///< ͨ��״̬���ı�������

} STATEPARAM,*LPSTATEPARAM;



/// \brief ������Ϣ
typedef struct CallerInfotag
{
    TCHAR called[128];		///< ����
    TCHAR calling[128];		///< ����
    TCHAR origcalled[128];	///< ԭʼ����
} CALLERINFO,*LPCALLERINFO;


/// \brief ������Ϣ
/// \see \link CHANNELEVENTTYPE::EVENT_CALLIN EVENT_CALLIN \endlink  
typedef  CallerInfotag CALLINPARAM,*LPCALLINPARAM;


/// \brief �κ���Ϣ
/// \see \link CHANNELEVENTTYPE::EVENT_DIALUP EVENT_DIALUP \endlink  
typedef CallerInfotag DIALUPPARAM,*LPDIALUPPARAM;

/// \brief ������Ϣ
/// \see \link CHANNELEVENTTYPE::EVENT_RINGBACK EVENT_RINGBACK \endlink  
typedef struct RingbackInfotag
{

} RINGBACKPARAM,*LPRINGBACKPARAM;

/// \brief ������Ϣ
/// \see \link CHANNELEVENTTYPE::EVENT_RINGING EVENT_RINGING \endlink  
typedef struct RingingInfotag
{

} RINGINGPARAM,*LPRINGINGPARAM;


/// \brief ��ͨ��Ϣ
/// \see \link CHANNELEVENTTYPE::EVENT_TALKING EVENT_TALKING \endlink  
typedef struct TalkingInfotag
{

} TALKINGPARAM,*LPTALKINGPARAM;


enum RELEASEATTRIBUTE;
/// \brief �һ���Ϣ
/// \see \link CHANNELEVENTTYPE::EVENT_HANGUP EVENT_HANGUP \endlink 
typedef struct HangInfotag
{
    RELEASEATTRIBUTE reason;			///< �һ�ԭ�򣬲μ�::RELEASEATTRIBUTE

} HANGUPPARAM , *LPHANGUPPARAM;


/// \brief ���յ�DTMF
/// \see \link CHANNELEVENTTYPE::EVENT_RECVDTMF EVENT_RECVDTMF \endlink 
typedef struct RecvDtmftag
{
    int     result;         ///< 0:��ʱ  1:���յ�DTMF 2:ֹͣ
	TCHAR   dtmf;			///< ���յ���DTMF�ַ�
} RECVDTMFPARAM,*LPRECVDTMFPARAM;



/// \brief ����DTMF���
/// \see \link CHANNELEVENTTYPE::EVENT_SENDDTMF EVENT_SENDDTMF \endlink 
typedef struct SendDtmfInfotag
{
    int     result;          ///< 0:ʧ�� 1:��� 2:ֹͣ

} SENDDTMFPARAM,*LPSENDDTMFPARAM;


/// \brief ����FSK���
/// \see \link CHANNELEVENTTYPE::EVENT_SENDFSK EVENT_SENDFSK \endlink 
typedef struct SendFskInfotag
{
    int     result;          ///< 0:ʧ�� 1:��� 2:ֹͣ
} SENDFSKPARAM,*LPSENDFSKPARAM;


/// \brief ���յ�FSK
/// \see \link CHANNELEVENTTYPE::EVENT_RECVFSK EVENT_RECVFSK \endlink 
typedef struct RecvFskInfotag
{
    int    result;          ///< 0:��ʱ  1:���յ�FSK(���к�������) 2:ֹͣ 3:���յ�FSK����(û�к�������)
    int    size;			///< ���յ�FSK���ݵĻ����С
    BYTE   buf;				///< ���յ�FSK���ݵĻ����׵���
} RECVFSKPARAM,*LPRECVFSKPARAM;



/// \brief ���յ��ź���
/// \see \link CHANNELEVENTTYPE::EVENT_RECVTONE EVENT_RECVTONE \endlink 
typedef struct RecvToneInfotag
{
    int     result;          ///< 0:���ճ�ʱ  1:���յ�TONE
	DWORD   tone;            ///< ���յ����ź�ʱ���ͣ��μ�:TONEATTRIBUTE
} RECVTONEPARAM,*LPRECVTONEPARAM;


/// \brief �ź����������
/// \see \link CHANNELEVENTTYPE::EVENT_SENDTONE EVENT_SENDTONE \endlink 
typedef struct SendToneInfotag
{
    int     result;          ///< 0:ʧ�� 1:��ʱ 2:ֹͣ
	DWORD   tone;            ///< �ź������ͣ��μ�::TONEATTRIBUTE
} SENDTONEPARAM,*LPSENDTONEPARAM;



enum PLAYATTRIBUTE;
/// \brief �������
/// \see \link CHANNELEVENTTYPE::EVENT_PLAY EVENT_PLAY \endlink 
typedef struct PlayInfotag
{
    int             result;     ///< 0:ʧ��  1:��� 2:ֹͣ 3:DTMF���
	PLAYATTRIBUTE   attribute;  ///< �������� �μ�::PLAYATTRIBUTE
    TCHAR           dtmf;       ///< ��Ϸ�����DTMF
} PLAYPARAM,*LPPLAYPARAM;



/// \brief �ڴ�¼��������������Ϣ
typedef struct MemoryBlockbag
{
	int		index;		/// 0:block1��Ч��1:block2��Ч
	BYTE*	block1;		/// ��������ַ
	int		size1;		/// �������С
	BYTE*	block2;		/// ��������ַ
	int		size2;		/// �������С
} MEMORYBLOCK,*LPMEMORYBLOCK;
                             
/// \brief �ڴ�¼����������Ϣ
/// \see \link CHANNELEVENTTYPE::EVENT_UPDATERECORDMEMORY EVENT_UPDATERECORDMEMORY \endlink 
typedef struct MemoryBlockbag UPDATERECORDMEMORYPARAM,*LPUPDATERECORDMEMORYPARAM;

/// \brief �ڴ������������Ϣ
/// \see \link CHANNELEVENTTYPE::EVENT_UPDATEPLAYMEMORY EVENT_UPDATEPLAYMEMORY \endlink 
typedef struct MemoryBlockbag UPDATEPLAYMEMORYPARAM,*LPUPDATEPLAYMEMORYPARAM;


enum RECORDATTRIBUTE;
/// \brief ¼�����
/// \see \link CHANNELEVENTTYPE::EVENT_RECORD EVENT_RECORD \endlink 
typedef struct RecordInfotag
{
    int               result;          /// 0:ʧ��  1:��� 2:ֹͣ 3:DTMF���
	RECORDATTRIBUTE   attribute;       /// �μ�::RECORDATTRIBUTE
    TCHAR             dtmf;            ///< ��Ϸ�����DTMF
} RECORDPARAM,*LPRECORDPARAM;

/// \brief �������
/// \see \link CHANNELEVENTTYPE::EVENT_SENDFLASH EVENT_SENDFLASH \endlink 
typedef struct SendFlashInfotag
{

} SENDFLASHPARAM,*LPSENDFLASHPARAM;

/// \brief ���յ�����
/// \see \link CHANNELEVENTTYPE::EVENT_RECVFLASH EVENT_RECVFLASH \endlink 
typedef struct RecvFlashInfotag
{

} RECVFLASHPARAM,*LPRECVFLASHPARAM;




/// \brief ����
/// \see \link CHANNELEVENTTYPE::EVENT_SLEEP EVENT_SLEEP \endlink 
typedef struct SleepInfotag
{

} SLEEPPARAM,*LPSLEEPPARAM; 





/// \brief ���˱����¼���Ϣ
/// \see \link CHANNELEVENTTYPE::EVENT_LOCAL_BLOCK EVENT_LOCAL_BLOCK \endlink 
typedef struct LocalBlockInfotag
{
} LOCALBLOCKPARAM,*LPLOCALBLOCKPARAM;




/// \brief Զ�˱����¼���Ϣ
/// \see \link CHANNELEVENTTYPE::EVENT_REMOTE_BLOCK EVENT_REMOTE_BLOCK \endlink 
typedef struct RemoteBlockInfotag
{

} REMOTEBLOCKPARAM,*LPREMOTEBLOCKPARAM;

/// \brief �û��¼���Ϣ
/// \see \link CHANNELEVENTTYPE::EVENT_USEREVENT EVENT_USEREVENT \endlink 
typedef struct UserEventInfotag
{
    UINT  delayid;          ///< delayid
	BOOL  once;				///< ֻ����һ��
	PVOID eventdata;        ///< ::CTI_DelayChEvent������¼�����ָ��
	int eventsize;          ///< ::CTI_DelayChEvent������¼�������С
} USEREVENTPARAM,*LPUSEREVENTPARAM;



/// \brief ϵͳ�¼���Ϣ
/// \see \link CHANNELEVENTTYPE::EVENT_SYSEVENT EVENT_SYSEVENT \endlink 
typedef struct SysEventInfotag
{
	PVOID eventdata;        ///< ::CTI_DelaySysEvent������¼�����ָ��
	int eventsize;          ///< ::CTI_DelaySysEvent������¼�������С

} SYSEVENTPARAM,*LPSYSEVENTPARAM;


/// \} 

/// \defgroup EventType �¼�����
/// \brief 
/// \details 
/// \{ 


enum CHANNELEVENTTYPE
{
    EVENT_NONE,
	/// \brief ϵͳ�¼�
	/// \par ��������:
	/// ::CTI_DelayedSysEvent
	/// \par �¼�����:
	/// ::SYSEVENTPARAM
	EVENT_SYSEVENT = 0x100,


	/// \brief ����ͨ��
	/// \par ��������:
	/// ::CTI_Init
	/// \par �¼�����:
	/// ::ADDCHANNELPARAM
    EVENT_ADDCHANNEL,     

	/// \brief �Ƴ�ͨ��
	/// \par ��������:
	/// (ͨ���Ƴ�)���豸����
	/// \par �¼�����:
	/// ::DELCHANNELPARAM
    EVENT_DELCHANNEL,
    

    /// \brief ͨ���ͷ�
    /// \par ��������:
    /// ::CTI_Hangup��::EVENT_HANGU
    /// \par �¼�����:
    /// ::CLEANPARAM
    EVENT_RELEASE,

	/// \brief ͨ������::STATE_IDLE״̬
	/// \par ��������:
	/// (ͨ������)���豸����
	/// \par �¼�����:
	/// ::IDLEPARAM
    EVENT_IDLE  =   0x200,

	/// \brief ͨ������::STATE_UNUSABLE״̬
	/// \par ��������:
	/// (ͨ��������)���豸����
	/// \par �¼�����:
	/// ::UNUSABLEPARAM
    EVENT_UNUSABLE,

	/// \brief ͨ���ڲ�״̬�ı�
	/// \par ��������:
	/// ���豸����
	/// \par �¼�����:
	/// ::STATEPARAM
	EVENT_STATE,

	/// \brief ͨ������::STATE_CALLIN״̬
	/// \par ��������:
	/// (���е���)���豸����
	/// \par �¼�����:
	/// ::CALLINPARAM
	EVENT_CALLIN,

	/// \brief ͨ������::STATE_RINGING״̬
	/// \par ��������:
	/// ::CTI_Ringback
	/// \par �¼�����:
	/// ::RINGINGPARAM
	EVENT_RINGING,

	/// \brief ͨ������::STATE_DIALUP״̬
	/// \par ��������:
	/// ::CTI_Dialup
	/// \par �¼�����:
	/// ::DIALUPPARAM
    EVENT_DIALUP,

	/// \brief ͨ������::STATE_RINGBACK״̬
	/// \par ��������:
	/// (������)���豸����
	/// \par �¼�����:
	/// ::RINGBACKPARAM
	EVENT_RINGBACK,

	/// \brief ͨ������::STATE_TALKING״̬
	/// \par ��������:
	/// ::CTI_Pickup
	/// \par �¼�����:
	/// ::TALKINGPARAM
	EVENT_TALKING,

	/// \brief ͨ������::STATE_HANGUP״̬
	/// \par ��������:
	/// (�Զ˹һ�)���豸����
	/// \par �¼�����:
	/// ::HANGUPPARAM
	EVENT_HANGUP,

	/// \brief ���˱���(���Ժ��������ܺ���)
    /// \par ��������
    /// (���˱���)���豸����
    /// \par �¼�����
    /// ::LOCALBLOCKPARAM
    EVENT_LOCAL_BLOCK,

	/// \brief Զ�˱���(���Ժ��룬���ܺ���)
    /// \par ��������
    /// (Զ�˱���)���豸����
    /// \par �¼�����
    /// ::REMOTEBLOCKPARAM
    EVENT_REMOTE_BLOCK,

	/// \brief ���յ�DTMF
	/// \par ��������:
	/// ::CTI_RecvDtmf
	/// \par �¼�����:
	/// ::RECVDTMFPARAM
    EVENT_RECVDTMF  =   0x300,

	/// \brief ����DTMF���
	/// \par ��������:
	/// ::CTI_SendDtmf
	/// \par �¼�����:
	/// ::SENDDTMFPARAM
    EVENT_SENDDTMF,

	/// \brief ���յ�FSK
	/// \par ��������:
	/// ::CTI_RecvFsk
	/// \par �¼�����:
	/// ::RECVFSKPARAM
    EVENT_RECVFSK,

	/// \brief ����FSK���
	/// \par ��������:
	/// ::CTI_SendFsk
	/// \par �¼�����:
	/// ::SENDFSKPARAM
    EVENT_SENDFSK,

	/// \brief ����TONE���
	/// \par ��������:
	/// ::CTI_RecvTone
	/// \par �¼�����:
	/// ::RECVTONEPARAM
    EVENT_RECVTONE,

	/// \brief ����TONE���
	/// \par ��������:
	/// ::CTI_SendTone
	/// \par �¼�����:
	/// ::SENDTONEPARAM
	EVENT_SENDTONE,

	/// \brief �������
	/// \par ��������
	/// ::CTI_PlayFile,::CTI_PlayQueue
	/// \par �¼�����
	/// ::PLAYPARAM
	EVENT_PLAY,

	/// \brief ¼�����
	/// \par ��������
	/// ::CTI_RecordFile
	/// \par �¼�����
	/// ::RECORDPARAM
    EVENT_RECORD,

	/// \brief �ڴ�����������ѷ���
	/// \par ��������
	/// ::CTI_PlayMemory
	/// \par �¼�����
	/// ::UPDATEPLAYMEMORYPARAM
	EVENT_UPDATEPLAYMEMORY,

	/// \brief �ڴ�¼����������¼��
	/// \par ��������
	/// ::CTI_RecordMemory
	/// \par �¼�����
	/// ::UPDATERECORDMEMORYPARAM
	EVENT_UPDATERECORDMEMORY,

    /// \brief ��������
    /// \par ��������
    /// ::CTI_SendFlash
    /// \par �¼�����
    /// ::SENDFLASHPARAM
    EVENT_SENDFLASH,            

    /// \brief ��������
    /// \par ��������
    /// ::CTI_RecvFlash
    /// ::RECVFLASHPARAM
	EVENT_RECVFLASH,


    /// \brief ����
    /// \par ��������
    /// (����)���豸����
    /// \par �¼�����
    /// ::SLEEPPARAM
    EVENT_SLEEP,



    /// \brief �û��¼�
    /// \par ��������
    /// ::CTI_DelayedChEvent
    /// \par �¼�����
    /// ::USEREVENTPARAM
	EVENT_USEREVENT = 0x400,


    EVENT_TOTAL     = 0x1000,
};
/// \} 

/// \defgroup FUNCTIONPARAM ��������
/// \brief 
/// \details 
/// \{ 



/// \brief �¼�����
/// \see CTIEVENTPROC
typedef struct ChannelEventDescriptor
{
    int              chid;				///< ͨ��ID
	CHANNELEVENTTYPE eventid;			///< �¼����� �μ�:: CHANNELEVENTTYPE 
    LPVOID           eventdata;			///< �¼�����ָ��
    int              eventsize;			///< �¼�������С
    LPARAM           notifyparam;		///< �ص��������� 
	DWORD            chownerdata;		///< ͨ���������� �μ�::CTI_SetChOwnerData
#if _WIN32_WINNT<0x0600 || WINVER<0x0600
	DWORD            time;				///< �¼�������ʱ��
#else
	ULONGLONG        time;				///< �¼�������ʱ��
#endif
    ChannelEventDescriptor()
    {
    }

    ChannelEventDescriptor(int ch,CHANNELEVENTTYPE type,LPVOID data,int size,LPARAM param,DWORD owner):
        chid(ch),
        eventid(type),
        eventdata(data),
        eventsize(size),
        notifyparam(param),
        chownerdata(owner),
#if _WIN32_WINNT<0x0600 || WINVER<0x0600
        time(::GetTickCount())
#else
		time(::GetTickCount64())
#endif
    {

    }
   
} CHANNELEVENT,*LPCHANNELEVENT;


static LPCTSTR EventDescription(CHANNELEVENTTYPE eventid)
{
    if(eventid>=EVENT_USEREVENT)
    {
        static TCHAR EventName[256];
        _stprintf(EventName,_T("EVENT_USEREVENT+%d"),eventid-EVENT_USEREVENT);
        return EventName;
    }

    switch(eventid)
    {
    case EVENT_SYSEVENT:    return _T("EVENT_SYSEVENT");
    case EVENT_ADDCHANNEL:  return _T("EVENT_ADDCHANNEL");
    case EVENT_DELCHANNEL:  return _T("EVENT_DELCHANNEL");
    case EVENT_RELEASE:     return _T("EVENT_RELEASE");
    case EVENT_IDLE:        return _T("EVENT_IDLE");
    case EVENT_TALKING:     return _T("EVENT_TALKING");
    case EVENT_HANGUP:      return _T("EVENT_HANGUP");
    case EVENT_RECVDTMF:    return _T("EVENT_RECVDTMF");
    case EVENT_SENDDTMF:    return _T("EVENT_SENDDTMF");
    case EVENT_RECVFSK:     return _T("EVENT_RECVFSK");
    case EVENT_SENDFSK:     return _T("EVENT_SENDFSK");
    case EVENT_PLAY:        return _T("EVENT_PLAY");
    case EVENT_RECORD:      return _T("EVENT_RECORD");
    case EVENT_RECVFLASH:   return _T("EVENT_RECVFLASH");
    case EVENT_SENDFLASH:	return _T("EVENT_SENDFLASH");
    case EVENT_UNUSABLE:    return _T("EVENT_UNUSABLE");
    case EVENT_STATE:		return _T("EVENT_STATE");
    case EVENT_CALLIN:      return _T("EVENT_CALLIN");
    case EVENT_RINGING:     return _T("EVENT_RINGING");
    case EVENT_DIALUP:      return _T("EVENT_DIALUP");
    case EVENT_RINGBACK:    return _T("EVENT_RINGBACK");
    case EVENT_RECVTONE:    return _T("EVENT_RECVTONE");
    case EVENT_SENDTONE:    return _T("EVENT_SENDTONE");
    case EVENT_LOCAL_BLOCK: return _T("EVENT_LOCAL_BLOCK");
    case EVENT_REMOTE_BLOCK:return _T("EVENT_REMOTE_BLOCK");
    case EVENT_UPDATEPLAYMEMORY:return _T("EVENT_UPDATEPLAYMEMORY");
    case EVENT_UPDATERECORDMEMORY:return _T("EVENT_UPDATERECORDMEMORY");
    case EVENT_SLEEP:       return _T("EVENT_SLEEP");
    case EVENT_NONE:        return _T("EVENT_NONE");

    }

    return _T("EVENT_UNKNOWN");
}








typedef void (WINAPI *CTIEVENTPROC)(LPCHANNELEVENT lpchevent);


/// \brief �豸����
/// \see ::CTI_Init
struct DeviceDescriptor
{
    HWND         monitorwnd;        ///< ͨ�����Ӵ���
    CTIEVENTPROC notifyfun;			///< �¼�֪ͨ�ص�������ַ
    LPARAM       notifyparam;		///< �¼�֪ͨ�ص���������

    int          threadmode;        ///< �߳�ģʽ 0��1���̴߳���64ͨ���¼��� 1��1���̴߳���1��ͨ���¼�������ֵ��ϵͳĬ���߳�ģʽ��

};

/// \brief ͨ������
enum CHANNELTYPE
{
    TYPE_UNKNOWN		=	0x00,	///< δ֪
    TYPE_ANALOG_TRUNK	=	0x01,	///< ģ���м�
    TYPE_ANALOG_USER	=	0x02,	///< ģ���û�
    TYPE_ANALOG_RECORD	=	0x04,	///< ģ��¼��
    TYPE_ANALOG_EMPTY	=	0x08,	///< ģ����ģ��
    TYPE_SS7_TUP		=	0x10,	///< SS7TUP
    TYPE_SS7_ISUP		=	0x20,	///< SS7ISUP
    TYPE_DSS1_USER		=	0x40,	///< ISDN�û���
    TYPE_DSS1_NET		=	0x80,	///< ISDN�����
    TYPE_SS1			=	0x100,	///< SS1ͨ��
	TYPE_FAX			=	0x200,	///< ����

};

static LPCTSTR TypeDescription(CHANNELTYPE type)
{
    switch(type)
    {
    case TYPE_UNKNOWN:          return _T("δ֪");
    case TYPE_ANALOG_EMPTY:     return _T("ģ�����");
    case TYPE_ANALOG_RECORD:    return _T("ģ��¼��");
    case TYPE_ANALOG_TRUNK:     return _T("ģ���м�");
    case TYPE_ANALOG_USER:      return _T("ģ���û�");
    case TYPE_SS7_ISUP:         return _T("SS7ISUP");
    case TYPE_SS7_TUP:          return _T("SS7TUP");
    case TYPE_DSS1_NET:         return _T("ISDN�����");
    case TYPE_DSS1_USER:        return _T("ISDN�û���");
    case TYPE_SS1:              return _T("SS1");
    case TYPE_FAX:              return _T("����");
    default:                    return _T("ERROR");
    }
    return _T("ERROR"); 
}

/// \brief ͨ��״̬
enum  CHANNELSTATE
{
    STATE_UNKNOWN,		///< δ֪
    STATE_INITIAL, 		///< ��ʼ��
    STATE_UNUSABLE,		///< ������
    STATE_IDLE,			///< ����
    STATE_DIALUP,		///< �κ�
    STATE_CALLIN,		///< ����
    STATE_RINGBACK,		///< ����
    STATE_RINGING,		///< ����
    STATE_TALKING,		///< ͨ��
    STATE_HANGUP,		///< �Ҷ�
	STATE_RELEASE,		///< �ͷ�
    STATE_LOCAL_BLOCK,	///< ��������
    STATE_REMOTE_BLOCK, ///< Զ������
    STATE_SLEEP,        ///< ����
};
static LPCTSTR StateDescription(CHANNELSTATE state )
{
    switch(state)
    {
    case STATE_UNKNOWN:         return _T("δ֪");
    case STATE_INITIAL:         return _T("��ʼ��");
    case STATE_UNUSABLE:        return _T("������");
    case STATE_IDLE:            return _T("����");
    case STATE_DIALUP:          return _T("����");
    case STATE_CALLIN:          return _T("����");
    case STATE_RINGING:         return _T("����");
    case STATE_RINGBACK:        return _T("����");
    case STATE_TALKING:         return _T("ͨ��");
    case STATE_HANGUP:          return _T("�Ҷ�");
	case STATE_RELEASE:			return _T("�ͷ�");
    case STATE_LOCAL_BLOCK:     return _T("��������");
    case STATE_REMOTE_BLOCK:    return _T("Զ������");
    case STATE_SLEEP:           return _T("����");
    default:                    return _T("ERROR");
    }
}


/// \brief ͨ�����з���
enum CHANNELWAY
{
    WAY_UNKNOWN,	///< δ֪
    WAY_IDLE,		///< ����
    WAY_CALLIN,		///< ����
    WAY_CALLOUT,	///< ����
};
static LPCTSTR WayDescription(CHANNELWAY way)
{
    switch(way)
    {
    case WAY_UNKNOWN:           return _T("δ֪");
    case WAY_IDLE:              return _T("����");
    case WAY_CALLIN:            return _T("����");
    case WAY_CALLOUT:           return _T("����");
    default:                    return _T("ERROR");
    }
}
/// \brief �κű�־��Ӧ���־
/// \see ::CTI_Dialup
/// \note ����ģ������ͨ���ź����������н���ͷ���æ�������һ��¼�
enum CHECKFLAG
{
    CHECKFLAG_NONE       =   0x00,		///< δָ��
	CHECKFLAG_CHECKTALK  =   0x01,		///< ��⵽˵���������� ::EVENT_TALKING �¼���
    CHECKFLAG_CHECKHANG  =   0x02,		///< ��⵽æ�������� ::EVENT_HANGUP �¼���
};


/// \brief �κź�������
/// \see ::CTI_Dialup
enum NUMBERATTRIBUTE
{
    NUMBER_NONE,		 ///< δָ��
    NUMBER_CITY,		 ///< �л�
    NUMBER_COUNTRY,		 ///< ��;
    NUMBER_INTERNATIONAL,///< ����
};


/// \brief �ͷ�ԭ��
/// \see ::CTI_Hangup
enum RELEASEATTRIBUTE
{
	RELEASE_NONE,	   ///< δָ��
    RELEASE_NORMAL,    ///< ����
    RELEASE_USERBUSY,  ///< �û�æ
    RELEASE_VACANT,    ///< �պ�
    RELEASE_REJECTED,  ///< �ܾ�
    RELEASE_NOANSWER,  ///< ��Ӧ��
    RELEASE_LINEFAULT, ///< ��·����
    RELEASE_UNUSABLE,  ///< ������
    
};

static LPCTSTR ReleaseAttributeDescription(RELEASEATTRIBUTE releaseattribute)
{
	switch(releaseattribute)
	{
	case RELEASE_NORMAL:    return _T("����");
	case RELEASE_USERBUSY:  return _T("�û�æ");
	case RELEASE_VACANT:    return _T("�պ�");
	case RELEASE_REJECTED:  return _T("�ܾ�");
	case RELEASE_NOANSWER:  return _T("��Ӧ��");
	case RELEASE_LINEFAULT: return _T("��·����");
	case RELEASE_UNUSABLE:  return _T("������");
	default:				return _T("δ֪");
	}

	return _T("\0");
}


#define STOPSOUNDCHARSETALL       _T("0123456789*#ABCD")
#define STOPSOUNDCHARSETSIMPLE    _T("0123456789*#")
#define STOPSOUNDCHARSETNONE      _T("")



/*
1�������źţ��������б����û�������Ϊ25��3Hz���Ҳ���г��ʧ�治����10���������ѹ��Чֵ90+-15V���������5s��������һ���ͣ�4s�ϣ��ϡ���ʱ��ƫ�������10����
2����������������֪ͨ�����û����Բ��š�����������Ƶ��Ϊ450��25Hz�Ľ�����Դ�����͵�ƽΪ-10��3dBm�����������ź�����
3��������������ʾ�����û����ڱ�����״̬������Ƶ��Ϊ450��25Hz�Ľ�����Դ�����͵�ƽΪ-10��3dBm������5s�������ź�������1s�ͣ�4s�ϣ���������һ�¡�
4��æ��������ʾ���ν�����������æ�򱻽��û�æ������Ƶ��Ϊ450��25Hz�Ľ�����Դ�����͵�ƽΪ-10��3dBm������0.7�������ź�������0.35�ͣ�0.35�ϡ��ýв�Ӧ90s����æ����
5����;֪ͨ����������֪ͨ���ڽ�������ͨ�����û��г�;�绰������Ƶ��Ϊ450��25Hz�Ľ�����Դ�����͵�ƽΪ-20��3dBm������1.2s���ȼ���������ź�������0.2s�ͣ�0.2s�ϣ�0.2s�ͣ�0.6�ϡ�
6���պ���������֪ͨ�����û��������еı��к���Ϊ�պŻ������Ƶĺ��룬����Ƶ��Ϊ450��25Hz�Ľ�����Դ�����͵�ƽΪ-10��3dBm������1.4s���ȼ���������ź��������ظ�����0.1s�͡�0.1s�Ϻ�0.4s�ͣ�0.4�ϣ������Ƶ绰�����պ���æ������պ�����
7���Ŷӵȴ����������ھ����Ŷ����ܵĽ�������֪ͨ�����û��ȴ�Ӧ�𣬲���Ƶ��450��25Hz�Ľ�����Դ�����͵�ƽΪ-10��3dBm����Ҫʱ���û���������¼��֪ͨ��������ר���ź�����
8��֤ʵ������������ȥ̨����Ա�Է����յ��źţ����ں�ʵ�����û��������ȷ�ԣ�����Ƶ��Ϊ950��50Hz�Ľ�����Դ�����͵�ƽΪ-20��3dBm���������ź�����
9���߹������������ɲ���̨���͸��ò��һ��û����źţ���Ŀ����֪ͨ�û��һ�������Ƶ��950��50Hz�Ľ�����Դ�����͵�ƽΪ0��25dBm�����������ź�������ȱ仯���弶������ͼ������ߡ�
���������ź��У��������⣬�����ź������࣬�ź���ԴΪ450��25Hz��950��50Hz��г��ʧ�治����10���������ź����ϡ���ƫ��ֱ𲻵ó���10%��

1.���ֲ���������һ�֡�ཡ�ཡ�����ཡ�ཡ�������һ��һ������������40���룬ͣ40���룩��Ƶ����450�ա������ĵ绰�Ǽ���ĳ����ҵ���ܺ���������Ͳ�����Ĳ����������������Ⲧ��������������������������������绰�� 
2.ӵ��������һ�֡�ཡ�ཡ��Ķ�������0.7�룬���0.7�룩��Ƶ����450�ա����е���æ��������æ��������ʾ�̿ؽ�������ĳ��ԭ�����ӵ����ͨ�� 
3.�պ�������һ�֡�ཡ�ཡ�ཡ�ཡ�����������һ������������������0.1�룬��������0.4�룩��Ƶ��450�գ���ʾ�����еĵ绰��������δʹ�õĿպš�
4.�߹�������һ��Ƶ��Ϊ950�յ��������������弶��ȣ�������С�𽥱�󡣵�������绰û����Ͳ��ػ�������Ͳû�и��ʱ�����������߹���������������Ͳ��á�
5.æ������һ�֡�ཡ�ཡ�ཡ�ཡ��Ķ̴�������0.35�룬��0.35�룩����ʾ���ڻ��߱�ռ�ã��������еĵ绰����ʹ�á�


*/

/// \brief �ź�������
/// \see ::CTI_SendTone,::CTI_RecvTone
enum TONEATTRIBUTE
{
	TONE_NONE		=	0x00,	///< δָ��
    TONE_SILENCE	=	0x01,   ///< ����
    TONE_DIAL		=	0x02,   ///< ������
    TONE_RINGBACK	=	0x04,	///< ������
    TONE_BUSY		=	0x08,   ///< æ��
    TONE_BLOCK		=	0x10,   ///< ӵ����
    TONE_CUSTOM     =	0x20	///< �Զ���
};

/// \brief ������ʽ
enum PLAYATTRIBUTE
{
	PLAY_NONE,		///< δָ��
    PLAY_FILE,      ///< �����ļ�
    PLAY_MEMORY,    ///< �����ڴ�
    PLAY_QUEUE,     ///< �����ж�
    PLAY_INDEX,     ///< ��������
};

/// \brief ¼����ʽ
enum RECORDATTRIBUTE
{
	RECORD_NONE,	///< δָ��
	RECORD_FILE,	///< ¼�����ļ�
	RECORD_MEMORY,  ///< ¼�����ڴ�
};


/// \brief ��������
enum FUNCTIONADDRINDEX
{
    Addr_None               = 0,
    Addr_Init               = 1,
    Addr_Term               = 2,

    Addr_GetChType			= 10,
    Addr_SetChOwnerData     = 11,
    Addr_GetChOwnerData     = 12,
	Addr_GetChState			= 13,

    Addr_GetChTotal         = 20,
    Addr_SearchIdleCh       = 21,
    Addr_LoadPlayIndex      = 22,
    Addr_Features           = 23,
    Addr_GetNativeAPI       = 24,
    Addr_GetLastErrorCode   = 25,
    Addr_FormatErrorCode    = 26,


    Addr_Ringback           = 40,
    Addr_Pickup             = 41,
    Addr_Dialup             = 42,
    Addr_Hangup             = 43,


    Addr_SendDtmf           = 50,
    Addr_StopSendDtmf       = 51,
    Addr_IsSendDtmf         = 52,

    Addr_RecvDtmf           = 60,
    Addr_StopRecvDtmf       = 61,
    Addr_IsRecvDtmf         = 62,


    Addr_SendFsk            = 70,
    Addr_SendFskFull        = 71,
    Addr_StopSendFsk        = 72,
    Addr_IsSendFsk          = 73,

    Addr_RecvFsk            = 80,
    Addr_StopRecvFsk        = 81,
    Addr_IsRecvFsk          = 82,


    Addr_SendTone           = 90,
    Addr_SendToneEx         = 91,
    Addr_StopSendTone       = 92,
    Addr_IsSendTone         = 93,

    Addr_RecvTone           = 100,
    Addr_StopRecvTone       = 101,
    Addr_IsRecvTone         = 102,

    Addr_PlayFile           = 110,
    Addr_PlayMemory         = 111,
    Addr_PlayQueue          = 112,
    Addr_PlayIndex          = 114,
    Addr_StopPlay           = 115,
    Addr_IsPlay             = 116,
    Addr_UpDatePlayMemory   = 117,


    Addr_RecordFile         = 130,
    Addr_RecordMemory       = 131,
    Addr_StopRecord         = 132,
    Addr_IsRecord           = 133,
    Addr_UpDateRecordMemory = 134,

    Addr_SendFlash          = 140,
    Addr_RecvFlash          = 141,
    Addr_StopRecvFlash      = 142,
    Addr_IsRecvFlash        = 143,



    Addr_Listen             = 160,
    Addr_UnListen           = 161,


    Addr_DelayChEvent       = 170,
    Addr_DelaySysEvent      = 171,
	Addr_CancelDelay	    = 172,
	Addr_BindExclusiveThread= 173,
	Addr_UnBindExclusiveThread=174,
    Addr_GetSelectCh        = 175,

	Addr_InsertChEvent      = 176,

	Addr_PauseCh			= 177,
	Addr_ResumeCh		    = 178,



    Addr_Total              = 400,
};

/// \} 

/// \} 






#pragma warning(default:4996)
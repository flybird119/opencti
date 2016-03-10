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
	created:	12:4:2009   11:31
	filename: 	e:\project\opencti.code\trunk\002\src\cti\CtiAPI.h
	file path:	e:\project\opencti.code\trunk\002\src\cti
	file base:	CtiAPI
	file ext:	h
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/

/*! \mainpage  opencti�����ֲ�
* \section ����
* ���ã� ��ӭʹ�� opencti!\n
*
* opencti��Ŀ�꣺ͳһ����������ý�彻������̽ӿڡ�\n
* ����һ��ͨ�õ�CTI�����⣬֧�ֶ���������ȳ������������������Ȳ�Ʒ��\n
* 
* �������ѭCPL���֤������ʹ�û򷢲��������\n
* CPL���֤��Common Public License 1.0 (http://opensource.org/licenses/cpl.php)\n
* ��Ҳ���������������һ����ΪCPL.txt���ļ��л��CPL���֤����ϸ��Ϣ����������\n
* ������ʽʹ�ñ�������㶼����ͬ�ⱾЭ���е�����������ң�����뱣֤������\n
* �������ԡ��㲻�ܴӱ������ɾ������������Լ������κζ�����\n
*
* Ϊ����ֹ�������ܵ��к���֧������Ȩ����������κ���ʽ���޸İ汾��������opencti\n
* ����Ȥ������ϣ�������ķ�չ�������ף����ǻ�ӭ�����뵽opencti��Ŀ���������ǣ����ס\n
* ����opencti��Ŀ�飬�κ������˻�����֯����opencti���޸İ汾���ǲ�����ɵġ�\n
*
* �������ṩ�˶������Եİ汾���ṩ�����İ汾ֻ��Ϊ�˷����Ķ����ڲ�����������\n
* �£��Լ������İ汾��License(zh-CN).txt Ϊ׼��\n
*
*
* \section ��ϵ
*  ��ַ��http://www.opencti.cn\n
*  qqȺ��21596142\n
*  Mail��opencti@@qq.com\n
*
*
* \section ����
*  - ���ڱ������Ա���������ޣ����������������������ѣ�����ϵopencti@@qq.com֪֮��ʹδ��֮�汾�ܸĽ���
*  - ���ڱ�д��æ������֮���������⣬����ϵopencti@@qq.com֪֮��ʹδ��֮�汾�ܸ�����
*
*
*
*/



/// \defgroup ProgrammingGuide  ���ָ��
/// \brief opencti���ָ��
/// \{ 




/// \} 



/// \file ctiapi.h
/// \brief opencti�ӿں���

#pragma once
#include "../Cti/CtiDefine.h"


/// \defgroup CtiAPI API����
/// \brief 
/// \details 
/// \{ 








/// \defgroup systemapi ϵͳ
/// \brief 
/// \details 
/// \{ 
//


/// \brief ��ʼ���豸
/// \param pctidevicedescriptor �豸������Ϣ���μ�::DeviceDescriptor
/// \retval ��0 �ɹ�
/// \retval 0 ʧ��
/// \warning �������к���ǰ������Ҫ�ȵ���::CTI_Init����ʼ���豸��
/// \warning ::CTI_Init����0,ҲҪ����::CTI_Term����ֹ�豸��
/// \see ::EVENT_ADDCHANNEL
/// \see ::CTI_Term
int WINAPI CTI_Init(DeviceDescriptor* pctidevicedescriptor);

/// \brief ��ֹ�豸
/// \warning �����˳�ʱҪ��ֹ�豸��
void WINAPI CTI_Term();




/// \brief ��ȡ�豸����
/// \details �����豸�Ƿ�֧�ָ���������
/// \descriptor  �������������μ�::FUNCTIONADDRINDEX 
/// \retval TRUE ֧��
/// \retval FALSE ��֧��
/// \see ::FUNCTIONADDRINDEX
BOOL WINAPI CTI_Features(FUNCTIONADDRINDEX descriptor);

/// \brief ��ȡ�豸ԭʼAPI�ӿ�
/// \param funname ԭʼAPI������
/// \return ԭʼAPI����ָ�롣
/// \note �����ȡ�豸ԭʼAPI�ӿ�ʧ�ܣ�����NULL��
void* WINAPI CTI_GetNativeAPI(LPCTSTR funname);

/// \brief ��ȡ�������
/// \return �������
/// \note ���ýӿں���ʧ�ܺ󣬿�����������ȡʧ����Ϣ��
/// \see ::CTI_FormatErrorCode
int WINAPI CTI_GetLastErrorCode();

/// \brief �Ѵ������ת��Ϊ�ɶ����ı�
/// \return ������������
/// \see ::CTI_GetLastErrorCode
LPCTSTR WINAPI CTI_FormatErrorCode(int code);

/// \brief ��ȡͨ������
/// \return ͨ������
int WINAPI CTI_GetChTotal();

/// \brief ��ȡͨ������
/// \return ::CHANNELTYPE
/// \see ::CHANNELTYPE
CHANNELTYPE WINAPI CTI_GetChType(int ch);

/// \brief ���ҿ���ͨ��
/// \param chteyp ͨ�����ͣ��μ�::CHANNELTYPE
/// \param pChFilter ͨ������������λ��ͨ������ʼID����λ��ͨ���Ľ���ID��
/// \param FilterLen pChFilter��ά��
/// \param pChExcept Ҫ�Ŷӵ�ͨ����
/// \param ExceptLen pChExcept ��ά��
/// \note ��������������ڸ�����ͨ��ID��Χ������ָ�����͵Ŀ���ͨ����
/// \return ��һ������������ͨ��ID
/// \see ::CTI_Dialup
int WINAPI CTI_SearchIdleCh(CHANNELTYPE chtype, int* pChFilter, int FilterLen,int*pChExcept,int ExceptLen);

/// \brief ����ͨ�������������
/// \retval ��0 �ɹ�
/// \retval 0 ʧ��
/// \see ::CTI_GetChOwnerData
int WINAPI CTI_SetChOwnerData(int ch,DWORD OwnerData);

/// \brief ��ȡͨ�������������
/// \retval ��0 ::CTI_GetChOwnerData���õ�ͨ������������ݡ�
/// \retval 0 ʧ�ܻ�ͨ��û����������ݡ�::CTI_GetLastErrorCode���Ի�ȡ��ϸ��Ϣ��
/// \see ::CTI_SetChOwnerData
int WINAPI CTI_GetChOwnerData(int ch,DWORD OwnerData);


/// \brief ��ȡͨ��״̬
/// \return ::CHANNELSTATE
/// \see ::CHANNELSTATE
CHANNELSTATE WINAPI CTI_GetChState(int ch);





/// \} 




/// \defgroup junctionapi ���� 
/// \brief 
/// \details 
/// \{ 


/// \brief ����
/// \param ch ͨ����
/// \param calling ���к��룬��󳤶�26�ַ���ֻ���������ֿ�
/// \param callingattribute ���к������ʣ�ֻ���������ֿ����μ�::NUMBERATTRIBUTE��
/// \param called ���к��룬��󳤶�26�ַ���
/// \param origcalled ԭʼ���У���󳤶�26�ַ���ֻ���������ֿ�
/// \param origcalledattribute ԭʼ���к�������, ֻ���������ֿ����μ�::NUMBERATTRIBUTE��
/// \param CallerIDBlock ����ȥ����������ΪTRUE������Ϊ�١�ֻ�����ֿ���Ч������Ҫ�ַ���ͨ���ҵ��
/// \param flags �μ�::CHECKFLAG��ֻ������ģ�����ߡ�
/// \retval 1 �ɰ�
/// \retval 0 ʧ�ܣ�::CTI_GetLastErrorCode��ȡʧ��ԭ��
/// \see ::EVENT_DIALUP
/// \see ::CTI_SearchIdleCh
/// \see ::EVENT_HANGUP
int WINAPI CTI_Dialup(int ch,LPCTSTR calling,NUMBERATTRIBUTE callingattribute,LPCTSTR called,LPCTSTR origcalled,NUMBERATTRIBUTE origcalledattribute,BOOL CallerIDBlock,int flags);

/// \brief Ӧ��
/// \param ch ͨ����
/// \param flags �μ�::CHECKFLAG��ֻ������ģ�����ߡ�
/// \retval 1 �ɹ�
/// \retval 0 ʧ�ܣ�::CTI_GetLastErrorCode��ȡʧ��ԭ��
/// \see ::EVENT_TALKING
int WINAPI CTI_Pickup(int ch,int flags);

/// \brief ����
/// \param ch ͨ���š�
/// \retval ��0 �ɹ�
/// \retval 0 ʧ�ܣ�::CTI_GetLastErrorCode��ȡʧ��ԭ��
/// \see EVENT_RINGING
int WINAPI CTI_Ringback(int ch);


/// \brief �һ�
/// \param ch ͨ���š�
/// \param attribute �һ�ԭ��ֵ��ֻ���������ֿ����μ�::RELEASEATTRIBUTE��
/// \retval ��0 �ɹ�
/// \retval 0 ʧ�ܣ�::CTI_GetLastErrorCode��ȡʧ��ԭ��
/// \see EVENT_RELEASE
/// \warning
/// ģ��������(STATE_CALLIN,STATE_RINGING)���ܹҶ�
/// ģ����ϯ��(STATE_CALLIN,STATE_RINGING,STATE_TALKING)���ܹҶ�
int WINAPI CTI_Hangup(int ch,RELEASEATTRIBUTE attribute);

/// \} 



/// \defgroup dtmfapi DTMF 
/// \brief 
/// \details 
/// \{ 

/// \brief ����DTMF
/// \param ch ͨ���š�
/// \param dtmf ���͵�dtmf�ַ���ָ�롣
/// \param len  ���͵�dtmf�ַ������ȡ�
/// \param flags ��ʱδ�ã�Ϊ0��
/// \retval ����0 �Ѵ����豸���ͻ�����dtmf�ĸ�����
/// \retval 0 ʧ�ܣ�::CTI_GetLastErrorCode��ȡʧ��ԭ��
/// \note 
/// ���øú�����ֻ��������DTMF���͹��̣�DTMF���ͽ����󣬻��յ�::EVENT_SENDDTMF֪ͨ�¼���\n
/// DTMF���պͷ��Ͳ�������ͬʱ���С�\n
/// DTMF���͡�FSK���͡�TONE���͡����������ڻ����ϵ��\n
int WINAPI  CTI_SendDtmf(int ch,LPCTSTR dtmf,int len,int flags);

/// \brief ֹͣ����DTMF
/// \retval 1 �ɹ�
/// \retval 0 ʧ�ܣ�::CTI_GetLastErrorCode��ȡʧ��ԭ��
/// \note
/// ֻ���ڵ���::CTI_SendDtmf�����ɹ��󣬻�û���յ�::EVENT_SENDDTMF�¼�ʱ�����Ե�������������ж�DTMF���͹��̡�\n
/// ������������󣬽������յ�::EVENT_SENDDTMF�¼���
int WINAPI  CTI_StopSendDtmf(int ch);

/// \brief ��ȡDTMF����״̬
/// \retval 1 DTMF���ͽ�����
/// \retval 0 DTMF����ֹͣ��
int WINAPI  CTI_IsSendDtmf(int ch);

/// \brief ��������DTMF
/// \param ch ͨ���š�
/// \param time ���ճ�ʱ��Ϊ0��������ʱ����0,time������û�н��յ�dtmf�ͷ���::EVENT_RECVDTMF�¼���
/// \param flags ��ʱδ�ã�Ϊ0��
/// \return �ɹ�����1,ʧ�ܷ���0��
/// \note
/// ���øú�����ֻ��������DTMF���չ��̣��յ�DTMF�󣬻��յ�::EVENT_RECVDTMF֪ͨ�¼���\n
/// ����Ѿ�������DTMF���գ���ֱ�ӷ���0��\n
/// DTMF���պͷ��Ͳ�������ͬʱ���С�\n
/// ����һ��DTMF�󣬲����Զ�ֹͣDTMF���չ��̣�ֻ�е���::CTI_StopRecvDtmf��timeʱ����û�н��յ�DTMF�Ż�ֹͣDTMF���չ��̡�
/// \see ::EVENT_RECVDTMF
int WINAPI  CTI_RecvDtmf(int ch,int time,int flags);

/// \brief ֹͣ����DTMF
/// \return �ɹ�����1,ʧ�ܷ���0��
/// \note
/// ֻ�е���::CTI_RecvDtmf�ɹ�����ҪֹͣDTMF���գ���Ҫ�������������
/// ������������󣬲����յ�::EVENT_RECVDTMF�¼���
int WINAPI  CTI_StopRecvDtmf(int ch);

/// \brief ��ȡDTMF���״̬
/// \retval 0 DTMF�����ֹͣ
/// \retval ��0 DTMFF���������
int WINAPI  CTI_IsRecvDtmf(int ch);

/// \} 








/// \defgroup fskctiapi FSK 
/// \brief 
/// \details 
/// \{ 

/// \brief ����FSK
/// \details 
/// \param ch ͨ����
/// \param fsk Ҫ���͵�FSK���ݻ�������ַ
/// \param len fsk���ݳ���
/// \param flags ��ʱδ�ã�Ϊ0��
/// \retval 0 ʧ��
/// \retval 1 �ɹ�
/// \note   
/// ����������ܿ���FSK�źŵ�ͬ����(0101��)�ͱ�־��(111��)���������������豸��Ĭ��ֵ��\n
/// ���ͽ����󣬻���::EVENT_SENDFSK�¼�֪ͨ��\n
/// ����Ѿ��ڷ���FSK�����������ֱ�ӷ���0��\n
/// FSK���պͷ��Ͳ���ͬʱ���С�\n
/// DTMF���ͺ�FSK���ͺ�TONE���ͺͷ��������ڻ����ϵ��\n
int WINAPI  CTI_SendFsk(int ch,const BYTE* fsk,int len,int flags);

/// \brief ����FSK
/// \details 
/// \param ch ͨ����
/// \param sync ͬ����λ��
/// \param mark ��־��λ��
/// \param fsk Ҫ���͵�FSK���ݻ�������ַ
/// \param len fsk���ݳ���
/// \param flags ��ʱδ�ã�Ϊ0��
/// \retval 0 ʧ��
/// \retval 1 �ɹ�
/// \note   
/// ��������FSK�źŵ�ͬ������������Ҫ���������������FSK���������ܺ�::CTI_SendFskһ����
int WINAPI  CTI_SendFskFull(int ch,int sync,int mark,const BYTE* fsk,int len,int flags);

/// \brief ֹͣ����FSK
/// \return �ɹ�����1��ʧ�ܷ���0��
/// \note
/// �ڵ���::CTI_SendFsk��::CTI_SendFskFull����FSK���ͺ�Ҫ��ֹ���͹��̣��ɵ������������\n
/// ������������󣬲����յ�::EVENT_SENFFSK�¼���\n
int WINAPI  CTI_StopSendFsk(int ch);

/// \brief ��ȡFSK����״̬
/// \return �ڷ���FSK����1�����򷵻�0��
int WINAPI  CTI_IsSendFsk(int ch);

/// \brief ����FSK����
/// \details 
/// \param ch ͨ����
/// \param time ���ýӹ���ʱ(��λ����)�������Ϊ0��������������
/// \param flags ��ʱδ�ã�Ϊ0��
/// \return �ɹ�����1��ʧ�ܷ���0��
/// \note 
/// ����Ѿ�������FSK���գ���ֱ�ӷ���0��\n
/// FSK���պͷ��Ͳ���ͬʱ���У��ڵ���::CTI_SendFskû��::CTI_StopSendFsk��û���յ�::EVENT_SENDFSK�¼������ܵ���::CTI_RecvFsk����
int WINAPI  CTI_RecvFsk(int ch,int time,int flags);

/// \brief ֹͣFSK����
/// \return �ɹ�����1��ʧ�ܷ���0��
int WINAPI  CTI_StopRecvFsk(int ch);

/// \brief ��ȡFSK����״̬
/// \return �ڽ���FSK����1�����򷵻�0��
int WINAPI  CTI_IsRecvFsk(int ch);

/// \} 







/// \defgroup playctiapi ����
/// \brief 
/// \details 
/// \{ 

/// \brief �ļ�����
/// fullpath �ļ��ľ���·��
/// StopstrDtmfCharSet ��ֹ���������DTMF�ַ��������ΪNULL��û����ֹ���������DTMF�ַ�����
/// \retval 1 �ɹ�
/// \retval 0 ʧ�ܣ�::CTI_GetLastErrorCode���Եõ�ʧ��ԭ��
int WINAPI CTI_PlayFile(int ch,LPCTSTR fullpath,LPCTSTR StopstrDtmfCharSet);

/// \brief  �ڴ����
/// StopstrDtmfCharSet ��ֹ���������DTMF�ַ��������ΪNULL��û����ֹ���������DTMF�ַ�����
/// \retval 1 �ɹ�
/// \retval 0 ʧ�ܣ�::CTI_GetLastErrorCode���Եõ�ʧ��ԭ��
/// \note
/// ���ֻ��һ��������,block2��ΪNULL,size2��Ϊ0��
/// ����˫������������ÿ����һ������������::EVENT_UPDATEPLAYMEMORY�¼���֪ͨӦ�ó�����·�����������
int WINAPI CTI_PlayMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet);

/// \brief �ļ��жӷ��� 
/// fullpath �ļ�ָ������ LPCSTR* p[n]={�ļ�1,�ļ�2,�ļ�n};
/// StopstrDtmfCharSet ��ֹ���������DTMF�ַ��������ΪNULL��û����ֹ���������DTMF�ַ�����
/// \retval 1 �ɹ�
/// \retval 0 ʧ�ܣ�::CTI_GetLastErrorCode���Եõ�ʧ��ԭ��
int WINAPI CTI_PlayQueue(int ch,LPCTSTR fullpath[],int size,LPCTSTR StopstrDtmfCharSet);

/// \brief �����ļ�����
/// \retval 1 �ɹ�
/// \retval 0 ʧ�ܣ�::CTI_GetLastErrorCode���Եõ�ʧ��ԭ��
int WINAPI CTI_LoadPlayIndex(LPCTSTR fullpath,LPCTSTR Alias);

/// \brief �ļ��������� 
/// StopstrDtmfCharSet ��ֹ���������DTMF�ַ��������ΪNULL��û����ֹ���������DTMF�ַ�����
/// \retval 1 �ɹ�
/// \retval 0 ʧ�ܣ�::CTI_GetLastErrorCode���Եõ�ʧ��ԭ��
/// \note
/// Alias��ʽ ����1\0����2\0\0
int WINAPI CTI_PlayIndex(int ch,LPCTSTR Alias,LPCTSTR StopstrDtmfCharSet);


/// \brief ֹͣ���� 
/// \retval 1 �ɹ�
/// \retval 0 ʧ�ܣ�::CTI_GetLastErrorCode���Եõ�ʧ��ԭ��
int WINAPI CTI_StopPlay(int ch);


/// \brief ��ȡ����״̬
/// \retval 1 ����������
/// \retval 0 ������ֹͣ
int WINAPI CTI_IsPlay(int ch);

/// \brief �����ڴ����������
int WINAPI CTI_UpDatePlayMemory(int ch,int index,BYTE* block,int size);
/// \} 


/// \defgroup recordapi ¼�� 
/// \brief 
/// \details 
/// \{ 

/// \brief �ļ�¼�� 
/// \param ch ͨ����
/// \param fullpath ¼���ļ�·��
/// \param time ¼��ʱ��(��λ����)�����Ϊ0���򲻻��Զ�ֹͣ��
/// \retval 1 �ɹ�
/// \retval 0 ʧ�ܣ�::CTI_GetLastErrorCode���Եõ�ʧ��ԭ��
/// \note ¼��timeʱ�����Զ�ֹͣ��������::EVENT_RECORD�¼���
int WINAPI CTI_RecordFile(int ch,LPCTSTR fullpath,int time,LPCTSTR StopstrDtmfCharSet);

/// \brief ��ʼ�ڴ�¼�� 
/// \param ch ͨ����
/// \param block1 ¼����������ַ
/// \param size1  ¼����������С
/// \param block2 ¼����������ַ
/// \param size2 ¼����������С
/// \retval 1 �ɹ�
/// \retval 0 ʧ�ܣ�::CTI_GetLastErrorCode���Եõ�ʧ��ԭ��
/// \note
/// �ڴ�¼��ԭ��:
/// �����ڴ�¼�����Ȱ�¼�����ݴ���block1�������block1д����
/// ����::EVENT_UPDATERECORDMEMORY�¼�������¼�����ݴ���block2��
/// ���ѭ����ֱ��::CTI_StopRecord������
/// ���block2==NULL��size2==0,��block1д����ͻ�ֹͣ¼����������::EVENT_RECORD�¼���
int WINAPI CTI_RecordMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet);

/// \brief ֹͣ¼�� 
/// \retval 1 �ɹ�
/// \retval 0 ʧ�ܣ�::CTI_GetLastErrorCode���Եõ�ʧ��ԭ��
int WINAPI CTI_StopRecord(int ch);


/// \brief ��ȡ¼��״̬ 
/// \retval 1 ¼��������
/// \retval 0 ¼��ֹͣ��
int WINAPI CTI_IsRecord(int ch);


/// \brief �����ڴ�¼��������
int WINAPI CTI_UpDateRecordMemory(int ch,int index,BYTE* block,int size);
/// \} 



/// \defgroup exchangeapi ��ͨ 
/// \brief 
/// \details 
/// \{ 

/// \brief ����ͨ�� 
/// \param initiativech ���������ͨ��
/// \param passivelych �����ӵ�ͨ��
/// \retval ��0 �ɹ�
/// \retval 0 ʧ�ܣ�::CTI_GetLastErrorCode���Եõ�ʧ��ԭ��
/// \note ���ӳɹ���initiativechͨ��������passivelych��������
/// \warning һ��ͨ�����Ա����ͨ������һ��ͨ��ֻ��������һ��ͨ����
int WINAPI CTI_Listen(int initiativech,int passivelych);

/// \brief ������
/// \param initiativech ���������ͨ��
/// \param passivelych �����ӵ�ͨ��
/// \retval ��0 �ɹ�
/// \retval 0 ʧ�ܣ�::CTI_GetLastErrorCode���Եõ�ʧ��ԭ��
int WINAPI CTI_UnListen(int initiativech,int passivelych);

/// \} 






/// \defgroup toneapi �ź��� 
/// \brief 
/// \details 
/// \{ 

/// \brief �����ź���
/// \param ch ͨ���š�
/// \param attribute �ź������ͣ��μ�::TONEATTRIBUTE
/// \param time �ź�������ʱ�䣬��λ���룬Ϊ0����ֹͣ��
/// \param flags ��ʱδ�ã�Ϊ0��
/// \return �ɹ����ط�0��ʧ�ܷ�0��
/// \note
/// �ź�������ʱ�䵽�󣬻�ֹͣ�����ź�����������::EVENT_SENDTONE�¼���
int WINAPI CTI_SendTone(int ch,TONEATTRIBUTE attribute,int time,int flags);

/// ��ʱδʵ�֡�
int WINAPI CTI_SendToneEx(int ch, int nFreq1, int nVolume1, int nFreq2, int nVolume2,DWORD dwOnTime, DWORD dwOffTime,int time,int flags);

/// ֹͣ�����ź�����
/// \return �ɹ����ط�0��ʧ�ܷ�0��
int WINAPI CTI_StopSendTone(int ch);

/// \brief ��÷����ź���״̬
/// \param ch ͨ���š�
/// \retval ��0 ���ڷ����ź���
/// \retval 0 �����ź�����ֹͣ
int WINAPI CTI_IsSendTone(int ch);


/// \brief ��������ź���
/// \param ch ͨ���š�
/// \param time ���ճ�ʱ��0��������ʱ����0��time������û�нӼ�⵽�ź����ʹ���::EVENT_RECVTONE�¼�����ֹͣ����ź�����
/// \param flags ��ʱδ�ã�Ϊ0��
/// \return �ɹ����ط�0,ʧ�ܷ���0��
/// \note
/// ֻ��ͨ��״̬��::STATE_RINGBACK��::STATE_TALKING��::STATE_RINGINGʱ�ſ�����������ź�����\n
/// ���øú����󣬼�⵽�ź����ᴥ��::EVENT_RECVTONE�¼���\n
/// ����ͨ����ε���CTI_RecvTone�����벻ͬ��::TONEATTRIBUTE,ʵ��ͬʱ�������ź�����\n
/// ����ź����ͷ����ź�����������ͬʱ���С�
/// \see ::EVENT_RECVTONE
int WINAPI CTI_RecvTone(int ch,TONEATTRIBUTE attribute,int time,int flags);

/// \brief ֹͣ����ź���
/// \param ch ͨ����
/// \param attribute �ź�������
/// \return �ɹ����ط�0��ʧ�ܷ���0��
int WINAPI CTI_StopRecvTone(int ch,TONEATTRIBUTE attribute);

/// \brief  ��ü���ź���״̬
/// \param ch ͨ���š�
/// \param attribute �ź�������
/// \retval ��0 ���ڼ���ź���
/// \retval 0 ����ź�����ֹͣ
int WINAPI CTI_IsRecvTone(int ch,TONEATTRIBUTE attribute);

/// \} 



/// \defgroup flashapi ����
/// \brief 
/// \details 
/// \{ 

/// \brief ����һ������
/// \param ch ͨ����
/// \param time ����ʱ��
/// \return �ɹ�����1��ʧ�ܷ���0��
/// \note
/// ֻ��ģ������֧�����������
/// \see ::EVENT_SENDFLASH
int WINAPI CTI_SendFlash(int ch,int time);

/// \brief �����������
/// \return �ɹ�����1��ʧ�ܷ���0��
/// \note
/// ֻ��ģ������ͨ��֧�����������
/// \see ::EVENT_RECVFLASH
int WINAPI CTI_RecvFlash(int ch);

/// \brief ֹͣ�������
/// \return �ɹ�����1��ʧ�ܷ���0�� 
int WINAPI CTI_StopRecvFlash(int ch);

/// \brief ��ȡ�������״̬
/// \return �����������������1�����򷵻�0��
int WINAPI CTI_IsRecvFlash(int ch);

/// \} 




/// \defgroup faxapi ���� 
/// \brief 
/// \details 
/// \{ 

/// \} 




/// \defgroup confapi ����
/// \brief 
/// \details 
/// \{ 

/// \} 


/// \defgroup e1api E1
/// \brief 
/// \details 
/// \{ 

/// \} 






/// \defgroup assisapi ����
/// \brief 
/// \details 
/// \{ 


/// \brief ��ʱ���ʹ��ڻ����::EVENT_USEREVENT�¼����¼����С�
/// \param cmsDelay ��ʱʱ��(��λms_��
/// \param once TRUE��ִֻ��һ�Σ�FALSE��ѭ����
/// \param chid ͨ��ID��
/// \param eventid �¼�ID��ע�⣺������ڻ����::EVENT_USEREVENT��
/// \param eventdata �¼�������
/// \param eventsize ������С��
/// \retval ��0 DelayID��
/// \retval 0 ʧ�ܣ�::CTI_GetLastErrorCode��ȡʧ��ԭ��
/// \warning ֻҪ�յ����͵��¼��󣬲����ͷ�eventdata��ռ���ڴ档
/// \see ::EVENT_USEREVENT
/// \see ::USEREVENTPARAM
int WINAPI CTI_DelayChEvent(int cmsDelay,BOOL once,int ch,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize);


/// \brief  ��ʱ����::EVENT_SYSEVEN�¼����¼����С�
/// \param cmsDelay ��ʱʱ��(��λms)��
/// \param once TRUE��ִֻ��һ�Σ�FALSE��ѭ����
/// \param identify ��ʶ����
/// \param eventdata �¼�������
/// \param eventsize ������С��
/// \retval ��0 DelayID��
/// \retval 0 ʧ�ܡ�
/// \warning ֻҪ�յ����͵��¼��󣬲����ͷ�eventdata��ռ���ڴ档
/// \see ::EVENT_SYSEVENT
/// \see ::SYSEVENTPARAM
int WINAPI CTI_DelaySysEvent(int cmsDelay,BOOL once,int identify,LPVOID eventdata,int eventsize);



/// \brief ������ڻ����::EVENT_USEREVENT�¼����¼����ж�����
/// \param chid ͨ��ID��
/// \param eventid �¼�ID��ע�⣺������ڻ����::EVENT_USEREVENT��
/// \param eventdata �¼�������
/// \param eventsize ������С��
/// \retval ��0 �ɹ���
/// \retval 0 ʧ�ܣ�::CTI_GetLastErrorCode��ȡʧ��ԭ��
/// \warning ֻҪ�յ����͵��¼��󣬲����ͷ�eventdata��ռ���ڴ档
/// \see ::EVENT_USEREVENT
/// \see ::USEREVENTPARAM
int WINAPI CTI_InsertChEvent(int ch,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize);




/// \brief ȡ��Delay�¼�
/// \ch ���-1 ȡ��CTI_DelaySysEvent�������¼�
/// \param delayid DelayID�� ���0:ȡ��ָ��ͨ��������Delay�¼�
/// \retval ��0 �ɹ���
/// \retval 0 ʧ�ܣ�::CTI_GetLastErrorCode��ȡʧ��ԭ��
/// \note ::CTI_CancelDelay�󣬽������յ�IDΪdelayid��Delay�¼���
int WINAPI CTI_CancelDelay(int ch,int delayid);



/// \brief  �󶨵���ռ�߳�
/// \details ͨ���󶨵���ռ�̺߳�ͨ���¼�����һ���������߳��лص���������һ��ͨ��һ���߳� ��
int WINAPI CTI_BindExclusiveThread(int ch);

/// \brief  ȡ���󶨵���ռ�߳�
/// \details ���н������������Զ�ȡ���󶨵���ռ�̡߳�
int WINAPI CTI_UnBindExclusiveThread(int ch);


/// \brief  ��ͣ����
/// \details  
void WINAPI CTI_PauseCh(int ch);

/// \brief  �ָ�����
/// \details  
void WINAPI CTI_ResumeCh(int ch);





/// \brief ��ȡ���Ӵ���ѡ�е�ͨ��
int  WINAPI CTI_GetSelectCh();

/// \} 





/// \} 
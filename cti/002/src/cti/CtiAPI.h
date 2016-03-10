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
	created:	12:4:2009   11:31
	filename: 	e:\project\opencti.code\trunk\002\src\cti\CtiAPI.h
	file path:	e:\project\opencti.code\trunk\002\src\cti
	file base:	CtiAPI
	file ext:	h
	author:		opencti@msn.com
	
	purpose:	
*********************************************************************/

/*! \mainpage  opencti开发手册
* \section 声明
* 您好！ 欢迎使用 opencti!\n
*
* opencti的目标：统一语音卡，多媒体交换机编程接口。\n
* 它是一个通用的CTI开发库，支持东进、三汇等厂商语音卡，交换机等产品。\n
* 
* 你可以遵循CPL许可证下自由使用或发布本软件。\n
* CPL许可证：Common Public License 1.0 (http://opensource.org/licenses/cpl.php)\n
* 你也可以在这个发布中一个名为CPL.txt的文件中获得CPL许可证的详细信息。无论你以\n
* 何种形式使用本软件，你都必须同意本协议中的所有条款。并且，你必须保证这个软件\n
* 的完整性。你不能从本软件中删除这个声明，以及其他任何东西。\n
*
* 为了阻止产生可能的有害分支，你无权发布本软件任何形式的修改版本。如果你对opencti\n
* 感兴趣，并且希望对它的发展做出贡献，我们欢迎您参与到opencti项目中来。但是，请记住\n
* 除了opencti项目组，任何其他人或者组织发布opencti的修改版本都是不被许可的。\n
*
* 本声明提供了多种语言的版本。提供其它的版本只是为了方便阅读，在产生歧义的情况\n
* 下，以简体中文版本：License(zh-CN).txt 为准。\n
*
*
* \section 联系
*  网址：http://www.opencti.cn\n
*  qq群：21596142\n
*  Mail：opencti@@qq.com\n
*
*
* \section 补充
*  - 由于本人语言表达能力有限，因描述不清而导致理解困难，请联系opencti@@qq.com知之。使未来之版本能改进。
*  - 由于编写匆忙，错误之外在所难免，请联系opencti@@qq.com知之。使未来之版本能改正。
*
*
*
*/



/// \defgroup ProgrammingGuide  编程指南
/// \brief opencti编程指南
/// \{ 




/// \} 



/// \file ctiapi.h
/// \brief opencti接口函数

#pragma once
#include "../Cti/CtiDefine.h"


/// \defgroup CtiAPI API函数
/// \brief 
/// \details 
/// \{ 








/// \defgroup systemapi 系统
/// \brief 
/// \details 
/// \{ 
//


/// \brief 初始化设备
/// \param pctidevicedescriptor 设备描述信息，参见::DeviceDescriptor
/// \retval 非0 成功
/// \retval 0 失败
/// \warning 调用所有函数前，都需要先调用::CTI_Init来初始化设备。
/// \warning ::CTI_Init返回0,也要调用::CTI_Term来终止设备。
/// \see ::EVENT_ADDCHANNEL
/// \see ::CTI_Term
int WINAPI CTI_Init(DeviceDescriptor* pctidevicedescriptor);

/// \brief 终止设备
/// \warning 程序退出时要终止设备。
void WINAPI CTI_Term();




/// \brief 获取设备特性
/// \details 测试设备是否支持给定函数。
/// \descriptor  函数索引导表。参见::FUNCTIONADDRINDEX 
/// \retval TRUE 支持
/// \retval FALSE 不支持
/// \see ::FUNCTIONADDRINDEX
BOOL WINAPI CTI_Features(FUNCTIONADDRINDEX descriptor);

/// \brief 获取设备原始API接口
/// \param funname 原始API函数名
/// \return 原始API函数指针。
/// \note 如果获取设备原始API接口失败，返回NULL。
void* WINAPI CTI_GetNativeAPI(LPCTSTR funname);

/// \brief 获取错误代码
/// \return 错误代码
/// \note 调用接口函数失败后，可以用它来获取失败信息。
/// \see ::CTI_FormatErrorCode
int WINAPI CTI_GetLastErrorCode();

/// \brief 把错误代码转化为可读的文本
/// \return 错误代码的描述
/// \see ::CTI_GetLastErrorCode
LPCTSTR WINAPI CTI_FormatErrorCode(int code);

/// \brief 获取通道总数
/// \return 通道总数
int WINAPI CTI_GetChTotal();

/// \brief 获取通道类型
/// \return ::CHANNELTYPE
/// \see ::CHANNELTYPE
CHANNELTYPE WINAPI CTI_GetChType(int ch);

/// \brief 查找空闲通道
/// \param chteyp 通道类型，参见::CHANNELTYPE
/// \param pChFilter 通道过滤器，低位：通道的起始ID，高位：通道的结束ID。
/// \param FilterLen pChFilter的维数
/// \param pChExcept 要排队的通道表
/// \param ExceptLen pChExcept 的维数
/// \note 这个函数，用于在给定的通道ID范围，查找指定类型的空闲通道。
/// \return 第一个符合条件的通道ID
/// \see ::CTI_Dialup
int WINAPI CTI_SearchIdleCh(CHANNELTYPE chtype, int* pChFilter, int FilterLen,int*pChExcept,int ExceptLen);

/// \brief 设置通道相关联的数据
/// \retval 非0 成功
/// \retval 0 失败
/// \see ::CTI_GetChOwnerData
int WINAPI CTI_SetChOwnerData(int ch,DWORD OwnerData);

/// \brief 获取通道相关联的数据
/// \retval 非0 ::CTI_GetChOwnerData设置的通道相关联的数据。
/// \retval 0 失败或通道没有相关联数据。::CTI_GetLastErrorCode可以获取详细信息。
/// \see ::CTI_SetChOwnerData
int WINAPI CTI_GetChOwnerData(int ch,DWORD OwnerData);


/// \brief 获取通道状态
/// \return ::CHANNELSTATE
/// \see ::CHANNELSTATE
CHANNELSTATE WINAPI CTI_GetChState(int ch);





/// \} 




/// \defgroup junctionapi 接续 
/// \brief 
/// \details 
/// \{ 


/// \brief 呼出
/// \param ch 通道号
/// \param calling 主叫号码，最大长度26字符，只适用于数字卡
/// \param callingattribute 主叫号码性质，只适用于数字卡。参见::NUMBERATTRIBUTE。
/// \param called 被叫号码，最大长度26字符。
/// \param origcalled 原始被叫，最大长度26字符，只适用于数字卡
/// \param origcalledattribute 原始被叫号码性质, 只适用于数字卡。参见::NUMBERATTRIBUTE。
/// \param CallerIDBlock 启用去电主叫隐藏为TRUE，否则为假。只对数字卡有效，还需要局方开通这个业务。
/// \param flags 参见::CHECKFLAG，只适用于模拟外线。
/// \retval 1 成败
/// \retval 0 失败，::CTI_GetLastErrorCode获取失败原因。
/// \see ::EVENT_DIALUP
/// \see ::CTI_SearchIdleCh
/// \see ::EVENT_HANGUP
int WINAPI CTI_Dialup(int ch,LPCTSTR calling,NUMBERATTRIBUTE callingattribute,LPCTSTR called,LPCTSTR origcalled,NUMBERATTRIBUTE origcalledattribute,BOOL CallerIDBlock,int flags);

/// \brief 应答
/// \param ch 通道号
/// \param flags 参见::CHECKFLAG，只适用于模拟外线。
/// \retval 1 成功
/// \retval 0 失败，::CTI_GetLastErrorCode获取失败原因。
/// \see ::EVENT_TALKING
int WINAPI CTI_Pickup(int ch,int flags);

/// \brief 回铃
/// \param ch 通道号。
/// \retval 非0 成功
/// \retval 0 失败，::CTI_GetLastErrorCode获取失败原因。
/// \see EVENT_RINGING
int WINAPI CTI_Ringback(int ch);


/// \brief 挂机
/// \param ch 通道号。
/// \param attribute 挂机原因值，只适用于数字卡。参见::RELEASEATTRIBUTE。
/// \retval 非0 成功
/// \retval 0 失败，::CTI_GetLastErrorCode获取失败原因。
/// \see EVENT_RELEASE
/// \warning
/// 模拟中续在(STATE_CALLIN,STATE_RINGING)不能挂断
/// 模拟坐席在(STATE_CALLIN,STATE_RINGING,STATE_TALKING)不能挂断
int WINAPI CTI_Hangup(int ch,RELEASEATTRIBUTE attribute);

/// \} 



/// \defgroup dtmfapi DTMF 
/// \brief 
/// \details 
/// \{ 

/// \brief 发送DTMF
/// \param ch 通道号。
/// \param dtmf 发送的dtmf字符串指针。
/// \param len  发送的dtmf字符串长度。
/// \param flags 暂时未用，为0。
/// \retval 大于0 已传入设备发送缓冲区dtmf的个数。
/// \retval 0 失败，::CTI_GetLastErrorCode获取失败原因。
/// \note 
/// 调用该函数后，只是启动了DTMF发送过程，DTMF发送结束后，会收到::EVENT_SENDDTMF通知事件。\n
/// DTMF接收和发送操作，可同时进行。\n
/// DTMF发送、FSK发送、TONE发送、放音，存在互斥关系。\n
int WINAPI  CTI_SendDtmf(int ch,LPCTSTR dtmf,int len,int flags);

/// \brief 停止发送DTMF
/// \retval 1 成功
/// \retval 0 失败，::CTI_GetLastErrorCode获取失败原因。
/// \note
/// 只有在调用::CTI_SendDtmf函数成功后，还没有收到::EVENT_SENDDTMF事件时，可以调用这个函数来中断DTMF发送过程。\n
/// 调用这个函数后，将不会收到::EVENT_SENDDTMF事件。
int WINAPI  CTI_StopSendDtmf(int ch);

/// \brief 获取DTMF发送状态
/// \retval 1 DTMF发送进行中
/// \retval 0 DTMF发送停止中
int WINAPI  CTI_IsSendDtmf(int ch);

/// \brief 启动接收DTMF
/// \param ch 通道号。
/// \param time 接收超时，为0，永不超时；非0,time毫秒内没有接收到dtmf就发送::EVENT_RECVDTMF事件。
/// \param flags 暂时未用，为0。
/// \return 成功返回1,失败返回0。
/// \note
/// 调用该函数后，只是启动了DTMF接收过程，收到DTMF后，会收到::EVENT_RECVDTMF通知事件。\n
/// 如果已经启动了DTMF接收，会直接返回0。\n
/// DTMF接收和发送操作，可同时进行。\n
/// 接收一个DTMF后，不会自动停止DTMF接收过程，只有调用::CTI_StopRecvDtmf或time时间内没有接收到DTMF才会停止DTMF接收过程。
/// \see ::EVENT_RECVDTMF
int WINAPI  CTI_RecvDtmf(int ch,int time,int flags);

/// \brief 停止接收DTMF
/// \return 成功返回1,失败返回0。
/// \note
/// 只有调用::CTI_RecvDtmf成功后，需要停止DTMF接收，才要调用这个函数。
/// 调用这个函数后，不会收到::EVENT_RECVDTMF事件。
int WINAPI  CTI_StopRecvDtmf(int ch);

/// \brief 获取DTMF检测状态
/// \retval 0 DTMF检测已停止
/// \retval 非0 DTMFF检测已启动
int WINAPI  CTI_IsRecvDtmf(int ch);

/// \} 








/// \defgroup fskctiapi FSK 
/// \brief 
/// \details 
/// \{ 

/// \brief 发送FSK
/// \details 
/// \param ch 通道号
/// \param fsk 要发送的FSK数据缓冲区地址
/// \param len fsk数据长度
/// \param flags 暂时未用，为0。
/// \retval 0 失败
/// \retval 1 成功
/// \note   
/// 这个函数不能控制FSK信号的同步串(0101…)和标志串(111…)，这两个参数用设备的默认值。\n
/// 发送结束后，会用::EVENT_SENDFSK事件通知。\n
/// 如果已经在发送FSK，这个函数会直接返回0。\n
/// FSK接收和发送不能同时进行。\n
/// DTMF发送和FSK发送和TONE发送和放音，存在互斥关系。\n
int WINAPI  CTI_SendFsk(int ch,const BYTE* fsk,int len,int flags);

/// \brief 发送FSK
/// \details 
/// \param ch 通道号
/// \param sync 同步串位数
/// \param mark 标志串位数
/// \param fsk 要发送的FSK数据缓冲区地址
/// \param len fsk数据长度
/// \param flags 暂时未用，为0。
/// \retval 0 失败
/// \retval 1 成功
/// \note   
/// 如果想控制FSK信号的同步引导串，需要用这个函数来发送FSK。其它功能和::CTI_SendFsk一样。
int WINAPI  CTI_SendFskFull(int ch,int sync,int mark,const BYTE* fsk,int len,int flags);

/// \brief 停止发送FSK
/// \return 成功返回1，失败返回0。
/// \note
/// 在调用::CTI_SendFsk或::CTI_SendFskFull启动FSK发送后，要终止发送过程，可调用这个函数。\n
/// 调用这个函数后，不会收到::EVENT_SENFFSK事件。\n
int WINAPI  CTI_StopSendFsk(int ch);

/// \brief 获取FSK发送状态
/// \return 在发送FSK返回1，否则返回0。
int WINAPI  CTI_IsSendFsk(int ch);

/// \brief 启动FSK接收
/// \details 
/// \param ch 通道号
/// \param time 设置接过超时(单位毫秒)，如果设为0，则永不超过。
/// \param flags 暂时未用，为0。
/// \return 成功返回1，失败返回0。
/// \note 
/// 如果已经启动了FSK接收，会直接返回0。\n
/// FSK接收和发送不能同时进行（在调用::CTI_SendFsk没有::CTI_StopSendFsk或没有收到::EVENT_SENDFSK事件，不能调用::CTI_RecvFsk）。
int WINAPI  CTI_RecvFsk(int ch,int time,int flags);

/// \brief 停止FSK接收
/// \return 成功返回1，失败返回0。
int WINAPI  CTI_StopRecvFsk(int ch);

/// \brief 获取FSK接收状态
/// \return 在接收FSK返回1，否则返回0。
int WINAPI  CTI_IsRecvFsk(int ch);

/// \} 







/// \defgroup playctiapi 放音
/// \brief 
/// \details 
/// \{ 

/// \brief 文件放音
/// fullpath 文件的绝对路径
/// StopstrDtmfCharSet 终止放音任务的DTMF字符集，如果为NULL则没有终止放音任务的DTMF字符集。
/// \retval 1 成功
/// \retval 0 失败，::CTI_GetLastErrorCode可以得到失败原因。
int WINAPI CTI_PlayFile(int ch,LPCTSTR fullpath,LPCTSTR StopstrDtmfCharSet);

/// \brief  内存放音
/// StopstrDtmfCharSet 终止放音任务的DTMF字符集，如果为NULL则没有终止放音任务的DTMF字符集。
/// \retval 1 成功
/// \retval 0 失败，::CTI_GetLastErrorCode可以得到失败原因。
/// \note
/// 如果只放一个缓冲区,block2设为NULL,size2设为0。
/// 对于双缓冲区放音，每放完一个缓冲区会用::EVENT_UPDATEPLAYMEMORY事件，通知应用程序更新放音缓冲区。
int WINAPI CTI_PlayMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet);

/// \brief 文件列队放音 
/// fullpath 文件指针数组 LPCSTR* p[n]={文件1,文件2,文件n};
/// StopstrDtmfCharSet 终止放音任务的DTMF字符集，如果为NULL则没有终止放音任务的DTMF字符集。
/// \retval 1 成功
/// \retval 0 失败，::CTI_GetLastErrorCode可以得到失败原因。
int WINAPI CTI_PlayQueue(int ch,LPCTSTR fullpath[],int size,LPCTSTR StopstrDtmfCharSet);

/// \brief 载入文件索引
/// \retval 1 成功
/// \retval 0 失败，::CTI_GetLastErrorCode可以得到失败原因。
int WINAPI CTI_LoadPlayIndex(LPCTSTR fullpath,LPCTSTR Alias);

/// \brief 文件索引放音 
/// StopstrDtmfCharSet 终止放音任务的DTMF字符集，如果为NULL则没有终止放音任务的DTMF字符集。
/// \retval 1 成功
/// \retval 0 失败，::CTI_GetLastErrorCode可以得到失败原因。
/// \note
/// Alias格式 别名1\0别名2\0\0
int WINAPI CTI_PlayIndex(int ch,LPCTSTR Alias,LPCTSTR StopstrDtmfCharSet);


/// \brief 停止放音 
/// \retval 1 成功
/// \retval 0 失败，::CTI_GetLastErrorCode可以得到失败原因。
int WINAPI CTI_StopPlay(int ch);


/// \brief 获取放音状态
/// \retval 1 放音进行中
/// \retval 0 放音已停止
int WINAPI CTI_IsPlay(int ch);

/// \brief 更新内存放音缓冲区
int WINAPI CTI_UpDatePlayMemory(int ch,int index,BYTE* block,int size);
/// \} 


/// \defgroup recordapi 录音 
/// \brief 
/// \details 
/// \{ 

/// \brief 文件录音 
/// \param ch 通道号
/// \param fullpath 录音文件路径
/// \param time 录音时间(单位毫秒)，如果为0，则不会自动停止。
/// \retval 1 成功
/// \retval 0 失败，::CTI_GetLastErrorCode可以得到失败原因。
/// \note 录音time时间后会自动停止，并触发::EVENT_RECORD事件。
int WINAPI CTI_RecordFile(int ch,LPCTSTR fullpath,int time,LPCTSTR StopstrDtmfCharSet);

/// \brief 开始内存录音 
/// \param ch 通道号
/// \param block1 录音缓冲区地址
/// \param size1  录音缓冲区大小
/// \param block2 录音缓冲区地址
/// \param size2 录音缓冲区大小
/// \retval 1 成功
/// \retval 0 失败，::CTI_GetLastErrorCode可以得到失败原因。
/// \note
/// 内存录音原理:
/// 启动内存录音后，先把录音数据存入block1，如果，block1写满后，
/// 触发::EVENT_UPDATERECORDMEMORY事件，并把录音数据存入block2，
/// 如果循环，直至::CTI_StopRecord结束。
/// 如果block2==NULL或size2==0,则block1写满后就会停止录音，并触发::EVENT_RECORD事件。
int WINAPI CTI_RecordMemory(int ch,BYTE* block1,int size1,BYTE* block2,int size2,LPCTSTR StopstrDtmfCharSet);

/// \brief 停止录音 
/// \retval 1 成功
/// \retval 0 失败，::CTI_GetLastErrorCode可以得到失败原因。
int WINAPI CTI_StopRecord(int ch);


/// \brief 获取录音状态 
/// \retval 1 录音进行中
/// \retval 0 录音停止中
int WINAPI CTI_IsRecord(int ch);


/// \brief 更新内存录音缓冲区
int WINAPI CTI_UpDateRecordMemory(int ch,int index,BYTE* block,int size);
/// \} 



/// \defgroup exchangeapi 连通 
/// \brief 
/// \details 
/// \{ 

/// \brief 连接通道 
/// \param initiativech 请求接连的通道
/// \param passivelych 被连接的通道
/// \retval 非0 成功
/// \retval 0 失败，::CTI_GetLastErrorCode可以得到失败原因。
/// \note 连接成功后，initiativech通道能听到passivelych的声音。
/// \warning 一个通道可以被多个通道听；一个通道只能主动听一个通道。
int WINAPI CTI_Listen(int initiativech,int passivelych);

/// \brief 拆开连接
/// \param initiativech 请求接连的通道
/// \param passivelych 被连接的通道
/// \retval 非0 成功
/// \retval 0 失败，::CTI_GetLastErrorCode可以得到失败原因。
int WINAPI CTI_UnListen(int initiativech,int passivelych);

/// \} 






/// \defgroup toneapi 信号音 
/// \brief 
/// \details 
/// \{ 

/// \brief 发送信号音
/// \param ch 通道号。
/// \param attribute 信号音类型，参见::TONEATTRIBUTE
/// \param time 信号音持续时间，单位毫秒，为0永不停止。
/// \param flags 暂时未用，为0。
/// \return 成功返回非0，失败返0。
/// \note
/// 信号音持续时间到后，会停止发送信号音，并触发::EVENT_SENDTONE事件。
int WINAPI CTI_SendTone(int ch,TONEATTRIBUTE attribute,int time,int flags);

/// 暂时未实现。
int WINAPI CTI_SendToneEx(int ch, int nFreq1, int nVolume1, int nFreq2, int nVolume2,DWORD dwOnTime, DWORD dwOffTime,int time,int flags);

/// 停止发送信号音。
/// \return 成功返回非0，失败返0。
int WINAPI CTI_StopSendTone(int ch);

/// \brief 获得发送信号音状态
/// \param ch 通道号。
/// \retval 非0 正在发送信号音
/// \retval 0 发送信号音已停止
int WINAPI CTI_IsSendTone(int ch);


/// \brief 启动检测信号音
/// \param ch 通道号。
/// \param time 接收超时，0：永不超时；非0：time毫秒内没有接检测到信号音就触发::EVENT_RECVTONE事件，并停止检测信号音。
/// \param flags 暂时未用，为0。
/// \return 成功返回非0,失败返回0。
/// \note
/// 只有通道状态是::STATE_RINGBACK、::STATE_TALKING、::STATE_RINGING时才可以启动检测信号音。\n
/// 调用该函数后，检测到信号音会触发::EVENT_RECVTONE事件。\n
/// 可以通过多次调用CTI_RecvTone，传入不同的::TONEATTRIBUTE,实现同时检测多种信号音。\n
/// 检测信号音和发送信号音操作可以同时进行。
/// \see ::EVENT_RECVTONE
int WINAPI CTI_RecvTone(int ch,TONEATTRIBUTE attribute,int time,int flags);

/// \brief 停止检测信号音
/// \param ch 通道号
/// \param attribute 信号音类型
/// \return 成功返回非0，失败返回0。
int WINAPI CTI_StopRecvTone(int ch,TONEATTRIBUTE attribute);

/// \brief  获得检测信号音状态
/// \param ch 通道号。
/// \param attribute 信号音类型
/// \retval 非0 正在检测信号音
/// \retval 0 检测信号音已停止
int WINAPI CTI_IsRecvTone(int ch,TONEATTRIBUTE attribute);

/// \} 



/// \defgroup flashapi 闪断
/// \brief 
/// \details 
/// \{ 

/// \brief 生产一个闪断
/// \param ch 通道号
/// \param time 闪断时长
/// \return 成功返回1，失败返回0。
/// \note
/// 只有模拟外线支持这个操作。
/// \see ::EVENT_SENDFLASH
int WINAPI CTI_SendFlash(int ch,int time);

/// \brief 启动检测闪断
/// \return 成功返回1，失败返回0。
/// \note
/// 只有模拟内线通道支持这个操作。
/// \see ::EVENT_RECVFLASH
int WINAPI CTI_RecvFlash(int ch);

/// \brief 停止检测闪断
/// \return 成功返回1，失败返回0。 
int WINAPI CTI_StopRecvFlash(int ch);

/// \brief 获取检测闪断状态
/// \return 检测闪断已启动返回1，否则返回0。
int WINAPI CTI_IsRecvFlash(int ch);

/// \} 




/// \defgroup faxapi 传真 
/// \brief 
/// \details 
/// \{ 

/// \} 




/// \defgroup confapi 会议
/// \brief 
/// \details 
/// \{ 

/// \} 


/// \defgroup e1api E1
/// \brief 
/// \details 
/// \{ 

/// \} 






/// \defgroup assisapi 辅助
/// \brief 
/// \details 
/// \{ 


/// \brief 延时发送大于或等于::EVENT_USEREVENT事件到事件队列。
/// \param cmsDelay 延时时间(单位ms_。
/// \param once TRUE：只执行一次；FALSE：循环。
/// \param chid 通道ID。
/// \param eventid 事件ID。注意：必须大于或等于::EVENT_USEREVENT。
/// \param eventdata 事件参数。
/// \param eventsize 参数大小。
/// \retval 非0 DelayID。
/// \retval 0 失败，::CTI_GetLastErrorCode获取失败原因。
/// \warning 只要收到发送的事件后，才能释放eventdata所占的内存。
/// \see ::EVENT_USEREVENT
/// \see ::USEREVENTPARAM
int WINAPI CTI_DelayChEvent(int cmsDelay,BOOL once,int ch,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize);


/// \brief  延时发送::EVENT_SYSEVEN事件到事件队列。
/// \param cmsDelay 延时时间(单位ms)。
/// \param once TRUE：只执行一次；FALSE：循环。
/// \param identify 标识符。
/// \param eventdata 事件参数。
/// \param eventsize 参数大小。
/// \retval 非0 DelayID。
/// \retval 0 失败。
/// \warning 只要收到发送的事件后，才能释放eventdata所占的内存。
/// \see ::EVENT_SYSEVENT
/// \see ::SYSEVENTPARAM
int WINAPI CTI_DelaySysEvent(int cmsDelay,BOOL once,int identify,LPVOID eventdata,int eventsize);



/// \brief 插入大于或等于::EVENT_USEREVENT事件到事件队列顶部。
/// \param chid 通道ID。
/// \param eventid 事件ID。注意：必须大于或等于::EVENT_USEREVENT。
/// \param eventdata 事件参数。
/// \param eventsize 参数大小。
/// \retval 非0 成功。
/// \retval 0 失败，::CTI_GetLastErrorCode获取失败原因。
/// \warning 只要收到发送的事件后，才能释放eventdata所占的内存。
/// \see ::EVENT_USEREVENT
/// \see ::USEREVENTPARAM
int WINAPI CTI_InsertChEvent(int ch,CHANNELEVENTTYPE eventid,LPVOID eventdata,int eventsize);




/// \brief 取消Delay事件
/// \ch 如果-1 取消CTI_DelaySysEvent产生的事件
/// \param delayid DelayID。 如果0:取消指定通道的所有Delay事件
/// \retval 非0 成功。
/// \retval 0 失败，::CTI_GetLastErrorCode获取失败原因。
/// \note ::CTI_CancelDelay后，将不会收到ID为delayid的Delay事件。
int WINAPI CTI_CancelDelay(int ch,int delayid);



/// \brief  绑定到独占线程
/// \details 通道绑定到独占线程后，通道事件将在一个独立的线程中回调，类似于一个通道一个线程 。
int WINAPI CTI_BindExclusiveThread(int ch);

/// \brief  取消绑定到独占线程
/// \details 呼叫结束后，驱动会自动取消绑定到独占线程。
int WINAPI CTI_UnBindExclusiveThread(int ch);


/// \brief  暂停处理
/// \details  
void WINAPI CTI_PauseCh(int ch);

/// \brief  恢复处理
/// \details  
void WINAPI CTI_ResumeCh(int ch);





/// \brief 获取监视窗口选中的通道
int  WINAPI CTI_GetSelectCh();

/// \} 





/// \} 
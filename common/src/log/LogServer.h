/********************************************************************
	created:	2008/12/06
	created:	6:12:2008   21:11
	filename: 	f:\xugood.comm\src\Log\LogServer.h
	file path:	f:\xugood.comm\src\Log
	file base:	LogServer
	file ext:	h
	author:		xugood@live.cn
	
	purpose:	
*********************************************************************/

#pragma once


#include <string>
#include <map>
#include ".\common\Lock.h"
#include "LogMessage.h"


typedef struct tagCHANNELINFO
{

    UINT           flag;
    FILE*          file;

    UINT           total;       //文件最大大小
    UINT           size;        //文件已写大小
    LoggingSeverity__ sev;
   
}CHANNELINFO,*LPCHANNELINFO;


#define     OUTFILE_ENABLE          1
#define     OUTMONITOR_ENABLE       2

#define     DATEFORMATNAME          4


#pragma warning (disable:4251)


#ifdef LOGMODULE_EXPORTS
class __declspec(dllexport) CLogServer
#else
class __declspec(dllimport) CLogServer
#endif

{
public:
    explicit CLogServer(int nCmdShow);
    ~CLogServer(void);


public:

    static CLogServer* GetInstance();
    static void LoadLogServer(int nCmdShow);
    static void FreeLogServer();
    void OutLog(const char* channel,const char* pstrstream,int streamsize,LoggingSeverity__ sev);
    BOOL SetDirectory(const char* directorypath);

    bool Logable(const char* channel,LoggingSeverity__ sev);
    void LogMonitor(const char*channel,BOOL onoff);
    void LogLevel(const char* channel,LoggingSeverity__ sev);
    void LogSize(const char* channel,UINT size);

    LPCHANNELINFO GetChannelInfo(const char* channel);

    void TimerUpDataFile();



private:
    LPCHANNELINFO FindChannel(const char* channel,const char** constchannel=NULL);
    LPCHANNELINFO AddChannel(const char* channel,const char** constchannel);
    FILE*         CreateLog(const char* channel,BOOL format=FALSE);
    std::map<std::string,LPCHANNELINFO>  channel_;
    COMMON::CLock                     crit_;
    char directorypath_[MAX_PATH];


    static CLogServer*      s_pThis;
    static int              s_ThisRef;
    static COMMON::CLock    s_crit;




};
#pragma warning (default:4251)
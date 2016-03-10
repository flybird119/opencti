/********************************************************************
	created:	2008/12/06
	created:	6:12:2008   21:11
	filename: 	f:\xugood.comm\src\Log\LogMessage.h
	file path:	f:\xugood.comm\src\Log
	file base:	LogMessage
	file ext:	h
	author:		xugood@live.cn
	
	purpose:	
*********************************************************************/

#pragma once

#include <Windows.h>
#include <tchar.h>
#include <sstream>


#define LOG_INIT(LOGNAME,MONITOR,LEVEL,SIZE)      \
    SET_LOG_MONITOR(LOGNAME,MONITOR);             \
    SET_LOG_LEVEL(LOGNAME,LEVEL);                 \
    SET_LOG_SIZE(LOGNAME,SIZE)      





#define TraceComm(...)    TRACE_FULL("COMMON",LEVEL_SENSITIVE)<<__VA_ARGS__


enum LoggingSeverity__ { LEVEL_NULL,LEVEL_SENSITIVE, LEVEL_VERBOSE, LEVEL_INFO, LEVEL_WARNING, LEVEL_ERROR,LEVEL_FATAL};





#define TRACE_FULL(channel,level)                                   \
    if (CLogMessage::Logable(channel,level))                        \
        CLogMessage(channel).Init(__FILE__, __LINE__,level)



#define TRACE_SHORT(channel,level)                                   \
    if (CLogMessage::Logable(channel,level))                        \
        CLogMessage(channel).Init(level)


#define TRACE_TEXT(channel,level)                                   \
    if (CLogMessage::Logable(channel,level))                        \
        CLogMessage(channel).Init()


#define SET_LOG_DIRECTORY(directory)              CLogMessage::LogDirectory(directory)
#define SET_LOG_GLOBAL_LEVEL(level)               CLogMessage::LogLevel(level)

#define SET_LOG_LEVEL(channel,level)              CLogMessage::LogLevel(channel,level)
#define SET_LOG_MONITOR(channel,onoff)            CLogMessage::LogMonitor(channel,onoff)

#define SET_LOG_SIZE(channel,size)                CLogMessage::LogSize(channel,size)

//程序启动时，Load一次 ncmdshow:-1不启动UI
#define LOAD_LOG_SERVER(nCmdShow)                 CLogMessage::LoadLogServer(nCmdShow)
#define FREE_LOG_SERVER()                         CLogMessage::FreeLogServer()







#define LOG_ENDL            "\r\n"





class  CLogMessage
{

public:


    CLogMessage(const char* channel);
	~CLogMessage(void);

    //输出 文件名 行号 时间 等级
    std::ostringstream & Init(char* file,long line,LoggingSeverity__ sev);
    //输出 时间 等级
    std::ostringstream & Init(LoggingSeverity__ sev);
    //输出 无
    std::ostringstream & Init();


	//等级控制 是否输出
	static bool Logable(const char* channel,LoggingSeverity__ sev);


    static void LogMonitor(const char* channel,BOOL onoff);
    static void LogLevel(const char* channel,LoggingSeverity__ sev);
    static void LogSize(const char* channel,UINT size);
    static void LogDirectory(const char* filepath);
    static void LogLevel(LoggingSeverity__ sev);

    static void LoadLogServer(int nCmdShow);
    static void FreeLogServer();

protected:

    const char* Spacing();
    const char* Describe(LoggingSeverity__ sev);
    const char* GetCurrentDateTime(char* datetimebuf);

private:
    std::ostringstream  print_stream_;
    const char*         channel_;
    LoggingSeverity__   level_;
    


    static LoggingSeverity__ sev_;
};


std::ostream& operator<<(std::ostream& stream,const wchar_t* wchar);
std::ostream& operator<<(std::ostream& stream,const wchar_t wchar);
std::string TraceFormat(int buflen,const char* formatstr...);
std::string HexFormat(const char* buf,int buflen,int line);


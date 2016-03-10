/********************************************************************
	created:	2008/12/06
	created:	6:12:2008   21:11
	filename: 	f:\xugood.comm\src\Log\LogMessage.cpp
	file path:	f:\xugood.comm\src\Log
	file base:	LogMessage
	file ext:	cpp
	author:		xugood@live.cn
	
	purpose:	
*********************************************************************/
#include "stdafx.h"
#include "LogMessage.h"

#include "LogServer.h"

#include "./Common/frequent.h"

#pragma comment(lib,"LogModule.lib")

#pragma warning( disable : 4996)


LoggingSeverity__ CLogMessage::sev_=LEVEL_SENSITIVE;
void CLogMessage::LogLevel(LoggingSeverity__ sev)
{
    sev_=sev;
}
void CLogMessage::LogDirectory(const char* filepath)
{
    CLogServer::GetInstance()->SetDirectory(filepath);
}

void CLogMessage::LogMonitor(const char* channel,BOOL onoff)
{
    return CLogServer::GetInstance()->LogMonitor(channel,onoff);
}
void CLogMessage::LogLevel(const char* channel,LoggingSeverity__ sev)
{
    return CLogServer::GetInstance()->LogLevel(channel,sev);
}

void CLogMessage::LogSize(const char* channel,UINT size)
{
    return CLogServer::GetInstance()->LogSize(channel,size);
}


//等级控制 是否输出
bool CLogMessage::Logable(const char* channel,LoggingSeverity__ sev)
{
    if(sev>=sev_)
	    return CLogServer::GetInstance()->Logable(channel,sev);

    return false;
}


void CLogMessage::LoadLogServer(int nCmdShow)
{
    CLogServer::LoadLogServer(nCmdShow);
}

void CLogMessage::FreeLogServer()
{
    CLogServer::FreeLogServer();
}

CLogMessage::CLogMessage(const char* channel)
{
	channel_=channel;
}

CLogMessage::~CLogMessage(void)
{
    if(level_>LEVEL_NULL)
    {
        print_stream_<<LOG_ENDL;
    }
    std::string content = print_stream_.str();

	CLogServer::GetInstance()->OutLog(channel_,content.c_str(),content.size(),level_);

}

std::ostringstream & CLogMessage::Init(char* file,long line,LoggingSeverity__ sev)
{
    level_=sev;
	char datetimebuf[26];
	print_stream_<<GetCurrentDateTime(datetimebuf)<<Spacing()<<"File:"<<file<<Spacing()<<"Line:"<<line<<Spacing()<<"Severity:"<<Describe(sev)<<Spacing();
	return print_stream_;
}

std::ostringstream & CLogMessage::Init(LoggingSeverity__ sev)
{
    level_=sev;
	char datetimebuf[26];
	print_stream_<<GetCurrentDateTime(datetimebuf)<<Spacing()<<"Severity:"<<Describe(sev)<<Spacing();
	return print_stream_;
}

std::ostringstream & CLogMessage::Init()
{

    level_=LEVEL_NULL;
    return print_stream_;
}


const char* CLogMessage::Spacing()
{
	return " ";
}

const char* CLogMessage::Describe(LoggingSeverity__ sev)
{
	switch (sev) 
	{
	case LEVEL_SENSITIVE: return "Sensitive";
	case LEVEL_VERBOSE:   return "Verbose";
	case LEVEL_INFO:      return "Info";
	case LEVEL_WARNING:   return "Warning";
	case LEVEL_ERROR:     return "Error";
    case LEVEL_FATAL:     return "Fatal";
	default:           return "<unknown>";

	}

	return "";
}

const char* CLogMessage::GetCurrentDateTime(char* datetimebuf)
{
	SYSTEMTIME systime;
	::GetLocalTime(&systime);
	sprintf(datetimebuf,"[%02d-%02d %02d:%02d:%02d-%03d]",systime.wMonth,systime.wDay,systime.wHour,systime.wMinute,systime.wSecond,systime.wMilliseconds);
	return datetimebuf;
}




std::ostream& operator<<(std::ostream& stream,const wchar_t* wchar)
{
    COMMON::UNICODEChar unicodechar(wchar);
    stream<<unicodechar;   
    return stream;
}

std::ostream& operator<<(std::ostream& stream,const wchar_t wchar)
{
	wchar_t wchars[2]={wchar,0};
	COMMON::UNICODEChar unicodechar(wchars);
	stream<<unicodechar;   
	return stream;

}

std::string TraceFormat(int buflen,const char* formatstr...)
{
    va_list argList;
    va_start( argList, formatstr);
    char* pstrbuf=new char[buflen+1];
    _vsnprintf(pstrbuf,buflen,formatstr,argList);
    va_end( argList );

    std::string outstr(pstrbuf);

    delete []pstrbuf;
    return outstr;

}

std::string HexFormat(const char* bufferptr,int size,int linelen)
{
    if(linelen==0) linelen=16;

   
    int row=size/linelen+(size%linelen ? 1 : 0);
    int outsize=row*(linelen+linelen*3+1);

    std::string outstring;
    outstring.assign(outsize+1,'.');

    char* outptr=(char*)(outstring.c_str());

    int outposition=0;
    int inposition=0;
    int len=0;


    char hexbuf[4]="\0";

    while(inposition<size)
    {
        (size-inposition<linelen) ? len=size-inposition : len=linelen;
        char* ptr1=outptr+outposition,* ptr2=ptr1+linelen*3;
        for(int i=0;i<len;++i)
        {
            sprintf(hexbuf,"%02X ",(int)(BYTE)(*(bufferptr+inposition+i)));
            memcpy(ptr1+i*3,hexbuf,3);


            if(isgraph((BYTE)(*(bufferptr+inposition+i))) && *(bufferptr+inposition+i))
            {
                *(ptr2+i)=*(bufferptr+inposition+i);
            }
            else
            {
                *(ptr2+i)='.';
            }


        }

#if _MSC_VER>1200
		for(int i=len;i<linelen;++i)
#else
		for(i=len;i<linelen;++i)
#endif
        {
            *(ptr2+i)='.';
            memcpy(ptr1+i*3,".. ",3);
        }

        *(ptr2+linelen)='\n';
        *(ptr1+linelen*3-1)='\t';

        outposition+=linelen*3+linelen+1;
        inposition+=linelen;

    }

    *(outptr+outposition)=0;


    return outstring;
}

#pragma warning( default : 4996)
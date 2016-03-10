/********************************************************************
	created:	2008/12/06
	created:	6:12:2008   21:11
	filename: 	f:\xugood.comm\src\Log\LogServer.cpp
	file path:	f:\xugood.comm\src\Log
	file base:	LogServer
	file ext:	cpp
	author:		xugood@live.cn
	
	purpose:	
*********************************************************************/


#include "StdAfx.h"
#include "LogServer.h"

#include ".\common\frequent.h"
#include ".\monitor\monitor.h"


#include <time.h>
#include <io.h> 




#pragma warning(disable:4996)






CLogServer*      CLogServer::s_pThis=NULL;
int              CLogServer::s_ThisRef=0;
COMMON::CLock    CLogServer::s_crit(COMMON::CRITICAl);


CLogServer* CLogServer::GetInstance()
{
    _ASSERT_EX(s_pThis,_T("CLogServer::GetInstance s_pThis is NULL"));
    return s_pThis;
}
void CLogServer::LoadLogServer(int nCmdShow)
{
    LOCKSCOPE(s_crit);

    if(++s_ThisRef && !s_pThis)
        s_pThis=new CLogServer(nCmdShow);
}

void CLogServer::FreeLogServer()
{
    LOCKSCOPE(s_crit);

    if(!--s_ThisRef && s_pThis)
    {
        delete s_pThis;
        s_pThis=NULL;
    }

}


CLogServer::CLogServer(int nCmdShow) : crit_(COMMON::CRITICAl)
{
   SetDirectory("C:\\Log\\");

   if(nCmdShow!=-1)
   {
       Log_StartUI(nCmdShow);
   }

}

CLogServer::~CLogServer(void)
{
    for(std::map<std::string,LPCHANNELINFO>::iterator channeliter=channel_.begin();channeliter!=channel_.end();++channeliter)
    {
        if(channeliter->second->file)
        {
            fclose(channeliter->second->file);
        }
        delete channeliter->second;
    }
    Log_StopUI();

}

BOOL CLogServer::SetDirectory(const char* directorypath)
{
    int directorylen=strlen(directorypath);
    if(directorylen<MAX_PATH && COMMON::CreatePathA(directorypath))
    {
        strncpy(directorypath_,directorypath,MAX_PATH);
        if(directorypath_[directorylen-1]!='\\')
        {
            directorypath_[directorylen]='\\';
        }
        return TRUE;
    }

    _ASSERT_EX(0,_T("SetDirectory"));
    

    return FALSE;
}
bool CLogServer::Logable(const char* channel,LoggingSeverity__ sev)
{
    LPCHANNELINFO chinfo= FindChannel(channel);
    if(chinfo)
    {
        return (sev>=chinfo->sev);
    }
    return false;
}

void CLogServer::LogMonitor(const char*channel,BOOL onoff)
{
    LPCHANNELINFO chinfo= FindChannel(channel);
    if(chinfo)
    {
        if(onoff)
        {
            chinfo->flag|=OUTMONITOR_ENABLE;
           Log_ShowLog(channel,TRUE);

        }
        else
        {
            chinfo->flag&=~OUTMONITOR_ENABLE;
            Log_ShowLog(channel,FALSE);
        }   
        
    }
}

void CLogServer::LogLevel(const char* channel,LoggingSeverity__ sev)
{
    LPCHANNELINFO chinfo= FindChannel(channel);
    if(chinfo)
    {
        chinfo->sev=sev;
    }
}

void CLogServer::LogSize(const char* channel,UINT size)
{
    LPCHANNELINFO chinfo= FindChannel(channel);
    if(chinfo)
    {
        chinfo->total=size;
    }
}



LPCHANNELINFO CLogServer::GetChannelInfo(const char* channel)
{
    return FindChannel(channel);
}

void CLogServer::TimerUpDataFile()
{

    LOCKSCOPE(crit_);

    for(std::map<std::string,LPCHANNELINFO>::iterator channeliter=channel_.begin();channeliter!=channel_.end();++channeliter)
    {
        if(channeliter->second->file)
        {
            fclose(channeliter->second->file);
            channeliter->second->file=CreateLog(channeliter->first.c_str(),TRUE);
            channeliter->second->size=0;
        }
       
    }
   

}


void CLogServer::OutLog(const char* channel,const char* pstrstream,int streamsize,LoggingSeverity__ sev)
{
    
    if(channel==NULL)
    {
        channel="default";
    }

    const char* constchannel=NULL;

    LPCHANNELINFO chinfo= FindChannel(channel,&constchannel);
    if(chinfo)
    {
        if(chinfo->file && chinfo->flag&OUTFILE_ENABLE)
        {
            LOCKSCOPE(crit_);
            int w=fwrite(pstrstream,1,streamsize,chinfo->file);
            if(w>0)
            {
                chinfo->size+=w;

                if(chinfo->total && chinfo->size >= chinfo->total)
                {
                    fclose(chinfo->file);
                    chinfo->file=CreateLog(constchannel,TRUE);
                    chinfo->size=0;
                }
                else if(sev==LEVEL_FATAL)
                {
                    fflush(chinfo->file);
                }
            }
        }

        if(chinfo->flag&OUTMONITOR_ENABLE)
        {
			
			char* pbuf=(char*)malloc(streamsize+1);
			strncpy(pbuf,pstrstream,streamsize);
			pbuf[streamsize]=0;
            Log_PrintLog(constchannel,pbuf,sev);
                
        }
    }
}

LPCHANNELINFO CLogServer::FindChannel(const char* channel,const char** constchannel)
{
    LOCKSCOPE(crit_);

    std::map<std::string,LPCHANNELINFO>::iterator channeliter=channel_.find(channel);

    if(channeliter!=channel_.end())
    {
        if(constchannel)
            *constchannel=channeliter->first.c_str();
        return channeliter->second;
    }

    return AddChannel(channel,constchannel);
}

LPCHANNELINFO CLogServer::AddChannel(const char* channel,const char** constchannel)
{
    LOCKSCOPE(crit_);

    LPCHANNELINFO chinfo= new CHANNELINFO;
    memset(chinfo,0,sizeof(CHANNELINFO));

    chinfo->file=CreateLog(channel,TRUE);
    _ASSERT_EX(chinfo->file,_T("CreateLog Fail"));

    chinfo->sev=LEVEL_SENSITIVE;
    chinfo->size=0;
    chinfo->flag|=OUTFILE_ENABLE;
    chinfo->total=0;
    //channel_[channel]=chinfo;

    typedef std::pair<std::string,LPCHANNELINFO> PairChannel;
    std::pair<std::map<std::string,LPCHANNELINFO>::iterator, bool > pr=channel_.insert(PairChannel(channel,chinfo));

    if(pr.second)
    {
        if(constchannel)
            *constchannel=(pr.first)->first.c_str();
        return chinfo;

    }
    _ASSERT_EX(0,_T("insert map Fial"));

     return NULL;

}

FILE*         CLogServer::CreateLog(const char* channel,BOOL format)
{
    char fullpath[MAX_PATH]="\0";

    if(format)
    {
        time_t t=time(NULL);
        struct tm *area = localtime(&t); 

        for(int i=0;i<=1000;++i)
        {
            _ASSERT_EX(i<1000,_T("文件名用完"));
            sprintf(fullpath,"%s%s-%02d%02d-%03d.log",directorypath_,channel,area->tm_mon+1,area->tm_mday,i);
            if(access(fullpath,0)==-1)
            {
                break;
            }
        }

    }
    else
    {
        strncpy(fullpath,directorypath_,MAX_PATH);
        strcat(fullpath,channel);

        char Unique[20];
        itoa((int)time(0),Unique,10);
        strcat(fullpath,"-");
        strcat(fullpath,Unique);

        strcat(fullpath,".log");
    }


    return fopen(fullpath,"wb");
}



#pragma warning(default:4996)
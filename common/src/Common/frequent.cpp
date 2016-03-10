/********************************************************************
	created:	2008/12/06
	created:	6:12:2008   21:11
	filename: 	f:\xugood.comm\src\Common\frequent.cpp
	file path:	f:\xugood.comm\src\Common
	file base:	frequent
	file ext:	cpp
	author:		xugood@live.cn
	
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "frequent.h"
#include <Windows.h>
#include <sys/timeb.h>

#include <stdio.h>

#include <objbase.h>

#include <sys/stat.h>

#pragma warning( disable : 4996)

void COMMON::time64(__int64 *t)
{
	struct _timeb timebuffer;
	_ftime( &timebuffer );
	 *t=(__int64)timebuffer.time*1000+timebuffer.millitm;
}

__int64 COMMON::time64()
{
	struct _timeb timebuffer;
	_ftime( &timebuffer );
	return (__int64)timebuffer.time*1000+timebuffer.millitm;

}


Millisecond COMMON::GetMillisecond()
{
#if _WIN32_WINNT>=0x0600 || WINVER>=0x0600
	return ::GetTickCount64();
#else
	/*
	DWORD dwver=GetVersion();
	WORD wver=((WORD)(BYTE)dwver)*10;
	wver+=(WORD)dwver>>8;

	//DWORD winVer=GetVersion();
	//DWORD dwMajorVersion=(DWORD)(LOBYTE(LOWORD(winVer)));
	//DWORD dwMinorVersion=(DWORD)(HIBYTE(LOWORD(winVer)));  

	if(wver>=60)
	{
		HMODULE hmodule=::LoadLibrary(_T("Kernel32.dll"));
		typedef ULONGLONG (WINAPI *tGetTickCount64)(VOID);
		tGetTickCount64 pGetTickCount64 = (tGetTickCount64)::GetProcAddress(hmodule,"GetTickCount64");
		ULONGLONG  t= pGetTickCount64();
		::FreeLibrary(hmodule);
		return t;
	}
	else
	{
		struct _timeb timebuffer;
		_ftime( &timebuffer );
		return timebuffer.time*1000+timebuffer.millitm;
	}
	*/

	struct _timeb timebuffer;
	_ftime( &timebuffer );
	return timebuffer.time*1000+timebuffer.millitm;

#endif
}

BSTR COMMON::FormatErrorMsg(HRESULT hr,LPCTSTR errmsg)
{
    LPVOID lpMsgBuf;
    FormatMessage( 
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM | 
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL 
        );


    LPTSTR punionbuf=new TCHAR[_tcslen(errmsg)+_tcslen((LPCTSTR)lpMsgBuf)+3];
    _stprintf(punionbuf,_T("%s[%s]"),errmsg,lpMsgBuf);
    LocalFree(lpMsgBuf);

#if defined(UNICODE) || defined(_UNICODE)
    BSTR retbstr=SysAllocString(punionbuf);
#else
    int asize=(int)strlen(punionbuf)+1;
    int wsize=MultiByteToWideChar(CP_ACP, 0, punionbuf, asize, 0, 0 );
    wchar_t* punicode=new wchar_t[wsize];
    MultiByteToWideChar(CP_ACP, 0, punionbuf, asize, punicode, wsize );
    BSTR retbstr=SysAllocString(punicode);
    delete []punicode;
#endif
    delete []punionbuf;
    return retbstr;
}

void COMMON::ShowError(LPTSTR lpszFunction)
{ 
	TCHAR szBuf[80]; 
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError(); 

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	wsprintf(szBuf, 
		_T("%s failed with error %d: %s"), 
		lpszFunction, dw, lpMsgBuf); 

	MessageBox(NULL, szBuf, _T("Error"), MB_OK); 

	LocalFree(lpMsgBuf);
}


void COMMON::DebugTrace(LPCTSTR formatstr...)
{
	va_list argList;
	va_start( argList, formatstr);

	TCHAR buffer[1024];

	_vsntprintf(buffer,sizeof(buffer)/sizeof(TCHAR)-1,formatstr,argList);

	OutputDebugString(buffer);

	va_end( argList );


}

void COMMON::DebugFile(LPCTSTR formatstr...)
{
	va_list argList;
	va_start( argList, formatstr);

	TCHAR buffer[1024];
	_vsntprintf(buffer,sizeof(buffer)/sizeof(TCHAR)-1,formatstr,argList);


	int   bufsize=0;
	char* bufptr=NULL;

#if defined(_UNICODE) || defined(UNICODE)
	bufsize=CharWtoA(buffer,NULL,0);
	bufptr=(char*)malloc(bufsize);
	CharWtoA(buffer,bufptr,bufsize);
	
#else
	bufsize=_tcslen(buffer);
	bufptr=buffer;
#endif

	
	TCHAR filename[MAX_PATH]=_T("\0");
	if(::GetModuleFileName(::GetModuleHandleA(NULL),filename,MAX_PATH-1))
	{

		if(FullFileNameConvDirectory(filename))
		{

			if(DirectoryConvFullFileName(filename,_T("DebugFile.log")))
			{
				FILE* hfile= _tfopen(filename,_T("a"));
				if(hfile)
				{
					fwrite(bufptr,1,bufsize,hfile);
					fclose(hfile);
				}
			}


		}


	}

#if defined(_UNICODE) || defined(UNICODE)
	free(bufptr);
#endif

	va_end( argList );
}


int  COMMON::CharAtoW(const char* lpAbuffer,wchar_t *lpWbuffer,int wsize)
{
	
	return MultiByteToWideChar(CP_ACP, 0, lpAbuffer, (int)strlen(lpAbuffer)+1, lpWbuffer, wsize );

}


int  COMMON::CharWtoA(const wchar_t *lplpWbuffer,char* lpAbuffer,int asize)
{
	return WideCharToMultiByte(CP_ACP,0,lplpWbuffer,wcslen(lplpWbuffer)+1,lpAbuffer,asize,0,0);
}

BOOL COMMON::FullFileNameConvDirectory(TCHAR* szfullfilename)
{
	if(!szfullfilename) return FALSE;

	int size=_tcslen(szfullfilename);
	for(int i=size;i>0;--i)
	{
		if(szfullfilename[i]==_T('\\'))
		{
			szfullfilename[i]=0;
			return TRUE;
		}
	}

	return FALSE;

}

BOOL COMMON::DirectoryConvFullFileName(TCHAR*szdirectory,const TCHAR* szfilename)
{
	int directorysize=_tcslen(szdirectory);
	int filenamesize=_tcslen(szfilename);

	if(directorysize+filenamesize>MAX_PATH)  return FALSE;

	if(directorysize>0 && filenamesize>0)
	{
		if(szdirectory[directorysize-1]!=_T('\\'))
		{
			_tcscat(szdirectory,_T("\\"));
		}
	
		_tcscat(szdirectory,szfilename);

		return TRUE;
	}

	return FALSE;
}

BOOL COMMON::FileNameConvCurrentFullPath(TCHAR* szfullpath,const TCHAR* szfilename)
{
    GetModuleFileName(GetCurrentModule(),szfullpath,MAX_PATH);
    COMMON::FullFileNameConvDirectory(szfullpath);

    return DirectoryConvFullFileName(szfullpath,szfilename);
   
}



char* COMMON::Dump(char*bufferptr,int size,int linelen)
{
	int row=size/linelen+(size%linelen ? 1 : 0);
	int outsize=row*(linelen+linelen*3+1);

	char* outptr=(char*)malloc(outsize+1);

	int outposition=0;
	int inposition=0;
	int len=0;


	char hexbuf[5]="\0";

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




	return outptr;
}

char* COMMON::DumpStatic(char*bufferptr,int size,int linelen)
{
	static char buffer[1024*4];

	
	char* outptr=Dump(bufferptr,size,linelen);

	int   outsize=min(sizeof(buffer)-1,strlen(outptr));

	strncpy(buffer,outptr,outsize);

	buffer[outsize]=0;
	
	free(outptr);

	return buffer;

}

void COMMON::BigEndianExChangeLittleEndian(BYTE* ptr,int len)
{
	BYTE exchange=0;
	int  position=0;

	for(int i=0;i<len/2;++i)
	{
		exchange=*(ptr+i);
		position=len-i-1;
		*(ptr+i)=*(ptr+position);
		*(ptr+position)=exchange;
	}

}

void COMMON::time_tTotchar(time_t t,TCHAR* outtchar)
{
   // TCHAR timer1[256];
   // TCHAR timer2[256];
    FILETIME ft;
    LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
    ft.dwLowDateTime = (DWORD) ll;
    ft.dwHighDateTime = (DWORD)(ll >> 32);

    FILETIME localft;
    FileTimeToLocalFileTime(&ft,&localft);
    SYSTEMTIME st ;
    FileTimeToSystemTime( &localft, &st );
    _stprintf(outtchar,_T("%d-%d-%d %d:%d:%d"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);



   
}


const char* COMMON::newGUID()
{
    static char buf[64] = {0};
    GUID guid;
    CoInitialize(NULL);
    if (S_OK == ::CoCreateGuid(&guid))
    {
        _snprintf(buf, sizeof(buf),
            "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
            guid.Data1,
            guid.Data2,
            guid.Data3,
            guid.Data4[0], guid.Data4[1],
            guid.Data4[2], guid.Data4[3],
            guid.Data4[4], guid.Data4[5],
            guid.Data4[6], guid.Data4[7]);
    }
    CoUninitialize();
    return (const char*)buf;
}


int COMMON::ValidatePathA(const char*  directorypath)
{
    struct   _stat   buf;   
    int   result   =   _stat(   directorypath,   &buf   );   
    if(   result   ==   0   )   
    {   
        if   (   (buf.st_mode   &   _S_IFDIR)   ==   _S_IFDIR   )     
        {   
            //   it's   folder   
            return   TYPE_FOLDER;   
        }     
        else   if   (   (buf.st_mode   &   _S_IFREG)   ==   _S_IFREG   )     
        {   
            //   it's   file   
            return   TYPE_FILE;   
        }   
    }   
    return   TYPE_NULL;   
}



int  COMMON::CreateFolderA(const char* templedirectorypath,const char* directorypath)
{
    if(templedirectorypath)
    {
        return ::CreateDirectoryExA(templedirectorypath,directorypath,NULL);
    }
    else
    {
        return ::CreateDirectoryA(directorypath,NULL);
    }
    return 1;
}


BOOL COMMON::CreatePathA(const char* directorypath)
{
    int len=(int)strlen(directorypath);
    if(len<3) return FALSE;

    int origpos=0;


    if(directorypath[0]=='\\' && directorypath[1]=='\\')
    {
        origpos=2;
    }


    char origpathbuf[MAX_PATH]="\0";
    char newpathbuf[MAX_PATH]="\0";
    char oldpathbuf[MAX_PATH]="\0";


    strncpy(origpathbuf,directorypath,len);
    if(origpathbuf[len-1]!='\\')
    {
        origpathbuf[len]='\\';
        origpathbuf[len+1]=0;
    }

    char* psz=(char*)origpathbuf+origpos;

    while(*psz)
    {
        if(*psz=='\\')
        {

            if(oldpathbuf[0]==0)
            {
                strncpy(oldpathbuf,origpathbuf,psz-origpathbuf);
            }
            else
            {
                strncpy(newpathbuf,origpathbuf,psz-origpathbuf);

                if(ValidatePathA(newpathbuf)!=TYPE_FOLDER)
                {
                    if(!CreateFolderA(NULL,newpathbuf))
                    {
                        _ASSERT_EX(0,_T("CreateFolderA"));

                    }
                }
                strncpy(oldpathbuf,newpathbuf,strlen(newpathbuf));
            }
        }

        psz++;
    }



    return TRUE;


}


#if   _MSC_VER   >=   1300   
#ifndef   _delayimp_h   
extern   "C"   IMAGE_DOS_HEADER   __ImageBase;   
#endif   
#endif   

HMODULE   COMMON::GetCurrentModule()
{   
#if   _MSC_VER   <   1300    

	//   Here's   a   trick   that   will   get   you   the   handle   of   the   module   
	//   you're   running   in   without   any   a-priori   knowledge:   
	//   http://www.dotnet247.com/247reference/msgs/13/65259.aspx   

	MEMORY_BASIC_INFORMATION   mbi;   
	static   int   dummy;   
	VirtualQuery(   &dummy,   &mbi,   sizeof(mbi)   );   
	return   reinterpret_cast<HMODULE>(mbi.AllocationBase);   
#else 
	return   reinterpret_cast<HMODULE>(&__ImageBase);   
#endif   

}   




#pragma warning( default : 4996)
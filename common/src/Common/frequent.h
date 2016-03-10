/********************************************************************
	created:	2008/12/06
	created:	6:12:2008   21:09
	filename: 	f:\xugood.comm\src\Common\frequent.h
	file path:	f:\xugood.comm\src\Common
	file base:	frequent
	file ext:	h
	author:		xugood@live.cn
	
	purpose:	
*********************************************************************/

#pragma once

#include <Windows.h>
#include <tchar.h>

#include <comdef.h>

#include <crtdbg.h>

#include <string>


#define		TYPE_FOLDER		2			//文件夹
#define		TYPE_FILE		1			//文件
#define		TYPE_NULL		0			//不存在


#ifdef _DEBUG


	#if _MSC_VER>1200

		#define	_ASSERT_EXA(expr,...) \
			(void) ((!!(expr)) || \
			(1 != _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, __VA_ARGS__)) || \
			(_CrtDbgBreak(), 0))

		#define	_ASSERT_EXW(expr,...) \
			(void) ((!!(expr)) || \
			(1 != _CrtDbgReportW(_CRT_ASSERT, _CRT_WIDE(__FILE__), __LINE__, NULL, __VA_ARGS__)) || \
			(_CrtDbgBreak(), 0))

	#else

		#if __INTEL_COMPILER

			#define	_ASSERT_EXA(expr,...) \
				do { if (!(expr) && \
				(1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, __VA_ARGS__))) \
				_CrtDbgBreak(); } while (0)

			#define	_ASSERT_EXW(expr,...) \
				do { if (!(expr) && \
				(1 == _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, NULL))) \
				_CrtDbgBreak(); } while (0)
		#else
			#define	_ASSERT_EXA(expr) 
			#define	_ASSERT_EXW(expr)
		#endif

	#endif


	
#if defined(__INTEL_COMPILER) || _MSC_VER>1200
	#if defined(_UNICODE) || defined(UNICODE)
	#define	_ASSERT_EX(expr,...) _ASSERT_EXW(expr,__VA_ARGS__)
	#else
	#define	_ASSERT_EX(expr,...) _ASSERT_EXA(expr,__VA_ARGS__)
	#endif
#else
	#if defined(_UNICODE) || defined(UNICODE)
	#define	_ASSERT_EX(expr) _ASSERT_EXW(expr)
	#else
	#define	_ASSERT_EX(expr) _ASSERT_EXA(expr)
	#endif
#endif



#else

	#if defined(__INTEL_COMPILER) || _MSC_VER>1200
		#define	_ASSERT_EXA(expr,...) 
		#define	_ASSERT_EXW(expr,...)
		#define	_ASSERT_EX(expr,...)
	#else
		#define	_ASSERT_EXA(expr,__VA_ARGS__) 
		#define	_ASSERT_EXW(expr,__VA_ARGS__)
		#define	_ASSERT_EX(expr,__VA_ARGS__)
	#endif

#endif

#if defined(__INTEL_COMPILER) || _MSC_VER>1200
	#ifdef _DEBUG
		#define 	_TRACE(format,...)		COMMON::DebugTrace(format,__VA_ARGS__)
	#else
		#define		_TRACE(format,...)
	#endif
#else
	#define		_TRACE(format,__VA_ARGS__)
#endif



#if defined(_UNICODE) || defined(UNICODE)
typedef std::wstring        tstring;
#else
typedef std::string         tstring;
#endif




typedef ULONGLONG	Millisecond;




namespace COMMON
{
	void time64(__int64* t);
	__int64 time64();

	Millisecond GetMillisecond();


    BSTR FormatErrorMsg(HRESULT hr,LPCTSTR errmsg);
	void ShowError(LPTSTR lpszFunction) ;

	void DebugTrace(LPCTSTR formatstr...);
	void DebugFile(LPCTSTR formatstr...);


	int  CharAtoW(const char* lpAbuffer,wchar_t *lplpWbuffer,int wsize);
	int  CharWtoA(const wchar_t *lplpWbuffer,char* lpAbuffer,int asize);


	BOOL FullFileNameConvDirectory(TCHAR* szfullfilename);
	BOOL DirectoryConvFullFileName(TCHAR*szdirectory,const TCHAR* szfilename);
    BOOL FileNameConvCurrentFullPath(TCHAR* szfullpath,const TCHAR* szfilename);

	char* Dump(char*bufferptr,int size,int linelen=20);
	char* DumpStatic(char*bufferptr,int size,int linelen=20);

	//大头字节转小头字节(网络字节到主机字节)
	void BigEndianExChangeLittleEndian(BYTE* ptr,int len);

	



    void time_tTotchar(time_t t,TCHAR* outtchar);

    const char* newGUID();


    int  ValidatePathA(const char*  directorypath);
    int  CreateFolderA(const char* templedirectorypath,const char* directorypath);
    BOOL CreatePathA(const char* directorypath);


	HMODULE GetCurrentModule();








    class UNICODEChar
    {
    private:
        UNICODEChar():pascii(NULL){}
    public:

        UNICODEChar(const wchar_t* punicode):pascii(NULL)
        {
            if(punicode)
            {
                int wsize=(int)wcslen(punicode)+1;
                int asize=WideCharToMultiByte(CP_ACP,0,punicode,wsize,0,0,0,0);
                pascii=new char[asize];
                WideCharToMultiByte(CP_ACP,0,punicode,wsize,pascii,asize,0,0);
            }
            else
            {
                pascii=new char;
                *pascii=0;
            }
        }
        ~UNICODEChar()
        {
            if(pascii)
            {
                delete []pascii;
            }
        }

        operator LPCSTR()
        {
            return (LPCSTR)pascii;
        }

    private:
        char* pascii;
    };

    class ASCIIChar
    {
    private:
        ASCIIChar():punicode(NULL){}

    public:
        ASCIIChar(const char* pascii):punicode(NULL)
        {
            if(pascii)
            {
                int asize=(int)strlen(pascii)+1;
                int wsize=MultiByteToWideChar(CP_ACP, 0, pascii, asize, 0, 0 );
                punicode=new wchar_t[wsize];
                MultiByteToWideChar(CP_ACP, 0, pascii, asize, punicode, wsize );
            }
            else
            {
                punicode=new wchar_t;
                *punicode=0;
            }
        }
        ~ASCIIChar()
        {
            if(punicode)
            {
                delete []punicode;
            }

        }

        operator LPCWSTR()
        {
            return (LPCWSTR)punicode;
        }

    private:
        wchar_t*    punicode;
    };






	typedef struct CharPtrCmp
	{
		bool operator()( const char * s1, const char * s2 ) const
		{
			return strcmp( s1, s2 ) < 0;
		}
	}*pCharPtrCmp;


	typedef struct TCharPtrCmp
	{
		bool operator()( const TCHAR * s1, const TCHAR * s2 ) const
		{
			return _tcscmp( s1, s2 ) < 0;
		}
	}*pTCharPtrCmp;

}
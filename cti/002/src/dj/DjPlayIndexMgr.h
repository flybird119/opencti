#pragma once

#include <vector>
#include <map>
#include <algorithm>


class CDjPlayIndexMgr
{
public:

	CDjPlayIndexMgr(void)
	{
	}

	~CDjPlayIndexMgr(void)
	{
		while(!m_index.empty())
		{
			PopIndex();
		}
	}

private:

	struct IndexInfo
	{
		IndexInfo(HANDLE hfile_,HANDLE hmapping_,char* lpbuffer_):
		hfile(hfile_),
		hmapping(hmapping_),
		lpbuffer(lpbuffer_)
		{

		}
		HANDLE	hfile;
		HANDLE  hmapping;
		char*   lpbuffer;
	};

	std::vector<IndexInfo> m_index;
	std::map<LPCTSTR,int,COMMON::TCharPtrCmp>  m_alias;

public:

	char* AddIndex(LPCTSTR fullpath,LPCTSTR Alias,int& size)
	{
		if(m_alias.find(Alias)!=m_alias.end())
		{
			return NULL;
		}

		HANDLE hfile=::CreateFile(fullpath,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
		if(hfile!=INVALID_HANDLE_VALUE)
		{
			HANDLE hmapping=::CreateFileMapping(hfile,NULL,PAGE_READWRITE,0,0,NULL);
			if(hmapping!=0)
			{
				char* lpbuffer=(char*)::MapViewOfFile(hmapping,FILE_MAP_WRITE,0,0,0);
				if(lpbuffer!=NULL)
				{
					m_index.push_back(IndexInfo(hfile,hmapping,lpbuffer));

					m_alias[Alias]=m_index.size()-1;

					size=GetFileSize(hfile,0);
					return lpbuffer;
				}
			}
		}


		return NULL;

	}


	int   GetIndex(LPCTSTR Alias)
	{
		std::map<LPCTSTR,int,COMMON::TCharPtrCmp>::iterator iter = m_alias.find(Alias);
		if(iter!=m_alias.end())
		{
			return iter->second;
		}

		return -1;
	}

	void   PopIndex()
	{

		for(std::map<LPCTSTR,int,COMMON::TCharPtrCmp>::iterator iter=m_alias.begin();iter!=m_alias.end();++iter)
		{
			if(iter->second==m_index.size()-1)
			{
				m_alias.erase(iter);
				break;
			}
		}

		IndexInfo& index=m_index.back();
		::UnmapViewOfFile(index.lpbuffer);
		::CloseHandle(index.hmapping);
		::CloseHandle(index.hfile);

		m_index.pop_back();

	}
};



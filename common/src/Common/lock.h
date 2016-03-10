/********************************************************************
	created:	2009/04/12
	created:	12:4:2009   14:03
	filename: 	e:\project\xugood.comm\src\Common\lock.h
	file path:	e:\project\xugood.comm\src\Common
	file base:	lock
	file ext:	h
	author:		xugood@live.cn
	
	purpose:	
*********************************************************************/

#pragma once

#include <assert.h>
#include <TCHAR.h>
#include <Windows.h>
#include ".\common\ShareRestrictedSD.h"




#define LOCKSCOPE(...) COMMON::LockScope lockscope_(__VA_ARGS__)





namespace COMMON
{





enum LOCKENTITY
{
    MUTEX,
    CRITICAl,
};

class CLock
{
public:

	CLock(LOCKENTITY entity,UINT timeout=100)
        :entity_(entity)
        ,timeout_(timeout)
        ,ref_(0)
        ,mutex_(0)
	{
        if(entity_==MUTEX)
        {
           mutex_=::CreateMutex(NULL,FALSE,NULL);
           assert(mutex_);
        }
        else if(entity_==CRITICAl)
        {
           InitializeCriticalSectionAndSpinCount(&crit_,4000);

        }

	}


	CLock(const TCHAR* szLockName,UINT timeout=100)
	{
        entity_=MUTEX;
        mutex_=::CreateMutex(CShareRestrictedSD::GetInstance()->GetSA(),FALSE,szLockName);
		assert(mutex_);

	}

	~CLock()
	{
        if(entity_==MUTEX)
        {  
            CloseHandle(mutex_);
        }
        else if(entity_==CRITICAl)
        {
            DeleteCriticalSection(&crit_);
        }
	}


	int Lock(UINT timeout=0)
	{
        int ret=0;

        if(entity_==MUTEX)
        {  
            switch(::WaitForSingleObject(mutex_,timeout ? timeout : timeout_))
            {
            case WAIT_ABANDONED:
            case WAIT_OBJECT_0:
                ret=1;
                break;
            case WAIT_FAILED:
            case WAIT_TIMEOUT:
                ret=0;
                break;
            }
        }
        else if(entity_==CRITICAl)
        {
            EnterCriticalSection(&crit_);
            ++ref_;

            ret=1;
        }




		return ret;
	}

	int UnLock()
	{
       
        int ret=0;

        if(entity_==MUTEX)
        {  
            if(ReleaseMutex(mutex_))
            {
                ret=1;
            }
        }
        else if(entity_==CRITICAl)
        {
            LeaveCriticalSection(&crit_);
            --ref_;
            ret=1;           
        }

        return ret;

	}



	int IsLock()
	{
        int ret=0;

        if(entity_==MUTEX)
        {  

            switch(::WaitForSingleObject(mutex_,0))
            {
            case WAIT_TIMEOUT:
                ret=1;
                break;

            case WAIT_ABANDONED:
            case WAIT_OBJECT_0:
                ::ReleaseMutex(mutex_);
            case WAIT_FAILED:
                ret=0;
                break;
           
            }


        }
        else if(entity_==CRITICAl)
        {
            if(TryEnterCriticalSection(&crit_))
            {
                LeaveCriticalSection(&crit_);
                ret=0;
            }
            else
            {
                ret=1;
            }

           
        }

        return ret;

	}


private:

	HANDLE			    mutex_;
    CRITICAL_SECTION    crit_;
    

    LOCKENTITY          entity_;
    UINT			    timeout_;

    volatile long       ref_;



};

class LockScope {
public:
    LockScope(CLock& lock,UINT timeout=0) 
    {
        plock_ = &lock;
        plock_->Lock(timeout);
    }

    ~LockScope() 
    {
       plock_->UnLock();
    }

private:
    CLock *plock_;

};







}
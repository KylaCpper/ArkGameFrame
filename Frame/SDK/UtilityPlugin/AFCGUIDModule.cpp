/*
* This source file is part of ArkGameFrame
* For the latest info, see https://github.com/ArkGame
*
* Copyright (c) 2013-2017 ArkGame authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#include "AFCGUIDModule.h"

namespace GUIDModule
{
//refer:https://github.com/nebula-im/snowflake4cxx
//�޸�Ϊʱ��� * 100008 + sequence

#if ARK_PLATFORM == PLATFORM_WIN
# include <windows.h>
# include <winsock2.h>
# include <time.h>
#else
# include <sys/time.h>
#endif

#if ARK_PLATFORM == PLATFORM_WIN
int gettimeofday(struct timeval* tp, void *tzp)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm.tm_isdst = -1;
    clock = mktime(&tm);
    tp->tv_sec = clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return (0);
}
#endif

uint64_t GetNowInMsec()
{
    struct timeval tv;
    int  nRet = gettimeofday(&tv, 0);
    return uint64_t(tv.tv_sec) * 1000 + tv.tv_usec / 1000;
}

uint64_t WaitUntilNextMillis(uint64_t last_timestamp)
{
    uint64_t timestamp = GetNowInMsec();
    while(timestamp <= last_timestamp)
    {
        timestamp = GetNowInMsec();
    }
    return timestamp;
}

class IdWorkerUnThreadSafe
{
public:
    IdWorkerUnThreadSafe(){}

    uint64_t GetNextID()
    {
        uint64_t timestamp = GetNowInMsec();

        //in current microsecond
        if(last_timestamp_ == timestamp)
        {
            sequence_ += 1;
            if(sequence_ == 0)
            {
                timestamp = WaitUntilNextMillis(last_timestamp_);
            }
        }
        else
        {
            sequence_ = 0;
        }

        last_timestamp_ = timestamp;
        return ((timestamp - ARK_EPOCH) * ARK_GUID_POWER + sequence_);
    }

protected:
    uint64_t last_timestamp_{ 0 };
    uint32_t sequence_{ 0 };
};

class IdWorkerThreadSafe
{
public:
    IdWorkerThreadSafe() {}

    uint64_t GetNextID()
    {
        std::lock_guard<std::mutex> g(lock_);
        return id_worker_.GetNextID();
    }

protected:
    IdWorkerUnThreadSafe id_worker_;
    std::mutex lock_;
};

}

//////////////////////////////////////////////////////////////////////////

AFCGUIDModule::AFCGUIDModule(AFIPluginManager* p)
{
    pPluginManager = p;
}

bool AFCGUIDModule::Init()
{
    return true;
}

bool AFCGUIDModule::AfterInit()
{
    return true;
}

bool AFCGUIDModule::Execute()
{
    return true;
}

bool AFCGUIDModule::BeforeShut()
{
    if(NULL != m_pIDWoker)
    {
        delete m_pIDWoker;
        m_pIDWoker = NULL;
    }

    return true;
}

bool AFCGUIDModule::Shut()
{
    return true;
}

void AFCGUIDModule::SetGUIDMask(uint64_t mask)
{
    mnMask = mask;
}

AFGUID AFCGUIDModule::CreateGUID()
{
    assert(NULL != m_pIDWoker);

    uint64_t nLow = m_pIDWoker->GetNextID();
    AFGUID newGUID(mnMask, nLow);

    return newGUID;
}

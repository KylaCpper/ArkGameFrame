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

#pragma once

#include "AFPlatform.hpp"
#include "AFMacros.hpp"
#include "AFArrayPod.hpp"
#include "AFStringPod.hpp"
#include "AFGUID.h"

//////////////////////////////////////////////////////////////////////////
template<typename T>
class IsBuildinType
{
public:
    enum {YES = false, NO = true};
};

#define MAKE_BUILDIN_TYPE(T)                \
    template<> class IsBuildinType<T>       \
    {                                       \
    public:                                 \
        enum { YES = true, NO = false };    \
    };

template <typename T>
bool CheckBuildinType(const T& t)
{
    return IsBuildinType<T>::YES ? true : false;

    //if (IsBuildinType<T>::YES)
    //{
    //    std::cout << typename(T)  << " is BuildIn type" << std::endl;
    //}
    //else
    //{
    //    std::cout << typename(T) << " is BuildIn type" << std::endl;
    //}
}

MAKE_BUILDIN_TYPE(uint16_t)
MAKE_BUILDIN_TYPE(uint32_t)
MAKE_BUILDIN_TYPE(uint64_t)
MAKE_BUILDIN_TYPE(int16_t)
MAKE_BUILDIN_TYPE(int32_t)
MAKE_BUILDIN_TYPE(int64_t)
MAKE_BUILDIN_TYPE(AFGUID)

//////////////////////////////////////////////////////////////////////////

template<typename T, typename NODE>
class AFArrayMap {};

//partial template specialization for T=BUILD-IN types
//build-in type is defined above
//template<typename T, typename NODE>
//class AFArrayMap<T, NODE>
//{
//public:
//protected:
//private:
//    ArraryPod<NODE*, 1, CoreAlloc> mxNodes;
//    //StringPod<char, size_t, StringTraits<char>> mxIndices;
//};

//partial template specialization for KEY=std::string
template<typename NODE>
class AFArrayMap<std::string, NODE>
{
public:
    AFArrayMap()
    {

    }

    ~AFArrayMap()
    {
        ReleaseAll();
        mxIndices.Clear();
    }

    void Clear()
    {
        ReleaseAll();
        mxIndices.Clear();
    }

    bool AddElement(const std::string& name, NODE* data)
    {
        mxIndices.Add(name.c_str(), mxNodes.size());
        mxNodes.push_back(data);
        return true;
    }

    NODE* GetElement(const std::string& name)
    {
        size_t index;
        if (!mxIndices.GetData(name.c_str(), index))
        {
            return NULL;
        }

        return mxNodes[index];
    }

    NODE* operator[](size_t index)
    {
        ARK_ASSERT_RET_VAL(index < GetCount(), NULL);

        return mxNodes[index];
    }

    bool RemoveElement(const std::string& name)
    {
        size_t index;
        if (!mxIndices.GetData(name.c_str(), index))
        {
            return false;
        }
        
        //�ͷ��ڴ�
        if (NULL != mxNodes[index])
        {
            delete mxNodes[index];
            mxNodes[index] = NULL;
        }

        mxNodes.remove(index);
        mxIndices.Remove(name.c_str());
        return true;
    }

    size_t GetCount() const
    {
        return mxNodes.size();
    }

protected:
    void ReleaseAll()
    {
        for (size_t i = 0; i < GetCount(); ++i)
        {
            delete mxNodes[i];
            mxNodes[i] = NULL;
        }

        mxNodes.clear();
    }

private:
    ArraryPod<NODE*, 1, CoreAlloc> mxNodes;
    StringPod<char, size_t, StringTraits<char>> mxIndices;
};

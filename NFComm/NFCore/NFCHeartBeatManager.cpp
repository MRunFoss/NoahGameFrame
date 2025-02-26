// -------------------------------------------------------------------------
//    @FileName         :    NFCHeartBeatManager.cpp
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-12-17
//    @Module           :    NFCHeartBeatManager
//
// -------------------------------------------------------------------------

#include "NFCHeartBeatManager.h"

NFCHeartBeatManager::~NFCHeartBeatManager()
{
    mHeartBeatElementMapEx.ClearAll();
}

void NFCHeartBeatElement::DoHeartBeatEvent()
{
    HEART_BEAT_FUNCTOR_PTR cb;
    bool bRet = First(cb);
    while (bRet)
    {
        cb.get()->operator()(self, strBeatName, fBeatTime, nCount);

        bRet = Next(cb);
    }
}
//////////////////////////////////////////////////////////////////////////
bool NFCHeartBeatManager::Execute(const float fLastTime, const float fAllTime)
{
    NF_SHARE_PTR<NFCHeartBeatElement> pElement = mHeartBeatElementMapEx.First();
    while (pElement.get())
    {
        int nCount = pElement->nCount;
        float fTime = pElement->fTime;

        if (fTime > 0.000000f)
        {
            fTime -= fLastTime;
        }

        if (fTime <= 0.001f)
        {
            nCount--;
        }
        else
        {
            pElement->fTime = fTime;
        }

        if (nCount <= 0)
        {
            //Do Event
            pElement->DoHeartBeatEvent();

            //等待删除
            mRemoveListEx.Add(pElement->strBeatName);
        }
        else
        {
            if (pElement->nCount != nCount)
            {
                pElement->nCount = nCount;
                //Do Event
                pElement->DoHeartBeatEvent();
                pElement->fTime = pElement->fBeatTime;
            }
        }

        pElement = mHeartBeatElementMapEx.Next();
    }

    //删除所有过时心跳
    std::string strHeartBeatName;
    bool bRet = mRemoveListEx.First(strHeartBeatName);
    while (bRet)
    {
        mHeartBeatElementMapEx.RemoveElement(strHeartBeatName);

        bRet = mRemoveListEx.Next(strHeartBeatName);
    }

    mRemoveListEx.ClearAll();

    //////////////////////////////////////////////////////////////////////////
    //添加新心跳也是延时添加的
    for (std::list<NFCHeartBeatElement>::iterator iter = mAddListEx.begin(); iter != mAddListEx.end(); ++iter)
    {
        if (NULL == mHeartBeatElementMapEx.GetElement(iter->strBeatName))
        {
            NF_SHARE_PTR<NFCHeartBeatElement> pHeartBeatEx(NF_NEW NFCHeartBeatElement());
            *pHeartBeatEx = *iter;
            mHeartBeatElementMapEx.AddElement(pHeartBeatEx->strBeatName, pHeartBeatEx);
        }
    }

    mAddListEx.clear();

    return true;
}

bool NFCHeartBeatManager::RemoveHeartBeat(const std::string& strHeartBeatName)
{
    return mRemoveListEx.Add(strHeartBeatName);
}

NFIDENTID NFCHeartBeatManager::Self()
{
    return mSelf;
}

//////////////////////////////////////////////////////////////////////////
bool NFCHeartBeatManager::AddHeartBeat(const NFIDENTID self, const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const float fTime, const int nCount)
{
    NFCHeartBeatElement xHeartBeat;
    xHeartBeat.fTime = fTime;
    xHeartBeat.fBeatTime = fTime;
    xHeartBeat.nCount = nCount;
    xHeartBeat.self = self;
    xHeartBeat.strBeatName = strHeartBeatName;
    xHeartBeat.Add(cb);
    mAddListEx.push_back(xHeartBeat);

    return true;
}

bool NFCHeartBeatManager::Exist(const std::string& strHeartBeatName)
{
    if (mHeartBeatElementMapEx.GetElement(strHeartBeatName))
    {
        return true;
    }

    return false;
}

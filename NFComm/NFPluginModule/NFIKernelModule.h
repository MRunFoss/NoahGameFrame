// -------------------------------------------------------------------------
//    @FileName         :    NFIKernelModule.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2012-12-15
//    @Module           :    NFIKernelModule
//
// -------------------------------------------------------------------------

#ifndef _NFI_KERNEL_MODULE_H_
#define _NFI_KERNEL_MODULE_H_

#include <iostream>
#include <string>
#include <functional>
#include "NFILogicModule.h"
#include "NFComm/NFCore/NFIObject.h"
#include "NFComm/NFPluginModule/NFIdentID.h"

class NFIKernelModule
    : public NFILogicModule
{

public:

    template<typename BaseType>
    bool AddHeartBeat(const NFIDENTID self, const std::string& strHeartBeatName, BaseType* pBase, int (BaseType::*handler)(const NFIDENTID&, const std::string&, const float, const int), const float fTime, const int nCount)
    {
        HEART_BEAT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        HEART_BEAT_FUNCTOR_PTR functorPtr(new HEART_BEAT_FUNCTOR(functor));
        return AddHeartBeat(self, strHeartBeatName, functorPtr, fTime, nCount);
    }

    virtual bool FindHeartBeat(const NFIDENTID& self, const std::string& strHeartBeatName) = 0;

    virtual bool RemoveHeartBeat(const NFIDENTID& self, const std::string& strHeartBeatName) = 0;

    template<typename BaseType>
    bool AddRecordCallBack(const NFIDENTID& self, const std::string& strRecordName, BaseType* pBase, int (BaseType::*handler)(const NFIDENTID&, const RECORD_EVENT_DATA&, const NFIDataList&, const NFIDataList&))
    {
        RECORD_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        RECORD_EVENT_FUNCTOR_PTR functorPtr(new RECORD_EVENT_FUNCTOR(functor));
        return AddRecordCallBack(self, strRecordName, functorPtr);
    }

    template<typename BaseType>
    bool AddPropertyCallBack(const NFIDENTID& self, const std::string& strPropertyName, BaseType* pBase, int (BaseType::*handler)(const NFIDENTID&, const std::string&, const NFIDataList&, const NFIDataList&))
    {
        PROPERTY_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        PROPERTY_EVENT_FUNCTOR_PTR functorPtr(new PROPERTY_EVENT_FUNCTOR(functor));
        return AddPropertyCallBack(self, strPropertyName, functorPtr);
    }

    ////////////////event//////////////////////////////////////////////////////////
    template<typename BaseType>
    bool AddEventCallBack(const NFIDENTID& self, const int nEventID, BaseType* pBase, int (BaseType::*handler)(const NFIDENTID&, const int, const NFIDataList&))
    {
        EVENT_PROCESS_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        EVENT_PROCESS_FUNCTOR_PTR functorPtr(new EVENT_PROCESS_FUNCTOR(functor));
        return AddEventCallBack(self, nEventID, functorPtr);
    }

    template<typename BaseType>
    bool AddClassCallBack(const std::string& strClassName, BaseType* pBase, int (BaseType::*handler)(const NFIDENTID&, const std::string&, const CLASS_OBJECT_EVENT, const NFIDataList&))
    {
        CLASS_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        CLASS_EVENT_FUNCTOR_PTR functorPtr(new CLASS_EVENT_FUNCTOR(functor));
        return AddClassCallBack(strClassName, functorPtr);
    }
    //////////////////////////////////////////////////////////////////////////
    //只能网络模块注册，回调用来同步对象类事件,所有的类对象都会回调
    template<typename BaseType>
    bool ResgisterCommonClassEvent(BaseType* pBase, int (BaseType::*handler)(const NFIDENTID&, const std::string&, const CLASS_OBJECT_EVENT, const NFIDataList&))
    {
        CLASS_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        CLASS_EVENT_FUNCTOR_PTR functorPtr(new CLASS_EVENT_FUNCTOR(functor));
        return ResgisterCommonClassEvent(functorPtr);
    }

    //只能网络模块注册，回调用来同步对象属性事件,所有的类属性都会回调
    template<typename BaseType>
    bool ResgisterCommonPropertyEvent(BaseType* pBase, int (BaseType::*handler)(const NFIDENTID&, const std::string&, const NFIDataList&, const NFIDataList&))
    {
        PROPERTY_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        PROPERTY_EVENT_FUNCTOR_PTR functorPtr(new PROPERTY_EVENT_FUNCTOR(functor));
        return ResgisterCommonPropertyEvent(functorPtr);
    }

    //只能网络模块注册，回调用来同步对象类表事件,所有的类表都会回调
    template<typename BaseType>
    bool ResgisterCommonRecordEvent(BaseType* pBase, int (BaseType::*handler)(const NFIDENTID&, const RECORD_EVENT_DATA&, const NFIDataList&, const NFIDataList&))
    {
        RECORD_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        RECORD_EVENT_FUNCTOR_PTR functorPtr(new RECORD_EVENT_FUNCTOR(functor));
        return ResgisterCommonRecordEvent(functorPtr);
    }
   
    /////////////////////////////////////////////////////////////////

    virtual int Command(const NFIDataList& var) = 0;

    virtual bool IsContainer(const NFIDENTID& self) = 0;

    virtual bool ExistContainer(const int nContainerIndex) = 0;

    virtual NF_SHARE_PTR<NFIObject> GetObject(const NFIDENTID& ident) = 0;

    virtual NF_SHARE_PTR<NFIObject> CreateObject(const NFIDENTID& self, const int nContainerID, const int nGroupID, const std::string& strClassName, const std::string& strConfigIndex, const NFIDataList& arg) = 0;

    virtual bool DestroyObject(const NFIDENTID& self) = 0;

    virtual bool DestroyAll() = 0;

    virtual bool SetComponentEnable(const NFIDENTID& self, const std::string& strComponentName, const bool bEnable) = 0;
    virtual bool QueryComponentEnable(const NFIDENTID& self, const std::string& strComponentName) = 0;

    virtual bool FindProperty(const NFIDENTID& self, const std::string& strPropertyName) = 0;

    virtual bool SetPropertyInt(const NFIDENTID& self, const std::string& strPropertyName, const NFINT64 nValue) = 0;
    virtual bool SetPropertyFloat(const NFIDENTID& self, const std::string& strPropertyName,  const float fValue) = 0;
    virtual bool SetPropertyDouble(const NFIDENTID& self, const std::string& strPropertyName, const double dValue) = 0;
    virtual bool SetPropertyString(const NFIDENTID& self, const std::string& strPropertyName, const std::string& strValue) = 0;
    virtual bool SetPropertyObject(const NFIDENTID& self, const std::string& strPropertyName, const NFIDENTID& objectValue) = 0;

    virtual NFINT64 GetPropertyInt(const NFIDENTID& self, const std::string& strPropertyName) = 0;
    virtual float GetPropertyFloat(const NFIDENTID& self, const std::string& strPropertyName) = 0;
    virtual double GetPropertyDouble(const NFIDENTID& self, const std::string& strPropertyName) = 0;
    virtual const std::string& GetPropertyString(const NFIDENTID& self, const std::string& strPropertyName) = 0;
    virtual NFIDENTID GetPropertyObject(const NFIDENTID& self, const std::string& strPropertyName) = 0;

    virtual NF_SHARE_PTR<NFIRecord> FindRecord(const NFIDENTID& self, const std::string& strRecordName) = 0;

    virtual bool ClearRecord(const NFIDENTID& self, const std::string& strRecordName) = 0;

    virtual bool SetRecordInt(const NFIDENTID& self, const std::string& strRecordName, const int nRow, const int nCol, const NFINT64 nValue) = 0;
    virtual bool SetRecordFloat(const NFIDENTID& self, const std::string& strRecordName, const int nRow, const int nCol,  const float fValue) = 0;
    virtual bool SetRecordDouble(const NFIDENTID& self, const std::string& strRecordName, const int nRow, const int nCol, const double dwValue) = 0;
    virtual bool SetRecordString(const NFIDENTID& self, const std::string& strRecordName, const int nRow, const int nCol, const std::string& strValue) = 0;
    virtual bool SetRecordObject(const NFIDENTID& self, const std::string& strRecordName, const int nRow, const int nCol, const NFIDENTID& objectValue) = 0;
    
    virtual bool SetRecordInt(const NFIDENTID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFINT64 value) = 0;
    virtual bool SetRecordFloat(const NFIDENTID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const float value) = 0;
    virtual bool SetRecordDouble(const NFIDENTID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const double value) = 0;
    virtual bool SetRecordString(const NFIDENTID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const std::string& value) = 0;
    virtual bool SetRecordObject(const NFIDENTID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag, const NFIDENTID& value) = 0;

    virtual NFINT64 GetRecordInt(const NFIDENTID& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual float GetRecordFloat(const NFIDENTID& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual double GetRecordDouble(const NFIDENTID& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual const std::string& GetRecordString(const NFIDENTID& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual NFIDENTID GetRecordObject(const NFIDENTID& self, const std::string& strRecordName, const int nRow, const int nCol) = 0;

    virtual NFINT64 GetRecordInt(const NFIDENTID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    virtual float GetRecordFloat(const NFIDENTID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    virtual double GetRecordDouble(const NFIDENTID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    virtual const std::string& GetRecordString(const NFIDENTID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;
    virtual NFIDENTID GetRecordObject(const NFIDENTID& self, const std::string& strRecordName, const int nRow, const std::string& strColTag) = 0;

    virtual bool SwitchScene(const NFIDENTID& self, const int nTargetSceneID, const int nTargetGroupID, const float fX, const float fY, const float fZ, const float fOrient, const NFIDataList& arg) = 0;

    virtual bool AddProperty(const NFIDENTID& self, const std::string& strPropertyName, const TDATA_TYPE varType, bool bPublic ,  bool bPrivate ,  bool bSave, bool bView, int nIndex, const std::string& strScriptFunction) = 0;
    virtual bool AddRecord(const NFIDENTID& self, const std::string& strRecordName, const NFIDataList& TData, const NFIDataList& varKey, const NFIDataList& varDesc, const NFIDataList& varTag, const NFIDataList& varRelatedRecord, const int nRows, bool bPublic,  bool bPrivate,  bool bSave, bool bView, int nIndex) = 0;
    ////////////////////////////////////////////////////////////////

    virtual bool CreateContainer(const int nSceneIndex, const std::string& strSceneConfigID) = 0;

    virtual int GetOnLineCount() = 0;

    virtual int GetMaxOnLineCount() = 0;

    virtual int GetContainerOnLineCount(const int nContainerID) = 0;

    virtual int GetContainerOnLineCount(const int nContainerID, const int nGroupID) = 0;

    virtual int GetContainerOnLineList(const int nContainerID, NFIDataList& var) = 0;
    virtual int GetAllContainerObjectList(NFIDataList& var) = 0;

    virtual int RequestGroupScene(const int nContainerID) = 0;

    virtual bool ReleaseGroupScene(const int nContainerID, const int nGroupID) = 0;

    virtual bool GetGroupObjectList(const int nContainerID, const int nGroupID, NFIDataList& list) = 0;

    virtual NFIDENTID GetGridID(const float fX, const float fY, const float fZ) = 0;

    virtual bool GetRangObjectList(const NFIDENTID& self, const int nContainerID, const int nGroupID, const float fRang, NFIDataList& list) = 0;

    virtual bool GetRangObjectList(const float fX, const float fY, const float fZ, const int nContainerID, const int nGroupID, const float fRang, NFIDataList& list) = 0;

    virtual int GetObjectByProperty(const int nContainerID, const std::string& strPropertyName, const NFIDataList& valueArg, NFIDataList& list) = 0;

    virtual void Random(int nStart, int nEnd, int nCount, NFIDataList& valueList) = 0;

    virtual bool LogInfo(const NFIDENTID ident) = 0;

protected:
    virtual bool AddEventCallBack(const NFIDENTID& self, const int nEventID, const EVENT_PROCESS_FUNCTOR_PTR& cb) = 0;
    virtual bool AddClassCallBack(const std::string& strClassName, const CLASS_EVENT_FUNCTOR_PTR& cb) = 0;

    virtual bool AddRecordCallBack(const NFIDENTID& self, const std::string& strRecordName, const RECORD_EVENT_FUNCTOR_PTR& cb) = 0;
    virtual bool AddPropertyCallBack(const NFIDENTID& self, const std::string& strPropertyName, const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;
    virtual bool AddHeartBeat(const NFIDENTID& self, const std::string& strHeartBeatName, const HEART_BEAT_FUNCTOR_PTR& cb, const float fTime, const int nCount) = 0;
protected:

    //只能网络模块注册，回调用来同步对象类事件,所有的类对象都会回调
    virtual bool ResgisterCommonClassEvent(const CLASS_EVENT_FUNCTOR_PTR& cb) = 0;

    //只能网络模块注册，回调用来同步对象属性事件,所有的类属性都会回调
    virtual bool ResgisterCommonPropertyEvent(const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;

    //只能网络模块注册，回调用来同步对象类表事件,所有的类表都会回调
    virtual bool ResgisterCommonRecordEvent(const RECORD_EVENT_FUNCTOR_PTR& cb) = 0;
};

#endif
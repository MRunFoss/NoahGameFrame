// -------------------------------------------------------------------------
//    @FileName         ：    NFCEctypeModule.h
//    @Author           ：    LvSheng.Huang
//    @Date             ：    2012-12-15
//    @Module           ：    NFCEctypeModule
//
// -------------------------------------------------------------------------

#ifndef _NFC_ECTYPE_MODULE_H_
#define _NFC_ECTYPE_MODULE_H_

#include <iostream>
#include "NFComm/NFPluginModule/NFIEctypeModule.h"
#include "NFComm/NFPluginModule/NFIElementInfoModule.h"
#include "NFComm/NFPluginModule/NFIEventProcessModule.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFISceneProcessModule.h"
#include "NFComm/NFPluginModule/NFIAwardPackModule.h"
#include "NFComm/NFPluginModule/NFIPackModule.h"
#include "NFComm/NFPluginModule/NFIPropertyModule.h"
#include "NFComm/NFPluginModule/NFILevelModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "NFComm/NFPluginModule/NFILogicClassModule.h"

class NFCEctypeModule : public NFIEctypeModule
{
public:
    NFCEctypeModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual ~NFCEctypeModule() {};

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute(const float fLasFrametime, const float fStartedTime);
    virtual bool AfterInit();

    //创建角色激活副本状态
    virtual bool AddNewEctype(const NFIDENTID self);

    //设置通关副本状态
    virtual bool AddEctypeActiveState(const NFIDENTID self, const int nContainerID);

    //完成某难度副本
    virtual bool CompleteEctypeMode(const NFIDENTID self, const int nContainerID, const int nStar);

    //能否进副本
    virtual bool CanEntryCloneScene(const NFIDENTID self, const int nContainerID);

    virtual int OnEctypeSettleEvent(const NFIDENTID& self, int nResult, int nLevel);

protected:

    int OnObjectClassEvent(const NFIDENTID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const NFIDataList& var);

    int OnObjectGroupIDEvent(const NFIDENTID& self, const std::string& strPropertyName, const NFIDataList& oldVar, const NFIDataList& newVar);

protected:

    //给结算奖励
    int AddEctypeAward(const NFIDENTID& self, const int nSceneID);
 
private:
    NFIElementInfoModule* m_pElementInfoModule;
    NFISceneProcessModule* m_pSceneProcessModule;
    NFIAwardPackModule* m_pAwardModule;
    NFIPackModule* m_pPackModule;
    NFIPropertyModule* m_pPropertyModule;
    NFILevelModule* m_pLevelModule;
    NFIKernelModule* m_pKernelModule;
    NFILogModule* m_pLogModule;
    NFIEventProcessModule* m_pEventProcessModule;
    NFILogicClassModule* m_pLogicClassModule;
};

#endif
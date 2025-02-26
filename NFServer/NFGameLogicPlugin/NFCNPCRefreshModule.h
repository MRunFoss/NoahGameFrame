// -------------------------------------------------------------------------
//    @FileName         :   NFCNPCRefreshModule.h
//    @Author               :    LvSheng.Huang
//    @Date                 :    2013-10-17
//    @Module               :    NFCNPCRefreshModule
//    @Desc                 :    NPC����ˢ��
// -------------------------------------------------------------------------

#ifndef _NFC_NPC_REFRESH_MODULE_H_
#define _NFC_NPC_REFRESH_MODULE_H_

#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFINPCRefreshModule.h"
#include "NFComm/NFPluginModule/NFIGameLogicModule.h"
#include "NFComm/NFPluginModule/NFIEventProcessModule.h"
#include "NFComm/NFPluginModule/NFISceneProcessModule.h"
#include "NFComm/NFPluginModule/NFIElementInfoModule.h"
#include "NFComm/NFPluginModule/NFIPackModule.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class NFCNPCRefreshModule
    : public NFINPCRefreshModule
{
public:
    NFCNPCRefreshModule( NFIPluginManager* p )
    {
        pPluginManager = p;
    }
    virtual ~NFCNPCRefreshModule() {};

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute( const float fLasFrametime, const float fStartedTime );
    virtual bool AfterInit();

protected:
    int OnObjectClassEvent( const NFIDENTID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const NFIDataList& var );

    int OnObjectHPEvent( const NFIDENTID& self, const std::string& strPropertyName, const NFIDataList& oldVar, const NFIDataList& newVar);

    int OnDeadDestroyHeart( const NFIDENTID& self, const std::string& strHeartBeat, const float fTime, const int nCount);
private:

    NFIEventProcessModule* m_pEventProcessModule;
    NFIElementInfoModule* m_pElementInfoModule;
    NFIKernelModule* m_pKernelModule;
    NFISceneProcessModule* m_pSceneProcessModule;
    NFIPackModule* m_pPackModule;

};


#endif

// -------------------------------------------------------------------------
//    @FileName      :    NFCDataProcessModule.h
//    @Author           :    LvSheng.Huang
//    @Date             ��    2013-10-03
//    @Module           :    NFCDataProcessModule
//    @Desc             :
// -------------------------------------------------------------------------

#ifndef _NFC_DATAPROCESS_MODULE_H_
#define _NFC_DATAPROCESS_MODULE_H_

#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIGameLogicModule.h"
#include "NFComm/NFPluginModule/NFIEventProcessModule.h"
#include "NFComm/NFPluginModule/NFIDataProcessModule.h"
#include "NFComm/NFPluginModule/NFIClusterModule.h"
#include "NFComm/NFPluginModule/NFIUUIDModule.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"
#include "NFComm/NFPluginModule/NFIObjectSaveModule.h"

class NFCDataProcessModule
    : public NFIDataProcessModule
{
public:

    NFCDataProcessModule( NFIPluginManager* p )
    {
        pPluginManager = p;

        mstrRoleTable = "Player";
        mstrAccountTable = "AccountInfo";
    }
    virtual ~NFCDataProcessModule() {};

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute( const float fLasFrametime, const float fStartedTime );
    virtual bool AfterInit();

    virtual const bool LoadDataFormNoSql( const NFIDENTID& self );
    //virtual const bool SaveDataToNoSql( const NFIDENTID& self, bool bOffline = false );

	virtual const NFIDENTID CreateRole(const std::string& strAccount, const std::string& strName, const int nJob, const int nSex);
	virtual const bool DeleteRole(const std::string& strAccount, const NFIDENTID xID);
	virtual const NFIDENTID GetChar(const std::string& strAccount, const std::vector<std::string>& xFieldVec, std::vector<std::string>& xValueVec);

protected:
    int OnObjectClassEvent( const NFIDENTID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const NFIDataList& var );

    void OnOnline(const NFIDENTID& self);
    void OnOffline(const NFIDENTID& self);

private:
    NFIEventProcessModule* m_pEventProcessModule;
    NFIKernelModule* m_pKernelModule;
    NFIClusterModule* m_pClusterSQLModule;
	NFIGameLogicModule* m_pGameLogicModule;
	NFIUUIDModule* m_pUUIDModule;
    NFIObjectSaveModule* m_pObjectSaveModule;

private:
    std::string mstrRoleTable;
    std::string mstrAccountTable;
};


#endif

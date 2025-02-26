// -------------------------------------------------------------------------
//    @FileName      :    NFCTestComponent.h
//    @Author           :    LvSheng.Huang
//    @Date             :    2014-05-14 08:51
//    @Module           :   NFCTestComponent
//
// -------------------------------------------------------------------------

#ifndef _NFC_TEST_COMPONENT_H_
#define _NFC_TEST_COMPONENT_H_

#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIEventProcessModule.h"
#include "NFComm/NFPluginModule/NFIElementInfoModule.h"
#include "NFComm/NFPluginModule/NFILuaScriptModule.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class NFCTestComponent : public NFIComponent
{
public:
    NFCTestComponent(NFIPluginManager* p)
    {
		pPluginManager = p;
    }
    virtual ~NFCTestComponent() {}


    virtual bool Init();
    virtual bool AfterInit();

    virtual int OnASyncEvent(const NFIDENTID& self, const int event, std::string& arg);

private:
};

#endif

// -------------------------------------------------------------------------
//    @FileName         ��    NFIDataProcessModule.h
//    @Author           ��    LvSheng.Huang
//    @Date             ��    2013-10-03
//    @Module           ��    NFIDataProcessModule
//
// -------------------------------------------------------------------------

#ifndef _NFI_DATAPROCESS_MODULE_H_
#define _NFI_DATAPROCESS_MODULE_H_

#include <iostream>
#include "NFILogicModule.h"
#include "NFIActorDataModule.h"

class NFIDataProcessModule
    : public NFIActorDataModule
{

public:
    virtual const bool  LoadDataFormNoSql(const NFIDENTID& self) = 0;

	virtual const NFIDENTID CreateRole(const std::string& strAccount, const std::string& strName, const int nJob, const int nSex) = 0;
	virtual const bool DeleteRole(const std::string& strAccount, const NFIDENTID xID) = 0;
	virtual const NFIDENTID GetChar(const std::string& strAccount, const std::vector<std::string>& xFieldVec, std::vector<std::string>& xValueVeec) = 0;

};

#endif
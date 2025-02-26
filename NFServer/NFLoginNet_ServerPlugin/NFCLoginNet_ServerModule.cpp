// -------------------------------------------------------------------------
//    @FileName      :    NFCLoginNet_ServerModule.cpp
//    @Author           :    LvSheng.Huang
//    @Date             :    2013-01-02
//    @Module           :    NFCLoginNet_ServerModule
//    @Desc             :
// -------------------------------------------------------------------------

//#include "stdafx.h"
#include "NFCLoginNet_ServerModule.h"
#include "NFLoginNet_ServerPlugin.h"

const std::string PROPERTY_ACCOUNT = "Account";
const std::string PROPERTY_VERIFIED = "Verified";

bool NFCLoginNet_ServerModule::Init()
{
	return true;
}

bool NFCLoginNet_ServerModule::Shut()
{
	return true;
}

bool NFCLoginNet_ServerModule::BeforeShut()
{
	return true;
}

bool NFCLoginNet_ServerModule::AfterInit()
{
	
	m_pEventProcessModule = dynamic_cast<NFIEventProcessModule*>(pPluginManager->FindModule("NFCEventProcessModule"));
	m_pKernelModule = dynamic_cast<NFIKernelModule*>(pPluginManager->FindModule("NFCKernelModule"));
	m_pLoginLogicModule = dynamic_cast<NFILoginLogicModule*>(pPluginManager->FindModule("NFCLoginLogicModule"));
	m_pLogModule = dynamic_cast<NFILogModule*>(pPluginManager->FindModule("NFCLogModule"));
	m_pLogicClassModule = dynamic_cast<NFILogicClassModule*>(pPluginManager->FindModule("NFCLogicClassModule"));
    m_pElementInfoModule = dynamic_cast<NFIElementInfoModule*>(pPluginManager->FindModule("NFCElementInfoModule"));
    m_pLoginToMasterModule = dynamic_cast<NFILoginToMasterModule*>(pPluginManager->FindModule("NFCLoginToMasterModule"));
	m_pUUIDModule = dynamic_cast<NFIUUIDModule*>(pPluginManager->FindModule("NFCUUIDModule"));
    

	assert(NULL != m_pEventProcessModule);
	assert(NULL != m_pKernelModule);
	assert(NULL != m_pLoginLogicModule);
	assert(NULL != m_pLogModule);
	assert(NULL != m_pLogicClassModule);
    assert(NULL != m_pElementInfoModule);

	assert(NULL != m_pLoginToMasterModule);
	assert(NULL != m_pUUIDModule);

	m_pEventProcessModule->AddEventCallBack(NFIDENTID(), NFED_ON_CLIENT_LOGIN_RESULTS, this, &NFCLoginNet_ServerModule::OnLoginResultsEvent);
	m_pEventProcessModule->AddEventCallBack(NFIDENTID(), NFED_ON_CLIENT_SELECT_SERVER_RESULTS, this, &NFCLoginNet_ServerModule::OnSelectWorldResultsEvent);

	NF_SHARE_PTR<NFILogicClass> xLogicClass = m_pLogicClassModule->GetElement("Server");
	if (xLogicClass.get())
	{
		NFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
		std::string strConfigName; 
		for (bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
		{
			const int nServerType = m_pElementInfoModule->GetPropertyInt(strConfigName, "Type");
            const int nServerID = m_pElementInfoModule->GetPropertyInt(strConfigName, "ServerID");
			if (nServerType == NF_SERVER_TYPES::NF_ST_LOGIN && pPluginManager->AppID() == nServerID)
			{
				const int nPort = m_pElementInfoModule->GetPropertyInt(strConfigName, "Port");				
				const int nMaxConnect = m_pElementInfoModule->GetPropertyInt(strConfigName, "MaxOnline");
				const int nCpus = m_pElementInfoModule->GetPropertyInt(strConfigName, "CpuCount");

				m_pUUIDModule->SetIdentID(nServerID);

				Initialization(NFIMsgHead::NF_Head::NF_HEAD_LENGTH, this, &NFCLoginNet_ServerModule::OnReciveClientPack, &NFCLoginNet_ServerModule::OnSocketClientEvent, nMaxConnect, nPort, nCpus);		
			}
		}
	}
	
	return true;
}

int NFCLoginNet_ServerModule::OnLoginResultsEvent(const NFIDENTID& object, const int nEventID, const NFIDataList& var)
{
	if (3 != var.GetCount()
		|| !var.TypeEx(TDATA_TYPE::TDATA_INT, TDATA_TYPE::TDATA_OBJECT, TDATA_TYPE::TDATA_STRING, TDATA_TYPE::TDATA_UNKNOWN))
	{
		return -1;
	}

	const int nState = var.Int(0);
	const NFIDENTID xIdent = var.Object(1);
	const std::string& strAccount = var.String(2);

	NF_SHARE_PTR<int> xFD = mxClientIdent.GetElement(xIdent);
	if (xFD)
	{
		if (0 != nState)
		{
			//此帐号密码错误或者被封号
			//登录失败
			NFMsg::AckEventResult xMsg;
			xMsg.set_event_code(NFMsg::EGEC_ACCOUNTPWD_INVALID);

			SendMsgPB(NFMsg::EGameMsgID::EGMI_ACK_LOGIN, xMsg, *xFD);

			return 0;
		}

		NetObject* pNetObject = GetNet()->GetNetObject(*xFD);
		if (pNetObject)
		{
			//记录他登录过
			pNetObject->SetConnectKeyState(1);
			pNetObject->SetAccount(strAccount);
		}

		//把服务器列表广播下去

		NFMsg::AckEventResult xData;
		xData.set_event_code(NFMsg::EGEC_ACCOUNT_SUCCESS);

		SendMsgPB(NFMsg::EGameMsgID::EGMI_ACK_LOGIN, xData, *xFD);

		//SynWorldToClient(unAddress);

		m_pLogModule->LogNormal(NFILogModule::NLL_INFO_NORMAL, NFIDENTID(0, *xFD), "Login successed :", strAccount.c_str());

	}
	
	return 0;
}


int NFCLoginNet_ServerModule::OnSelectWorldResultsEvent(const NFIDENTID& object, const int nEventID, const NFIDataList& var)
{
	if (7 != var.GetCount()
		|| !var.TypeEx(TDATA_TYPE::TDATA_INT, TDATA_TYPE::TDATA_OBJECT, TDATA_TYPE::TDATA_INT, TDATA_TYPE::TDATA_STRING,
		TDATA_TYPE::TDATA_STRING, TDATA_TYPE::TDATA_INT, TDATA_TYPE::TDATA_STRING, TDATA_TYPE::TDATA_UNKNOWN))
	{
		return -1;
	}

	const int nWorldID = var.Int(0);
	const NFIDENTID xClientIdent = var.Object(1);
	const int nLoginID = var.Int(2);
	const std::string& strAccount = var.String(3);
	const std::string& strWorldAddress = var.String(4);
	int nPort = var.Int(5);
	const std::string& strWorldKey = var.String(6);

	NF_SHARE_PTR<int> xFD = mxClientIdent.GetElement(xClientIdent);
	if (xFD)
	{
		NFMsg::AckConnectWorldResult xMsg;
		xMsg.set_world_id(nWorldID);
		xMsg.mutable_sender()->CopyFrom(NFToPB(xClientIdent));
		xMsg.set_login_id(nLoginID);
		xMsg.set_account(strAccount);
		xMsg.set_world_ip(strWorldAddress);
		xMsg.set_world_port(nPort);
		xMsg.set_world_key(strWorldKey);

		SendMsgPB(NFMsg::EGameMsgID::EGMI_ACK_CONNECT_WORLD, xMsg, *xFD);
	}

	return 0;
}

bool NFCLoginNet_ServerModule::Execute(const float fLasFrametime, const float fStartedTime)
{
	return NFINetModule::Execute(fLasFrametime, fStartedTime);
}

void NFCLoginNet_ServerModule::OnClientConnected(const int nAddress)
{
	NetObject* pObject = GetNet()->GetNetObject(nAddress);
	if (pObject)
	{
		NFIDENTID xIdent =m_pUUIDModule->CreateGUID();
		pObject->SetClientID(xIdent);
        mxClientIdent.AddElement(xIdent, NF_SHARE_PTR<int> (NF_NEW int(nAddress)) );
	}
}

void NFCLoginNet_ServerModule::OnClientDisconnect(const int nAddress)
{
	NetObject* pObject = GetNet()->GetNetObject(nAddress);
	if (pObject)
	{
		NFIDENTID xIdent = pObject->GetClientID();
		mxClientIdent.RemoveElement(xIdent);
	}
}

int NFCLoginNet_ServerModule::OnLoginProcess( const NFIPacket& msg )
{
	NFIDENTID nPlayerID;
	NFMsg::ReqAccountLogin xMsg;
	if (!RecivePB(msg, xMsg, nPlayerID))
	{
		return 0;
	}
    
    NetObject* pNetObject = GetNet()->GetNetObject(msg.GetFd());
    if (pNetObject)
    {
        //还没有登录过
        if (pNetObject->GetConnectKeyState() == 0)
        {
            NFCDataList val;
            val << pNetObject->GetClientID()<< xMsg.account() << xMsg.password();
            m_pEventProcessModule->DoEvent(NFIDENTID(), NFED_ON_CLIENT_LOGIN, val);
        }
    }
	
	return 0;
}

int NFCLoginNet_ServerModule::OnSelectWorldProcess( const NFIPacket& msg )
{
	NFIDENTID nPlayerID;
	NFMsg::ReqConnectWorld xMsg;
	if (!RecivePB(msg, xMsg, nPlayerID))
	{
		return 0;
	}

	NetObject* pNetObject = GetNet()->GetNetObject(msg.GetFd());
	if (!pNetObject)
	{
		return 0;
	}

	//没登录过
	if (pNetObject->GetConnectKeyState() <= 0)
	{
		return 0;
	}

	NFCDataList val;
	val << xMsg.world_id() << pNetObject->GetClientID() << pPluginManager->AppID() << pNetObject->GetAccount();
	m_pEventProcessModule->DoEvent(NFIDENTID(), NFED_ON_CLIENT_SELECT_SERVER, val);

	return 0;
}

int NFCLoginNet_ServerModule::OnReciveClientPack(const NFIPacket& msg )
{
	//统一解包
	int nMsgID = msg.GetMsgHead()->GetMsgID();
	switch (nMsgID)
	{

	case NFMsg::EGameMsgID::EGMI_REQ_LOGIN:
		OnLoginProcess(msg);
		break;

	case NFMsg::EGameMsgID::EGMI_REQ_LOGOUT:
		break;

	case NFMsg::EGameMsgID::EGMI_REQ_CONNECT_WORLD:
		OnSelectWorldProcess(msg);
		break;
		
	case NFMsg::EGameMsgID::EGMI_REQ_WORLD_LIST:
		OnViewWorldProcess(msg);
		break;

		// 	case NFMsg::EGameMsgID::EGMI_ACK_CONNECT_WORLD:
		// 		OnObjectPropertyIntProcess(msg);
		// 		break;



	default:
		printf("NFNet || 非法消息:unMsgID=%d\n", nMsgID);
		break;
	}
	return 0;
}

int NFCLoginNet_ServerModule::OnSocketClientEvent( const int nSockIndex, const NF_NET_EVENT eEvent, NFINet* pNet )
{
    if (eEvent & NF_NET_EVENT_EOF) 
    {
        m_pLogModule->LogNormal(NFILogModule::NLL_INFO_NORMAL, NFIDENTID(0, nSockIndex), "NF_NET_EVENT_EOF", "Connection closed", __FUNCTION__, __LINE__);
        OnClientDisconnect(nSockIndex);
    } 
    else if (eEvent & NF_NET_EVENT_ERROR) 
    {
        m_pLogModule->LogNormal(NFILogModule::NLL_INFO_NORMAL, NFIDENTID(0, nSockIndex), "NF_NET_EVENT_ERROR", "Got an error on the connection", __FUNCTION__, __LINE__);
        OnClientDisconnect(nSockIndex);
    }
    else if (eEvent & NF_NET_EVENT_TIMEOUT)
    {
        m_pLogModule->LogNormal(NFILogModule::NLL_INFO_NORMAL, NFIDENTID(0, nSockIndex), "NF_NET_EVENT_TIMEOUT", "read timeout", __FUNCTION__, __LINE__);
        OnClientDisconnect(nSockIndex);
    }
    else  if (eEvent == NF_NET_EVENT_CONNECTED)
    {
        m_pLogModule->LogNormal(NFILogModule::NLL_INFO_NORMAL, NFIDENTID(0, nSockIndex), "NF_NET_EVENT_CONNECTED", "connectioned success", __FUNCTION__, __LINE__);
        OnClientConnected(nSockIndex);
    }

	return 0;	
}

void NFCLoginNet_ServerModule::SynWorldToClient( const int nFD )
{
    NFMsg::AckServerList xData;
    xData.set_type(NFMsg::RSLT_WORLD_SERVER);

    NFMapEx<int, NFMsg::ServerInfoReport>& xWorldMap = m_pLoginToMasterModule->GetWorldMap();
	NFMsg::ServerInfoReport* pWorldData =  xWorldMap.FirstNude();
	while (pWorldData)
	{
		NFMsg::ServerInfo* pServerInfo = xData.add_info();

		pServerInfo->set_name(pWorldData->server_name());
		pServerInfo->set_status(pWorldData->server_state());
		pServerInfo->set_server_id(pWorldData->server_id());
		pServerInfo->set_wait_count(0);

		pWorldData = xWorldMap.NextNude();
	}
    

    SendMsgPB(NFMsg::EGameMsgID::EGMI_ACK_WORLD_LIST, xData, nFD);
}

int NFCLoginNet_ServerModule::OnViewWorldProcess( const NFIPacket& msg )
{
	NFIDENTID nPlayerID;
	NFMsg::ReqServerList xMsg;
	if (!RecivePB(msg, xMsg, nPlayerID))
	{
		return 0;
	}

	if (xMsg.type() == NFMsg::RSLT_WORLD_SERVER)
	{
		SynWorldToClient(msg.GetFd());
	}

	return 0;
}



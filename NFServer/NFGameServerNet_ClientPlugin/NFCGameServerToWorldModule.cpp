// -------------------------------------------------------------------------
//    @FileName      :    NFCGameServerNet_ClientModule.cpp
//    @Author           :    LvSheng.Huang
//    @Date             :    2013-01-02
//    @Module           :    NFCGameServerNet_ClientModule
//    @Desc             :
// -------------------------------------------------------------------------

//#include "stdafx.h"
#include "NFCGameServerToWorldModule.h"
#include "NFGameServerNet_ClientPlugin.h"
#include "NFComm/NFMessageDefine/NFMsgDefine.h"
#include "NFComm/NFPluginModule/NFIClusterClientModule.hpp"

bool NFCGameServerToWorldModule::Init()
{
    return true;
}

bool NFCGameServerToWorldModule::Shut()
{

    return true;
}


bool NFCGameServerToWorldModule::Execute(const float fLasFrametime, const float fStartedTime)
{
	return NFIClusterClientModule::Execute(fLasFrametime, fStartedTime);
}

void NFCGameServerToWorldModule::Register(NFINet* pNet)
{
    //成功就注册
	NF_SHARE_PTR<NFILogicClass> xLogicClass = m_pLogicClassModule->GetElement("Server");
	if (xLogicClass.get())
	{
		NFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
		std::string strConfigName; 
		for (bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
		{
			const int nServerType = m_pElementInfoModule->GetPropertyInt(strConfigName, "Type");
            const int nServerID = m_pElementInfoModule->GetPropertyInt(strConfigName, "ServerID");
			if (nServerType == NF_SERVER_TYPES::NF_ST_GAME && pPluginManager->AppID() == nServerID)
			{
				const int nPort = m_pElementInfoModule->GetPropertyInt(strConfigName, "Port");
				const int nMaxConnect = m_pElementInfoModule->GetPropertyInt(strConfigName, "MaxOnline");
				const int nCpus = m_pElementInfoModule->GetPropertyInt(strConfigName, "CpuCount");
				const std::string& strName = m_pElementInfoModule->GetPropertyString(strConfigName, "Name");
				const std::string& strIP = m_pElementInfoModule->GetPropertyString(strConfigName, "IP");

				NFMsg::ServerInfoReportList xMsg;
				NFMsg::ServerInfoReport* pData = xMsg.add_server_list();

				pData->set_server_id(nServerID);
				pData->set_server_name(strName);
				pData->set_server_cur_count(0);
				pData->set_server_ip(strIP);
				pData->set_server_port(nPort);
				pData->set_server_max_online(nMaxConnect);
				pData->set_server_state(NFMsg::EST_NARMAL);
				pData->set_server_type(nServerType);

				NF_SHARE_PTR<ServerData> pServerData = GetServerNetInfo(pNet);
				if (pServerData)
				{
					int nTargetID = pServerData->nGameID;
					SendToServerByPB(nTargetID, NFMsg::EGameMsgID::EGMI_GTW_GAME_REGISTERED, xMsg);

					m_pLogModule->LogNormal(NFILogModule::NLL_INFO_NORMAL, NFIDENTID(0, pData->server_id()), pData->server_name(), "Register");
				}
			}
		}
	}
}


void NFCGameServerToWorldModule::RefreshWorldInfo()
{
	//     _tagPT_KEY_BASE_MSG baseMsg;
	//     NFMsg::ServerInfoReport xMsg;
	// 
	//     xMsg.set_server_id(m_pGameLogicModule->GetGameID());
	//     xMsg.set_server_port(m_pGameServerModule->GetServerPort());
	//     xMsg.set_server_cur_count(0);
	//     xMsg.set_server_max_online(50000);
	// 
	//     if (xMsg.SerializeToString(&baseMsg.strSyncInfo))
	//     {
	//         baseMsg._unMsgID = GTW_GAME_REFRESH;
	//         RakNet::BitStream oBitStream;
	//         baseMsg.EnCode(&oBitStream);
	//         SendBitStream(&oBitStream);
	//     }
}

bool NFCGameServerToWorldModule::AfterInit()
{
    m_pEventProcessModule = dynamic_cast<NFIEventProcessModule*>(pPluginManager->FindModule("NFCEventProcessModule"));
    m_pKernelModule = dynamic_cast<NFIKernelModule*>(pPluginManager->FindModule("NFCKernelModule"));
    m_pLogicClassModule = dynamic_cast<NFILogicClassModule*>(pPluginManager->FindModule("NFCLogicClassModule"));
    m_pElementInfoModule = dynamic_cast<NFIElementInfoModule*>(pPluginManager->FindModule("NFCElementInfoModule"));
    m_pLogModule = dynamic_cast<NFILogModule*>(pPluginManager->FindModule("NFCLogModule"));
    m_pGameServerNet_ServerModule = dynamic_cast<NFIGameServerNet_ServerModule*>(pPluginManager->FindModule("NFCGameServerNet_ServerModule"));

    assert(NULL != m_pEventProcessModule);
    assert(NULL != m_pKernelModule);
    assert(NULL != m_pLogicClassModule);
    assert(NULL != m_pElementInfoModule);
    assert(NULL != m_pLogModule);
    assert(NULL != m_pGameServerNet_ServerModule);

	m_pKernelModule->ResgisterCommonClassEvent(this, &NFCGameServerToWorldModule::OnClassCommonEvent);

	NFIClusterClientModule::Bind(this, &NFCGameServerToWorldModule::OnReciveWSPack, &NFCGameServerToWorldModule::OnSocketWSEvent);
    
    m_pEventProcessModule->AddClassCallBack("Player", this, &NFCGameServerToWorldModule::OnObjectClassEvent);

	// 连接world server
	NF_SHARE_PTR<NFILogicClass> xLogicClass = m_pLogicClassModule->GetElement("Server");
	if (xLogicClass.get())
	{
		NFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
		std::string strConfigName; 
		for (bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
		{
			const int nServerType = m_pElementInfoModule->GetPropertyInt(strConfigName, "Type");
			const int nServerID = m_pElementInfoModule->GetPropertyInt(strConfigName, "ServerID");
			if (nServerType == NF_SERVER_TYPES::NF_ST_WORLD)
			{
				const int nPort = m_pElementInfoModule->GetPropertyInt(strConfigName, "Port");
				const int nMaxConnect = m_pElementInfoModule->GetPropertyInt(strConfigName, "MaxOnline");
				const int nCpus = m_pElementInfoModule->GetPropertyInt(strConfigName, "CpuCount");
				const std::string& strName = m_pElementInfoModule->GetPropertyString(strConfigName, "Name");
				const std::string& strIP = m_pElementInfoModule->GetPropertyString(strConfigName, "IP");

				ServerData xServerData;

				xServerData.nGameID = nServerID;
				xServerData.eServerType = (NF_SERVER_TYPE)nServerType;
				xServerData.strIP = strIP;
				xServerData.nPort = nPort;
				xServerData.strName = strName;
				xServerData.eState = NFMsg::EServerState::EST_NARMAL;

				NFIClusterClientModule::AddServer(xServerData);
			}
		}
	}

	m_pKernelModule->CreateContainer(-2, "");

	return true;
}

int NFCGameServerToWorldModule::OnLoadRoleDataBeginProcess(const NFIPacket& msg)
{
    //     _tagPT_KEY_BASE_MSG baseMsg;
    //     pMsgPacket->DeCode(&baseMsg);
    //
    //     NFMsg:: xMsg;
    //     if ( !xMsg.ParseFromString( baseMsg.strSyncInfo ) )
    //     {
    //         return 0;
    //     }
    //
    //     _tagMTD_LOAD_ROLEDATA loadData;
    //     RakNet::BitStream* pBitStream = pMsgPacket->GetBitStream();
    //     loadData.DeCode(*pBitStream);
    //
    //     m_pEventProcessModule->DoEvent(NFIDENTID(), NFED_ON_DATABASE_SERVER_LOADROE_BEGIN, NFCDataList() << loadData._szAccountName);

    return 0;
}

int NFCGameServerToWorldModule::OnLoadRoleDataFinalProcess(const NFIPacket& msg)
{

    //     _tagDTM_LOAD_ROLEDATA_FINAL loadFinal;
    //     RakNet::BitStream* pBitStream = pMsgPacket->GetBitStream();
    //     loadFinal.DeCode(*pBitStream);
    //
    //     //创建对象
    //     //开始认领内存，
    //     NFCDataList valueRoleList;
    //     valueRoleList << loadFinal._szAccountName;
    //     valueRoleList << loadFinal._szRoleName1;
    //     valueRoleList << loadFinal.nPlayerConfidIndex1;
    //     valueRoleList << loadFinal._szRoleName2;
    //     valueRoleList << loadFinal.nPlayerConfidIndex2;
    //     valueRoleList << loadFinal._szRoleName3;
    //     valueRoleList << loadFinal.nPlayerConfidIndex3;
    //     valueRoleList << loadFinal._szRoleName4;
    //     valueRoleList << loadFinal.nPlayerConfidIndex4;
    //
    //     m_pEventProcessModule->DoEvent(NFIDENTID(), NFED_ON_DATABASE_SERVER_LOADROE_FINAL_RESULTS, valueRoleList);


    //test
    //  _tagMTD_SAVE_ROLEDATA saveRole;
    //  RakNet::BitStream bitStream;
    //
    //  if ( strlen( loadFinal._szRoleName1 ) > 0 )
    //  {
    //      strcpy( saveRole._szRoleName, loadFinal._szRoleName1 );
    //  }
    //  else if ( strlen( loadFinal._szRoleName2 ) > 0 )
    //  {
    //      strcpy( saveRole._szRoleName, loadFinal._szRoleName2 );
    //  }
    //  else if ( strlen( loadFinal._szRoleName3 ) > 0 )
    //  {
    //      strcpy( saveRole._szRoleName, loadFinal._szRoleName3 );
    //  }
    //  else if ( strlen( loadFinal._szRoleName4 ) > 0 )
    //  {
    //      strcpy( saveRole._szRoleName, loadFinal._szRoleName4 );
    //  }
    //
    //  saveRole.EnCode( bitStream );
    //  SendBitStream( &bitStream );

    return 0;
}

//离开世界回到客户端时，产生
int NFCGameServerToWorldModule::OnDataLoadBeginEvent(const NFIDENTID& object, const int nEventID, const NFIDataList& var)
{
    //1号gs发送
    //if (1 == m_pGameLogicModule->GetGameID())
    {
        //      _tagMTD_LOAD_ROLEDATA loadData;
        //
        //      strcpy(loadData._szAccountName, var.String(0));
        //      RakNet::BitStream oBitStream;
        //      loadData.EnCode(oBitStream);
        //
        //      SendBitStream(&oBitStream);

        //NOSQL直接加载
    }


    return 0;
}

// int NFCGameServerNet_ClientModule::OnToWorldEvent( const NFIDENTID& object, const int nEventID, const NFIDataList& var )
// {
//     if ( 4 != var.GetCount() )
//     {
//         return 0;
//     }
//
//  if (m_pGameLogicModule->GetGameID() == 1)
//  {
//      int nGameServer = var.Int( 0 );
//      const char* pstrAccount = var.String( 1 );
//      const char* pstrRoleName = var.String( 2 );
//      int nSceneIndex = var.Int( 3 );
//
//      RakNet::BitStream oBitStream;
//      _tagGTG_SWAP_GAMESERVER swapWorld;
//
//      swapWorld._nGameServerID = nGameServer;
//      strcpy( swapWorld._szAccount, pstrAccount );
//      strcpy( swapWorld._szRoleName, pstrRoleName );
//      swapWorld._nSceneIndex = nSceneIndex;
//
//      swapWorld.EnCode( oBitStream );
//      SendBitStream( &oBitStream );
//  }
//
//
//     return 0;
// }

int NFCGameServerToWorldModule::OnEnquireSceneInfoProcess(const NFIPacket& msg)
{
    //  _tagWTG_ENQUIRE_SCENE_INFO enquireSceneInfo;
    //  RakNet::BitStream* pBitStream = pMsgPacket->GetBitStream();
    //  enquireSceneInfo.DeCode( *pBitStream );


    return 0;
}

int NFCGameServerToWorldModule::OnSwapGSEvent(const NFIDENTID& object, const int nEventID, const NFIDataList& var)
{
    //如果是单服，则不允许切换gs
    if (5 != var.GetCount())
    {
        return 0;
    }

    //     int nGameServerID = var.Int(0);
    //     const std::string& strAccount = var.String(1);
    //     const std::string& strRoleName = var.String(2);
    //     int nSceneIndex = var.Int(3);
    //     int nPlayerConfigIndex = var.Int(4);
    //
    //     _tagGTG_SWAP_GAMESERVER swapGS;
    //
    //     swapGS._nGameServerID = nGameServerID;
    //     strcpy(swapGS._szAccount, strAccount.c_str());
    //     strcpy(swapGS._szRoleName, strRoleName.c_str());
    //     swapGS._nSceneIndex = nSceneIndex;
    //     swapGS._nPlayerConfigIndex = nPlayerConfigIndex;
    //
    //     RakNet::BitStream oBitStream;
    //     swapGS.EnCode(oBitStream);
    //
    //     SendBitStream(&oBitStream);

    return 0;
}

int NFCGameServerToWorldModule::OnSwapGSProcess(const NFIPacket& msg)
{
    //查看此GSID是否正确
    //     _tagGTG_SWAP_GAMESERVER swapGameServer;
    //     RakNet::BitStream* pBitStream = pMsgPacket->GetBitStream();
    //     swapGameServer.DeCode(*pBitStream);
    //
    //     if (m_pGameLogicModule->GetGameID() == swapGameServer._nGameServerID)
    //     {
    //         if (m_pKernelModule->ExistContainer(swapGameServer._nSceneIndex))
    //         {
    //             //OK，可以接纳玩家
    //             NFCDataList valServerList;
    //             valServerList << swapGameServer._nGameServerID << swapGameServer._szAccount << swapGameServer._szRoleName << swapGameServer._nSceneIndex << swapGameServer._nPlayerConfigIndex;
    //             m_pEventProcessModule->DoEvent(NFIDENTID(), NFED_ON_CLIENT_SELECTROLE_ENTER, valServerList);
    //         }
    //     }

    return 0;
}

int NFCGameServerToWorldModule::OnClassCommonEvent(const NFIDENTID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const NFIDataList& var)
{
//     if (strClassName == "Player")
//     {
//         if (CLASS_OBJECT_EVENT::COE_DESTROY == eClassEvent
//             && m_pKernelModule->GetPropertyInt(self, "SceneID") > 0)
//         {
//             //被删除，也有可能是大厅角色，因此场景ID一定大于0才行
//             //删除他的连接和连接信息,包括世界服务器
//             _tagPT_KEY_BASE_MSG baseMsg;
//             NFMsg::WantToExitWorld xMsg;
// 
//             xMsg.set_world_id(0);
//             xMsg.set_account(m_pKernelModule->GetPropertyString(self, "Account"));
// 
//             if (xMsg.SerializeToString(&baseMsg.strSyncInfo))
//             {
//                 baseMsg._unMsgID = LTW_CLIENT_EXIT_WORLD;
//                 RakNet::BitStream oBitStream;
//                 baseMsg.EnCode(&oBitStream);
//                 SendBitStream(&oBitStream);
//             }
//         }
// 
//     }

    return 0;
}

int NFCGameServerToWorldModule::OnReciveWSPack( const NFIPacket& msg )
{
    switch (msg.GetMsgHead()->GetMsgID())
    {
    case NFMsg::EGameMsgID::EGMI_ACK_CREATE_GUILD:
        OnAckCreateGuildProcess(msg);
        break;
    case NFMsg::EGameMsgID::EGMI_ACK_JOIN_GUILD:
        OnAckJoinGuildProcess(msg);        
        break;
    case NFMsg::EGameMsgID::EGMI_ACK_LEAVE_GUILD:
        OnAckLeaveGuildProcess(msg);
        break;
// 
//     case NFMsg::EGameMsgID::EGMI_ACK_PROPERTY_INT:
//         TransPBToProxy< NFMsg::ObjectPropertyInt>(msg);
//         break;
// 
//     case NFMsg::EGameMsgID::EGMI_ACK_PROPERTY_FLOAT:
//         TransPBToProxy<NFMsg::ObjectPropertyFloat>(msg);
//         break;
// 
//     case NFMsg::EGameMsgID::EGMI_ACK_PROPERTY_DOUBLE:
//         TransPBToProxy<NFMsg::ObjectPropertyFloat>(msg);
//         break;
// 
//     case NFMsg::EGameMsgID::EGMI_ACK_PROPERTY_STRING:
//         TransPBToProxy<NFMsg::ObjectPropertyString>(msg);
//         break;
// 
//     case NFMsg::EGameMsgID::EGMI_ACK_PROPERTY_OBJECT:
//         TransPBToProxy<NFMsg::ObjectPropertyObject>(msg);
//         break;
// 
//     case NFMsg::EGameMsgID::EGMI_ACK_RECORD_INT:
//         TransPBToProxy<NFMsg::ObjectRecordInt>(msg);
//         break;
// 
//     case NFMsg::EGameMsgID::EGMI_ACK_RECORD_FLOAT:
//         TransPBToProxy<NFMsg::ObjectRecordFloat>(msg);
//         break;
// 
//     case NFMsg::EGameMsgID::EGMI_ACK_RECORD_DOUBLE:
//         TransPBToProxy<NFMsg::ObjectRecordFloat>(msg);
//         break;
// 
//     case NFMsg::EGameMsgID::EGMI_ACK_RECORD_STRING:
//         TransPBToProxy<NFMsg::ObjectRecordString>(msg);
//         break;
// 
//     case NFMsg::EGameMsgID::EGMI_ACK_RECORD_OBJECT:
//         TransPBToProxy<NFMsg::ObjectRecordObject>(msg);
//         break;
//     case NFMsg::EGameMsgID::EGMI_EVENT_RESULT:
//         TransPBToProxy<NFMsg::AckEventResult>(msg);
//         break;

    default:
        TransPBToProxy(msg);
        break;
    }
    return 0;
}

int NFCGameServerToWorldModule::OnSocketWSEvent( const int nSockIndex, const NF_NET_EVENT eEvent, NFINet* pNet )
{
	if (eEvent & NF_NET_EVENT_EOF) 
	{
	} 
	else if (eEvent & NF_NET_EVENT_ERROR) 
	{
	}
	else if (eEvent & NF_NET_EVENT_TIMEOUT)
	{
	}
	else  if (eEvent == NF_NET_EVENT_CONNECTED)
	{
		m_pLogModule->LogNormal(NFILogModule::NLL_INFO_NORMAL, NFIDENTID(0, nSockIndex), "NF_NET_EVENT_CONNECTED", "connectioned success", __FUNCTION__, __LINE__);
		Register(pNet);

	}

    return 0;
}


int NFCGameServerToWorldModule::OnAckCreateGuildProcess( const NFIPacket& msg )
{
    NFIDENTID nPlayerID;
    NFMsg::ReqAckCreateGuild xData;
	if (!NFINetModule::RecivePB(msg, xData, nPlayerID))
    {
        return 0;
    }

	NFIDENTID xGuild = NFINetModule::PBToNF(xData.guild_id());
    m_pKernelModule->SetPropertyObject(nPlayerID, "GuildID", xGuild);

    m_pGameServerNet_ServerModule->SendMsgPBToGate(msg.GetMsgHead()->GetMsgID(), xData, nPlayerID);

    //
    NFMsg::ReqAckJoinGuild xJoinMsg;
    *xJoinMsg.mutable_guild_id() = NFINetModule::NFToPB(xGuild);
    xJoinMsg.set_guild_name(xData.guild_name());

    SendSuitByPB(xGuild.nData64, NFMsg::EGameMsgID::EGMI_REQ_JOIN_GUILD, xJoinMsg, 0, nPlayerID);

    return 0;
}

int NFCGameServerToWorldModule::OnAckJoinGuildProcess( const NFIPacket& msg )
{
    NFIDENTID nPlayerID;
    NFMsg::ReqAckJoinGuild xData;
	if (!NFINetModule::RecivePB(msg, xData, nPlayerID))
    {
        return 0;
    }

	NFIDENTID xGuild = NFINetModule::PBToNF(xData.guild_id());
    if (!xGuild.IsNull())
    {
        m_pKernelModule->SetPropertyObject(nPlayerID, "GuildID", xGuild);
    }

    m_pGameServerNet_ServerModule->SendMsgPBToGate(msg.GetMsgHead()->GetMsgID(), xData, nPlayerID);
    return 0;
}

int NFCGameServerToWorldModule::OnAckLeaveGuildProcess( const NFIPacket& msg )
{
    NFIDENTID nPlayerID;
    NFMsg::ReqAckLeaveGuild xData;
	if (!NFINetModule::RecivePB(msg, xData, nPlayerID))
    {
        return 0;
    }

    m_pKernelModule->SetPropertyObject(nPlayerID, "GuildID", NFIDENTID());

    m_pGameServerNet_ServerModule->SendMsgPBToGate(msg.GetMsgHead()->GetMsgID(), xData, nPlayerID);
    return 0;
}

int NFCGameServerToWorldModule::OnObjectClassEvent( const NFIDENTID& self, const std::string& strClassName, const CLASS_OBJECT_EVENT eClassEvent, const NFIDataList& var )
{
    if ( strClassName == "Player" )
    {
        if ( CLASS_OBJECT_EVENT::COE_DESTROY == eClassEvent )
        {
            SendOffline(self);
        }
        else if ( CLASS_OBJECT_EVENT::COE_CREATE_FINISH == eClassEvent )
        {
            SendOnline(self);
        }
    }

    return 0;
}

void NFCGameServerToWorldModule::SendOnline( const NFIDENTID& self )
{
    NFMsg::RoleOnlineNotify xMsg;

    const NFIDENTID& xGuild = m_pKernelModule->GetPropertyObject(self, "GuildID");
    *xMsg.mutable_guild() = NFINetModule::NFToPB(xGuild);

    SendSuitByPB(xGuild.nData64, NFMsg::EGMI_ACK_ONLINE_NOTIFY, xMsg, 0, self);
}

void NFCGameServerToWorldModule::SendOffline( const NFIDENTID& self )
{
    NFMsg::RoleOfflineNotify xMsg;

    const NFIDENTID& xGuild = m_pKernelModule->GetPropertyObject(self, "GuildID");
    *xMsg.mutable_guild() = NFINetModule::NFToPB(xGuild);

    SendSuitByPB(xGuild.nData64, NFMsg::EGMI_ACK_OFFLINE_NOTIFY, xMsg, 0, self);
}

int NFCGameServerToWorldModule::TransPBToProxy( const NFIPacket& msg )
{
    NFIDENTID nPlayerID;
    std::string strData;
    if (!NFINetModule::RecivePB(msg, strData, nPlayerID))
    {
        return 0;
    }

    m_pGameServerNet_ServerModule->SendMsgPBToGate(msg.GetMsgHead()->GetMsgID(), strData, nPlayerID);

    return 0;
}

void NFCGameServerToWorldModule::LogServerInfo( const std::string& strServerInfo )
{
	m_pLogModule->LogNormal(NFILogModule::NLL_INFO_NORMAL, NFIDENTID(), strServerInfo, "");
}
// template<class TPBClass> 
// int NFCGameServerToWorldModule::TransPBToProxy( const NFIPacket& msg )
// {
//    NFIDENTID nPlayerID;
//    TPBClass xData;
//    if (!RecivePB(msg, xData, nPlayerID))
//    {
//        return 0;
//    }
//
//    m_pGameServerNet_ServerModule->SendMsgPBToGate(msg.GetMsgHead()->GetMsgID(), xData, nPlayerID);
//}
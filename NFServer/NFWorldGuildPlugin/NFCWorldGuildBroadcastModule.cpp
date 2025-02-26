// -------------------------------------------------------------------------
//    @FileName      :    NFCWorldGuildBroadcastModule.cpp
//    @Author           :    LvSheng.Huang
//    @Date             :    2015-05-24 08:51
//    @Module           :   NFCWorldGuildBroadcastModule
//
// -------------------------------------------------------------------------

#include "NFWorldGuildPlugin.h"
#include "NFCWorldGuildBroadcastModule.h"
#include "NFComm/Config/NFConfig.h"
#include "NFComm/NFPluginModule/NFPlatform.h"
#include "NFComm/NFMessageDefine/NFMsgShare.pb.h"
#include "NFComm/NFPluginModule/NFINetModule.h"

bool NFCWorldGuildBroadcastModule::Init()
{
    return true;
}

bool NFCWorldGuildBroadcastModule::Shut()
{
    return true;
}

bool NFCWorldGuildBroadcastModule::Execute(const float fLasFrametime, const float fStartedTime)
{
    return true;
}

bool NFCWorldGuildBroadcastModule::AfterInit()
{
    m_pEventProcessModule = dynamic_cast<NFIEventProcessModule*>(pPluginManager->FindModule("NFCEventProcessModule"));
    m_pKernelModule = dynamic_cast<NFIKernelModule*>(pPluginManager->FindModule("NFCKernelModule"));
    m_pWorldGuildDataModule = dynamic_cast<NFIWorldGuildDataModule*>(pPluginManager->FindModule("NFCWorldGuildDataModule"));
    m_pWorldNet_ServerModule = dynamic_cast<NFIWorldNet_ServerModule*>(pPluginManager->FindModule("NFCWorldNet_ServerModule"));
    m_pWorldGuildModule = dynamic_cast<NFIWorldGuildModule*>(pPluginManager->FindModule("NFCWorldGuildModule"));

    assert(NULL != m_pEventProcessModule);
    assert(NULL != m_pKernelModule);
    assert(NULL != m_pWorldGuildDataModule);
    assert(NULL != m_pWorldNet_ServerModule);
    assert(NULL != m_pWorldGuildModule);

    m_pKernelModule->ResgisterCommonPropertyEvent( this, &NFCWorldGuildBroadcastModule::OnPropertyCommonEvent );
    m_pKernelModule->ResgisterCommonRecordEvent( this, &NFCWorldGuildBroadcastModule::OnRecordCommonEvent );

    return true;
}

int NFCWorldGuildBroadcastModule::OnPropertyCommonEvent( const NFIDENTID& self, const std::string& strPropertyName, const NFIDataList& oldVar, const NFIDataList& newVar)
{
    if ( oldVar.GetCount() <= 0 )
    {
        return 0;
    }

    if ( "Guild" != m_pKernelModule->GetPropertyString( self, "ClassName" ))
    {
        return 0;
    }

    NFCDataList valueBroadCaseList;
    NFCDataList valueBroadCaseGameList;

    if (!m_pWorldGuildModule->GetOnlineMember(NFIDENTID(), self, valueBroadCaseList, valueBroadCaseGameList))
    {
        return 0;
    }

    if ( valueBroadCaseList.GetCount() <= 0 || valueBroadCaseList.GetCount() != valueBroadCaseGameList.GetCount())
    {
        return 0;
    }

    switch ( oldVar.Type( 0 ) )
    {
    case TDATA_INT:
        {
            NFMsg::ObjectPropertyInt xPropertyInt;
            NFMsg::Ident* pIdent = xPropertyInt.mutable_player_id();
            *pIdent = NFINetModule::NFToPB(self);

            NFMsg::PropertyInt* pDataInt = xPropertyInt.add_property_list();
            pDataInt->set_property_name( strPropertyName );
            pDataInt->set_data( newVar.Int( 0 ) );

            m_pWorldNet_ServerModule->SendMsgToGame(valueBroadCaseList, valueBroadCaseGameList, NFMsg::EGMI_ACK_PROPERTY_INT, xPropertyInt);

        }
        break;

    case TDATA_FLOAT:
        {
            NFMsg::ObjectPropertyFloat xPropertyFloat;
            NFMsg::Ident* pIdent = xPropertyFloat.mutable_player_id();
            *pIdent = NFINetModule::NFToPB(self);

            NFMsg::PropertyFloat* pDataFloat = xPropertyFloat.add_property_list();
            pDataFloat->set_property_name( strPropertyName );
            pDataFloat->set_data( newVar.Float( 0 ) );

            m_pWorldNet_ServerModule->SendMsgToGame(valueBroadCaseList, valueBroadCaseGameList, NFMsg::EGMI_ACK_PROPERTY_FLOAT, xPropertyFloat);
        }
        break;

    case TDATA_DOUBLE:
        {
            NFMsg::ObjectPropertyFloat xPropertyDouble;
            NFMsg::Ident* pIdent = xPropertyDouble.mutable_player_id();
            *pIdent = NFINetModule::NFToPB(self);

            NFMsg::PropertyFloat* pDataFloat = xPropertyDouble.add_property_list();
            pDataFloat->set_property_name( strPropertyName );
            pDataFloat->set_data( newVar.Double( 0 ) );

            m_pWorldNet_ServerModule->SendMsgToGame(valueBroadCaseList, valueBroadCaseGameList, NFMsg::EGMI_ACK_PROPERTY_DOUBLE, xPropertyDouble);
        }
        break;

    case TDATA_STRING:
        {
            NFMsg::ObjectPropertyString xPropertyString;
            NFMsg::Ident* pIdent = xPropertyString.mutable_player_id();
            *pIdent = NFINetModule::NFToPB(self);

            NFMsg::PropertyString* pDataString = xPropertyString.add_property_list();
            pDataString->set_property_name( strPropertyName );
            pDataString->set_data( newVar.String( 0 ) );

            m_pWorldNet_ServerModule->SendMsgToGame(valueBroadCaseList, valueBroadCaseGameList, NFMsg::EGMI_ACK_PROPERTY_STRING, xPropertyString);
        }
        break;

    case TDATA_OBJECT:
        {
            NFMsg::ObjectPropertyObject xPropertyObject;
            NFMsg::Ident* pIdent = xPropertyObject.mutable_player_id();
            *pIdent = NFINetModule::NFToPB(self);

            NFMsg::PropertyObject* pDataObject = xPropertyObject.add_property_list();
            pDataObject->set_property_name( strPropertyName );
            *pDataObject->mutable_data() = NFINetModule::NFToPB(newVar.Object( 0 ));

            m_pWorldNet_ServerModule->SendMsgToGame(valueBroadCaseList, valueBroadCaseGameList, NFMsg::EGMI_ACK_PROPERTY_OBJECT, xPropertyObject);

        }
        break;

    default:
        break;
    }


    return 0;
}


int NFCWorldGuildBroadcastModule::OnRecordCommonEvent( const NFIDENTID& self, const RECORD_EVENT_DATA& xEventData, const NFIDataList& oldVar, const NFIDataList& newVar )
{
    const std::string& strRecordName = xEventData.strRecordName; 
    int nOpType = xEventData.nOpType;
    int nRow = xEventData.nRow;
    int nCol = xEventData.nCol;

    if ( "Guild" != m_pKernelModule->GetPropertyString( self, "ClassName" ))
    {
        return 0;
    }

    NFCDataList valueBroadCaseList;
    NFCDataList valueBroadCaseGameList;
    if (!m_pWorldGuildModule->GetOnlineMember(NFIDENTID(), self, valueBroadCaseList, valueBroadCaseGameList))
    {
        return 0;
    }

    if (valueBroadCaseList.GetCount() <= 0 || valueBroadCaseList.GetCount() != valueBroadCaseGameList.GetCount())
    {
        return 0;
    }

    switch ( nOpType )
    {
    case NFIRecord::RecordOptype::Add:
        {
            NFMsg::ObjectRecordAddRow xAddRecordRow;
            NFMsg::Ident* pIdent = xAddRecordRow.mutable_player_id();
            *pIdent = NFINetModule::NFToPB(self);

            xAddRecordRow.set_record_name( strRecordName );

            NFMsg::RecordAddRowStruct* pAddRowData = xAddRecordRow.add_row_data();
            pAddRowData->set_row(nRow);

            //add row 需要完整的row
            for ( int i = 0; i < newVar.GetCount(); i++ )
            {
                switch ( newVar.Type( i ) )
                {
                case TDATA_INT:
                    {
                        //添加的时候数据要全s
                        int nValue = newVar.Int( i );
                        //if ( 0 != nValue )
                        {
                            NFMsg::RecordInt* pAddData = pAddRowData->add_record_int_list();
                            pAddData->set_col( i );
                            pAddData->set_row( nRow );
                            pAddData->set_data( nValue );
                        }
                    }
                    break;
                case TDATA_FLOAT:
                    {
                        float fValue = newVar.Float( i );
                        //if ( fValue > 0.001f  || fValue < -0.001f )
                        {
                            NFMsg::RecordFloat* pAddData = pAddRowData->add_record_float_list();
                            pAddData->set_col( i );
                            pAddData->set_row( nRow );
                            pAddData->set_data( fValue );
                        }
                    }
                    break;
                case TDATA_DOUBLE:
                    {
                        float fValue = newVar.Double( i );
                        //if ( fValue > 0.001f  || fValue < -0.001f )
                        {
                            NFMsg::RecordFloat* pAddData = pAddRowData->add_record_float_list();
                            pAddData->set_col( i );
                            pAddData->set_row( nRow );
                            pAddData->set_data( fValue );
                        }
                    }
                    break;
                case TDATA_STRING:
                    {
                        const std::string& str = newVar.String( i );
                        //if (!str.empty())
                        {
                            NFMsg::RecordString* pAddData = pAddRowData->add_record_string_list();
                            pAddData->set_col( i );
                            pAddData->set_row( nRow );
                            pAddData->set_data( str );
                        }
                    }
                    break;
                case TDATA_OBJECT:
                    {
                        NFIDENTID identValue = newVar.Object( i );
                        //if (!identValue.IsNull())
                        {
                            NFMsg::RecordObject* pAddData = pAddRowData->add_record_object_list();
                            pAddData->set_col( i );
                            pAddData->set_row( nRow );

                            *pAddData->mutable_data() = NFINetModule::NFToPB(identValue);
                        }
                    }
                    break;
                default:
                    break;
                }
            }

            m_pWorldNet_ServerModule->SendMsgToGame(valueBroadCaseList, valueBroadCaseGameList, NFMsg::EGMI_ACK_ADD_ROW, xAddRecordRow);
        }
        break;
    case NFIRecord::RecordOptype::Del:
        {
            NFMsg::ObjectRecordRemove xReoveRecordRow;

            NFMsg::Ident* pIdent = xReoveRecordRow.mutable_player_id();
            *pIdent = NFINetModule::NFToPB(self);

            xReoveRecordRow.set_record_name( strRecordName );
            xReoveRecordRow.add_remove_row( nRow );

            m_pWorldNet_ServerModule->SendMsgToGame(valueBroadCaseList, valueBroadCaseGameList, NFMsg::EGMI_ACK_REMOVE_ROW, xReoveRecordRow);


            const int nCol = xEventData.nCol;
            NF_SHARE_PTR<NFIRecord> pRecord = m_pKernelModule->FindRecord(self, strRecordName);
            if (pRecord && strRecordName == "GuildMemberList")
            {
                //add
                const NFIDENTID& nPlayer = pRecord->GetObject(nRow, NFMsg::GuildMemberList_GUID);
                NFINT64 nGameID = pRecord->GetInt(nRow, NFMsg::GuildMemberList_GameID);

                NFCDataList varSelf;
                NFCDataList varGameID;

                varSelf << nPlayer;
                varGameID << nGameID;

                NFCDataList varObject;
                varObject << self;
                m_pWorldNet_ServerModule->OnObjectListLeave(varSelf, varObject);
            }
        }
        break;
    case NFIRecord::RecordOptype::Swap:
        {
            //其实是2个row交换
            NFMsg::ObjectRecordSwap xSwapRecord;
            *xSwapRecord.mutable_player_id() = NFINetModule::NFToPB(self);

            xSwapRecord.set_origin_record_name( strRecordName );
            xSwapRecord.set_target_record_name( strRecordName ); // 暂时没用
            xSwapRecord.set_row_origin( nRow );
            xSwapRecord.set_row_target( nCol );

            m_pWorldNet_ServerModule->SendMsgToGame(valueBroadCaseList, valueBroadCaseGameList, NFMsg::EGMI_ACK_SWAP_ROW, xSwapRecord);
        }
        break;
    case NFIRecord::RecordOptype::UpData:
        {
            //装备的过程中是不能更新的
            const int nCol = xEventData.nCol;
            NF_SHARE_PTR<NFIRecord> pRecord = m_pKernelModule->FindRecord(self, strRecordName);
            if (pRecord && strRecordName == "GuildMemberList")
            {
                if (nCol == NFMsg::GuildMemberList_Online && newVar.Int( 0 ) > 0)
                {
                    //add
                    const NFIDENTID& nPlayer = pRecord->GetObject(nRow, NFMsg::GuildMemberList_GUID);
                    NFINT64 nGameID = pRecord->GetInt(nRow, NFMsg::GuildMemberList_GameID);

                    NFCDataList varSelf;
                    NFCDataList varGameID;

                    varSelf << nPlayer;
                    varGameID << nGameID;

                    NFCDataList varObject;
                    varObject << self;
                    m_pWorldNet_ServerModule->OnObjectListEnter(varSelf, varObject);


                    m_pWorldNet_ServerModule->OnPropertyEnter(varSelf, varGameID, self);
                    m_pWorldNet_ServerModule->OnRecordEnter(varSelf, varGameID, self);
                }
            }

            switch ( oldVar.Type( 0 ) )
            {
            case TDATA_INT:
                {
                    NFMsg::ObjectRecordInt xRecordChanged;
                    *xRecordChanged.mutable_player_id() = NFINetModule::NFToPB(self);

                    xRecordChanged.set_record_name( strRecordName );
                    NFMsg::RecordInt* recordProperty = xRecordChanged.add_property_list();
                    recordProperty->set_row( nRow );
                    recordProperty->set_col( nCol );
                    int nData = newVar.Int( 0 );
                    recordProperty->set_data( nData );

                    m_pWorldNet_ServerModule->SendMsgToGame(valueBroadCaseList, valueBroadCaseGameList, NFMsg::EGMI_ACK_RECORD_INT, xRecordChanged);

                }
                break;

            case TDATA_FLOAT:
                {
                    NFMsg::ObjectRecordFloat xRecordChanged;
                    *xRecordChanged.mutable_player_id() = NFINetModule::NFToPB(self);

                    xRecordChanged.set_record_name( strRecordName );
                    NFMsg::RecordFloat* recordProperty = xRecordChanged.add_property_list();
                    recordProperty->set_row( nRow );
                    recordProperty->set_col( nCol );
                    recordProperty->set_data( newVar.Float( 0 ) );

                    m_pWorldNet_ServerModule->SendMsgToGame(valueBroadCaseList, valueBroadCaseGameList, NFMsg::EGMI_ACK_RECORD_FLOAT, xRecordChanged);

                }
                break;
            case TDATA_DOUBLE:
                {
                    NFMsg::ObjectRecordFloat xRecordChanged;
                    *xRecordChanged.mutable_player_id() = NFINetModule::NFToPB(self);

                    xRecordChanged.set_record_name( strRecordName );
                    NFMsg::RecordFloat* recordProperty = xRecordChanged.add_property_list();
                    recordProperty->set_row( nRow );
                    recordProperty->set_col( nCol );
                    recordProperty->set_data( newVar.Double( 0 ) );

                    m_pWorldNet_ServerModule->SendMsgToGame(valueBroadCaseList, valueBroadCaseGameList, NFMsg::EGMI_ACK_RECORD_DOUBLE, xRecordChanged);

                }
                break;
            case TDATA_STRING:
                {
                    NFMsg::ObjectRecordString xRecordChanged;
                    *xRecordChanged.mutable_player_id() = NFINetModule::NFToPB(self);

                    xRecordChanged.set_record_name( strRecordName );
                    NFMsg::RecordString* recordProperty = xRecordChanged.add_property_list();
                    recordProperty->set_row( nRow );
                    recordProperty->set_col( nCol );
                    recordProperty->set_data( newVar.String( 0 ) );

                    m_pWorldNet_ServerModule->SendMsgToGame(valueBroadCaseList, valueBroadCaseGameList, NFMsg::EGMI_ACK_RECORD_STRING, xRecordChanged);
                }
                break;
            case TDATA_OBJECT:
                {
                    NFMsg::ObjectRecordObject xRecordChanged;
                    *xRecordChanged.mutable_player_id() = NFINetModule::NFToPB(self);

                    xRecordChanged.set_record_name( strRecordName );
                    NFMsg::RecordObject* recordProperty = xRecordChanged.add_property_list();
                    recordProperty->set_row( nRow );
                    recordProperty->set_col( nCol );
                    *recordProperty->mutable_data() = NFINetModule::NFToPB(newVar.Object( 0 ));

                    m_pWorldNet_ServerModule->SendMsgToGame(valueBroadCaseList, valueBroadCaseGameList, NFMsg::EGMI_ACK_RECORD_OBJECT, xRecordChanged);
                }
                break;

            default:
                return 0;
                break;
            }
        }
        break;
    case NFIRecord::RecordOptype::Create:
        return 0;
        break;
    case NFIRecord::RecordOptype::Cleared:
        {
        }
        break;
    default:
        break;
    }

    return 0;
 }
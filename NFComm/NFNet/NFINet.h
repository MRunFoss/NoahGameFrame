// -------------------------------------------------------------------------
//    @FileName         ：    NFINet.h
//    @Author           ：    LvSheng.Huang
//    @Date             ：    2013-12-15
//    @Module           ：    NFINet
//    @Desc             :     INet
// -------------------------------------------------------------------------

#ifndef __NFI_NET_H__
#define __NFI_NET_H__

#include <cstring>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <stdint.h>
#include <iostream>
#include <map>
//#include <xtree>

#if NF_PLATFORM != NF_PLATFORM_WIN
#include <netinet/in.h>
# ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif
#include <sys/socket.h>
#endif

#include <vector>
#include <functional>
#include <memory>
#include <list>
#include <vector>
#include "NFIPacket.h"
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/thread.h>
#include <event2/event_compat.h>
#include "NFComm/NFPluginModule/NFPlatform.h"
#include "NFComm/NFPluginModule/NFIdentID.h"

#pragma pack(push, 1)

enum NF_NET_EVENT
{
    NF_NET_EVENT_EOF = 0x10,	//掉线
    NF_NET_EVENT_ERROR = 0x20,//位置错误
    NF_NET_EVENT_TIMEOUT = 0x40,//连接超时
    NF_NET_EVENT_CONNECTED = 0x80,//连接成功(作为客户端)
};

class NFINet;

typedef std::function<int(const NFIPacket& msg)> NET_RECIEVE_FUNCTOR;
typedef NF_SHARE_PTR<NET_RECIEVE_FUNCTOR> NET_RECIEVE_FUNCTOR_PTR;

typedef std::function<int(const int nSockIndex, const NF_NET_EVENT nEvent, NFINet* pNet)> NET_EVENT_FUNCTOR;
typedef NF_SHARE_PTR<NET_EVENT_FUNCTOR> NET_EVENT_FUNCTOR_PTR;

typedef std::function<void(int severity, const char *msg)> NET_EVENT_LOG_FUNCTOR;
typedef NF_SHARE_PTR<NET_EVENT_LOG_FUNCTOR> NET_EVENT_LOG_FUNCTOR_PTR;



class NetObject
{
public:
    NetObject(NFINet* pThis, int32_t nFd, sockaddr_in& addr, bufferevent* pBev)
    {
        m_pNet = pThis;

        nIndex = nFd;
        bev = pBev;
        memset(&sin, 0, sizeof(sin));
        sin = addr;

        mnErrorCount = 0;
        mnLogicState = 0;
        mnUserData = 0;
        mnUserID = NFIDENTID();
		mbRemoveState = false;
    }

    virtual ~NetObject()
    {
    }

    int AddBuff(const char* str, uint32_t nLen)
    {
        mstrBuff.append(str, nLen);

        return mstrBuff.length();
    }

    int CopyBuffTo(char* str, uint32_t nStart, uint32_t nLen)
    {
        if (nStart + nLen > mstrBuff.length())
        {
            return 0;
        }

        memcpy(str, mstrBuff.data() + nStart, nLen);

        return nLen;
    }

    int RemoveBuff(uint32_t nStart, uint32_t nLen)
    {
        if (nStart < 0)
        {
            return 0;
        }

        //移除前面，则后面跟上
        if (nStart + nLen > mstrBuff.length())
        {
            return 0;
        }

        //把后面的挪到前面来
        mstrBuff.erase(nStart, nLen);

        return mstrBuff.length();
    }

    const char* GetBuff()
    {
        return mstrBuff.data();
    }

    int GetBuffLen() const
    {
        return mstrBuff.length();
    }

    int GetFd() const
    {
        return nIndex;
    }

    int GetErrorCount() const
    {
        return mnErrorCount;
    }

    int GetConnectKeyState() const
    {
        return mnLogicState;
    }

    void SetConnectKeyState(const int nState)
    {
        mnLogicState = nState;
    }

    int GetGameID() const
    {
        return mnUserData;
    }

    void SetGameID(const int nData)
    {
        mnUserData = nData;
    }

    const std::string& GetAccount() const
    {
        return mstrUserData;
    }

    void SetAccount(const std::string& strData)
    {
        mstrUserData = strData;
    }

	const bool GetRemoveState() const
	{
		return mbRemoveState;
	}

	void SetRemoveState(const bool bState)
	{
		mbRemoveState = bState;
	}

    const NFIDENTID& GetUserID()
    {
        return mnUserID;
    }

    void SetUserID(const NFIDENTID& nUserID)
    {
        mnUserID = nUserID;
    }
	const NFIDENTID& GetClientID()
	{
		return mnClientID;
	}

	void SetClientID(const NFIDENTID& xClientID)
	{
		mnClientID = xClientID;
	}
    int IncreaseError(const int nError = 1)
    {
        return mnErrorCount += nError;
    }

    bufferevent * GetBuffEvent()
    {
        return bev;
    }

    NFINet* GetNet()
    {
        return m_pNet;
    }
private:
    uint32_t nIndex;
    sockaddr_in sin;
    bufferevent *bev;
    std::string mstrBuff;

    uint16_t mnErrorCount;
    int32_t mnLogicState;
    int32_t mnUserData;
    std::string mstrUserData;
	bool mbRemoveState;
	NFIDENTID mnUserID;
	NFIDENTID mnClientID;
    NFINet* m_pNet;
};

class NFINet
{
public:
	virtual  bool Execute(const float fLasFrametime, const float fStartedTime) = 0;

	virtual  void Initialization(const char* strIP, const unsigned short nPort) = 0;
	virtual  int Initialization(const unsigned int nMaxClient, const unsigned short nPort, const int nCpuCount = 4) = 0;

	virtual  bool Final() = 0;
    virtual  bool Reset() = 0;

	//带合法包头
	virtual bool SendMsg(const NFIPacket& msg, const int nSockIndex = 0) = 0;
	virtual bool SendMsgToAllClient(const NFIPacket& msg) = 0;

	//数据裸发
	virtual bool SendMsg(const char* msg, const uint32_t nLen, const int nSockIndex = 0) = 0;
	virtual bool SendMsgToAllClient(const char* msg, const uint32_t nLen) = 0;

	virtual int OnRecivePacket(const int nSockIndex, const char* msg, const uint32_t nLen){return 1;};

	virtual bool CloseNetObject(const int nSockIndex) = 0;
    virtual NetObject* GetNetObject(const int nSockIndex) = 0;
    virtual bool AddNetObject(const int nSockIndex, NetObject* pObject) = 0;

	virtual bool AddBan(const int nSockIndex, const int32_t nTime = -1) = 0;
	virtual bool RemoveBan(const int nSockIndex) = 0;

	virtual NFIMsgHead::NF_Head GetHeadLen() = 0;
	virtual bool IsServer() = 0;

	virtual bool Log(int severity, const char *msg) = 0;

	template<typename BaseType>
	void SetNetLogCB(BaseType* pBaseType, void (BaseType::*handleRecieve)(int severity, const char *msg))
	{
		//static??
		mLogEventCB.push_back(std::bind(handleRecieve, pBaseType, std::placeholders::_1, std::placeholders::_2));
	}

protected:
	static std::vector<NET_EVENT_LOG_FUNCTOR> mLogEventCB;

};

#pragma pack(pop)

#endif

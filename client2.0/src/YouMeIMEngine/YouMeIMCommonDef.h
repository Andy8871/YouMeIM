#pragma once
//定义所有的错误码，这个错误码内部使用
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeIMEngine/pb/im_common.pb.h>
#include <YouMeIMEngine/YouMeInterface.h>
#include <YouMeCommon/XSharedArray.h>
#include <YouMeCommon/XAny.h>
#include <map>
#include <mutex>

class YouMeIMManager;

class IPacketCallback
{
public:
	//接收到服务器包
	virtual void OnRecvPacket(const YouMeIMProtocol::ServerReqHead& head, const byte*pBuffer, int iBufferSize) = 0;
	//对于一些需要回应的包，通过这个回调来告诉上层，接收服务器的回应包超时了。
	virtual void OnRecvPacketTimeOut(XINT64 iUniqueID) = 0;
};

//消息类型实现模板类
template <class T>
class BaseMessageBodyTypeT :public T
{
public:
	virtual YouMeIMMessageBodyType GetMessageType()override
	{
		return m_messageType;
	}
protected:
	YouMeIMMessageBodyType m_messageType;
};
//提供给外部的很多接口都有properties 这东西，抽象出来一个模板类，方便使用
template <class T>
class BasePropertiesT :public T
{
	friend class YouMeIMMessageManager;
public:
	virtual void SetProperties(const std::string& strKey, const XString& strValue) override
	{
		m_properties[strKey] = strValue;
	}
	virtual bool GetProperties(const std::string& strKey, XString& strValue) override
	{
		std::map<std::string, XString>::iterator it = m_properties.find(strKey);
		if (it == m_properties.end())
		{
			return false;
		}
		strValue = it->second;
		return true;
	}
private:
	std::map<std::string, XString> m_properties;
};
struct UDPTCPPacketInfo
{
	//包数据
	youmecommon::CXSharedArray<char> pPacket;
	//上一次发送时间,unix time 时间戳,单位ms
	XINT64 ulSendTime;
	YouMeIMProtocol::Command_TYPE commandType;
	XINT64 ulTimeout;
	UDPTCPPacketInfo()
	{
		commandType = YouMeIMProtocol::Command_Unknow;
		ulSendTime = 0;
		ulTimeout = 0;
	}
};



//用来保存所有的发送的消息对应关系，当确认包回来了的时候，知道是哪个类型以及包的信息
struct MessageInfo
{
	MessageInfo(YouMeIMProtocol::Command_TYPE type)
	{
		command = type;
	}
	YouMeIMProtocol::Command_TYPE command;
	//命令的参数
	std::map<std::string, youmecommon::CXAny> commandParam;

	void SetParam(const std::string&strKey, const youmecommon::CXAny& value)
	{
		commandParam[strKey] = value;
	}


	template<typename ValueType>
	ValueType GetParam(const std::string&strKey, const ValueType& defaultValue)
	{
		std::map<std::string, youmecommon::CXAny>::iterator it = commandParam.find(strKey);
		if (it != commandParam.end())
		{
			return it->second.Get<ValueType>();
		}
		return defaultValue;
	}

	XString GetParam(const std::string&strKey, const XString& defaultValue)
	{
		return GetParam<XString>(strKey, defaultValue);
	}
};



//标准管理器处理基类
class IManagerBase
{
public:
	IManagerBase(YouMeIMManager* pIManager)
	{
		m_pIManager = pIManager;
	}
	//下面两个函数用来处理上一个管理器没有处理的包
	virtual bool OnRecvPacket(const YouMeIMProtocol::ServerReqHead& head, const byte*pBuffer, int iBufferSize) = 0;
	virtual bool OnRecvPacketTimeOut(XINT64 iUniqueID) = 0;
protected:
	//保存一个当前类发送的数据
	std::map<XINT64, std::shared_ptr<MessageInfo> > m_messageMap;
	std::mutex m_messageCacheMutex;
	YouMeIMManager* m_pIManager;
};

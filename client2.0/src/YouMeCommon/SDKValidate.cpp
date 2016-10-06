//
//  SDKValidate.cpp
//  YouMeIM
//
//  Created by joexie on 16/5/8.
//
//
#include <YouMeCommon/SyncTCP.h>
#include "SDKValidate.hpp"
#include <YouMeCommon/pb/common.pb.h>
#include <YouMeCommon/Log.h>
#include <YouMeCommon/RSAUtil.h>
#include <YouMeCommon/CryptUtil.h>
#include <YouMeCommon/StringUtil.hpp>
#include <YouMeCommon/NetworkService.h>
#include <YouMeCommon/XDNSParse.h>
#include <YouMeCommon/profiledb.h>
namespace youmecommon
{
CSDKValidate::CSDKValidate()
{
    m_status = SDKValidateStatus_NotInit;
}

bool CSDKValidate::Init(const XString &strDomainOrIP, int iPort,const XString& strDefaultIP)
{
    m_strDefaultIP = strDefaultIP;
    m_strDomainOrIP = strDomainOrIP;
    m_iServerPort = iPort;
    m_status = SDKValidateStatus_Init;
    return  true;
}
bool CSDKValidate::InterParseSecret(const XString &strSecret,CRSAUtil& rsa)
{
    bool bParseSuccess = false;
    do
    {
        CXSharedArray<char> pDecodeBuffer;
        if (!::youmecommon::CCryptUtil::Base64Decoder(XStringToUTF8(strSecret), pDecodeBuffer))
        {
            YouMe_LOG_Error(__XT("解析secret 失败了:%s"),strSecret.c_str());
            break;
        }
        if (131 != pDecodeBuffer.GetBufferLen())
        {
            YouMe_LOG_Error (__XT("base64 解密不对，:%s 长度:%d"), strSecret.c_str (),pDecodeBuffer.GetBufferLen());
            break;
        }
        CXSharedArray<unsigned char> publicKey;
        CXSharedArray<unsigned char>  module;
        
        //模数
        module.Allocate(128);
        memcpy(module.Get(), pDecodeBuffer.Get(), 128);
        
        // publickey
        publicKey.Allocate(3);
        memcpy(publicKey.Get(), pDecodeBuffer.Get()+128, 3);
        
        bParseSuccess = rsa.SetPublicKey (publicKey, module);
    } while (0);
    
    return bParseSuccess;
}

SDKValidateErrorcode CSDKValidate::StartValidate(std::map<std::string, CXAny>&configurations,CXCondWait* sdkValidWait)
{
	assert(sdkValidWait != NULL);
    YouMe_LOG_Info(__XT("Enter :%d server:%s default:%s port:%d"),m_status,m_strDomainOrIP.c_str(),m_strDefaultIP.c_str(), m_iServerPort);
    if (m_status == SDKValidateStatus_NotInit) {
        return SDKValidateErrorcode_NotInit;
    }
    if (m_status == SDKValidateStatus_Ing) {
        return SDKValidateErrorcode_ValidateIng;
    }
    //开始构建数据
    CRSAUtil rsa;
    if (!InterParseSecret(CYMApplication::GetInstance().GetAppSecret(),rsa))
    {
        YouMe_LOG_Error(__XT("使用的 secret 解析失败:%s"),CYMApplication::GetInstance().GetAppSecret().c_str());
        return SDKValidateErrorcode_ILLEGAL_SDK;
    }
    //使用公钥对一个随机的数字进行rsa 加密
    int iRand = rand ();
    //对数据进行RSA加密
    CXSharedArray<unsigned char>  rsaEncryptBuffer;
    if (!rsa.EncryptByPublicKey ((const unsigned char *)&iRand,sizeof(int), rsaEncryptBuffer))
    {
        YouMe_LOG_Error (__XT("加密原始数据失败了"));
        return SDKValidateErrorcode_ILLEGAL_SDK;
    }
    YouMeCommonProtocol::SDKValidateRequest validProtocol;
    validProtocol.set_data(rsaEncryptBuffer.Get(), rsaEncryptBuffer.GetBufferLen());
	validProtocol.set_allocated_head(CreateRequestHead(0));
    //开始校验
	
	sdkValidWait->Reset();
    std::string strSendData;
    validProtocol.SerializeToString(&strSendData);
    
    //开始DNS 域名解析
    std::vector<XString> ipList;

	//CXDNSParse::ParseDomain2(m_strDomainOrIP, ipList, 3000);
    CXDNSParse::Instance()->ParseDomain2(m_strDomainOrIP, ipList, 3000);
    if (ipList.empty() && !m_strDefaultIP.empty()) {
        ipList.push_back(m_strDefaultIP);
    }
	//如果是空则从配置里面读取一个
	if (ipList.empty())
	{
		XString strBackupIP;
		CProfileDB::Instance().getSetting(XStringToUTF8(m_strDomainOrIP), strBackupIP);
		if (!strBackupIP.empty())
		{
			ipList.push_back(strBackupIP);
		}
	}

    //退出吧
    if (ipList.empty()) {
        YouMe_LOG_Error(__XT("域名解析失败，并且没有设置默认的ip，退出吧"));
        return SDKValidateErrorcode_DNSParseError;
    }
    //更新ip
	CProfileDB::Instance().setSetting(XStringToUTF8(m_strDomainOrIP), ipList[0]);
	YouMe_LOG_Info(__XT("开始给服务器发送SDK 验证包"));

    for (size_t j=0; j<ipList.size(); j++)
    {
        ::youmecommon::CSyncTCP sdkTCP;
        if (!sdkTCP.Init(ipList.at(j), m_iServerPort))
        {
            YouMe_LOG_Info(__XT("TCP 初始化失败了，win32 平台是否调用了wsastartup"));
            return SDKValidateErrorcode_Fail;
        }
		if (!sdkTCP.Connect(3000))
		{
			YouMe_LOG_Warning(__XT("TCP 连接服务器失败了：%s"),ipList.at(j).c_str());
			continue;
		}
        sdkTCP.SendData(strSendData.c_str(), strSendData.length());
                
        //判断是否要退出
		if (WaitResult_Timeout != sdkValidWait->WaitTime(0)) {
            YouMe_LOG_Info (__XT("SDK 验证的时候上层要求退出了，赶紧的退出"));
            return SDKValidateErrorcode_Abort;
        }
        //接受数据
        CXSharedArray<char> recvBuffer;
		int num = sdkTCP.RecvData(recvBuffer);
        YouMe_LOG_Info (__XT("SDK 验证返回数据长度:%d"),num);
        if (num <= 0)
        {
            YouMe_LOG_Warning(__XT("SDK 验证 接受数据超时:%s"),ipList.at(j).c_str());
            continue;
        }
        else
        {
            //成功退出
            YouMeCommonProtocol::SDKValidateResponse response;
            if (!response.ParseFromArray(recvBuffer.Get(),num))
            {
                YouMe_LOG_Error(__XT("protocol 协议解析失败，服务器可能协议不一致"),ipList.at(j).c_str());
                return SDKValidateErrorcode_ProtoError;
            }
                    
            CXSharedArray<unsigned char> rsaDeBuffer ;
            if (!rsa.DecryptByPublicKey ((const unsigned char *)response.data().c_str(), response.data().length(), rsaDeBuffer))
            {
                YouMe_LOG_Error (__XT("解密失败"));
                return SDKValidateErrorcode_ServerError;
            }
                    
            int iStatus = response.status ();
            YouMe_LOG_Info(__XT("status:%d : %d :%s"), iStatus,response.timeout(),ipList.at(j).c_str());
            if (0 == iStatus)
            {
                for (int i = 0; i < response.configurations_size (); i++)
                {
                    YouMe_LOG_Info(__XT("type：%d key:%s value:%s"),response.configurations(i).type(),UTF8TOXString(response.configurations (i).name ()).c_str(), UTF8TOXString(response.configurations (i).value ()).c_str());
                    switch (response.configurations(i).type()) {
                    case YouMeCommonProtocol::NAME_BOOL:
                        {
							configurations.insert(std::map<std::string, CXAny>::value_type(response.configurations(i).name(), (CStringUtilT<char>::str_to_bool(response.configurations(i).value().c_str()))));
                        }
                            break;
                    case YouMeCommonProtocol::NAME_INT32:
                        {
							configurations.insert(std::map<std::string, CXAny>::value_type(response.configurations(i).name(), CStringUtilT<char>::str_to_sint32(response.configurations(i).value().c_str())));
                        }
                            break;
                    case YouMeCommonProtocol::NAME_INT64:
                        {
							configurations.insert(std::map<std::string, CXAny>::value_type(response.configurations(i).name(),CStringUtilT<char>::str_to_sint64(response.configurations(i).value().c_str())));
                        }
                    case YouMeCommonProtocol::NAME_UIN32:
                        {
							configurations.insert(std::map<std::string, CXAny>::value_type(response.configurations(i).name(), CStringUtilT<char>::str_to_uint32(response.configurations(i).value().c_str())));
                        }
                    case YouMeCommonProtocol::NAME_UINT64:
                        {
							configurations.insert(std::map<std::string, CXAny>::value_type(response.configurations(i).name(), CStringUtilT<char>::str_to_uint64(response.configurations(i).value().c_str())));
                        }
                            break;
                    case YouMeCommonProtocol::NAME_STRING:
                        {
                            configurations.insert (std::map<std::string, CXAny>::value_type (response.configurations (i).name (),UTF8TOXString(response.configurations (i).value ())));
                        }
                            break;
                        default:
							YouMe_LOG_Warning(__XT("发现了没有处理的类型 type：%d key:%s value:%s"), response.configurations(i).type(), UTF8TOXString(response.configurations(i).name()).c_str(), UTF8TOXString(response.configurations(i).value()).c_str());
                            break;
                    }
                }
                return SDKValidateErrorcode_Success;
            }
        }
    }
    
    return SDKValidateErrorcode_NetworkInvalid;
}

::YouMeCommonProtocol::PacketHead* CSDKValidate::CreateRequestHead(int iEvtType)
{
	::YouMeCommonProtocol::PacketHead* pHead = new ::YouMeCommonProtocol::PacketHead;
	pHead->set_brand(XStringToUTF8(CYMApplication::GetInstance().GetBrand()));
	pHead->set_msgtype(iEvtType);
	pHead->set_msgversion(CYMApplication::GetInstance().GetProtoVer());
#ifdef WIN32
	pHead->set_platform(YouMeCommonProtocol::Platform_Windows);
#elif OS_OSX
	pHead->set_platform(YouMeCommonProtocol::Platform_OSX);
#elif OS_IOS
	pHead->set_platform(YouMeCommonProtocol::Platform_IOS);
#elif OS_IOSSIMULATOR
	pHead->set_platform(YouMeCommonProtocol::Platform_IOS);
#elif OS_ANDROID
	pHead->set_platform(YouMeCommonProtocol::Platform_Android);
#elif OS_LINUX
	pHead->set_platform(YouMeCommonProtocol::Platform_Linux);
#endif
	pHead->set_sysversion(XStringToUTF8(CYMApplication::GetInstance().GetSysVer()));
	pHead->set_ymsdkversion(CYMApplication::GetInstance().GetYMSDKVersion());
	pHead->set_cpuarch(XStringToUTF8(CYMApplication::GetInstance().GetCPUArch()));
	pHead->set_cpuchip(XStringToUTF8(CYMApplication::GetInstance().GetCPUChip()));
	pHead->set_model(XStringToUTF8(CYMApplication::GetInstance().GetModel()));
	pHead->set_appkey(XStringToUTF8(CYMApplication::GetInstance().GetAppKey()));
	pHead->set_identify(XStringToUTF8(CYMApplication::GetInstance().GetIdentify()));
	pHead->set_packagename(XStringToUTF8(CYMApplication::GetInstance().GetPackageName()));

	switch (::youmecommon::CNetworkService::Instance()->getNetworkType()) {
	case ::youmecommon::NetworkType_3G:
		pHead->set_networktype(::YouMeCommonProtocol::NetworkType_3G);
		break;
	case ::youmecommon::NetworkType_Wifi:
		pHead->set_networktype(::YouMeCommonProtocol::NetworkType_Wifi);
		break;
	default:
		pHead->set_networktype(::YouMeCommonProtocol::NetworkType_Unknow);
		break;
	}

	pHead->set_sdkname("");
	return pHead;
}
}

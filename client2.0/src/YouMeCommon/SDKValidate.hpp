//
//  SDKValidate.hpp
//  YouMeIM
//
//  Created by joexie on 16/5/8.
//
//

#ifndef SDKValidate_hpp
#define SDKValidate_hpp

#include <stdio.h>
#include <CrossPlatformDefine/PlatformDef.h>
#include <XCondWait.h>
#include <YouMeCommon/XAny.h>
#include <YouMeCommon/YMApplication.hpp>
#include <YouMeCommon/pb/common.pb.h>

namespace youmecommon
{
class CRSAUtil;
//sdk 验证的错误码
enum SDKValidateErrorcode
{
    SDKValidateErrorcode_Success=0,
    SDKValidateErrorcode_NotInit =1,
    SDKValidateErrorcode_ValidateIng =2,
    SDKValidateErrorcode_NetworkInvalid=3,
    SDKValidateErrorcode_ILLEGAL_SDK=4,
    SDKValidateErrorcode_Abort=5,
    SDKValidateErrorcode_ProtoError=6,
    SDKValidateErrorcode_ServerError=7,
    SDKValidateErrorcode_DNSParseError=8,
    SDKValidateErrorcode_Fail=1000
};


class CSDKValidate
{
    //状态，内部使用
    enum SDKValidateStatus
    {
        SDKValidateStatus_NotInit,
        SDKValidateStatus_Init,
        SDKValidateStatus_Ing,
        SDKValidateStatus_Complete,
    };

public:
    CSDKValidate();
    //初始化，传入 服务器域名或者ip，如果是域名则自动解析
    bool Init(const XString& strDomainOrIP,int iPort,const XString& strDefaultIP=__XT(""));
    //开始验证,函数是阻塞的，需要调用者开线程,只提供一个同步的接口，外部如果需要取消的话 sdkValidWait 设置信号即可
	SDKValidateErrorcode StartValidate(std::map<std::string, CXAny>&configurations, CXCondWait* sdkValidWait);
  
	static ::YouMeCommonProtocol::PacketHead* CreateRequestHead(int iEvtType);
private:
    //服务器的域名和端口
    XString m_strDomainOrIP;
    int m_iServerPort;
    SDKValidateStatus m_status;
    bool InterParseSecret(const XString& strSecret,CRSAUtil& rsa);
    //默认的ip地址，如果域名解析失败了使用默认ip
    XString m_strDefaultIP;

};

}

#endif /* SDKValidate_hpp */

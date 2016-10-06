//
//  CYMApplication.cpp
//  YouMeIM
//
//  Created by 智璋 谢 on 16/5/10.
//
//

#include "YMApplication.hpp"
#include <YouMeCommon/CrossPlatformDefine/IYouMePacketInfo.h>
#include <YouMeCommon/NetworkService.h>

#ifdef WIN32
#include "CrossPlatformDefine/Windows/YouMeApplication_Win.h"
#elif (OS_IOS || OS_IOSSIMULATOR)
#include "CrossPlatformDefine/IOS/YouMeApplication_iOS.h"
#elif OS_OSX
#include "CrossPlatformDefine/OSX/YouMeApplication_OSX.h"
#endif

CYMApplication& CYMApplication::GetInstance()
{
    static CYMApplication s_Instance;
    return s_Instance;
}

CYMApplication::CYMApplication()
{
    IYouMeSystemProvider* pd = NULL;
#ifdef WIN32
	pd = new YouMeApplication_Win;
#elif (OS_IOS || OS_IOSSIMULATOR)
    pd = new YouMeApplication_iOS;
#elif OS_OSX
    pd = new  YouMeApplication_OSX;
#elif OS_ANDROID
  
#endif
    
    if (NULL != pd)
    {
        SetBrand(pd->getBrand ());
        SetSysVer(pd->getSystemVersion ());
        SetCPUArch(pd->getCpuArchive ());
        SetPackageName(pd->getPackageName ());
        SetIdentify(pd->getUUID ());
        SetModel(pd->getModel ());
        SetCPUChip(pd->getCpuShip ());
        SetDocumentDir(pd->getDocumentPath ());
        
        delete pd;
    }

}

int CYMApplication::GetProtoVer()
{
    return m_iProtoVer;
}

void CYMApplication::SetProtoVer(int iProtoVer)
{
    m_iProtoVer = iProtoVer;
}

//win32 平台下需要SDK设置这个东西，其他平台的话由COMMON 设置
void CYMApplication::SetPackageName(const XString& strPackageName)
{
    m_strPackage = strPackageName;
}

XString CYMApplication::GetPackageName()
{
    return m_strPackage;
}

//appkey 和 密码
void CYMApplication::SetAppKey(const XString& strAppKey)
{
    m_strAppKey = strAppKey;
}

void CYMApplication::SetAppSecret(const XString& strAppSecret)
{
    m_strSecret = strAppSecret;
}

XString CYMApplication::GetAppKey()
{
    return m_strAppKey;
}

XString CYMApplication::GetAppSecret()
{
    return m_strSecret;
}

//sdk 版本号，SDK 设置
void CYMApplication::SetYMSDKVersion(int iValue)
{
    m_iYMSDKVersion = iValue;
}

int CYMApplication::GetYMSDKVersion()
{
    return m_iYMSDKVersion;
}

//系统版本
void CYMApplication::SetSysVer(const XString& strValue)
{
    m_strSysVersion = strValue;
}

XString CYMApplication::GetSysVer()
{
    return m_strSysVersion;
}

//品牌，型号
void CYMApplication::SetBrand(const XString& strValue)
{
    m_strBrand = strValue;
}

XString CYMApplication::GetBrand()
{
    return m_strBrand;
}

void CYMApplication::SetModel(const XString& strValue)
{
    m_strType = strValue;
}

XString CYMApplication::GetModel()
{
    return  m_strType;
}

//cpu 架构以及CPU 芯片
void CYMApplication::SetCPUArch(const XString& strValue)
{
    m_strCpuArch = strValue;
}

XString CYMApplication::GetCPUArch()
{
    return m_strCpuArch;
}

void CYMApplication::SetCPUChip(const XString& strValue)
{
    m_strCPUChip = strValue;
}

XString CYMApplication::GetCPUChip()
{
    return m_strCPUChip;
}


void CYMApplication::SetIdentify(const XString& strValue)
{
    m_strIdentify = strValue;
}

XString CYMApplication::GetIdentify()
{
    return m_strIdentify;
}

void CYMApplication::SetDocumentDir(const XString& strDocumentDir)
{
    m_strDocumentDir = strDocumentDir;
}

XString CYMApplication::GetDocumentDir()
{
    return m_strDocumentDir;
}



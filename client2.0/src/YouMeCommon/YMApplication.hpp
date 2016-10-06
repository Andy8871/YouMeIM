//
//  CYMApplication.hpp
//  YouMeIM
//
//  Created by 智璋 谢 on 16/5/10.
//
//

#ifndef CYMApplication_hpp
#define CYMApplication_hpp
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <stdio.h>
#include <YouMeCommon/NetworkService.h>

class CYMApplication
{
public:
    static CYMApplication& GetInstance();
    
    //协议头版本号
    int GetProtoVer();
    void SetProtoVer(int iProtoVer);

    //win32 平台下需要SDK设置这个东西，其他平台的话由COMMON 设置
    void SetPackageName(const XString& strPackageName);
    XString GetPackageName();
    //appkey 和 密码
    void SetAppKey(const XString& strAppKey);
    void SetAppSecret(const XString& strAppSecret);
    XString GetAppKey();
    XString GetAppSecret();
    
    //sdk 版本号，SDK 设置
    void SetYMSDKVersion(int iValue);
    int GetYMSDKVersion();
    //系统版本
    void SetSysVer(const XString& strValue);
    XString GetSysVer();
    //品牌，型号
    void SetBrand(const XString& strValue);
    XString GetBrand();
    void SetModel(const XString& strValue);
    XString GetModel();
    
    //cpu 架构以及CPU 芯片
    void SetCPUArch(const XString& strValue);
    XString GetCPUArch();
    void SetCPUChip(const XString& strValue);
    XString GetCPUChip();
    
    //唯一表示，android imei，iso uuid，windows 还没想好
    void SetIdentify(const XString& strValue);
    XString GetIdentify();
    
    //文档路径
    void SetDocumentDir(const XString& strDocumentDir);
    XString GetDocumentDir();
   
private:
    CYMApplication();
    
    int m_iProtoVer;
    XString m_strAppKey;
    XString m_strSecret;
    XString m_strBrand;//
    XString m_strType;//
    XString m_strSysVersion;//
    int m_iYMSDKVersion;
    XString m_strCpuArch;//
    XString m_strPackage;//windows 上面设置
    XString m_strIdentify;//
    
    
    XString m_strCPUChip;//
    XString m_strDocumentDir;//

};



#endif /* CYMApplication_hpp */

//
//  Application.mm
//  YouMeVoiceEngine
//
//  Created by wanglei on 15/9/17.
//  Copyright (c) 2015年 tencent. All rights reserved.
//
#include <mach/machine.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#import "YouMeApplication_OSX.h"
#import <CoreFoundation/CoreFoundation.h>
#import <Foundation/Foundation.h>
#include <string>

XString YouMeApplication_OSX::getBrand ()
{
    return "Apple";
}

XString YouMeApplication_OSX::getSystemVersion ()
{
    NSDictionary *systemVersionDictionary = [NSDictionary dictionaryWithContentsOfFile:@"/System/Library/CoreServices/SystemVersion.plist"];
    NSString *systemVersion = [systemVersionDictionary objectForKey:@"ProductVersion"];
    return [systemVersion UTF8String];
}

XString YouMeApplication_OSX::getCpuArchive ()
{
    size_t size;
    cpu_type_t type;
    cpu_subtype_t subtype;
    size = sizeof(type);
    sysctlbyname("hw.cputype", &type, &size, NULL, 0);
    
    size = sizeof(subtype);
    sysctlbyname("hw.cpusubtype", &subtype, &size, NULL, 0);
    
    // values for cputype and cpusubtype defined in mach/machine.h
    if (type == CPU_TYPE_X86)
    {
        return "x86";
    }
    else if (type == CPU_TYPE_X86_64)
    {
        return "x86_64";
    }
    else if (type == CPU_TYPE_POWERPC)
    {
        return "ppc";
    }
    else if (type == CPU_TYPE_POWERPC64)
    {
        return "ppc_64";
    }
    else if (type == CPU_TYPE_SPARC)
    {
        return "sparc";
    }
    else if (type == CPU_TYPE_ARM)
    {
        char szArchive[20] = {0};
        sprintf(szArchive, "ARM_%d",subtype);
        return szArchive;
    }
    else if(type == CPU_TYPE_ARM64)
    {
        char szArchive[20] = {0};
        sprintf(szArchive, "ARM64_%d",subtype);
        return szArchive;
    }
    
    return "unknown";
}

XString YouMeApplication_OSX::getPackageName ()
{
    return getBundleValueByName ("CFBundleIdentifier");
}

XString YouMeApplication_OSX::getBundleValueByName (const XString &name)
{
    NSDictionary *infoDictionary = [[NSBundle mainBundle] infoDictionary];
    if (infoDictionary == nil)
    {
        return "";
    }
    NSString *value = [infoDictionary objectForKey:[NSString stringWithUTF8String:name.c_str ()]];
    infoDictionary = nil;
    if (value == nil)
    {
        return "";
    }
    
    NSLog (@"name:%s,value:%s", name.c_str (), [value UTF8String]);
    
    return [value UTF8String];
}

XString YouMeApplication_OSX::getUUID ()
{
    NSArray * args = [NSArray arrayWithObjects:@"-rd1", @"-c", @"IOPlatformExpertDevice", @"|", @"grep", @"model", nil];
    NSTask * task = [NSTask new];
    [task setLaunchPath:@"/usr/sbin/ioreg"];
    [task setArguments:args];
    
    NSPipe * pipe = [NSPipe new];
    [task setStandardOutput:pipe];
    [task launch];
    
    NSArray * args2 = [NSArray arrayWithObjects:@"/IOPlatformUUID/ { split($0, line, \"\\\"\"); printf(\"%s\\n\", line[4]); }", nil];
    NSTask * task2 = [NSTask new];
    [task2 setLaunchPath:@"/usr/bin/awk"];
    [task2 setArguments:args2];
    
    NSPipe * pipe2 = [NSPipe new];
    [task2 setStandardInput:pipe];
    [task2 setStandardOutput:pipe2];
    NSFileHandle * fileHandle2 = [pipe2 fileHandleForReading];
    [task2 launch];
    
    NSData * data = [fileHandle2 readDataToEndOfFile];
    NSString * uuid = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    
    return [uuid UTF8String];
}

XString YouMeApplication_OSX::getModel ()
{
    XString strResult;
    size_t size;
    sysctlbyname("hw.model", NULL, &size, NULL, 0);
    if (0 == size) {
        return "";
    }
    char *pszBuffer = (char*)malloc(size);
    sysctlbyname("hw.model", pszBuffer, &size, NULL, 0);
    std::map<XString, XString>::iterator it = m_deviceNameMap.find(pszBuffer);
    if (it != m_deviceNameMap.end()) {
        strResult = it->second;
    }
    else
    {
        strResult = pszBuffer;
    }
    free(pszBuffer);
    //#endif
    return strResult;
}

XString YouMeApplication_OSX::getCpuShip ()
{
    return "";
}

XString YouMeApplication_OSX::getDocumentPath ()
{
    //获得应用程序沙盒的Documents目录，官方推荐数据文件保存在此
    NSArray *path = NSSearchPathForDirectoriesInDomains (NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *doc_path = [path objectAtIndex:0];
    const char *path_cstr = [[NSFileManager defaultManager] fileSystemRepresentationWithPath:doc_path];
    std::string documentDirectory = path_cstr;
    return documentDirectory;
}

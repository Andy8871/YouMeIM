//
//  MiniZip.h
//  SSZipArchive
//
//  Created by user on 15/4/21.
//  Copyright (c) 2015年 joexie. All rights reserved.
//

#ifndef __SSZipArchive__MiniZip__
#define __SSZipArchive__MiniZip__

#include <stdio.h>
#include <string>
#include <CrossPlatformDefine/PlatformDef.h>
namespace youmecommon{
class CMiniZip
{
public:
    CMiniZip()
    {
        m_hFile = NULL;
    }
    ~CMiniZip()
    {
        Close();
    }
	//如果是压缩文件，需要自己确保好父目录是存在的
    bool Open(const XString& strPath);
    //在压缩文件的根目录下放一个文件
    bool AddOneFileToZip(const XString& strFilePath);
    //压缩一个目录
   // bool AddDirToZip(const XString& strDir);
    void Close();
    
private:
    bool InterAddOneFileToZip(const XString& strFilePath, const XString& fileName);
    void* m_hFile;
};
}

#endif /* defined(__SSZipArchive__MiniZip__) */

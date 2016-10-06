//
//  MiniUnZip.h
//  Zip
//
//  Created by user on 15/4/22.
//  Copyright (c) 2015ๅนด joexie. All rights reserved.
//

#ifndef __Zip__MiniUnZip__
#define __Zip__MiniUnZip__

#include <CrossPlatformDefine/PlatformDef.h>
class CMiniUnZip
{
public:
    CMiniUnZip()
    {
        m_hHandle = NULL;
    }
    ~CMiniUnZip()
    {
        Close();
    }
    void Close();
    bool Open(const XString& strPath);
	//解压到一个目录
    bool UnZipToDir(const XString& strDir);
private:
    void* m_hHandle;
};
#endif /* defined(__Zip__MiniUnZip__) */

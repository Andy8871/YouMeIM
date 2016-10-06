//
//  MiniUnZip.cpp
//  Zip
//
//  Created by user on 15/4/22.
//  Copyright (c) 2015年 joexie. All rights reserved.
//

#include "MiniUnZip.h"
#include "unzip.h"
#include <YouMeCommon/minizip/unzip.h>
#include <YouMeCommon/XSharedArray.h>
#include <YouMeCommon/XFile.h>
#ifndef CHUNK
#define CHUNK 16384
#endif
bool CMiniUnZip::Open(const XString &strPath)
{
	m_hHandle = youmecommon::unzOpen(XStringToLocal(strPath).c_str());
    return m_hHandle != NULL;
}

bool CMiniUnZip::UnZipToDir(const XString &strDir)
{
	if (youmecommon::unzGoToFirstFile(m_hHandle) != UNZ_OK)
    {
        return false;
    }
    int ret = UNZ_OK;
	youmecommon::CXSharedArray<unsigned char> tmpBuffer;
	tmpBuffer.Allocate(CHUNK);
   
    do {
		ret = youmecommon::unzOpenCurrentFile(m_hHandle);
        if (ret != UNZ_OK) {
            break;
        }
		youmecommon::unz_file_info fileInfo;
		memset(&fileInfo, 0, sizeof(youmecommon::unz_file_info));
		ret = youmecommon::unzGetCurrentFileInfo(m_hHandle, &fileInfo, NULL, 0, NULL, 0, NULL, 0);
        if (ret != UNZ_OK) {
			youmecommon::unzCloseCurrentFile(m_hHandle);
            break;
        }
       
		youmecommon::CXSharedArray<char> fileNameBuffer(fileInfo.size_filename + 1);

        fileNameBuffer.Get()[fileInfo.size_filename] = 0;
		youmecommon::unzGetCurrentFileInfo(m_hHandle, &fileInfo, fileNameBuffer.Get(), fileInfo.size_filename + 1, NULL, 0, NULL, 0);
        bool bIsDir = false;
		if ((fileNameBuffer.Get()[fileInfo.size_filename-1] == '/') ||
			(fileNameBuffer.Get()[fileInfo.size_filename - 1] == '\\'))
		{
            bIsDir = true;
        }
		
		XString strFullPath = youmecommon::CXFile::CombinePath(strDir, LocalToXString(std::string(fileNameBuffer.Get())));
        if (bIsDir) {
			youmecommon::CXFile::make_dir_tree(strFullPath.c_str());
        }
        else
        {
            //创建上一层目录
			XString strUpDir = youmecommon::CXFile::get_upper_dir(strFullPath.c_str());
			youmecommon::CXFile::make_dir_tree(strUpDir.c_str());
            //读取文件
			youmecommon::CXFile file;
			if (file.LoadFile(strFullPath.c_str(), youmecommon::CXFile::Mode_CREATE_ALWAYS) == 0)
            {
                while (true) {
					int iRealRead = youmecommon::unzReadCurrentFile(m_hHandle, tmpBuffer.Get(), CHUNK);
                    if (iRealRead <= 0) {
                        break;
                    }
                    file.Write(tmpBuffer.Get(), iRealRead);
                }
                file.Close();
            }
        }
		youmecommon::unzCloseCurrentFile(m_hHandle);
		ret = youmecommon::unzGoToNextFile(m_hHandle);
    } while (ret == UNZ_OK && ret != UNZ_END_OF_LIST_OF_FILE);
    
    return true;
}
void CMiniUnZip::Close()
{
    if (NULL != m_hHandle) {
		youmecommon::unzClose(m_hHandle);
        m_hHandle = NULL;
    }
}
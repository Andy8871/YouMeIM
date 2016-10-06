//
//  MiniZip.cpp
//  SSZipArchive
//
//  Created by user on 15/4/21.
//  Copyright (c) 2015年 joexie. All rights reserved.
//

#include "MiniZip.h"
#include <YouMeCommon/XFile.h>
#include <ctime>
#include "zip.h"
#include <zlib/zlib.h>

namespace youmecommon {
   
#define CHUNK 16384
bool CMiniZip::Open(const XString& strPath)
{
	std::string strLocalPath = XStringToLocal(strPath);
	m_hFile = youmecommon::zipOpen(strLocalPath.c_str(), APPEND_STATUS_CREATE);
    return m_hFile != NULL;
}

void CMiniZip::Close()
{
    if (NULL != m_hFile) {
		youmecommon::zipClose(m_hFile, NULL);
        m_hFile = NULL;
    }
}

bool CMiniZip::InterAddOneFileToZip(const XString &strFilePath, const XString &fileName)
{
    CXFile file;
    XString strFileName = CXFile::GetFileName(strFilePath.c_str());

    
    //最后修改时间
	//最后修改时间
	std::time_t lastModifyTime = std::time(nullptr);
	youmecommon::zip_fileinfo zipInfo = { { 0 } };
	std::tm* timeStr = std::localtime(&lastModifyTime);
	zipInfo.tmz_date.tm_hour = timeStr->tm_hour;
	zipInfo.tmz_date.tm_mday = timeStr->tm_mday;
	zipInfo.tmz_date.tm_min = timeStr->tm_min;
	zipInfo.tmz_date.tm_mon = timeStr->tm_mon;
	zipInfo.tmz_date.tm_sec = timeStr->tm_sec;
	zipInfo.tmz_date.tm_year = timeStr->tm_year;
    
    //增加一个zip 文件
	std::string strLocalPath = XStringToLocal(strFileName);
	youmecommon::zipOpenNewFileInZip(m_hFile, strLocalPath.c_str(), &zipInfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
    if (file.LoadFile(strFilePath.c_str(), CXFile::Mode_OpenExist) != 0)
    {
        youmecommon::zipCloseFileInZip(m_hFile);
        return false;
    }
    //循环写入
	byte* buffer = new byte[CHUNK];
    while (true) {
		size_t realRead = (size_t)file.Read(buffer, CHUNK);
        if (realRead <= 0) {
            break;
        }
		youmecommon::zipWriteInFileInZip(m_hFile, buffer, (unsigned int)realRead);
    }
	delete []buffer;
	youmecommon::zipCloseFileInZip(m_hFile);
    return true;
}


bool CMiniZip::AddOneFileToZip(const XString &strFilePath)
{
    return InterAddOneFileToZip(strFilePath, __XT(""));
}
/*
bool CMiniZip::AddDirToZip(const XString &strDir)
{
    XString strTmpDir = strDir;
    //这个地方要换成跨平台的
	if (!boost::ends_with(strTmpDir,XPreferredSeparator))
   {
       strTmpDir += XPreferredSeparator;
   }
	if (!boost::filesystem::is_directory(strTmpDir))
    {
		return false;
    }
    
    //枚举文件
    boost::filesystem::recursive_directory_iterator end_itr;
    for ( boost::filesystem::recursive_directory_iterator itr( strTmpDir );
         itr != end_itr;
         ++itr )
    {
		
		XString strFullPath = itr->path().c_str();
        XStringStream strRelativePathStream;
        strRelativePathStream << boost::replace_first_copy(strFullPath, strTmpDir, __XT(""));
        XString strRelative = strRelativePathStream.str();
        //
        if (boost::filesystem::is_directory(itr->status()))
        {
            //添加一个目录,目录需要有一个/
            if (!boost::ends_with(strFullPath, XPreferredSeparator)) {
                strRelative += XPreferredSeparator;
            }
            InterAddOneFileToZip(strFullPath, strRelative);
        }
        else
        {
            //添加一个文件
            InterAddOneFileToZip(strFullPath, strRelative);
        }
    }
    
    return true;
}

*/
}
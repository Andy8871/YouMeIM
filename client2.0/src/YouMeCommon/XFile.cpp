#include "XFile.h"
#include <assert.h>
#include <algorithm>
#ifdef WIN32
#include <tchar.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif // WIN32
namespace youmecommon {

CXFile::CXFile(void):m_pFile(NULL)
{

}

CXFile::~CXFile(void)
{
	Close();	
}

int CXFile::LoadFile( const XString& strPath,Mode mode)
{
	assert(NULL == m_pFile);
	XString strMode;
	if (mode == Mode_CREATE_ALWAYS)
	{
		strMode = __XT("wb+");
	}
	else if (mode == Mode_Open_ALWAYS)
	{
#ifdef WIN32
		int iCode = _waccess(strPath.c_str(), 0);
#else
		int iCode = access(strPath.c_str(), 0);
#endif
		if (0 == iCode)
		{
			strMode = __XT("rb+");
		}
		else
		{
			strMode = __XT("wb+");
		}
	}
	else if(mode == Mode_OpenExist)
	{
		strMode = __XT("rb+");
	}
    else if(mode == Mode_OpenExist_ReadOnly)
    {
        strMode = __XT("rb");
    }
	else
	{
		assert(false);
	}
	#ifdef WIN32
	 m_pFile = _wfopen(strPath.c_str(),strMode.c_str());

	#else	
		m_pFile = fopen(strPath.c_str(),strMode.c_str());
	#endif
	if (NULL == m_pFile)
	{
		return -1;
	}
	if ((mode == Mode_Open_ALWAYS) 
		|| (mode == Mode_OpenExist))
	{
		//移动文件指针到头部
		XFSeek( m_pFile, 0, SEEK_SET);
	}

	return 0;
}

XINT64 CXFile::GetFileSize()
{
	if (NULL == m_pFile)
	{
		return -1;
	}
	//保存当前文件指针
	XINT64 iCurPos = XFTell(m_pFile);
	//移动到文件末尾
	XFSeek(m_pFile,0,SEEK_END);
	XINT64 iFileSize = XFTell(m_pFile);
	XFSeek(m_pFile,iCurPos,SEEK_SET);
	return iFileSize;
}

XINT64 CXFile::GetFilePosition()
{
	return XFTell(m_pFile);
}

XINT64 CXFile::SetFilePositon(XINT64 filePos)
{
	return XFSeek(m_pFile,filePos,SEEK_SET);
}

XINT64 CXFile::Read(byte* buffer,XINT64 wantRead)
{
	return fread(buffer,1,(size_t)wantRead,m_pFile);
}

XINT64 CXFile::Write(const byte* buffer,XINT64 wantWrite)
{
	return fwrite(buffer, 1, (size_t)wantWrite, m_pFile);
}

XINT64 CXFile::SetFileSize(XINT64 fileSize)
{
#ifdef WIN32
	_chsize_s(_fileno(m_pFile),fileSize);
	//返回当前文件的大小
	return GetFileSize();
#else
	ftruncate(fileno(m_pFile),fileSize);
	assert(false);
	return -1;
#endif
	
}


void CXFile::Close()
{
	if (NULL != m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
}

void CXFile::Skip(XINT64 wantSkipInBytes)
{
	SetFilePositon(GetFilePosition()+wantSkipInBytes);
}

bool CXFile::IsEnd()
{
	return feof(m_pFile);
}

XString CXFile::GetFileName(const XCHAR* file_path)
{
	if (NULL == file_path || '\0' == file_path[0])
	{
		return XString(__XT(""));
	}

#if defined(WIN32)
	const  XCHAR* pos1 = wcsrchr(file_path, '\\');
	const XCHAR* pos2 = wcsrchr(file_path, '/');
	const    XCHAR* pos = (std::max)(pos1, pos2);
#else
	XCHAR* pos = strrchr(file_path, '/');
#endif

	if (NULL == pos)
	{
		return (XString)file_path;
	}

	return (XString)(pos + 1);
}

bool CXFile::IsOpen()
{
	return m_pFile != NULL;
}

XINT64 CXFile::Seek(XINT64 ulOffset, int iFlag)
{
	return XFSeek(m_pFile, ulOffset, iFlag);
}

void CXFile::Flush()
{
	fflush(m_pFile);
}

XString CXFile::CombinePath(const XString& strDir, const XString& strFileName)
{
	XString dir = strDir;
	if (dir.size() == 0)
	{
		return strFileName;
	}
	XCHAR last_char = dir[dir.size() - 1];
	if (last_char == '\\' || last_char == '/')
	{
		// 末尾就是 '\\'，处理之前先去掉末尾的 '\\'
		dir.resize(dir.size() - 1);
	}
	dir += XPreferredSeparator;
	dir += strFileName;
	return dir;
}

bool CXFile::IsFileExist(const XString&strFilePath)
{

#ifdef WIN32
	WIN32_FIND_DATAW data;
	HANDLE handle = ::FindFirstFileW(strFilePath.c_str(), &data);
	if (handle == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	else
	{
		bool ret = (0 == (data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY));
		::FindClose(handle);
		return ret;
	}

#elif (defined OS_OSX)
	struct stat64 buf;
	int res = stat64(strFilePath.c_str(), &buf);
	if (-1 == res)
	{
		return false;
	}
	return S_ISREG(buf.st_mode);
#elif (defined OS_IOS) || (defined OS_IOSSIMULATOR)
	struct stat buf;
	int res = stat(strFilePath.c_str(), &buf);
	if (-1 == res)
	{
		return false;
	}

	return S_ISREG(buf.st_mode);

#elif (defined OS_ANDROID)
	struct stat64 buf;
	int res = stat64(strFilePath.c_str(), &buf);
	if (-1 == res)
	{
		return false;
	}

	return S_ISREG(buf.st_mode);
#else
#error "不支持的平台"
#endif

}

XString CXFile::get_upper_dir(const XCHAR* dir_path)
{
	if (NULL == dir_path || '\0' == dir_path[0])
	{
		return __XT("");
	}

#if defined(WIN32)
	XString dir = dir_path;
	XCHAR last_char = dir[dir.size() - 1];
	if (last_char == '\\' || last_char == '/')
	{
		// 末尾就是 '\\'，处理之前先去掉末尾的 '\\'
		dir.resize(dir.size() - 1);
	}
	XString::size_type pos1 = dir.rfind('\\');
	XString::size_type pos2 = dir.rfind('/');
	XString::size_type pos;
	if (pos1 != XString::npos && pos2 != XString::npos)
	{
		pos = max(pos1, pos2);
	}
	else
	{
		pos = (pos1 != XString::npos ? pos1 : pos2);
	}
#else
	XCHAR sperator = '/';

	XString dir = dir_path;
	if (dir[dir.size() - 1] == sperator)
	{
		// 末尾就是 '\\'，处理之前先去掉末尾的 '\\'
		dir.resize(dir.size() - 1);
	}

	XString::size_type pos = dir.rfind(sperator);
#endif

	if (XString::npos == pos)
	{
		// 字符串中不包含 '\\'
		return __XT("");
	}
	else
	{
		return dir.substr(0, pos + 1);
	}
}

bool CXFile::is_dir(const XCHAR* path)
{
	if (NULL == path || '\0' == path[0])
	{
		return false;
	}
    
#ifdef WIN32
    
    DWORD dwAttr = ::GetFileAttributesW(path);
    return (dwAttr != -1) && ((dwAttr&FILE_ATTRIBUTE_DIRECTORY) != 0);
#elif (defined OS_OSX)
    struct stat64 buf;
    int res = stat64(path, &buf);
    if (-1 == res)
    {
        return false;
    }
    return S_ISDIR(buf.st_mode);
#elif (defined OS_IOS) || (defined OS_IOSSIMULATOR)
    struct stat buf;
    int res = stat(path, &buf);
    if (-1 == res)
    {
        return false;
    }
    
    return S_ISDIR(buf.st_mode);
    
#elif (defined OS_ANDROID)
    struct stat64 buf;
    int res = stat64(path, &buf);
    if (-1 == res)
    {
        return false;
    }
    
    return S_ISDIR(buf.st_mode);
#else
#error "不支持的平台"
#endif
    
    
}

#ifdef WIN32

bool CXFile::make_dir(const XCHAR* path)
{
	if (NULL == path || '\0' == path[0])
	{
		return false;
	}

	return TRUE == CreateDirectoryW(path, NULL);
}



#else
bool   CXFile::make_dir(const XCHAR* path)
{
	if(NULL == path || '\0' == path[0])
	{
		return false;
	}

    if (is_dir(path)) {
        return true;
    }

	return 0 == mkdir(path, 777);

}
#endif // WIN32

bool CXFile::make_dir_tree(const XCHAR* path)
{
	if (is_dir(path))
	{
		// 如果已经存在，则返回TRUE
		return true;
	}
	bool ret = make_dir(path);
	if (ret == false)
	{
		XString upper_dir = get_upper_dir(path);

		if (!upper_dir.empty())
		{
			bool bCreateUpperDir = make_dir_tree(upper_dir.c_str());
			if (bCreateUpperDir)
			{
				return make_dir(path);
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}

void CXFile::remove_file(const XString& strFilePath)
{
#ifdef WIN32
	::DeleteFileW(strFilePath.c_str());
#else
	remove(strFilePath.c_str());
#endif
}


bool CXFile::rename_file(const XString& strSrcPath, const XString& strDestPath)
{
#ifdef WIN32
	return ::MoveFileW(strSrcPath.c_str(),strDestPath.c_str());
#else
	return rename(strSrcPath.c_str(),strDestPath.c_str())==0;
#endif
}

}
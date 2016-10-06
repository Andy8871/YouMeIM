#pragma once
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
namespace youmecommon {

class CXFile
{
public:
	enum Mode
	{
		Mode_CREATE_ALWAYS =0, //打开文件，如果文件不存在创建，如果存在则清0
		Mode_OpenExist,//打开文件已存在的文件，文件不存在则失败
		Mode_Open_ALWAYS, //打开文件，如果文件不存在则创建
        Mode_OpenExist_ReadOnly  //打开已存在文件，并且只读打开
	};

	CXFile(void);
	~CXFile(void);
	
	//打开文件
	int LoadFile(const XString& strPath,Mode mode);
	//判断文件是否打开
	bool IsOpen();
	//获取文件大小，如果获取失败返回-1
	XINT64 GetFileSize();
	//设置文件长度,返回-1 失败,MAC 下还没找到实现的代码
	XINT64 SetFileSize(XINT64 fileSize);

	//移动文件指针
	XINT64 Seek(XINT64 ulOffset, int iFlag);

	//获取当前文件位置
	XINT64 GetFilePosition();
	//设置当前文件位置
	XINT64 SetFilePositon(XINT64 filePos);
	//读取文件到缓冲,返回实际读取的文件个数
	XINT64 Read(byte* buffer,XINT64 wantRead);
	//写入文件
	XINT64 Write(const byte* buffer,XINT64 wantWrite);
	void Close();
	//读取的时候跳过n 字节
	void Skip(XINT64 wantSkipInBytes);
	//文件是否到了结尾
	bool IsEnd();
	void Flush();

	//静态函数，方便一些文件相关操作
	static XString GetFileName(const XCHAR* file_path);
	static XString CombinePath(const XString& strDir, const XString& strFileName);
	static bool IsFileExist(const XString&strFilePath);
	static bool     is_dir(const XCHAR* path);
	static XString get_upper_dir(const XCHAR* dir_path);
	static void remove_file(const XString& strFilePath);
	static bool rename_file(const XString& strSrcPath, const XString& strDestPath);
#if defined(WIN32)
	static bool     make_dir(const XCHAR* dir_path);
#else
	static bool   make_dir(const XCHAR* path);
#endif
	static bool   make_dir_tree(const XCHAR* dir_path);
private:
	FILE* m_pFile;
};
}
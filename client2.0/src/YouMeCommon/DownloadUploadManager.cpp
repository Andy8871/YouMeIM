#include "DownloadUploadManager.h"
#include <time.h>
#include <YouMeCommon/json/json.h>
#include <YouMeCommon/XFile.h>
#include <YouMeCommon/Log.h>
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>

CURLSH* CDownloadUploadManager::m_shareHandle = NULL;

size_t CDownloadUploadManager::OnUploadWrite(void *buffer, size_t size, size_t nmemb, void *user_p)
{
	std::string* pResponse = (std::string*)user_p;
	pResponse->append((char*)buffer, nmemb);
	return nmemb;
}

size_t CDownloadUploadManager::UploadReadCallback(void *ptr, size_t size, size_t nmemb, void *stream)
{
	size_t stRead = fread(ptr, size, nmemb, (FILE*)stream);
	return stRead;
}

//下载文件的回掉
size_t CDownloadUploadManager::OnDownloadFile(void *buffer, size_t size, size_t nmemb, void *user_p)
{
	youmecommon::CXFile* pFile = (youmecommon::CXFile*)user_p;
	//写文件
	size_t dwWrittenBytes = 0;
	const byte* pTmpBuffer = (const byte*)buffer;
	while (dwWrittenBytes < nmemb)
	{
		size_t dwWriteBytes = (size_t)pFile->Write(pTmpBuffer + dwWrittenBytes, nmemb - dwWrittenBytes);
		//这里需要考虑一下空间满的情况
		if (dwWriteBytes <= 0)
		{
			return CURL_READFUNC_ABORT;
		}
		else
		{
			dwWrittenBytes += dwWriteBytes;
		}
	}
	return nmemb;
}

//保存http 的各种请求的头部信息
size_t CDownloadUploadManager::OnHttpHead(void *buffer, size_t size, size_t nmemb, void *user_p)
{
	std::string* pResponse = (std::string*)user_p;
	pResponse->append((char*)buffer, nmemb);
	return nmemb;
}
bool CDownloadUploadManager::UploadFile(const XString&strUrl, const XString& strSrcPath, std::map<std::string, std::string>& httpHead, std::string& strResponse)
{
	return UploadFileToUpYun(strUrl, strSrcPath, true, httpHead, strResponse);
}

bool CDownloadUploadManager::HttpRequest(const XString& strUrl, const std::string& strBody, std::string& strResponse, bool bPost /*= true*/, int iTimeOut /*= -1*/, std::map<std::string, std::string>*pHead /*= NULL*/)
{
	CURL * easyHandle = NULL;
	bool bSuccess = false;
	do
	{
		easyHandle = curl_easy_init();
		if (NULL == easyHandle)
		{
			YouMe_LOG_Info(__XT("初始化失败"));
			break;
		}
		curl_easy_setopt(easyHandle, CURLOPT_URL, XStringToUTF8(strUrl).c_str());
		curl_easy_setopt(easyHandle, CURLOPT_NOSIGNAL, 1);

		if (bPost)
		{
			curl_easy_setopt(easyHandle, CURLOPT_POST, 1);
			curl_easy_setopt(easyHandle, CURLOPT_POSTFIELDS, strBody.c_str());
		}
		
		if (NULL == m_shareHandle)
		{
			m_shareHandle = curl_share_init();
			curl_share_setopt(m_shareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
		}
		curl_easy_setopt(easyHandle, CURLOPT_SHARE, m_shareHandle);
		if (iTimeOut != -1)
		{
			curl_easy_setopt(easyHandle, CURLOPT_TIMEOUT, iTimeOut);
		}
		curl_easy_setopt(easyHandle, CURLOPT_AUTOREFERER, 1); // 以下3个为重定向设置
		curl_easy_setopt(easyHandle, CURLOPT_FOLLOWLOCATION, 1); //返回的头部中有Location(一般直接请求的url没找到)，则继续请求Location对应的数据 
		curl_easy_setopt(easyHandle, CURLOPT_WRITEFUNCTION, OnUploadWrite);
		curl_easy_setopt(easyHandle, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(easyHandle, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(easyHandle, CURLOPT_SSL_VERIFYPEER, 0L);
		struct curl_slist *headers = NULL; /* init to NULL is important */
		if (NULL != pHead)
		{
			std::map<std::string, std::string>::iterator begin = pHead->begin();
			std::map<std::string, std::string>::iterator end = pHead->end();
			for (; begin != end; begin++)
			{
				std::stringstream strStream;
				strStream << begin->first << (": ") << begin->second;
				//描述
				headers = curl_slist_append(headers, strStream.str().c_str());
			}
			curl_easy_setopt(easyHandle, CURLOPT_HTTPHEADER, headers);
		}

		curl_easy_setopt(easyHandle, CURLOPT_WRITEDATA, (void *)&strResponse);
		// 执行上传操作
		int errorcode = curl_easy_perform(easyHandle);
		long httpCode = 0;
		curl_easy_getinfo(easyHandle, CURLINFO_RESPONSE_CODE, &httpCode);
		YouMe_LOG_Info(__XT("CURLcode:%d httpCode:%d"), errorcode, httpCode);
		if (NULL != headers)
		{
			curl_slist_free_all(headers);
		}
		if (0 == errorcode)
		{
			bSuccess = true;
		}
	} while (0);
	if (NULL != easyHandle)
	{
		curl_easy_cleanup(easyHandle);
	}

	return bSuccess;
}

bool CDownloadUploadManager::DownloadFile(const XString&strUrl, const XString&strSavePath)
{
	std::map<std::string, std::string> httpHead;
	std::string strResponse;
	return UploadFileToUpYun(strUrl, strSavePath, false, httpHead, strResponse);
}

bool CDownloadUploadManager::UploadFileToUpYun(const XString&strUrl, const XString&strSavePath, bool bUpload, std::map<std::string, std::string>& httpHead, std::string& strResponse)
{
	//饶了好大一圈，终于可以开始干活了。
	CURL* easyHandle = curl_easy_init();
	if (easyHandle == NULL)
	{
		return false;
	}
	curl_easy_setopt(easyHandle, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(easyHandle, CURLOPT_URL, XStringToUTF8(strUrl).c_str());
	curl_easy_setopt(easyHandle, CURLOPT_NOSIGNAL, 1);
	if (NULL == m_shareHandle)
	{
		m_shareHandle = curl_share_init();
		curl_share_setopt(m_shareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
	}
	curl_easy_setopt(easyHandle, CURLOPT_SHARE, m_shareHandle);
	curl_easy_setopt(easyHandle, CURLOPT_DNS_CACHE_TIMEOUT, 5);
	curl_easy_setopt(easyHandle, CURLOPT_CONNECTTIMEOUT, 5);
	//struct curl_httppost *formpost = NULL;

	FILE* file = NULL;
	if (!bUpload)
	{		
#ifdef WIN32
		errno_t errCode = _wfopen_s(&file, strSavePath.c_str(), __XT("wb+"));
		if (errCode != 0 || NULL == file)
		{
			curl_easy_cleanup(easyHandle);
			return false;
		}
#else
		file = fopen(strSavePath.c_str(), __XT("wb+"));
		if (NULL == file)
		{
			curl_easy_cleanup(easyHandle);
			return false;
		}
#endif

		curl_easy_setopt(easyHandle, CURLOPT_WRITEFUNCTION, OnDownloadFile);
		curl_easy_setopt(easyHandle, CURLOPT_WRITEDATA, (void *)&file);
	}
	else
	{
#ifdef WIN32
		errno_t errCode = _wfopen_s(&file, strSavePath.c_str(), __XT("rb"));
		if (errCode != 0 || NULL == file)
		{
			curl_easy_cleanup(easyHandle);
			return false;
		}
#else
		file = fopen(strSavePath.c_str(), __XT("rb"));
		if (NULL == file)
		{
			curl_easy_cleanup(easyHandle);
			return false;
		}
#endif
		fseek(file, 0, SEEK_END);
		long fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);

		curl_easy_setopt(easyHandle, CURLOPT_WRITEFUNCTION, OnUploadWrite);
		curl_easy_setopt(easyHandle, CURLOPT_WRITEDATA, (void *)&strResponse);
		curl_easy_setopt(easyHandle, CURLOPT_NOPROGRESS, 0);
		curl_easy_setopt(easyHandle, CURLOPT_UPLOAD, 1L);
		curl_easy_setopt(easyHandle, CURLOPT_PUT, 1L);
		curl_easy_setopt(easyHandle, CURLOPT_READFUNCTION, UploadReadCallback);
		curl_easy_setopt(easyHandle, CURLOPT_READDATA, file);
		curl_easy_setopt(easyHandle, CURLOPT_INFILESIZE, (curl_off_t)fileSize);
		//你Y 就不支持中文，当然仅限于坑爹的windwos系统
		//struct curl_httppost *lastptr = NULL;
		//curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "file", CURLFORM_FILE, XStringToLocal(strSavePath).c_str(), CURLFORM_END);
		//curl_easy_setopt(easyHandle, CURLOPT_HTTPPOST, formpost);

		if (httpHead.size() > 0)
		{
			struct curl_slist *headers = NULL;
			for (std::map<std::string, std::string>::iterator itr = httpHead.begin(); itr != httpHead.end(); ++itr)
			{
				std::stringstream strStream;
				strStream << itr->first << ": " << itr->second;
				headers = curl_slist_append(headers, strStream.str().c_str());
			}
			curl_easy_setopt(easyHandle, CURLOPT_HTTPHEADER, headers);
		}
	}
	CURLcode errorcode = curl_easy_perform(easyHandle);
	//if (NULL != formpost)
	//{
	//	curl_formfree(formpost);
	//}
	fclose(file);
	long httpCode = 0;
	curl_easy_getinfo(easyHandle, CURLINFO_RESPONSE_CODE, &httpCode);
	curl_easy_cleanup(easyHandle);
    YouMe_LOG_Info(__XT("CURLcode:%d httpCode:%d"),errorcode,httpCode);

	if ((errorcode != CURLE_OK) || (httpCode !=200))
	{
		//如果是下载的话则判断一下下载是否成功，失败的话就删掉文件
		if (!bUpload)
		{
#ifdef WIN32
			::DeleteFileW(strSavePath.c_str());
#else
			remove(strSavePath.c_str());
#endif // WIN32	
		}
		return false;
	}
	return true;
}

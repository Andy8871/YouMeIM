#pragma once
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#ifdef WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#endif
#include <YouMeCommon/curl/curl.h>
class CDownloadUploadManager
{
public:
	//下载文件
	static bool DownloadFile(const XString&strUrl, const XString&strSavePath);
	//上传文件
	static bool UploadFile(const XString&strUrl, const XString& strSrcPath, std::map<std::string, std::string>& httpHead, std::string& strResponse);
	//http 请求
	static bool HttpRequest(const XString& strUrl, const std::string& strBody, std::string& strResponse, bool bPost = true, int iTimeOut = -1, std::map<std::string, std::string>*pHead = NULL);
private:
	//上传文件到up 云接口，这玩意内部使用
	static bool UploadFileToUpYun(const XString&strUrl, const XString&strSavePath, bool bUpload, std::map<std::string, std::string>& httpHead, std::string& strResponse);
	static size_t OnHttpHead(void *buffer, size_t size, size_t nmemb, void *user_p);
	static size_t OnDownloadFile(void *buffer, size_t size, size_t nmemb, void *user_p);
	static size_t OnUploadWrite(void *buffer, size_t size, size_t nmemb, void *user_p);
	static size_t UploadReadCallback(void *ptr, size_t size, size_t nmemb, void *stream);

	static CURLSH* m_shareHandle;
};


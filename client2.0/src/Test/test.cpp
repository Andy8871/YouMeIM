#include <stdio.h>

#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeIMEngine/YouMeInterface.h>
#if WIN32
#include <Windows.h>
#include <WinSock2.h>
#else
#include <unistd.h>
#endif
#include <memory>
#include <thread>
#include <YouMeCommon/curl/curl.h>
#include <YouMeCommon/TimeUtil.h>
#include <YouMeCommon/DownloadUploadManager.h>
void testengine(int iIndex)
{
	curl_global_init(0);
	IYouMeIMManager* pInstance = IYouMeIMManager::CreateInstance();
		IYouMeIMManager::DestroyInstance(pInstance);
}
int main()
{
	IYouMeIMManager::Startup();
  
    IYouMeIMManager* pInstance = IYouMeIMManager::CreateInstance();
    pInstance->Init(__XT("YOUMEBC2B3171A7A165DC10918A7B50A4B939F2A187D0"),
                    __XT("r1+ih9rvMEDD3jUoU+nj8C7VljQr7Tuk4TtcByIdyAqjdl5lhlESU0D+SoRZ30sopoaOBg9EsiIMdc8R16WpJPNwLYx2WDT5hI/HsLl1NJjQfa9ZPuz7c/xVb8GHJlMf/wtmuog3bHCpuninqsm3DRWiZZugBTEj2ryrhK7oZncBAAE="),
                    __XT("com.youme.joe.test"));
    
	/*for (int i = 0; i < 10;i++)
   {
	   std::thread test(testengine,i);
	   test.detach();
   }*/
    usleep(100000000);
	IYouMeIMManager::CleanUp();
    return 0;
}
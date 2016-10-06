//
//  IYouMeCInterface.hpp
//  youme_voice_engine
//
//  Created by YouMe.im on 15/12/10.
//  Copyright © 2015年 tencent. All rights reserved.
//

#ifndef IYouMeCInterface_hpp
#define IYouMeCInterface_hpp


//这个文件对内封装的C 接口，不需要给外部。C# 只能调用C接口的函数
//为了简单只提供单实例的封装
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
#include <YouMeIMEngine/YouMeInterface.h>

extern "C"
{
	 //初始化和反初始化
	YOUMEDLL_API int IM_Init(const XCHAR* pszAppKey, const XCHAR* pszSecrect);
	YOUMEDLL_API void IM_Uninit();
    
	//登陆，登出
	YOUMEDLL_API int IM_Login(const XCHAR* pszYouMeID, const XCHAR* pszPasswd, const XCHAR* pszOldPasswd);
	YOUMEDLL_API int IM_Logout();

	//好友部分接口
	YOUMEDLL_API int IM_GetContactList();
    YOUMEDLL_API int IM_AddContact(const XCHAR* pszYouMeID,const XCHAR* pszReason);
    YOUMEDLL_API int IM_AgreeContactInvited(const XCHAR* pszYouMeID);
    YOUMEDLL_API int IM_RefuseContactInvited(const XCHAR* pszYouMeID,const XCHAR* pszReason);
	YOUMEDLL_API int IM_DeleteContact(const XCHAR* pszYouMeID);

    
    //消息接口
    YOUMEDLL_API int IM_GetContactOfflineMessage();
    YOUMEDLL_API int IM_SendTextMessage(const XCHAR* pszRecvID, YouMeIMChatType chatType, const XCHAR* pszContent, int *pRequestID);
    YOUMEDLL_API int IM_SendCustomMessage(const XCHAR* pszRecvID, YouMeIMChatType chatType, const char* pszCustomMessage,int iBufferLen, int *pRequestID) ;
    
	YOUMEDLL_API int IM_SendAudioMessage(const XCHAR* pszRecvID, YouMeIMChatType chatType,  int *pRequestID);
    //不会转文字，直接发送的音频
	YOUMEDLL_API int IM_SendOnlyAudioMessage(const XCHAR* pszRecvID, YouMeIMChatType chatType, int *pRequestID);
	YOUMEDLL_API int IM_StopAudioMessage(const XCHAR* pszParam);
    YOUMEDLL_API int IM_CancleAudioMessage();
    YOUMEDLL_API int IM_DownloadAudioFile(int iSerial,const XCHAR* pszSavePath);

	YOUMEDLL_API int IM_DownloadAudioFileSync(int iSerial, const XCHAR* pszSavePath);
    //聊天室接口
    YOUMEDLL_API int IM_JoinChatRoom(const XCHAR*pszID);
    YOUMEDLL_API int IM_LeaveChatRoom(const XCHAR*pszID);
    
    
	//提供一个get 接口，用来读取数据,如果没有数据会阻塞，调用登出的话，函数会返回
    YOUMEDLL_API const XCHAR* IM_GetMessage();
    YOUMEDLL_API void IM_PopMessage();
    
    YOUMEDLL_API int IM_GetSDKVer();
	YOUMEDLL_API void IM_SetServerZone(ServerZone zone);
	YOUMEDLL_API void IM_SetAudioCacheDir(const XCHAR* pszAudioCacheDIR);


	//设置模式0 正式环境 1开发环境 2 测试环境 3 商务环境。 默认正式环境。所以客户不需要调用这个接口
	YOUMEDLL_API void IM_SetMode(int iMode);

	YOUMEDLL_API XCHAR* IM_GetFilterText(const XCHAR* pszSource);
	YOUMEDLL_API void  IM_DestroyFilterText(XCHAR* pszSource);
}
#endif /* IYouMeCInterface_hpp */

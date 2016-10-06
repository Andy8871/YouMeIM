//
//  jsb_downloader.cpp
//  cocos2d_js_bindings
//
//  Created by joexie on 16/07/19.
//
//

#include "jsb_youmeim.h"
#include "cocos2d.h"
#include "spidermonkey_specifics.h"
#include "ScriptingCore.h"
#include "cocos2d_specifics.hpp"
#include "json/document.h"
#include <YouMeIMEngine/IYouMeCInterface.h>
#include <YouMeCommon/CrossPlatformDefine/PlatformDef.h>
//C:\testjs\MyJSGame\frameworks\cocos2d-x\build\Debug.win32\js-tests\src\tests-main.js
using namespace cocos2d;
enum YouMeIMCommand
{
	Command_Login = 1,
	Command_LogOut = 2,
	Command_LoginResponse = 3,
	Command_LogOutResponse = 4,
	Command_ContactLogin = 5,
	Command_ContactLogOut = 6,
	Command_Heart = 7,
	Command_HeartResponse = 8,
	Command_PackageAck = 9,
	Command_SessionExpire = 10,
	Command_ClientToServerPackageAck = 11,
	Command_Redirect = 12,
	Command_RedirectResponse = 13,
	Command_GetContactList = 1000,
	Command_GetContactListResponse = 1001,
	Command_GetBlacklistList = 1002,
	Command_GetBlacklistListResponse = 1003,
	Command_AddContact = 1004,
	Command_AddContactRequest = 1005,
	Command_ReceiveInvitedApplication = 1006,
	Command_AgreeContactInvited = 1007,
	Command_RefuseContactInvited = 1008,
	Command_InvitedResponse = 1009,
	Command_AddContactResponse = 1010,
	Command_DeleteContact = 1011,
	Command_DeleteContactResponse = 1012,
	Command_WasDeleted = 1013,
	Command_AddToBlacklist = 1014,
	Command_AddToBlacklistResponse = 1015,
	Command_AddedToBlacklist = 1016,
	Command_KickoutFromBlacklist = 1017,
	Command_KickoutFromBlacklistResponse = 1018,
	Command_KickoutedFromBlacklist = 1019,
	Command_GetContactInfo = 1020,
	Command_GetContactInfoResponse = 1021,
	Command_GetGroupInfo = 2000,
	Command_GetGroupInfoResponse = 2001,
	Command_GetAllGroup = 2002,
	Command_GetAllGroupResponse = 2003,
	Command_CreateGroup = 2004,
	Command_CreateGroupResponse = 2005,
	Command_DestroyGroup = 2006,
	Command_DestroyGroupResponse = 2007,
	Command_GroupDestroyed = 2008,
	Command_JoinGroup = 2009,
	Command_JoinGroupRequest = 2010,
	Command_JoinGroupResponse = 2011,
	Command_OtherJoinGroup = 2012,
	Command_LeaveGroup = 2013,
	Command_LeaveGroupResponse = 2014,
	Command_OtherLeaveGroup = 2015,
	Command_KickoutMember = 2016,
	Command_KickoutMemberResponse = 2017,
	Command_OtherKickoutedFromGroup = 2018,
	Command_GetGroupMembers = 2019,
	Command_GetGroupMembersResponse = 2020,
	Command_ModifyGroupInfo = 2021,
	Command_ModifyGroupInfoResponse = 2022,
	Command_GroupInfoModifyed = 2023,
	Command_AgreeGroupInvited = 2024,
	Command_RefuseGroupInvited = 2025,
	Command_GroupInvitedResponse = 2026,
	Command_ModifyMemberInfo = 2027,
	Command_ModifyMemberInfoResponse = 2028,
	Command_MemberInfoModifyed = 2029,
	Command_ReceiveGroupInvitedApplication = 2030,
	Commnad_Chat_Custom = 3000,
	Commnad_Chat_Text = 3001,
	Commnad_Chat_Emoji = 3002,
	Commnad_Chat_File = 3003,
	Commnad_SendMessage = 3004,
	Commnad_Chat_Voice = 3005,
	Commnad_Chat_Custom_Server = 3101,
	Commnad_Chat_Text_Server = 3102,
	Commnad_Chat_Emoji_Server = 3103,
	Commnad_Chat_File_Server = 3104,
	Commnad_RecvMessage = 3105,
	Commnad_Chat_Voice_Server = 3106,
	Command_GetOfflineMessageResponse = 3205,
	Command_GetOfflineMessage = 3206,
	Command_MessageToServerACK = 3207,
	Command_GetUploadtoken = 3208,
	Command_GetUploadtokenResponse = 3209,
	Command_GetDownloadtoken = 3210,
	Command_GetDownloadtokenResponse = 3211,
	Command_Download = 3212,
};
#ifdef WIN32

std::wstring P_UTF8_to_Unicode(const char* in, int len)
{
	wchar_t* pBuf = new wchar_t[len + 1];
	if (NULL == pBuf)
	{
		return __XT("");
	}
	size_t out_len = (len + 1) * sizeof(wchar_t);
	memset(pBuf, 0, (len + 1) * sizeof(wchar_t));
	wchar_t* pResult = pBuf;
	out_len = ::MultiByteToWideChar(CP_UTF8, 0, in, len, pBuf, len * sizeof(wchar_t));
	std::wstring out;
	out.assign(pResult, out_len);


	delete[] pResult;
	pResult = NULL;
	return out;
}

std::string P_Unicode_to_UTF8(const wchar_t* in)
{
	std::string out;
	int len = wcslen(in);
	size_t out_len = len * 3 + 1;
	char* pBuf = new char[out_len];
	if (NULL == pBuf)
	{
		return "";
	}
	char* pResult = pBuf;
	memset(pBuf, 0, out_len);


	out_len = ::WideCharToMultiByte(CP_UTF8, 0, in, len, pBuf, len * 3, NULL, NULL);
	out.assign(pResult, out_len);
	delete[] pResult;
	pResult = NULL;
	return out;
}

#define  UTF8TOPlatString(str) P_UTF8_to_Unicode(str.c_str(),str.length())
#define  PlatStringToUTF8(str) P_Unicode_to_UTF8(str.c_str())
#else
#define  UTF8TOPlatString(str) str 
#define  PlatStringToUTF8(str) str
#endif // WIN32


jsval youmecontact_to_jsval(JSContext* cx, const std::string& strYouMeID,ContactStatus status)
{
	JS::RootedObject proto(cx);
	JS::RootedObject parent(cx);
	JS::RootedValue prop(cx);
	JS::RootedObject tmp(cx, JS_NewObject(cx, NULL, proto, parent));
	if (!tmp) return JSVAL_NULL;
	prop.set(std_string_to_jsval(cx, strYouMeID));
	bool ok = JS_DefineProperty(cx, tmp, "YouMeID", prop, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
		JS_DefineProperty(cx, tmp, "Status", status, JSPROP_ENUMERATE | JSPROP_PERMANENT);
	if (ok) {
		return OBJECT_TO_JSVAL(tmp);
	}
	return JSVAL_NULL;
}

jsval youme_addcontact_response_to_jsval(JSContext* cx, const std::string& strYouMeID, ContactStatus status, YouMeIMContactEvent evt)
{
	JS::RootedObject proto(cx);
	JS::RootedObject parent(cx);
	JS::RootedValue prop(cx);
	JS::RootedObject tmp(cx, JS_NewObject(cx, NULL, proto, parent));
	if (!tmp) return JSVAL_NULL;
	prop.set(std_string_to_jsval(cx, strYouMeID));
	bool ok = JS_DefineProperty(cx, tmp, "YouMeID", prop, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
		JS_DefineProperty(cx, tmp, "Status", status, JSPROP_ENUMERATE | JSPROP_PERMANENT) && 
		JS_DefineProperty(cx, tmp, "Evt", evt, JSPROP_ENUMERATE | JSPROP_PERMANENT);
	if (ok) {
		return OBJECT_TO_JSVAL(tmp);
	}
	return JSVAL_NULL;
}

//被添加为好友
jsval youme_addedcontact_to_jsval(JSContext* cx, const std::string& strYouMeID, ContactStatus status, const std::string& strReason)
{
	JS::RootedObject proto(cx);
	JS::RootedObject parent(cx);
	JS::RootedValue prop(cx);
	JS::RootedObject tmp(cx, JS_NewObject(cx, NULL, proto, parent));
	if (!tmp) return JSVAL_NULL;
	prop.set(std_string_to_jsval(cx, strYouMeID));
	bool ok = JS_DefineProperty(cx, tmp, "YouMeID", prop, JSPROP_ENUMERATE | JSPROP_PERMANENT) &&
		JS_DefineProperty(cx, tmp, "Status", status, JSPROP_ENUMERATE | JSPROP_PERMANENT);
	prop.set(std_string_to_jsval(cx, strReason));
	ok &= JS_DefineProperty(cx, tmp, "Reason", prop, JSPROP_ENUMERATE | JSPROP_PERMANENT);
	if (ok) {
		return OBJECT_TO_JSVAL(tmp);
	}
	return JSVAL_NULL;
}
JSClass  *js_cocos2dx_youmeim_class;
JSObject *js_cocos2dx_youmeim_prototype;
class CYouMeIMJsWrapper
{
public:
	CYouMeIMJsWrapper()
	{
		JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
		_JSDelegate.construct(cx);
	}
public:
	mozilla::Maybe<JS::PersistentRootedObject> _JSDelegate;
};
CYouMeIMJsWrapper* g_SingleInstance;

static void js_cocos2dx_YouMeIM_finalize(JSFreeOp *fop, JSObject *obj) {
	CCLOG("jsbindings: finalizing JS object %p (YouMeIM)", obj);
}
bool js_cocos2dx_extension_YouMeIM_constructor(JSContext *cx, uint32_t argc, jsval *vp)
{
	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);	 
	if (argc == 0)
	{
		assert(g_SingleInstance == NULL);
		if (g_SingleInstance != NULL)
		{
			delete g_SingleInstance;
		}
		JS::RootedObject obj(cx, JS_NewObject(cx, js_cocos2dx_youmeim_class, JS::RootedObject(cx, js_cocos2dx_youmeim_prototype), JS::NullPtr()));
		
		// link the native object with the javascript object
		g_SingleInstance = new CYouMeIMJsWrapper();
		g_SingleInstance->_JSDelegate.ref() = obj;
		js_proxy_t *p = jsb_new_proxy(g_SingleInstance, obj);
		JS::AddNamedObjectRoot(cx, &p->obj, "YouMeIM");
		args.rval().set(OBJECT_TO_JSVAL(obj));

		return true;
	}

	JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}
//引擎初始化，只需要调用一次
bool js_cocos2dx_extension_IM_Init(JSContext *cx, uint32_t argc, jsval *vp) {

	if (argc == 2)
	{
		JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
		//获取两个参数，appkey，appsecrect
		std::string strAppKey;
		jsval_to_std_string(cx, args.get(0), &strAppKey);
		std::string strAppSecuret;
		jsval_to_std_string(cx, args.get(1), &strAppSecuret);
		//调用SDK 初始化函数
		int iErrorcode = IM_Init(UTF8TOPlatString(strAppKey).c_str(), UTF8TOPlatString(strAppSecuret).c_str());
		args.rval().set(int32_to_jsval(cx, iErrorcode));
		return true;
	}
	JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}
//引擎反初始化
bool js_cocos2dx_extension_IM_Uninit(JSContext *cx, uint32_t argc, jsval *vp) {

	IM_Uninit();
	return true;
}
void ParseJsonMsgThead()
{
	while (true)
	{
		const XCHAR* pszJsonMsg = IM_GetMessage();	
		if (pszJsonMsg == NULL)
		{
			continue;
		}
		rapidjson::Document readdoc;
#ifdef WIN32
		readdoc.Parse(P_Unicode_to_UTF8(pszJsonMsg).c_str());
#else
		readdoc.Parse(pszJsonMsg);
#endif
		YouMeIMCommand command = (YouMeIMCommand)readdoc["Command"].GetInt();
		YouMeIMErrorcode errorcode = (YouMeIMErrorcode)readdoc["Errorcode"].GetInt();
		switch (command) {
		case Command_Download:
		{		
			std::string strSavePath = readdoc["SavePath"].GetString();
			int iSerial = readdoc["Serial"].GetInt();
			Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
				JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
					JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
				jsval params[3];
				params[0] = int32_to_jsval(cx, iSerial);
				params[1] = int32_to_jsval(cx, errorcode);
				params[2] = std_string_to_jsval(cx, strSavePath);
				ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(g_SingleInstance->_JSDelegate.ref()), "OnDownload", 3, params);
			});
		}
		break;
		case Command_Login:
		{
			std::string YouMeID;
			ContactStatus status;
			if (errorcode == YouMeIMErrorcode_Success)
			{
				YouMeID = readdoc["YouMeID"].GetString();
				status = (ContactStatus)readdoc["Status"].GetInt();				
			}
			Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
				JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
					JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
				jsval params[2];
				params[0] = int32_to_jsval(cx, errorcode);
				params[1] = youmecontact_to_jsval(cx, YouMeID,status);
				ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(g_SingleInstance->_JSDelegate.ref()), "OnLogin", 2, params);
			});
		}
		break;
		case Command_LogOut:
		{
			Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
				JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
				JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
				ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(g_SingleInstance->_JSDelegate.ref()), "OnLogout", 0, NULL);
			});
		}
		break;
		case Command_AddContactRequest:
		case Command_WasDeleted:
		case Command_AddedToBlacklist:
		case Command_KickoutedFromBlacklist:
		case Command_DeleteContactResponse:
		case Command_AddToBlacklistResponse:
		case Command_KickoutFromBlacklistResponse:
		case Command_ContactLogin:
		case Command_ContactLogOut:
		{
			bool bAcceptCommand = true;
			JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
			JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();			
			std::string strYouMeID = readdoc["YouMeID"].GetString();			
			if (command == Command_AddContactRequest)
			{
			}
			else if (command == Command_WasDeleted)
			{
			}
			else if (command == Command_DeleteContactResponse)
			{
			}
			else if (command == Command_ContactLogin)
			{
			}
			else if (command == Command_ContactLogOut)
			{
			}
			else
			{
				bAcceptCommand = false;
			}
			if (bAcceptCommand)
			{
				Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
					JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
					JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
					jsval params[3];
					params[0] = int32_to_jsval(cx, command);
					params[1] = int32_to_jsval(cx, errorcode);
					params[2] = std_string_to_jsval(cx, strYouMeID);
					ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(g_SingleInstance->_JSDelegate.ref()), "OnContactCallback", 3, params);
				});
			}
		}

		break;
		case Command_AddContactResponse:
		{
			std::string strYouMeID = readdoc["YouMeID"].GetString();
			ContactStatus Status = (ContactStatus)readdoc["Status"].GetInt();
			YouMeIMContactEvent evt = (YouMeIMContactEvent)readdoc["EvtType"].GetInt();
			Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
				JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
					JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
				jsval params[3];
				params[0] = int32_to_jsval(cx, command);
				params[1] = int32_to_jsval(cx, errorcode);
				params[2] = youme_addcontact_response_to_jsval(cx, strYouMeID, Status, evt);
				ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(g_SingleInstance->_JSDelegate.ref()), "OnContactCallback", 3, params);
			});
		}
		break;
		case Command_ReceiveInvitedApplication:
		{	
			std::string strYouMeID = readdoc["YouMeID"].GetString();
			ContactStatus Status = (ContactStatus)readdoc["Status"].GetInt();
			std::string strReason = readdoc["Reason"].GetString();
			Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
				JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
					JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
				jsval params[3];
				params[0] = int32_to_jsval(cx, command);
				params[1] = int32_to_jsval(cx, errorcode);
				params[2] = youme_addedcontact_to_jsval(cx, strYouMeID, Status, strReason);
				ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(g_SingleInstance->_JSDelegate.ref()), "OnContactCallback", 3, params);
			});
		}
		break;
		case Command_InvitedResponse:
		{
			std::string strYouMeID = readdoc["YouMeID"].GetString();
			YouMeIMContactEvent evt = (YouMeIMContactEvent)readdoc["EvtType"].GetInt();
			Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
				JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
					JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
				jsval params[3];
				params[0] = int32_to_jsval(cx, command);
				params[1] = int32_to_jsval(cx, errorcode);
				params[2] = youme_addcontact_response_to_jsval(cx, strYouMeID, ContactStatus_Online, evt);
				ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(g_SingleInstance->_JSDelegate.ref()), "OnContactCallback", 3, params);
			});
		}
		break;
		case Command_GetContactList:
		case Command_GetContactInfo:
		case Command_GetBlacklistList:
		{
			const rapidjson::Value& membersArray = readdoc["Contacts"];
			struct ContactInfo
			{
				std::string strYouMeID;
				ContactStatus Status;
			};
			std::shared_ptr<std::vector<ContactInfo> >contactVect(new std::vector<ContactInfo>);
			for (int i = 0; i < membersArray.Size(); i++)
			{
				ContactInfo info;
				info.strYouMeID = membersArray[i]["YouMeID"].GetString();
				info.Status = (ContactStatus)membersArray[i]["Status"].GetInt();
				contactVect->push_back(info);
			}
			Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
				JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
					JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
				jsval params[3];
				params[0] = int32_to_jsval(cx, command);
				params[1] = int32_to_jsval(cx, errorcode);
				JS::RootedObject jsretArr(cx, JS_NewArrayObject(cx, 0));
				for (int i = 0; i < contactVect->size();i++)
				{
					JS::RootedValue arrElement(cx);
					arrElement = youmecontact_to_jsval(cx, contactVect->at(i).strYouMeID, contactVect->at(i).Status);
					if (!JS_SetElement(cx, jsretArr, i, arrElement)) {
						break;
					}
				}
				params[2] = OBJECT_TO_JSVAL(jsretArr);

				ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(g_SingleInstance->_JSDelegate.ref()), "OnContactCallback", 3, params);
			});
		
		}
		break;
		case Commnad_SendMessage:
		{
			int iSerial = readdoc["RequestID"].GetInt();
			Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
				JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
					JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
				jsval params[2];
				params[0] = int32_to_jsval(cx, iSerial);
				params[1] = int32_to_jsval(cx, errorcode);
				ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(g_SingleInstance->_JSDelegate.ref()), "OnSendMessageStatus", 2, params);
			});
		}
		break;
		case Commnad_Chat_Voice:
		{
			int iSerial = readdoc["RequestID"].GetInt();
			std::string strText = readdoc["Text"].GetString();
			std::string strLocalPath = readdoc["LocalPath"].GetString();
			Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
				JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
					JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
				jsval params[4];
				params[0] = int32_to_jsval(cx, iSerial);
				params[1] = int32_to_jsval(cx, errorcode);
				params[2] = std_string_to_jsval(cx, strText);
				params[3] = std_string_to_jsval(cx, strLocalPath);
				ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(g_SingleInstance->_JSDelegate.ref()), "OnSendAudioMessageStatus", 4, params);
			});
		}
		break;
		case Commnad_RecvMessage:
		{
			YouMeIMMessageBodyType bodyType = (YouMeIMMessageBodyType)readdoc["MessageType"].GetInt();
			YouMeIMChatType ChatType = (YouMeIMChatType)readdoc["ChatType"].GetInt();
			int Serial = (int)readdoc["Serial"].GetInt();
			std::string RecvID = readdoc["ReceiveID"].GetString();
			std::string SenderID = readdoc["SenderID"].GetString();
			std::string Content;
			std::string Param;
			if ((bodyType == MessageBodyType_TXT) || (bodyType == MessageBodyType_CustomMesssage))
			{
				Content = readdoc["Content"].GetString();
			}
			else if (bodyType == MessageBodyType_Voice)
			{
				Content = readdoc["Text"].GetString();	
				Param = readdoc["Param"].GetString();
			}
			Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
				JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
					JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
				jsval params[7];
				params[0] = int32_to_jsval(cx, bodyType);
				params[1] = int32_to_jsval(cx, ChatType);
				params[2] = int32_to_jsval(cx, Serial);
				params[3] = std_string_to_jsval(cx, RecvID);
				params[4] = std_string_to_jsval(cx, SenderID);
				params[5] = std_string_to_jsval(cx, Content);
				params[6] = std_string_to_jsval(cx, Param);
				ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(g_SingleInstance->_JSDelegate.ref()), "OnRecvMessage", 7, params);
			});
		}
		break;
		case Command_JoinGroupRequest:
		{
			std::string strGroupID = readdoc["GroupID"].GetString();
			Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
				JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
					JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
				jsval params[2];
				params[0] = int32_to_jsval(cx, errorcode);
				params[1] = std_string_to_jsval(cx, strGroupID);				
				ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(g_SingleInstance->_JSDelegate.ref()), "OnJoinGroupRequest", 2, params);
			});
		}
		break;
		case Command_JoinGroupResponse:
		{			
				std::string strGroupID = readdoc["GroupID"].GetString();
				std::string strGroupName = readdoc["GroupName"].GetString();
				YouMeIMGroupEvent groupEvt = (YouMeIMGroupEvent)readdoc["GroupEvt"].GetInt();
				YouMeIMGroupType GroupType = (YouMeIMGroupType)readdoc["GroupType"].GetInt();
				Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
					JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
						JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
					jsval params[5];
					params[0] = int32_to_jsval(cx, errorcode);
					params[1] = int32_to_jsval(cx, groupEvt);
					params[2] = int32_to_jsval(cx, GroupType);
					params[3] = std_string_to_jsval(cx, strGroupID);
					params[4] = std_string_to_jsval(cx, strGroupName);
					ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(g_SingleInstance->_JSDelegate.ref()), "OnJoinGroupResponse", 5, params);
				});
		}
		break;
		default:
			break;
		}

		IM_PopMessage();
	}	
}	



//登陆，登出
bool js_cocos2dx_extension_IM_Login(JSContext *cx, uint32_t argc, jsval *vp) {
	
	if (argc == 2)
	{
		JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
		//获取两个参数，appkey，appsecrect
		std::string strYouMeID;
		jsval_to_std_string(cx, args.get(0), &strYouMeID);
		std::string strPasswd;
		jsval_to_std_string(cx, args.get(1), &strPasswd);
		//调用SDK 初始化函数
		int iErrorcode = IM_Login(UTF8TOPlatString(strYouMeID).c_str(), UTF8TOPlatString(strPasswd).c_str(),__XT(""));
		args.rval().set(int32_to_jsval(cx, iErrorcode));
		if (YouMeIMErrorcode_Success == iErrorcode)
		{
			std::thread recvMsg(ParseJsonMsgThead);
			recvMsg.detach();
		}
		return true;
	}
	JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}

bool js_cocos2dx_extension_IM_Logout(JSContext *cx, uint32_t argc, jsval *vp) {

	int iErrorCode = IM_Logout();
	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	args.rval().set(int32_to_jsval(cx, iErrorCode));
	return true;
}

//好友部分接口
bool js_cocos2dx_extension_IM_GetContactList(JSContext *cx, uint32_t argc, jsval *vp) {

	int iErrorcode = IM_GetContactList();
	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	args.rval().set(int32_to_jsval(cx, iErrorcode));
	return true;
}

bool js_cocos2dx_extension_IM_AddContact(JSContext *cx, uint32_t argc, jsval *vp) {

	if (argc == 2)
	{
		JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
		//获取两个参数，appkey，appsecrect
		std::string strID;
		jsval_to_std_string(cx, args.get(0), &strID);

		std::string strReason;
		jsval_to_std_string(cx, args.get(1), &strReason);

		//调用离开聊天室
		int iErrorcode = IM_AddContact(UTF8TOPlatString(strID).c_str(), UTF8TOPlatString(strReason).c_str());
		args.rval().set(int32_to_jsval(cx, iErrorcode));
		return true;
	}
	JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}
bool js_cocos2dx_extension_IM_AgreeContactInvited(JSContext *cx, uint32_t argc, jsval *vp) {
	if (argc == 1)
	{
		JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
		//获取两个参数，appkey，appsecrect
		std::string strID;
		jsval_to_std_string(cx, args.get(0), &strID);

		//调用离开聊天室
		int iErrorcode = IM_AgreeContactInvited(UTF8TOPlatString(strID).c_str());
		args.rval().set(int32_to_jsval(cx, iErrorcode));
		return true;
	}
	JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}
bool js_cocos2dx_extension_IM_RefuseContactInvited(JSContext *cx, uint32_t argc, jsval *vp) {

	if (argc == 2)
	{
		JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
		//获取两个参数，appkey，appsecrect
		std::string strID;
		jsval_to_std_string(cx, args.get(0), &strID);

		std::string strReason;
		jsval_to_std_string(cx, args.get(1), &strReason);
		//调用离开聊天室
		int iErrorcode = IM_RefuseContactInvited(UTF8TOPlatString(strID).c_str(),UTF8TOPlatString(strReason).c_str());
		args.rval().set(int32_to_jsval(cx, iErrorcode));
		return true;
	}
	JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
	
}
bool js_cocos2dx_extension_IM_DeleteContact(JSContext *cx, uint32_t argc, jsval *vp) {
	if (argc == 1)
	{
		JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
		//获取两个参数，appkey，appsecrect
		std::string strID;
		jsval_to_std_string(cx, args.get(0), &strID);

		//调用离开聊天室
		int iErrorcode = IM_DeleteContact(UTF8TOPlatString(strID).c_str());
		args.rval().set(int32_to_jsval(cx, iErrorcode));
		return true;
	}
	JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}

//消息接口
bool js_cocos2dx_extension_IM_SendTextMessage(JSContext *cx, uint32_t argc, jsval *vp) {

	if (argc == 3)
	{
		JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
		//获取两个参数，appkey，appsecrect
		std::string strRecvID;
		jsval_to_std_string(cx, args.get(0), &strRecvID);

		int iChatType = 0;
		jsval_to_int32(cx, args.get(1), &iChatType);

		std::string strContent;
		jsval_to_std_string(cx, args.get(2), &strContent);
		//调用离开聊天室
		int iRequestID = -1;
		IM_SendTextMessage(UTF8TOPlatString(strRecvID).c_str(), (YouMeIMChatType)iChatType, UTF8TOPlatString(strContent).c_str(), &iRequestID);
		args.rval().set(int32_to_jsval(cx, iRequestID));
		return true;
	}
	JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}

bool js_cocos2dx_extension_IM_SendCustomMessage(JSContext *cx, uint32_t argc, jsval *vp) {


	return true;
}

bool js_cocos2dx_extension_IM_SendOnlyAudioMessage(JSContext *cx, uint32_t argc, jsval *vp) {

	if (argc == 2)
	{
		JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
		std::string strRecvID;
		jsval_to_std_string(cx, args.get(0), &strRecvID);

		int iChatType = 0;
		jsval_to_int32(cx, args.get(1), &iChatType);

		
		//调用离开聊天室
		int iRequestID = -1;
		IM_SendOnlyAudioMessage(UTF8TOPlatString(strRecvID).c_str(), (YouMeIMChatType)iChatType, &iRequestID);
		args.rval().set(int32_to_jsval(cx, iRequestID));
		return true;
	}
	JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}
bool js_cocos2dx_extension_IM_SendAudioMessage(JSContext *cx, uint32_t argc, jsval *vp) {

	if (argc == 2)
	{
		JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
		//获取两个参数，appkey，appsecrect
		std::string strRecvID;
		jsval_to_std_string(cx, args.get(0), &strRecvID);

		int iChatType = 0;
		jsval_to_int32(cx, args.get(1), &iChatType);
		

		//调用离开聊天室
		int iRequestID = -1;
		IM_SendAudioMessage(UTF8TOPlatString(strRecvID).c_str(), (YouMeIMChatType)iChatType, &iRequestID);
		args.rval().set(int32_to_jsval(cx, iRequestID));
		return true;
	}
	JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}

bool js_cocos2dx_extension_IM_StopAudioMessage(JSContext *cx, uint32_t argc, jsval *vp) {

	if (argc == 1)
	{
		JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
		std::string strParam;
		jsval_to_std_string(cx, args.get(0), &strParam);

		int iErrorcode = IM_StopAudioMessage(UTF8TOPlatString(strParam).c_str());
		args.rval().set(int32_to_jsval(cx, iErrorcode));
		return true;
	}
	return false;
}

bool js_cocos2dx_extension_IM_CancleAudioMessage(JSContext *cx, uint32_t argc, jsval *vp) {

	int iErrorcode = IM_CancleAudioMessage();
	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	args.rval().set(int32_to_jsval(cx, iErrorcode));
	return true;
}
bool js_cocos2dx_extension_IM_GetOfflineMessage(JSContext *cx, uint32_t argc, jsval *vp) {

	int iErrorcode = IM_GetContactOfflineMessage();
	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	args.rval().set(int32_to_jsval(cx, iErrorcode));
	return true;
}

bool js_cocos2dx_extension_IM_DownloadAudioFile(JSContext *cx, uint32_t argc, jsval *vp) {

	if (argc == 2)
	{
		JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
		//获取两个参数，appkey，appsecrect
		int iSerial = 0;
		jsval_to_int32(cx, args.get(0), &iSerial);
		std::string strSavePath;
		jsval_to_std_string(cx, args.get(1), &strSavePath);
		//调用离开聊天室
		int iErrorcode = IM_DownloadAudioFile(iSerial, UTF8TOPlatString(strSavePath).c_str());
		args.rval().set(int32_to_jsval(cx, iErrorcode));
		return true;
	}
	JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}

//聊天室接口
bool js_cocos2dx_extension_IM_JoinChatRoom(JSContext *cx, uint32_t argc, jsval *vp) {
	if (argc == 1)
	{
		JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
		//获取两个参数，appkey，appsecrect
		std::string strID;
		jsval_to_std_string(cx, args.get(0), &strID);

		//调用离开聊天室
		int iErrorcode = IM_JoinChatRoom(UTF8TOPlatString(strID).c_str());
		args.rval().set(int32_to_jsval(cx, iErrorcode));
		return true;
	}
	JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}
bool js_cocos2dx_extension_IM_LeaveChatRoom(JSContext *cx, uint32_t argc, jsval *vp) {

	if (argc == 1)
	{
		JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
		//获取两个参数，appkey，appsecrect
		std::string strID;
		jsval_to_std_string(cx, args.get(0), &strID);

		//调用离开聊天室
		int iErrorcode = IM_LeaveChatRoom(UTF8TOPlatString(strID).c_str());
		args.rval().set(int32_to_jsval(cx, iErrorcode));
		return true;
	}
	JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}

bool js_cocos2dx_extension_IM_SetServerZone(JSContext *cx, uint32_t argc, jsval *vp) {
	if (argc == 1)
	{
		JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
		int iServerZone = 0;
		jsval_to_int32(cx, args.get(0), &iServerZone);
		IM_SetServerZone((ServerZone)iServerZone);
	}
	return true;
}
//获取SDK 版本号
bool js_cocos2dx_extension_IM_GetSDKVer(JSContext *cx, uint32_t argc, jsval *vp) {

	int iVer  = IM_GetSDKVer();
	JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	args.rval().set(int32_to_jsval(cx, iVer));
	return true;
}

//关键词过滤
bool js_cocos2dx_extension_IM_GetFilterText(JSContext *cx, uint32_t argc, jsval *vp) {
	if (argc == 1)
	{
		JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
		//获取两个参数，appkey，appsecrect
		std::string strText;
		jsval_to_std_string(cx, args.get(0), &strText);
		
		//调用SDK 初始化函数
		XCHAR* pszFilteText = IM_GetFilterText(UTF8TOPlatString(strText).c_str());
#ifdef WIN32
		args.rval().set(charptr_to_jsval(cx, P_Unicode_to_UTF8(pszFilteText).c_str()));
#else
		args.rval().set(charptr_to_jsval(cx, pszFilteText));
#endif
		
		IM_DestroyFilterText(pszFilteText);
		return true;
	}
	JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
	return false;
}
void register_jsb_youmeim(JSContext *cx, JS::HandleObject global) {
	js_cocos2dx_youmeim_class = (JSClass *)calloc(1, sizeof(JSClass));
    js_cocos2dx_youmeim_class->name = "YouMeIM";
    js_cocos2dx_youmeim_class->addProperty = JS_PropertyStub;
    js_cocos2dx_youmeim_class->delProperty = JS_DeletePropertyStub;
    js_cocos2dx_youmeim_class->getProperty = JS_PropertyStub;
    js_cocos2dx_youmeim_class->setProperty = JS_StrictPropertyStub;
    js_cocos2dx_youmeim_class->enumerate = JS_EnumerateStub;
    js_cocos2dx_youmeim_class->resolve = JS_ResolveStub;
    js_cocos2dx_youmeim_class->convert = JS_ConvertStub;
	js_cocos2dx_youmeim_class->finalize = js_cocos2dx_YouMeIM_finalize;
    js_cocos2dx_youmeim_class->flags = JSCLASS_HAS_RESERVED_SLOTS(2);
    
    static JSFunctionSpec funcs[] = {
		JS_FN("IM_Init", js_cocos2dx_extension_IM_Init, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("IM_Uninit", js_cocos2dx_extension_IM_Uninit, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		//登陆等处
		JS_FN("IM_Login", js_cocos2dx_extension_IM_Login, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("IM_Logout", js_cocos2dx_extension_IM_Logout, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		//好友部分
		JS_FN("IM_GetContactList", js_cocos2dx_extension_IM_GetContactList, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("IM_AddContact", js_cocos2dx_extension_IM_AddContact, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("IM_AgreeContactInvited", js_cocos2dx_extension_IM_AgreeContactInvited, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("IM_RefuseContactInvited", js_cocos2dx_extension_IM_RefuseContactInvited, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FN("IM_DeleteContact", js_cocos2dx_extension_IM_DeleteContact, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
       //消息接口
	   JS_FN("IM_SendTextMessage", js_cocos2dx_extension_IM_SendTextMessage, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),
	   JS_FN("IM_SendCustomMessage", js_cocos2dx_extension_IM_SendCustomMessage, 4, JSPROP_PERMANENT | JSPROP_ENUMERATE),
	   JS_FN("IM_SendAudioMessage", js_cocos2dx_extension_IM_SendAudioMessage, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
	   JS_FN("IM_SendOnlyAudioMessage", js_cocos2dx_extension_IM_SendOnlyAudioMessage, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
	   JS_FN("IM_StopAudioMessage", js_cocos2dx_extension_IM_StopAudioMessage, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
	   JS_FN("IM_CancleAudioMessage", js_cocos2dx_extension_IM_CancleAudioMessage, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
	   JS_FN("IM_DownloadAudioFile", js_cocos2dx_extension_IM_DownloadAudioFile, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
	   JS_FN("IM_GetOfflineMessage", js_cocos2dx_extension_IM_GetOfflineMessage, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
	   //聊天室
	   JS_FN("IM_JoinChatRoom", js_cocos2dx_extension_IM_JoinChatRoom, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
	   JS_FN("IM_LeaveChatRoom", js_cocos2dx_extension_IM_LeaveChatRoom, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
	   
	  //获取SDK 版本号
	  JS_FN("IM_GetSDKVer", js_cocos2dx_extension_IM_GetSDKVer, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
	  //设置服务器区域
	  JS_FN("IM_SetServerZone", js_cocos2dx_extension_IM_SetServerZone, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
	  //关键词过滤
	  JS_FN("IM_GetFilterText", js_cocos2dx_extension_IM_GetFilterText, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		JS_FS_END
    };
    
    static JSFunctionSpec st_funcs[] = {
        JS_FS_END
    };
	js_cocos2dx_youmeim_prototype = JS_InitClass(
		cx, global,
		JS::NullPtr(),
		js_cocos2dx_youmeim_class,
		js_cocos2dx_extension_YouMeIM_constructor, 0, // constructor
		NULL,
		funcs,
		NULL, // no static properties
		st_funcs);
	anonEvaluate(cx, global, "(function () { return YouMeIM; })()").toObjectOrNull();
}

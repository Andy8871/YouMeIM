package com.youme.imengine;

import com.iflytek.cloud.SpeechUtility;

import android.content.Context;

public class IMEngine {
	/**
	 * 对外的错误码
	 */
	public final static int YouMeIMErrorcode_Success = 0;
	public final static int YouMeIMErrorcode_EngineNotInit=1;
	public final static int YouMeIMErrorcode_NotLogin = 2;
	public final static int YouMeIMErrorcode_ParamInvalid = 3;
	public final static int YouMeIMErrorcode_TimeOut = 4;
	public final static int YouMeIMErrorcode_StatusError = 5;
	public final static int YouMeIMErrorcode_SDKInvalid = 6;
	public final static int YouMeIMErrorcode_AlreadyLogin = 7;
	public final static int YouMeIMErrorcode_ServerError = 8;
	public final static int YouMeIMErrorcode_NetError = 9;
	public final static int YouMeIMErrorcode_LoginSessionError = 10;
	public final static int YouMeIMErrorcode_NotStartUp = 11;
	
	/**
	 * 服务器的错误码
	 */
	public final static int YouMeIMErrorcode_ALREADYFRIENDS = 1000;
	public final static int YouMeIMErrorcode_LoginInvalid = 1001;

	/**
	 * 语音部分错误码
	 */
	public final static int YouMeIMErrorcode_PTT_Start = 2000;
	public final static int YouMeIMErrorcode_PTT_Fail = 2001;
	public final static int YouMeIMErrorcode_PTT_DownloadFail = 2002;
	public final static int YouMeIMErrorcode_PTT_GetUploadTokenFail = 2003;
	public final static int YouMeIMErrorcode_PTT_UploadFail = 2004;
	public final static int YouMeIMErrorcode_Fail = 10000;
	
	
	/**
	 * YouMeIMChatType
	 */
	public final static int ChatType_Unknow = 0;
	public final static int ChatType_PrivateChat = 1;
	public final static int ChatType_GroupChat = 2;
	public final static int ChatType_RoomChat = 3;
	
	
	/**
	 * ServerZone
	 */
	public final static int ServerZone_China = 0;		// 中国
	public final static int ServerZone_Singapore = 1;	// 新加坡
	public final static int ServerZone_America = 2;		// 美国
	public final static int ServerZone_HongKong = 3;	// 香港
	public final static int ServerZone_Korea = 4;		// 韩国
	public final static int ServerZone_Australia = 5;	// 澳洲
	public final static int ServerZone_Deutschland = 6;	// 德国
	public final static int ServerZone_Brazil = 7;		// 巴西
	public final static int ServerZone_India = 8;		// 印度
	public final static int ServerZone_Japan = 9;		// 日本
	public final static int ServerZone_Ireland = 10;	// 爱尔兰
	
	
	public static void Init(Context context)
	{
		System.loadLibrary("YouMeIMEngine");
		AppPara.initPara(context);
		
		//初始化PTT
		SpeechUtility.createUtility(context, "appid=57554e68");
		//初始化一个语音合成对象
		YouMeSpeechRecognizer.Instance().Init(context);
	}
	
	//下面是IM 的封装
	//初始化和反初始化
	public static int IM_Init(String strAppKey, String strSecrect){
		return NativeEngine.Init(strAppKey, strSecrect);
	}
	public static void IM_Uninit(){
		NativeEngine.Uninit();
	}
    
	//登陆，登出
	public static int IM_Login(String strYouMeID, String strPasswd, String strOldPasswd){
		return NativeEngine.Login(strYouMeID, strPasswd, strOldPasswd);
	}
	public static int IM_Logout(){
		return NativeEngine.Logout();
	}

	//好友部分接口
	public static int IM_GetContactList(){
		return NativeEngine.GetContactList();
	}
    public static int IM_AddContact(String strYouMeID,String strReason){
    	return NativeEngine.AddContact(strYouMeID, strReason);
    }
    public static int IM_AgreeContactInvited(String strYouMeID){
    	return NativeEngine.AgreeContactInvited(strYouMeID);
    }
    public static int IM_RefuseContactInvited(String strYouMeID,String strReason){
    	return NativeEngine.RefuseContactInvited(strYouMeID, strReason);
    }
	public static int IM_DeleteContact(String strYouMeID){
		return NativeEngine.DeleteContact(strYouMeID);
	}

    
    //消息接口
    public static int IM_GetContactOfflineMessage(){
    	return NativeEngine.GetContactOfflineMessage();
    }
    public static int IM_SendTextMessage(String strRecvID, int chatType, String strContent, MessageRequestId requestID){
    	return NativeEngine.SendTextMessage(strRecvID, chatType, strContent, requestID);
    }
    public static int IM_SendCustomMessage(String strRecvID, int chatType, String strCustomMessage,int iBufferLen, MessageRequestId requestID){
    	return NativeEngine.SendCustomMessage(strRecvID, chatType, strCustomMessage, iBufferLen, requestID);
    }
    
	public static int IM_SendAudioMessage(String strRecvID, int chatType,  MessageRequestId requestID){
		return NativeEngine.SendAudioMessage(strRecvID, chatType, requestID);
	}
    //不会转文字，直接发送的音频
	public static int IM_SendOnlyAudioMessage(String strRecvID, int chatType, MessageRequestId requestID)
	{
		return NativeEngine.SendOnlyAudioMessage(strRecvID, chatType, requestID);
	}
	public static int IM_StopAudioMessage(String strParam){
		return NativeEngine.StopAudioMessage(strParam);
	}
    public static int IM_CancleAudioMessage(){
    	return NativeEngine.CancleAudioMessage();
    }
    public static int IM_DownloadAudioFile(int iSerial,String strSavePath){
    	return NativeEngine.DownloadAudioFile(iSerial, strSavePath);
    }
    //聊天室接口
    public static int IM_JoinChatRoom(String strID){
    	return NativeEngine.JoinChatRoom(strID);
    }
    public static int IM_LeaveChatRoom(String strID){
    	return NativeEngine.LeaveChatRoom(strID);
    }
    
    
	//提供一个get 接口，用来读取数据,如果没有数据会阻塞，调用登出的话，函数会返回
    public static String IM_GetMessage(){
    	return NativeEngine.GetMessage();
    }
    public static void IM_PopMessage(){
    	NativeEngine.PopMessage();
    }
    
    public static int IM_GetSDKVer(){
    	return NativeEngine.GetSDKVer();
    }
	public static void IM_SetServerZone(int zone){
		NativeEngine.SetServerZone(zone);
	}

	//设置模式0 正式环境 1开发环境 2 测试环境 3 商务环境。 默认正式环境。所以客户不需要调用这个接口
	public static void IM_SetMode(int iMode){
		NativeEngine.SetMode(iMode);
	}

	public static String IM_GetFilterText(String strSource){
		return NativeEngine.GetFilterText(strSource);
	}
	public static void  IM_DestroyFilterText(String strSource){
		NativeEngine.DestroyFilterText(strSource);
	}
	
	public class MessageRequestId{
		private int requestId;
		public void setId(int id){
			requestId = id;
		}
		public int getId(){
			return requestId;
		}
	}
}

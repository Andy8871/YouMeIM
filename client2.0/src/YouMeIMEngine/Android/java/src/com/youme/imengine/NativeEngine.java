package com.youme.imengine;

import java.io.FileInputStream;
import java.io.FileOutputStream;

import com.youme.imengine.IMEngine.MessageRequestId;

import android.R.integer;
import android.content.Context;

public class NativeEngine
{
	public native static void setModel (String model);

	public native static void setSysVersion (String sysversion);
	
	public native static void setDeviceIMEI (String deviceIMEI);

	public native static void setCPUArch (String ABI);

	public native static void setPackageName (String packageName);

	public native static void setDocumentPath (String path);
	
	public native static void onNetWorkChanged (int type);
	
	public native static void setBrand (String brand);
	
	public native static void setCPUChip(String strCPUChip);
	
	public native static void NotifySpeechFinish(long ulSerial, int iEvent, int iState,String strResult,String strWAVPath);
	
	/**
	 * 下面接口为IMEngineManager本地化接口
	 */
	public native static int Init(String strAppKey, String strSecrect);
	public native static void Uninit();
	//登陆，登出
	public native static int Login(String strYouMeID, String strPasswd, String strOldPasswd);
	public native static int Logout();
	//好友部分接口
	public native static int GetContactList();
	public native static int AddContact(String strYouMeID,String strReason);
	public native static int AgreeContactInvited(String strYouMeID);
	public native static int RefuseContactInvited(String strYouMeID,String strReason);
	public native static int DeleteContact(String strYouMeID);
	//消息接口
	public native static int GetContactOfflineMessage();
	public native static int SendTextMessage(String strRecvID, int chatType, String strContent, MessageRequestId requestID);
	public native static int SendCustomMessage(String strRecvID, int chatType, String strCustomMessage,int iBufferLen, MessageRequestId requestID);
	public native static int SendAudioMessage(String strRecvID, int chatType,  MessageRequestId requestID);
	//不会转文字，直接发送的音频
	public native static int SendOnlyAudioMessage(String strRecvID, int chatType, MessageRequestId requestID);
	public native static int StopAudioMessage(String strParam);
	public native static int CancleAudioMessage();
	public native static int DownloadAudioFile(int iSerial,String strSavePath);
	//聊天室接口
	public native static int JoinChatRoom(String strID);
	public native static int LeaveChatRoom(String strID);
	//提供一个get 接口，用来读取数据,如果没有数据会阻塞，调用登出的话，函数会返回
	public native static String GetMessage();
	public native static void PopMessage();
	public native static int GetSDKVer();
	public native static void SetServerZone(int zone);
	//设置模式0 正式环境 1开发环境 2 测试环境 3 商务环境。 默认正式环境。所以客户不需要调用这个接口
	public native static void SetMode(int iMode);
	public native static String GetFilterText(String strSource);
	public native static void  DestroyFilterText(String strSource);
		
}

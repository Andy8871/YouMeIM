package com.youme.ui.entries;


import com.google.gson.Gson;
import com.iflytek.thridparty.m;
import com.youme.imengine.IMEngine;
import com.youme.ui.listener.OnControllerListener;
import com.youme.ui.listener.OnLoginListener;
import com.youme.ui.utils.YouMeCallBackThread;

import android.R.integer;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.NonNull;
import android.util.Log;

public class YouMeIMManager {
	private final static String strAppKey = "YOUMEBC2B3171A7A165DC10918A7B50A4B939F2A187D0";
	private final static String strSecrect = "r1+ih9rvMEDD3jUoU+nj8C7VljQr7Tuk4TtcByIdyAqjdl5lhlESU0D+SoRZ30sopoaOBg9EsiIMdc8R16WpJPNwLYx2WDT5hI/HsLl1NJjQfa9ZPuz7c/xVb8GHJlMf/wtmuog3bHCpuninqsm3DRWiZZugBTEj2ryrhK7oZncBAAE=";
	private final static String DEFAULT_PASSWORD = "888888";
	private final static String DEFAULT_OLD_PASSWORD = "";
	
	private String worldId = null;
	private String allianceId = null;
	private YouMeContactModel user = null;
	private OnLoginListener onLoginListener = null;
	private OnControllerListener onControllerListener = null;
	
	public final static int CHAT_TYPR_UNKNOW = IMEngine.ChatType_Unknow;
	public final static int CHAT_TYPE_ROOM = IMEngine.ChatType_RoomChat;
	public final static int CHAT_TYPE_PRIVATE = IMEngine.ChatType_PrivateChat;
	
	public final static int RESPONSE_LOGIN = 1;
	
	private YouMeIMManager(){
		
	}
	private static YouMeIMManager instance = null;
	public static YouMeIMManager getInstance(){
		if (null == instance)
			instance = new YouMeIMManager();
		return instance;
	}
	
	/**
	 * 处理回调线程数据
	 */
	private Handler handler = new Handler(){
		@Override
		public void handleMessage(Message msg) {
			int what = msg.what;
			switch (what) {
			case RESPONSE_LOGIN:
				YouMeResponse.Login login = (YouMeResponse.Login)msg.obj;
				int errorcode = msg.arg1;
				if (null != onLoginListener){
					onLoginListener.onLogion(login.youmeId, errorcode, login.status);
				}
				/**
				 * 加入世界房间
				 */
				errorcode = IMEngine.IM_JoinChatRoom(worldId);
				
				/**
				 *  加入联盟房间
				 */
				if (null != allianceId){
					errorcode = IMEngine.IM_JoinChatRoom(allianceId);
				}
				
				
				break;

			default:
				break;
			}
		}
		
	};
	private YouMeCallBackThread mCallBackThread = new YouMeCallBackThread(handler);
	private String mDataSavePath = null;
	private static int mChatLevel = 0;
	
	
	
	public void setOnControllerListener(OnControllerListener l){
		onControllerListener = l;
	}
	
	/**
	 * 设置数据存储目录
	 * @param path	数据存储目录,参数类型为String
	 */
	public void setSaveDataPath(String path){
		mDataSavePath = path;
	}
	
	/**
	 * 初始化YouMeIMEngine
	 */
	public void initIM(){
		IMEngine.IM_Init(strAppKey, strSecrect);
		mCallBackThread.start();
	}
	
	/**
	 * 登录接口
	 * @param userModel	json序列化用户信息
	 * @param worldName 世界名称,必须不能为空
	 * @param allianceName 联盟名称,可为null或为空
	 * @param l 登录回调接口
	 */
	public void login(String userModel, @NonNull String worldId, String allianceId, OnLoginListener l){
		this.worldId = worldId;
		this.allianceId = allianceId;
		Gson gson = new Gson();
		user = (YouMeContactModel)gson.fromJson(userModel, YouMeContactModel.class);
		
		/**
		 * IM密码与游戏密码不相关，用户不需要修改
		 */
		int errorcode = IMEngine.IM_Login(user.getUserId(), DEFAULT_PASSWORD, DEFAULT_OLD_PASSWORD);
		onLoginListener = l;
	}
	
	/**
	 * 显示IM UI
	 */
	public void showIM(){
		
	}
	
	/**
	 * 隐藏IM UI
	 */
	public void hideIM(){
		
	}
	
	/**
	 * 设置联盟房间名
	 * @param room	参数类型为String,联盟房间名
	 */
	public void updateAllianceRoom(String room){
		
	}
	
	/**
	 * 设置初始化世界消息
	 * @param content 参数类型为String,参数格式为Json数据
	 */
	public void setInitialSystemMessage(String content){
		
	}
	
	public void addSystemMessage(String content){
		
	}
	
	public void setChatLevel(int level){
		mChatLevel = level;
	}
	
	public void setEmotionPackage(String emotion){
		
	}
	
	public int sendTextMessage(String strRecvID, int chatType, String strContent, IMEngine.MessageRequestId requestID){
		return IMEngine.IM_SendTextMessage(strRecvID, chatType, strContent, requestID);
	}
	
	public String getWorldId(){
		return worldId;
	}
}

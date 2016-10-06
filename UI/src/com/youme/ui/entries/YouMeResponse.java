package com.youme.ui.entries;


import java.util.List;

import com.google.gson.Gson;
import com.google.gson.annotations.SerializedName;

public class YouMeResponse{
	/**
	 * YouMe communication command
	 */
	public final static int COMMAND_UNKNOW = 0;
	public final static int COMMAND_LOGIN = 1;
	public final static int COMMAND_LOGOUT = 2;
	public final static int COMMAND_LOGINRESPONSE = 3;
	public final static int COMMAND_LOGOUTRESPONSE = 4;
	public final static int COMMAND_CONTACTLOGIN = 5;
	public final static int COMMAND_CONTACTLOGOUT = 6;
	public final static int COMMAND_HEART = 7;
	public final static int COMMAND_HEARTRESPONSE = 8;
	public final static int COMMAND_PACKAGEACK = 9;
	public final static int COMMAND_SESSIONEXPIRE = 10;
	public final static int COMMAND_CLIENTTOSERVERPACKAGEACK = 11;
	public final static int COMMAND_REDIRECT = 12;
	public final static int COMMAND_REDIRECTRESPONSE = 13;
	public final static int COMMAND_GETCONTACTLIST = 1000;
	public final static int COMMAND_GETCONTACTLISTRESPONSE = 1001;
	public final static int COMMAND_GETBLACKLISTLIST = 1002;
	public final static int COMMAND_GETBLACKLISTLISTRESPONSE = 1003;
	public final static int COMMAND_ADDCONTACT = 1004;
	public final static int COMMAND_ADDCONTACTREQUEST = 1005;
	public final static int COMMAND_RECEIVEINVITEDAPPLICATION = 1006;
	public final static int COMMAND_AGREECONTACTINVITED = 1007;
	public final static int COMMAND_REFUSECONTACTINVITED = 1008;
	public final static int COMMAND_INVITEDRESPONSE = 1009;
	public final static int COMMAND_ADDCONTACTRESPONSE = 1010;
	public final static int COMMAND_DELETECONTACT = 1011;
	public final static int COMMAND_DELETECONTACTRESPONSE = 1012;
	public final static int COMMAND_WASDELETED = 1013;
	public final static int COMMAND_ADDTOBLACKLIST = 1014;
	public final static int COMMAND_ADDTOBLACKLISTRESPONSE = 1015;
	public final static int COMMAND_ADDEDTOBLACKLIST = 1016;
	public final static int COMMAND_KICKOUTFROMBLACKLIST = 1017;
	public final static int COMMAND_KICKOUTFROMBLACKLISTRESPONSE = 1018;
	public final static int COMMAND_KICKOUTEDFROMBLACKLIST = 1019;
	public final static int COMMAND_GETCONTACTINFO = 1020;
	public final static int COMMAND_GETCONTACTINFORESPONSE = 1021;
	public final static int COMMAND_GETGROUPINFO = 2000;
	public final static int COMMAND_GETGROUPINFORESPONSE = 2001;
	public final static int COMMAND_GETALLGROUP = 2002;
	public final static int COMMAND_GETALLGROUPRESPONSE = 2003;
	public final static int COMMAND_CREATEGROUP = 2004;
	public final static int COMMAND_CREATEGROUPRESPONSE = 2005;
	public final static int COMMAND_DESTROYGROUP = 2006;
	public final static int COMMAND_DESTROYGROUPRESPONSE = 2007;
	public final static int COMMAND_GROUPDESTROYED = 2008;
	public final static int COMMAND_JOINGROUP = 2009;
	public final static int COMMAND_JOINGROUPREQUEST = 2010;
	public final static int COMMAND_JOINGROUPRESPONSE = 2011;
	public final static int COMMAND_OTHERJOINGROUP = 2012;
	public final static int COMMAND_LEAVEGROUP = 2013;
	public final static int COMMAND_LEAVEGROUPRESPONSE = 2014;
	public final static int COMMAND_OTHERLEAVEGROUP = 2015;
	public final static int COMMAND_KICKOUTMEMBER = 2016;
	public final static int COMMAND_KICKOUTMEMBERRESPONSE = 2017;
	public final static int COMMAND_OTHERKICKOUTEDFROMGROUP = 2018;
	public final static int COMMAND_GETGROUPMEMBERS = 2019;
	public final static int COMMAND_GETGROUPMEMBERSRESPONSE = 2020;
	public final static int COMMAND_MODIFYGROUPINFO = 2021;
	public final static int COMMAND_MODIFYGROUPINFORESPONSE = 2022;
	public final static int COMMAND_GROUPINFOMODIFYED = 2023;
	public final static int COMMAND_AGREEGROUPINVITED = 2024;
	public final static int COMMAND_REFUSEGROUPINVITED = 2025;
	public final static int COMMAND_GROUPINVITEDRESPONSE = 2026;
	public final static int COMMAND_MODIFYMEMBERINFO = 2027;
	public final static int COMMAND_MODIFYMEMBERINFORESPONSE = 2028;
	public final static int COMMAND_MEMBERINFOMODIFYED = 2029;
	public final static int COMMAND_RECEIVEGROUPINVITEDAPPLICATION = 2030;
	public final static int COMMNAD_CHAT_CUSTOM = 3000;
	public final static int COMMNAD_CHAT_TEXT = 3001;
	public final static int COMMNAD_CHAT_EMOJI = 3002;
	public final static int COMMNAD_CHAT_FILE = 3003;
	public final static int COMMNAD_SENDMESSAGE = 3004;
	public final static int COMMNAD_CHAT_VOICE = 3005;
	public final static int COMMNAD_CHAT_CUSTOM_SERVER = 3101;
	public final static int COMMNAD_CHAT_TEXT_SERVER = 3102;
	public final static int COMMNAD_CHAT_EMOJI_SERVER = 3103;
	public final static int COMMNAD_CHAT_FILE_SERVER = 3104;
	public final static int COMMNAD_RECVMESSAGE = 3105;
	public final static int COMMNAD_CHAT_VOICE_SERVER = 3106;
	public final static int COMMNAD_GETMESSAGE_NOTIFY = 3107;
	public final static int COMMAND_GETOFFLINEMESSAGERESPONSE = 3205;
	public final static int COMMAND_GETOFFLINEMESSAGE = 3206;
	public final static int COMMAND_MESSAGETOSERVERACK = 3207;
	public final static int COMMAND_GETUPLOADTOKEN = 3208;
	public final static int COMMAND_GETUPLOADTOKENRESPONSE = 3209;
	public final static int COMMAND_GETDOWNLOADTOKEN = 3210;
	public final static int COMMAND_GETDOWNLOADTOKENRESPONSE = 3211;
	public final static int COMMAND_DOWNLOAD = 3212;
	public final static int COMMAND_SERVERFORWARDMSG = 10001;
	public final static int COMMAND_SERVERFORWARDGROUPMSG = 10002;
	public final static int COMMAND_SERVERFORWARDROMMMSG = 10003;
	public final static int COMMAND_QUERYSERVERRUNSTATUS = 10100;
	public final static int COMMAND_QUERYSERVERRUNSTATUSRESPONSE = 10101;
	
	private static <T> T getResponseData(String msg, Class<T> classOfT){
		return (T)(new Gson().fromJson(msg, classOfT));
	}
	
	public static ResponseData getResponse(String msg){
		return getResponseData(msg, ResponseData.class);
	}
	
	public static Login getLoginInfo(String msg){
		return getResponseData(msg, Login.class);
	}
	
	public static Download getDownloadInfo(String msg){
		return getResponseData(msg, Download.class);
	}
	
	public static ChatVoice getChatVoiceInfo(String msg){
		return getResponseData(msg, ChatVoice.class);
	}
	
	public static AddContactRequest getAddContactRequestInfo(String msg){
		return getResponseData(msg, AddContactRequest.class);
	}
	
	public static SendMessage getSendMessageInfo(String msg){
		return getResponseData(msg, SendMessage.class);
	}
	
	public static RecvMessage getRecvMessageInfo(String msg){
		return getResponseData(msg, RecvMessage.class);
	}
	
	public static JoinGroupRequest getJoinGroupRequestIndo(String msg){
		return getResponseData(msg, JoinGroupRequest.class);
	}
	
	public static JoinGroupResponse getJoinGroupResponseInfo(String msg){
		return getResponseData(msg, JoinGroupResponse.class);
	}
	
	public static AddContactResponse getAddContactResponseInfo(String msg){
		return getResponseData(msg, AddContactResponse.class);
	}
	
	
	public static ContactList getContactListInfo(String msg){
		return getResponseData(msg, ContactList.class);
	}
	public class ResponseData{
		@SerializedName("Command")
		public int command;
		@SerializedName("Errorcode")
		public int errorCode;
		
	}
	
	public class Login{
		@SerializedName("YouMeID")
		public String youmeId;
		
		@SerializedName("Status")
		public int status;
	}
	
	
	public class Download{
		@SerializedName("Serial")
		public int serial;
		
		@SerializedName("SavePath")
		public String strSavePath;
		
		/*public int getSerial(){
			return serial;
		}
		
		public void setSerial(int serial){
			this.serial = serial;
		}
		
		public String getSavePath(){
			return strSavePath;
		}
		
		public void setSavePath(String path){
			strSavePath = path;
		}*/
	}
	
	public class ChatVoice{
		@SerializedName("RequestID")
		public int requestId;
		
		@SerializedName("Text")
		public String extraText;
		
		@SerializedName("LocalPath")
		public String localPath;
		
		@SerializedName("Duration")
		public int duration;
	}
	
	public class AddContactRequest{
		@SerializedName("YouMeID")
		public String youmeId;
		
		/*public String getYouMeId(){
			return youmeId;
		}
		
		public void setYouMeId(String id){
			youmeId = id;
		}*/
	}
	
	public class SendMessage{
		@SerializedName("RequestID")
		public int requestId;
		
		/*public int getRequestId(){
			return requestId;
		}
		
		public void setRequestId(int requestId){
			this.requestId = requestId;
		}*/
	}
	
	public class RecvMessage{
		@SerializedName("ChatType")
		public int chatType;
		
		@SerializedName("SenderID")
		public String senderId;
		
		@SerializedName("ReceiveID")
		public String receiveId;
		
		@SerializedName("Serial")
		public int serial;
		
		@SerializedName("MessageType")
		public int msgType;
		
		@SerializedName("Content")
		public String content;
		
		@SerializedName("Text")
		public String extraText;
		
		@SerializedName("Param")
		public String param;
		
		@SerializedName("Duration")
		public int duration;
		
		/*public int getChatType(){
			return chatType;
		}
		
		public String getSenderId(){
			return senderId;
		}
		
		public String getReceiveId(){
			return receiveId;
		}
		
		public int getSerial(){
			return serial;
		}
		
		public int getMsgType(){
			return msgType;
		}
		
		public String getContent(){
			return content;
		}
		
		public String getExtraText(){
			return extraText;
		}
		
		public String getParam(){
			return param;
		}
		
		public int getDuration(){
			return duration;
		}*/
	}
	
	public class JoinGroupRequest{
		@SerializedName("GroupID")
		public String groupId;
	}
	
	public class JoinGroupResponse{
		@SerializedName("GroupEvt")
		public int groupEvent;
		
		@SerializedName("GroupID")
		public String groupId;
		
		@SerializedName("GroupName")
		public String groupName;
		
		@SerializedName("GroupType")
		public int groupType;
	}
	
	public class AddContactResponse{
		@SerializedName("EvtType")
		public int contactEvent;
		
		@SerializedName("YouMeID")
		public String youmeId;
		
		@SerializedName("Status")
		public int status;
	}
	
	public class ReceiveInvitedApplication{
		
	}
	
	
	
	public static class ContactList{
		@SerializedName("Contacts")
		public List<Contact> contacts;
		public static class Contact {
			@SerializedName("YouMeID")
			private String youmeId;
			@SerializedName("Status")
			private int status;
			
			public String getYouMeId(){
				return youmeId;
			}
			
			public void setYouMeId(String id){
				youmeId = id;
			}
			
			public int getStatus(){
				return status;
			}
			
			public void setStatus(int status){
				this.status = status;
			}
		}
	}
	
}

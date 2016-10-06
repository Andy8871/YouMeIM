using UnityEngine;
using System.Collections;

namespace YouMe
{
	public class YouMeContactInfo
	{
		//contact info
		private string m_strYouMeID;
		private YouMeIMContactStatus m_iStatus;
		
		public string YouMeID {
			get {
				return m_strYouMeID;
			}
			set {
				m_strYouMeID = value;
			}
		}
		

		public YouMeIMContactStatus Status {
			get {
				return m_iStatus;
			}
			set {
				m_iStatus = value;
			}
		}
	}

	public class YouMeGroupInfo
	{
		private string m_strGroupID;
		private string m_strGroupName;
		private YouMe.YouMeIMGroupType groupType;

		public string GroupID {
			get {
				return m_strGroupID;
			}
			set {
				m_strGroupID = value;
			}
		}

		public string GroupName {
			get {
				return m_strGroupName;
			}
			set {
				m_strGroupName = value;
			}
		}

		public YouMe.YouMeIMGroupType GroupType {
			get {
				return groupType;
			}
			set {
				groupType = value;
			}
		}
	}

	public abstract class YouMeMessageInfoBase
	{
		private YouMe.YouMeIMChatType chatType;
		private string strSenderID;
		private string strRecvID;
		private int ulSerial;
		private YouMeIMMessageBodyType messageType;
		
		public YouMe.YouMeIMChatType ChatType {
			get {
				return chatType;
			}
			internal set {
				chatType = value;
			}
		}
		
		public string SenderID {
			get {
				return strSenderID;
			}
			set {
				strSenderID = value;
			}
		}
		
		public string RecvID {
			get {
				return strRecvID;
			}
			set {
				strRecvID = value;
			}
		}
		
		public YouMeIMMessageBodyType MessageType {
			get {
				return messageType;
			}
			set {
				messageType = value;
			}
		}

		public int Serial {
			get {
				return ulSerial;
			}
			set {
				ulSerial = value;
			}
		}
	}

	public class YouMeTextMessage:YouMeMessageInfoBase
	{
		private string strContent;
		
		public string Content {
			get {
				return strContent;
			}
			set {
				strContent = value;
			}
		}
	}

	public class YouMeCustomMessage:YouMeMessageInfoBase
	{
		private byte[] strContent;
		
		public byte[] Content {
			get {
				return strContent;
			}
			set {
				strContent = value;
			}
		}
	}
	public class YouMeVoiceMessage:YouMeMessageInfoBase
	{
		private string strText;
		private string strParam;
		private int iDuration;

		public int Duration {
			get {
				return iDuration;
			}
			set {
				iDuration = value;
			}
		}

		public string Text {
			get {
				return strText;
			}
			set {
				strText = value;
			}
		}

		public string Param {
			get {
				return strParam;
			}
			set {
				strParam = value;
			}
		}
	}
	public enum YouMeIMErrorCode
	{
		YouMeIMErrorcode_Success = 0,
		YouMeIMErrorcode_EngineNotInit = 1,
		YouMeIMErrorcode_NotLogin = 2,	
		YouMeIMErrorcode_ParamInvalid = 3,
		YouMeIMErrorcode_TimeOut = 4,
		YouMeIMErrorcode_StatusError = 5,
		YouMeIMErrorcode_SDKInvalid = 6,
		YouMeIMErrorcode_AlreadyLogin = 7,
		YouMeIMErrorcode_ServerError = 8,
		YouMeIMErrorcode_NetError = 9,
		YouMeIMErrorcode_LoginSessionError = 10,
		YouMeIMErrorcode_NotStartUp = 11,
		//服务器的错误码
		YouMeIMErrorcode_ALREADYFRIENDS = 1000,
		YouMeIMErrorcode_LoginInvalid = 1001,
		
		//语音部分错误码
		YouMeIMErrorcode_PTT_Start = 2000,
		YouMeIMErrorcode_PTT_Fail = 2001,
		YouMeIMErrorcode_Fail = 10000
	} 

	//添加好友回复事件
	public enum YouMeIMContactStatus
	{
		YouMeIMContactStatus_Online = 0,
		YouMeIMContactStatus_Offline = 1
	};
	//群类型
	public enum YouMeIMGroupType
	{
		//普通群
		GroupType_Group = 0,
		//聊天室
		GroupType_ChatRoom = 1
	};

	public enum YouMeIMGroupEvent
	{
		YouMeIMGroupEvent_Agree = 0,
		YouMeIMGroupEvent_Refuse = 1,
	};

	//添加好友回复事件
	public enum YouMeIMContactEvent
	{
		AddContactEvent_Agree = 0,
		AddContactEvent_Refuse = 1
	};
	public enum YouMeIMChatType
	{
		ChatType_Unknow = 0,
		ChatType_PrivateChat = 1,
		ChatType_GroupChat = 2,
		ChatType_RoomChat = 3,
	};
	public enum YouMeIMMessageBodyType
	{
		MessageBodyType_Unknow = 0,
		MessageBodyType_TXT = 1,
		MessageBodyType_CustomMesssage = 2,
		MessageBodyType_Emoji = 3,
		MessageBodyType_Image = 4,
		MessageBodyType_Voice = 5,
		MessageBodyType_Video = 6,
		MessageBodyType_File = 7
	};
	public enum YouMeIMCommand
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
	}
}
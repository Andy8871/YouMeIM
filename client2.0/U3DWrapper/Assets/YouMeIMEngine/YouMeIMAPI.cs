using UnityEngine;
using System.Runtime.InteropServices;
using System.Collections;
using System.Threading;

#if UNITY_IOS
using AOT;
#endif
#if UNITY_IOS || UNITY_ANDROID

namespace YouMe{

	public class YouMeIMAPI
	{
		//所有的C接口的导出		
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern int IM_Init([MarshalAs(UnmanagedType.LPTStr)] string strAppKey,[MarshalAs(UnmanagedType.LPTStr)] string strAppSecrect);



		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern void IM_Uninit();


		//login
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern int IM_Login([MarshalAs(UnmanagedType.LPTStr)] string strYouMeID,[MarshalAs(UnmanagedType.LPTStr)] string strPasswd,[MarshalAs(UnmanagedType.LPTStr)] string strOldPasswd);

		//logout
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern int IM_Logout();


		//contactlist
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern int IM_GetContactList();



		//addcontact
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern int IM_AddContact([MarshalAs(UnmanagedType.LPTStr)] string strYouMeID,[MarshalAs(UnmanagedType.LPTStr)] string strReason);

	
		//agreecontact
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern int IM_AgreeContactInvited([MarshalAs(UnmanagedType.LPTStr)] string strYouMeID);

		//refusecontat
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern int IM_RefuseContactInvited([MarshalAs(UnmanagedType.LPTStr)] string strYouMeID,[MarshalAs(UnmanagedType.LPTStr)] string strReason);

	
		//delete contact
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern int IM_DeleteContact([MarshalAs(UnmanagedType.LPTStr)] string strYouMeID);


		// offline message
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern int IM_GetContactOfflineMessage();

		//send text message
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern int IM_SendTextMessage([MarshalAs(UnmanagedType.LPTStr)] string strRecvID,int iChatType,[MarshalAs(UnmanagedType.LPTStr)] string strContent,ref int iRequestID);

		//send custom message
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern int IM_SendCustomMessage([MarshalAs(UnmanagedType.LPTStr)] string strRecvID,int iChatType,[MarshalAs(UnmanagedType.LPArray)] byte[] buffer ,int bufferLen, ref int iRequestID);

		//send audio message
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern int IM_SendAudioMessage([MarshalAs(UnmanagedType.LPTStr)] string strRecvID,int iChatType,ref int iRequestID);

		//send audio message
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern int IM_SendOnlyAudioMessage([MarshalAs(UnmanagedType.LPTStr)] string strRecvID,int iChatType,ref int iRequestID);


		//stop audio message
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern int IM_StopAudioMessage([MarshalAs(UnmanagedType.LPTStr)] string strParam);

		//cancle audio message
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern int IM_CancleAudioMessage();


		//download audio message
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern int IM_DownloadAudioFile(int iSerial,[MarshalAs(UnmanagedType.LPTStr)] string strSavePath);


		//join chatroom
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern int IM_JoinChatRoom([MarshalAs(UnmanagedType.LPTStr)] string strID);

		//leave chatroom
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern int IM_LeaveChatRoom([MarshalAs(UnmanagedType.LPTStr)] string strID);


		//leave chatroom
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern int IM_GetSDKVer();


		//getmessget 
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern System.IntPtr IM_GetMessage();



		//popmessage
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern void IM_PopMessage();



		//getfiltertext
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern System.IntPtr IM_GetFilterText([MarshalAs(UnmanagedType.LPTStr)] string strSource);
		
		
		
		//destroyfiltertext
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern void IM_DestroyFilterText(System.IntPtr pBuffer);

		//set mode
		#if UNITY_IPHONE
		[DllImport("__Internal")]
		#elif UNITY_ANDROID
		[DllImport("YouMeIMEngine")]
		#endif
		private static extern void IM_SetMode(int iMode);//设置模式0 正式环境 1开发环境 2 测试环境 3 商务环境。 默认正式环境。所以客户不需要调用这个接口

        #if UNITY_IPHONE
        [DllImport("__Internal")]
        #elif UNITY_ANDROID
        [DllImport("YouMeIMEngine")]
        #endif
        private static extern void IM_SetServerZone(int zone);



		/****************************************************************************************/
		private static YouMeIMAPI s_Instance = null;
		private Thread m_getMsgThread =null;
		private bool m_bExit = false;
		private YouMe.YouMeLoginListen m_loginListen;
		private YouMe.YouMeIMContactListen m_contactListen;
		private YouMe.YouMeIMMessageListen m_messageListen;
		private YouMe.YouMeIMGroupListen m_groupListen;
		private YouMe.YouMeIMDownloadListen m_downloadListen;
		public static YouMeIMAPI Instance()
		{
			if (s_Instance == null) 
			{
				s_Instance = new YouMeIMAPI();
			}
			return s_Instance;
		}

		public static int GetSDKVer()
		{
			return IM_GetSDKVer ();
		}
		public void SetLoginListen(YouMe.YouMeLoginListen listen)
		{
			m_loginListen = listen;
		}

		public void SetContactListen(YouMe.YouMeIMContactListen listen)
		{
			m_contactListen = listen;
		}

		public void SetMessageListen(YouMe.YouMeIMMessageListen listen)
		{
			m_messageListen = listen;
		}
		public void SetGroupListen(YouMe.YouMeIMGroupListen listen)
		{
			m_groupListen = listen;
		}
		public void SetDownloadListen(YouMe.YouMeIMDownloadListen listen)
		{
			m_downloadListen = listen;
		}
		//api 
		public YouMeIMErrorCode Init(string strAppKey,string strSecrect)
		{
			Debug.Log ("start init");
			YouMeIMErrorCode code = (YouMeIMErrorCode)IM_Init (strAppKey, strSecrect);
            if(code == YouMeIMErrorCode.YouMeIMErrorcode_Success){
                m_bExit = false;
            }
            return code;
		}
		public void UnInit()
		{
			m_bExit = true;
			IM_Uninit ();
			if((m_getMsgThread != null) && m_getMsgThread.IsAlive)
			{
				m_getMsgThread.Join();
			}
            m_getMsgThread = null;
		}

		//statci 
		public static string GetFilterText(string strSource)
		{
			System.IntPtr pBuffer = IM_GetFilterText (strSource);
			string strMessage = Marshal.PtrToStringAuto(pBuffer);
			IM_DestroyFilterText (pBuffer);
			return strMessage;
		}

		//get msg
		void GetMessage()
		{
			while (!m_bExit) {
				System.IntPtr pBuffer = IM_GetMessage();
				string strMessage = Marshal.PtrToStringAuto(pBuffer);
				Debug.Log("recv message:" + strMessage);
				if(null != strMessage)
				{
					try{
						ParseJsonMessageCallback(strMessage);
					}catch(System.Exception e){
						Debug.LogError(e.StackTrace);
					}
				}

				IM_PopMessage();
			}
		}

		void ParseJsonMessageCallback(string strMessage)
		{
			YouMe.JsonData jsonMessage =  YouMe.JsonMapper.ToObject (strMessage);
			YouMe.YouMeIMCommand command = (YouMe.YouMeIMCommand)(int)jsonMessage ["Command"];
			YouMe.YouMeIMErrorCode errorcode = (YouMe.YouMeIMErrorCode)(int)jsonMessage ["Errorcode"];
			switch (command) {
				case YouMeIMCommand.Command_Download:
				{
					if(null != m_downloadListen)
					{
						string strSavePath = (string)jsonMessage["SavePath"];
						int iSerial = (int)jsonMessage["Serial"];
						m_downloadListen.OnDownload(iSerial,errorcode,strSavePath);
					}
				}
				break;
				case YouMeIMCommand.Command_Login:
				{
					if(m_loginListen != null)
					{
						YouMe.YouMeContactInfo contact = new YouMe.YouMeContactInfo();
						if(errorcode == YouMeIMErrorCode.YouMeIMErrorcode_Success)
						{
							contact.YouMeID = (string)jsonMessage["YouMeID"];
							contact.Status = (YouMe.YouMeIMContactStatus)(int)jsonMessage["Status"];
						}
						m_loginListen.OnLogin(errorcode,contact);
					}
				}
				break;
				case YouMeIMCommand.Command_LogOut:
				{
					if(m_loginListen != null)
					{
						m_loginListen.OnLogout();
					}
				}
				break;
				case YouMeIMCommand.Command_AddContactRequest:
				case YouMeIMCommand.Command_WasDeleted:
				case YouMeIMCommand.Command_AddedToBlacklist:
				case YouMeIMCommand.Command_KickoutedFromBlacklist:
				case YouMeIMCommand.Command_DeleteContactResponse:
				case YouMeIMCommand.Command_AddToBlacklistResponse:
				case YouMeIMCommand.Command_KickoutFromBlacklistResponse:
				case YouMeIMCommand.Command_ContactLogin:
				case YouMeIMCommand.Command_ContactLogOut:
				{
					string strYouMeID = (string)jsonMessage["YouMeID"];
					if(null != m_contactListen)
					{
						if(command == YouMeIMCommand.Command_AddContactRequest)
						{
							m_contactListen.OnAddContactRequest(errorcode,strYouMeID);
						}
						else if(command == YouMeIMCommand.Command_WasDeleted)
						{
							m_contactListen.OnContactBeDeleted(strYouMeID);
						}
						else if(command == YouMeIMCommand.Command_DeleteContactResponse)
						{
							m_contactListen.OnDeleteContactResponse(errorcode,strYouMeID);
						}
						else if(command == YouMeIMCommand.Command_ContactLogin)
						{
							m_contactListen.OnContactLogin(strYouMeID);
						}
						else if(command == YouMeIMCommand.Command_ContactLogOut)
						{
							m_contactListen.OnContactLogout(strYouMeID);
						}
					}
				}

				break;
				case YouMeIMCommand.Command_AddContactResponse:
				{
					if(null != m_contactListen)
					{
						YouMe.YouMeContactInfo contact = new YouMe.YouMeContactInfo();			
						contact.YouMeID = (string)jsonMessage["YouMeID"];
						contact.Status = (YouMe.YouMeIMContactStatus)(int)jsonMessage["Status"];						
						m_contactListen.OnAddContactResponse((YouMe.YouMeIMContactEvent)(int)jsonMessage["EvtType"],contact);						
					}
				}
				break;
				case YouMeIMCommand.Command_ReceiveInvitedApplication:
				{
					if(null != m_contactListen)
					{
						YouMe.YouMeContactInfo contact = new YouMe.YouMeContactInfo();			
						contact.YouMeID = (string)jsonMessage["YouMeID"];
						contact.Status = (YouMe.YouMeIMContactStatus)(int)jsonMessage["Status"];
						m_contactListen.OnReceiveContactInvited(contact,(string)jsonMessage["Reason"]);
					}
				}
				break;
				case YouMeIMCommand.Command_InvitedResponse:
				{
					if(null != m_contactListen)
					{
						m_contactListen.OnContactInvitedResponse(errorcode,(YouMe.YouMeIMContactEvent)(int)jsonMessage["EvtType"],(string)jsonMessage["YouMeID"]);
					}
				}
				break;
				case YouMeIMCommand.Command_GetContactList:
				case YouMeIMCommand.Command_GetContactInfo:
				case YouMeIMCommand.Command_GetBlacklistList:
				{
					if(null != m_contactListen)
					{
						System.Collections.Generic.LinkedList<YouMeContactInfo> contactList = new System.Collections.Generic.LinkedList<YouMeContactInfo>();
						YouMe.JsonData jsonContacts = (YouMe.JsonData)jsonMessage["Contacts"];
						for(int i=0;i<jsonContacts.Count;i++)
						{
							YouMe.YouMeContactInfo contact = new YouMe.YouMeContactInfo();			
							contact.YouMeID = (string)jsonContacts[i]["YouMeID"];
							contact.Status = (YouMe.YouMeIMContactStatus)(int)jsonContacts[i]["Status"];
							contactList.AddLast(contact);					
						}
						m_contactListen.OnGetContactList(errorcode,contactList);
					}
				}
				break;
				case YouMeIMCommand.Commnad_SendMessage:
				{
					if(null != m_messageListen)
					{
					m_messageListen.OnSendMessageStatus((int)jsonMessage["RequestID"],errorcode);
					}
				}
				break;
			case YouMeIMCommand.Commnad_Chat_Voice:
			{
				if(null != m_messageListen)
				{
					string strText = (string)jsonMessage["Text"];
					string strLocalPath = (string)jsonMessage["LocalPath"];
					int iDuration = (int)jsonMessage["Duration"];
					m_messageListen.OnSendAudioMessageStatus((int)jsonMessage["RequestID"],errorcode,strText,strLocalPath,iDuration);
				}
			}
				break;
			case YouMeIMCommand.Commnad_RecvMessage:
			{
				if(null != m_messageListen)
				{
					YouMeIMMessageBodyType bodyType = (YouMeIMMessageBodyType)(int)jsonMessage["MessageType"];
					if(bodyType == YouMeIMMessageBodyType.MessageBodyType_TXT)
					{
						YouMeTextMessage message = new YouMeTextMessage();
						message.ChatType = (YouMeIMChatType)(int)jsonMessage["ChatType"];
						message.Serial = (int)jsonMessage["Serial"];
						message.MessageType = bodyType;
						message.RecvID = (string)jsonMessage["ReceiveID"];
						message.SenderID = (string)jsonMessage["SenderID"];
						message.Content = (string)jsonMessage["Content"];
						m_messageListen.OnRecvMessage(message);
					}
					else if(bodyType == YouMeIMMessageBodyType.MessageBodyType_CustomMesssage)
					{
						YouMeCustomMessage message = new YouMeCustomMessage();
						message.ChatType = (YouMeIMChatType)(int)jsonMessage["ChatType"];
						message.Serial = (int)jsonMessage["Serial"];
						message.MessageType = bodyType;
						message.RecvID = (string)jsonMessage["ReceiveID"];
						message.SenderID = (string)jsonMessage["SenderID"];
						string strBase64Content = (string)jsonMessage["Content"];
						message.Content = System.Convert.FromBase64String(strBase64Content);
						m_messageListen.OnRecvMessage(message);
					}
					else if(bodyType == YouMeIMMessageBodyType.MessageBodyType_Voice)
					{
						YouMeVoiceMessage message = new YouMeVoiceMessage();
						message.ChatType = (YouMeIMChatType)(int)jsonMessage["ChatType"];
						message.Serial = (int)jsonMessage["Serial"];
						message.MessageType = bodyType;
						message.RecvID = (string)jsonMessage["ReceiveID"];
						message.SenderID = (string)jsonMessage["SenderID"];
						message.Text = (string)jsonMessage["Text"];
						message.Param = (string)jsonMessage["Param"];
						message.Duration = (int)jsonMessage["Duration"];
						m_messageListen.OnRecvMessage(message);
					}
				}
			}
				break;
			case YouMeIMCommand.Command_JoinGroupRequest:
			{
				if(null != m_groupListen){
					m_groupListen.OnJoinGroupRequest(errorcode,(string)jsonMessage["GroupID"]);
				}
			}
				break;
			case YouMeIMCommand.Command_JoinGroupResponse:
			{
				if(null != m_groupListen)
				{
					YouMe.YouMeGroupInfo groupInfo = new YouMe.YouMeGroupInfo();
					groupInfo.GroupID = (string)jsonMessage["GroupID"];
					groupInfo.GroupName = (string)jsonMessage["GroupName"];
					groupInfo.GroupType = (YouMe.YouMeIMGroupType)(int)jsonMessage["GroupType"];
					m_groupListen.OnJoinGroupResponse(errorcode,(YouMe.YouMeIMGroupEvent)(int)jsonMessage["GroupEvt"],groupInfo);
				}
			}
				break;
			default:
					break;
			}
		}

		//login
		public YouMeIMErrorCode Login(string strYouMeID,string strPasswd)
		{
			return ChangePasswd (strYouMeID, strPasswd, "");
		}

		public YouMeIMErrorCode ChangePasswd(string strYouMeID,string strPasswd,string strOldPasswd)
		{
			YouMeIMErrorCode errorcode = (YouMeIMErrorCode)IM_Login(strYouMeID,strPasswd,strOldPasswd);
			if (errorcode == YouMe.YouMeIMErrorCode.YouMeIMErrorcode_Success) {
				if(null == m_getMsgThread)
				{
					m_getMsgThread = new Thread(new ThreadStart(GetMessage));
					m_getMsgThread.Start();
				}
			}
			return errorcode;
		}


		public YouMeIMErrorCode Logout()
		{
			return  (YouMeIMErrorCode)IM_Logout ();
		}

		public YouMeIMErrorCode AddContact(string strYouMeID,string strReason)
		{
			return (YouMeIMErrorCode)IM_AddContact (strYouMeID, strReason);
		}

		public YouMeIMErrorCode AgreeContactInvited(string strYouMeID)
		{
			return (YouMeIMErrorCode)IM_AgreeContactInvited (strYouMeID);
		}

		public YouMeIMErrorCode RefuseContactInvited(string strYouMeID,string strReason)
		{
			return (YouMeIMErrorCode)IM_RefuseContactInvited (strYouMeID, strReason);
		}

		public YouMeIMErrorCode GetContactList()
		{
			return (YouMeIMErrorCode)IM_GetContactList ();
		}

		public YouMeIMErrorCode DeleteContact(string strYouMeID)
		{
			return (YouMeIMErrorCode)IM_DeleteContact(strYouMeID);
		}


		//message
		public YouMeIMErrorCode GetContactOfflineMessage()
		{
			return (YouMeIMErrorCode)IM_GetContactOfflineMessage();
		}

		public YouMeIMErrorCode SendTextMessage(string strRecvID,YouMe.YouMeIMChatType chatType,string strContent,ref int iRequestID)
		{
			return (YouMeIMErrorCode)IM_SendTextMessage(strRecvID,(int)chatType,strContent,ref iRequestID);
		}

		public YouMeIMErrorCode SendAudioMessage(string strRecvID,YouMe.YouMeIMChatType chatType,ref int iRequestID)
		{
			return (YouMeIMErrorCode)IM_SendAudioMessage(strRecvID,(int)chatType,ref iRequestID);
		}

		public YouMeIMErrorCode SendOnlyAudioMessage(string strRecvID,YouMe.YouMeIMChatType chatType,ref int iRequestID)
		{
			return (YouMeIMErrorCode)IM_SendOnlyAudioMessage(strRecvID,(int)chatType,ref iRequestID);
		}
		//strParam can be empty
		public YouMeIMErrorCode StopAudioMessage(string strParam)
		{
			return (YouMeIMErrorCode)IM_StopAudioMessage (strParam);
		}

		public YouMeIMErrorCode CancleAudioMessage()
		{
			return (YouMeIMErrorCode)IM_CancleAudioMessage ();
		}
		public YouMeIMErrorCode DownloadAudioFile(int iSerial,string strSavePath)
		{
			return (YouMeIMErrorCode)IM_DownloadAudioFile (iSerial, strSavePath);
		}
		public YouMeIMErrorCode SendCustomMessage(string strRecvID,YouMe.YouMeIMChatType chatTpye,byte[] customMsg,ref int iRequestID)
		{
			return (YouMeIMErrorCode)IM_SendCustomMessage(strRecvID,(int)chatTpye,customMsg,customMsg.Length, ref iRequestID);
		}

		//chatroom
		public YouMeIMErrorCode JoinChatRoom (string strChatRoom)
		{
			return (YouMeIMErrorCode)IM_JoinChatRoom(strChatRoom);
		}

		public YouMeIMErrorCode LeaveChatRoom (string strChatRoom)
		{
			return (YouMeIMErrorCode)IM_LeaveChatRoom(strChatRoom);
		}

		private YouMeIMAPI()
		{
		}

        public void SetServerZone(int zone){
            IM_SetServerZone(zone);
        }
	}
}
#endif
using UnityEngine;
using System.Collections;
#if UNITY_IOS || UNITY_ANDROID
namespace U3dTest
{
public class testim : MonoBehaviour,
	YouMe.YouMeLoginListen ,
	YouMe.YouMeIMContactListen,
	YouMe.YouMeIMMessageListen,
	YouMe.YouMeIMGroupListen{

	private Vector2 m_Position = Vector2.zero;
	private string m_InGameLog = "";
	// Use this for initialization
	void Start () {
        #if !UNITY_EDITOR && UNITY_ANDROID
        AndroidJavaClass unity = new AndroidJavaClass ("com.unity3d.player.UnityPlayer");
        AndroidJavaObject currentActivity = unity.GetStatic<AndroidJavaObject> ("currentActivity");
        AndroidJavaClass YouMeManager = new AndroidJavaClass ("com.youme.imengine.IMEngine");
        YouMeManager.CallStatic ("Init", currentActivity);
        #endif
		YouMe.YouMeIMAPI.Instance ().SetLoginListen (this);
		YouMe.YouMeIMAPI.Instance ().SetContactListen (this);
		YouMe.YouMeIMAPI.Instance ().SetMessageListen (this);
		YouMe.YouMeIMAPI.Instance ().SetGroupListen (this);
		YouMe.YouMeIMAPI.Instance ().Init ("YOUMEBC2B3171A7A165DC10918A7B50A4B939F2A187D0", "r1+ih9rvMEDD3jUoU+nj8C7VljQr7Tuk4TtcByIdyAqjdl5lhlESU0D+SoRZ30sopoaOBg9EsiIMdc8R16WpJPNwLYx2WDT5hI/HsLl1NJjQfa9ZPuz7c/xVb8GHJlMf/wtmuog3bHCpuninqsm3DRWiZZugBTEj2ryrhK7oZncBAAE=");
	}
	
	// Update is called once per frame
	void Update () {
	
	}
	void showStatus(string msg) {
		m_InGameLog += msg;
		m_InGameLog += "\r\n";
	}
	void OnGUI() {
		
		int inset = Screen.width / 20;
		int space = Screen.width / 30;
		int btnsOneRow = 3;
		int btnWidth = (Screen.width - inset * 2 - space * (btnsOneRow - 1)) / btnsOneRow;
		int btnHeight = btnWidth / 3;
		
		int labelX = inset;
		int labelY = inset + (btnHeight + space) * 5;
		int labelWidth = Screen.width - labelX * 2;
		int labelHeight = Screen.height - labelY;
		
		GUI.BeginGroup(new Rect(labelX, labelY, labelWidth, labelHeight));
		
		m_Position = GUILayout.BeginScrollView (m_Position, GUILayout.Width(labelWidth), GUILayout.Height(labelHeight));
		GUILayout.Label(m_InGameLog);
		GUILayout.EndScrollView();
		
		GUI.EndGroup();
		
		if(GUI.Button(new Rect(inset, inset, btnWidth, btnHeight), "login")){
			YouMe.YouMeIMErrorCode errorcode = YouMe.YouMeIMAPI.Instance().Login("joexie","123456");
			Debug.Log("sendmessage: " + "errorcode: " + errorcode);
		}
		
		if(GUI.Button(new Rect(inset + btnWidth + space, inset, btnWidth, btnHeight), "logout")){
			Debug.Log("logout");
			showStatus("logout");
			YouMe.YouMeIMAPI.Instance().Logout();
			//VoiceChannelPlugin.ExitChannel();
		}
		
		if(GUI.Button(new Rect(inset + (btnWidth + space) * 2, inset, btnWidth, btnHeight), "getcontactlist")){
			showStatus("getcontactlist");
			YouMe.YouMeIMAPI.Instance().GetContactList();
		}



		if(GUI.Button(new Rect(inset, inset + btnHeight + space, btnWidth, btnHeight), "init")){
			Debug.Log("init");
			showStatus("init");
			YouMe.YouMeIMAPI.Instance ().Init ("YOUMEBC2B3171A7A165DC10918A7B50A4B939F2A187D0", "r1+ih9rvMEDD3jUoU+nj8C7VljQr7Tuk4TtcByIdyAqjdl5lhlESU0D+SoRZ30sopoaOBg9EsiIMdc8R16WpJPNwLYx2WDT5hI/HsLl1NJjQfa9ZPuz7c/xVb8GHJlMf/wtmuog3bHCpuninqsm3DRWiZZugBTEj2ryrhK7oZncBAAE=");

			//VoiceChannelPlugin.StartTalking();
		}

		if(GUI.Button(new Rect(inset+btnWidth + space, inset + btnHeight + space, btnWidth, btnHeight), "uninit")){
			Debug.Log("uninit");
			showStatus("uninit");
			YouMe.YouMeIMAPI.Instance().UnInit();
			//VoiceChannelPlugin.StartTalking();
		}

		if(GUI.Button(new Rect(inset+(btnWidth + space)*2, inset + (btnHeight + space), btnWidth, btnHeight), "offlinemessage")){
			Debug.Log("offlinemessage");
			showStatus("offlinemessage");
			YouMe.YouMeIMAPI.Instance().GetContactOfflineMessage();
			//VoiceChannelPlugin.StartTalking();
		}

		if(GUI.Button(new Rect(inset, inset + (btnHeight + space)*2, btnWidth, btnHeight), "addcontact")){
			Debug.Log("addcontact");
			showStatus("addcontact");
			YouMe.YouMeIMAPI.Instance ().AddContact("testu3d","reason");
			
			//VoiceChannelPlugin.StartTalking();
		}
		
		if(GUI.Button(new Rect(inset+btnWidth + space, inset +  (btnHeight + space)*2, btnWidth, btnHeight), "deletecontact")){
			Debug.Log("deletecontact");
			showStatus("deletecontact");
			YouMe.YouMeIMAPI.Instance().DeleteContact("testu3d");
			//VoiceChannelPlugin.StartTalking();
		}
		
		if(GUI.Button(new Rect(inset+(btnWidth + space)*2, inset + (btnHeight + space)*2, btnWidth, btnHeight), "sendmessage")){
			int iRequestID = 0;
			YouMe.YouMeIMErrorCode errorcode = YouMe.YouMeIMAPI.Instance().SendAudioMessage("testu3d",YouMe.YouMeIMChatType.ChatType_PrivateChat,ref iRequestID);
			Debug.Log("sendmessage: RequestID:" + iRequestID + "errorcode: " + errorcode);
			//VoiceChannelPlugin.StartTalking();

		}

		if(GUI.Button(new Rect(inset, inset + (btnHeight + space)*3, btnWidth, btnHeight), "joinchatroom")){
			Debug.Log("joinchatroom");
			YouMe.YouMeIMAPI.Instance ().JoinChatRoom("testu3d");
			
			//VoiceChannelPlugin.StartTalking();
		}
		
		if(GUI.Button(new Rect(inset+btnWidth + space, inset +  (btnHeight + space)*3, btnWidth, btnHeight), "leavechatroom")){
			Debug.Log("leavechatroom");
			showStatus("leavechatroom");
			YouMe.YouMeIMAPI.Instance().LeaveChatRoom("testu3d");
			//VoiceChannelPlugin.StartTalking();
		}
		
		if(GUI.Button(new Rect(inset+(btnWidth + space)*2, inset + (btnHeight + space)*3, btnWidth, btnHeight), "sendcustommessage")){
			Debug.Log("sendcustommessage");
			showStatus("sendcustommessage");
			int iRequestID = 0;
				string strText = "112345";
			
				YouMe.YouMeIMErrorCode errorcode =YouMe.YouMeIMAPI.Instance().SendCustomMessage("testu3d",YouMe.YouMeIMChatType.ChatType_PrivateChat,System.Text.Encoding.UTF8.GetBytes(strText),ref iRequestID);
			//VoiceChannelPlugin.StartTalking();
				Debug.Log("sendmessage: RequestID:" + iRequestID + "errorcode: " + errorcode);
		}

		if(GUI.Button(new Rect(inset, inset + (btnHeight + space) * 4, btnWidth, btnHeight), "Clear")){
			m_InGameLog = "";
		}

		if(GUI.Button(new Rect(inset+btnWidth, inset + (btnHeight + space) * 4, btnWidth, btnHeight), "StopAudio")){
				YouMe.YouMeIMAPI.Instance().StopAudioMessage("");
		}

		if(GUI.Button(new Rect(inset+(btnWidth + space)*2, inset + (btnHeight + space) * 4, btnWidth, btnHeight), "filter")){
			showStatus(YouMe.YouMeIMAPI.GetFilterText("这是江泽明de胡锦涛哦的法轮功的水电费水电费"));
		}
	}

	#region YouMeLoginListen implementation

	public void OnLogin (YouMe.YouMeIMErrorCode errorcode, YouMe.YouMeContactInfo contactinfo)
	{
		showStatus ("OnLogin: errorcode" + errorcode + " contact:" + contactinfo.YouMeID);
	}

	public void OnLogout ()
	{
		showStatus ("OnLogout");
	}

	#endregion

	#region YouMeIMContactListen implementation

	public void OnGetContactList (YouMe.YouMeIMErrorCode errorcode, System.Collections.Generic.LinkedList<YouMe.YouMeContactInfo> contactinfo)
	{
		Debug.Log ("OnGetContactList:" + errorcode);
		foreach( YouMe.YouMeContactInfo contact in contactinfo)
		{
			Debug.Log ("OnGetContactList contact:" + contact.YouMeID);
		}
	}

	public void OnAddContactRequest (YouMe.YouMeIMErrorCode errorcode, string strYouMeID)
	{
		Debug.Log ("OnAddContactRequest:" + errorcode + " contact:" + strYouMeID);
	}

	public void OnAddContactResponse (YouMe.YouMeIMContactEvent evtType, YouMe.YouMeContactInfo contactinfo)
	{
		Debug.Log ("OnAddContactResponse:" + evtType + " contact:" + contactinfo.YouMeID);
	}

	public void OnReceiveContactInvited (YouMe.YouMeContactInfo contactinfo, string strReason)
	{
		Debug.Log ("OnReceiveContactInvited:" + contactinfo.YouMeID +" strReason:" + strReason);
	}

	public void OnContactInvitedResponse (YouMe.YouMeIMErrorCode errorcode, YouMe.YouMeIMContactEvent evtType, string strYouMeID)
	{
		Debug.Log ("OnContactInvitedResponse:" + strYouMeID +" YouMeIMContactEvent:" + evtType);
	}

	public void OnContactBeDeleted (string strYouMeID)
	{
		Debug.Log ("OnContactBeDeleted:" + strYouMeID);
	}

	public void OnDeleteContactResponse (YouMe.YouMeIMErrorCode errorcode, string strYouMeID)
	{
		Debug.Log ("OnDeleteContactResponse:" + strYouMeID);
	}

	public void OnContactLogin (string strYouMeID)
	{
		Debug.Log ("OnContactLogin:" + strYouMeID);
	}

	public void OnContactLogout (string strYouMeID)
	{
		Debug.Log ("OnContactLogout:" + strYouMeID);
	}

	#endregion

	#region YouMeIMMessageListen implementation

	public void OnSendMessageStatus (int iRequestID,  YouMe.YouMeIMErrorCode errorcode)
	{
		Debug.Log ("OnSendMessageStatus request:" + iRequestID + "errorcode:" + errorcode);
	}
	public void OnSendAudioMessageStatus(int iRequestID,  YouMe.YouMeIMErrorCode errorcode,string strText,string strAudioPath,int iDuration)
	{
			Debug.Log ("OnSendAudioMessageStatus request:" + iRequestID + "errorcode:" + errorcode + " text:" + strText + " path:" + strAudioPath);
	}
	public void OnRecvMessage (YouMe.YouMeMessageInfoBase message)
	{
		if (message.MessageType == YouMe.YouMeIMMessageBodyType.MessageBodyType_TXT) {
				YouMe.YouMeTextMessage textMsg = (YouMe.YouMeTextMessage)message;
				Debug.Log ("OnRecvMessage text:" + textMsg.Content + " send:" + textMsg.SenderID + "recv:" + textMsg.RecvID);	
			} else if (message.MessageType == YouMe.YouMeIMMessageBodyType.MessageBodyType_CustomMesssage) {
				YouMe.YouMeCustomMessage customMsg = (YouMe.YouMeCustomMessage)message;
				Debug.Log ("OnRecvMessage custom:" + System.Convert.ToBase64String (customMsg.Content) + " send:" + customMsg.SenderID + "recv:" + customMsg.RecvID);	
			} else if (message.MessageType == YouMe.YouMeIMMessageBodyType.MessageBodyType_Voice) {
				YouMe.YouMeVoiceMessage voiceMsg = (YouMe.YouMeVoiceMessage)message;
				Debug.Log ("OnRecvMessage voice:" + voiceMsg.Text + " send:" + voiceMsg.SenderID + "recv:" + voiceMsg.RecvID);	
				YouMe.YouMeIMAPI.Instance().DownloadAudioFile(voiceMsg.Serial,"/sdcard/abc.wav");
			}
	}

	#endregion

		#region YouMeIMGroupListen implementation

		public void OnJoinGroupRequest (YouMe.YouMeIMErrorCode errorcode, string strGroupID)
		{

		}

		public void OnJoinGroupResponse (YouMe.YouMeIMErrorCode errorcode, YouMe.YouMeIMGroupEvent evt, YouMe.YouMeGroupInfo groupInfo)
		{

		}

		#endregion
}
}
#endif

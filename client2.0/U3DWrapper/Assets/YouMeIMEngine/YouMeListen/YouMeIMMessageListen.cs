using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace YouMe
{
	public interface YouMeIMMessageListen
	{
		void OnSendMessageStatus(int iRequestID,  YouMe.YouMeIMErrorCode errorcode);
		//接收到用户发来的消息
		void OnRecvMessage(YouMe.YouMeMessageInfoBase message);

		void OnSendAudioMessageStatus(int iRequestID,  YouMe.YouMeIMErrorCode errorcode,string strText,string strAudioPath,int iDuration);
	}

}
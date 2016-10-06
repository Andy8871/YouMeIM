using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace YouMe
{
	public interface YouMeIMContactListen
	{
		void OnGetContactList(YouMe.YouMeIMErrorCode errorcode,LinkedList<YouMeContactInfo> contactinfo);
		void OnAddContactRequest(YouMe.YouMeIMErrorCode errorcode,string strYouMeID);
		void OnAddContactResponse(YouMe.YouMeIMContactEvent evtType,YouMeContactInfo contactinfo);
		void OnReceiveContactInvited(YouMeContactInfo contactinfo,string strReason);
		void OnContactInvitedResponse(YouMe.YouMeIMErrorCode errorcode,YouMe.YouMeIMContactEvent evtType,string strYouMeID);
		void OnContactBeDeleted(string strYouMeID);
		void OnDeleteContactResponse(YouMe.YouMeIMErrorCode errorcode,string strYouMeID);
		void OnContactLogin(string strYouMeID);
		void OnContactLogout(string strYouMeID);
	}

}
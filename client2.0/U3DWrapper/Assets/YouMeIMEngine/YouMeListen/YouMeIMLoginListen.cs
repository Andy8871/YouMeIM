using UnityEngine;
using System.Collections;

namespace YouMe
{
	public interface YouMeLoginListen
	{
		void OnLogin(YouMe.YouMeIMErrorCode errorcode,YouMeContactInfo contactinfo);
		void OnLogout();
	}

}
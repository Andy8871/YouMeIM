using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace YouMe
{
	public interface YouMeIMGroupListen
	{
		void OnJoinGroupRequest(YouMe.YouMeIMErrorCode errorcode,string strGroupID);
		void OnJoinGroupResponse(YouMe.YouMeIMErrorCode errorcode,YouMe.YouMeIMGroupEvent evt,YouMeGroupInfo groupInfo);
	}

}
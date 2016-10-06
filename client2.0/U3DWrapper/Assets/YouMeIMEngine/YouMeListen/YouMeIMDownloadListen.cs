using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace YouMe
{
	public interface YouMeIMDownloadListen
	{
		void OnDownload(int iRequestID,  YouMe.YouMeIMErrorCode errorcode,string strSavePath);	
	}
}
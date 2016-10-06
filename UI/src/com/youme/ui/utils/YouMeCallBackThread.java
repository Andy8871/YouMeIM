package com.youme.ui.utils;


import com.youme.imengine.IMEngine;
import com.youme.ui.entries.YouMeIMManager;
import com.youme.ui.entries.YouMeResponse;

import android.os.Handler;
import android.os.Message;
import android.util.Log;


public class YouMeCallBackThread extends Thread {
	private final static String TAG = YouMeCallBackThread.class.getSimpleName();
	
	private volatile boolean bIsStop = false; 
	private Handler mHandler = null;
	
	public YouMeCallBackThread(Handler handler){
		mHandler = handler;
	}
	
	
	@Override
	public void run() {
		String msg = null;
		while (!bIsStop) {
			msg = IMEngine.IM_GetMessage();
			if (null == msg)
				continue;
			Log.d(TAG, "get response: " + msg);
			YouMeResponse.ResponseData responseData = YouMeResponse.getResponse(msg);
			switch (responseData.command) {
			case YouMeResponse.COMMAND_LOGIN:{
				YouMeResponse.Login login = YouMeResponse.getLoginInfo(msg);
				if (IMEngine.YouMeIMErrorcode_Success == responseData.errorCode){
					if (null != mHandler){
						Message message = mHandler.obtainMessage();
						message.what = YouMeIMManager.RESPONSE_LOGIN;
						message.obj = login;
						message.arg1 = responseData.errorCode;
						message.sendToTarget();
					}
				}
				
			}
				break;
			case YouMeResponse.COMMAND_LOGOUT:
				return;
			case YouMeResponse.COMMAND_JOINGROUPRESPONSE:
				
			default:
				break;
			}
			
			
			IMEngine.IM_PopMessage();
		}
	}
	
}

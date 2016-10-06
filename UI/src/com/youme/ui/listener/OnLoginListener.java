package com.youme.ui.listener;

import android.R.integer;

public interface OnLoginListener {
	/**
	 * 登录事件回调函数
	 * @param errorCode 登录错误码
	 */
	public void onLogion(String id, int errorCode, int status);
}

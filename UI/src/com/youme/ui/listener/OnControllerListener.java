package com.youme.ui.listener;

public interface OnControllerListener {
	/**
	 * 接收到新消息事件回调
	 * @param content 参数类型为String,消息简介：文本截取10个字符，表情为"[表情]"，语音为"[语音]"
	 */
	public void onReceiveMessage(String content);
	
	/**
	 * 头像点击事件回调
	 * @param uId 参数类型为String，内容是用户id	
	 */
	public void onInconClick(String uId);
	
	/**
	 * 关闭IM界面事件回调
	 */
	public void onHideIM();
	
	/**
	 * 发送新消息事件回调
	 * @param content 参数类型为String,消息简介：文本截取10个字符，表情为"[表情]"，语音为"[语音]"
	 */
	public void onSendMessage(String content);
}

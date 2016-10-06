package com.youme.ui.utils;


public class YouMeChatMessage {
	public final static int MSG_UNKNOWN = -1;
	
	//消息发送方
	public final static int MSG_OWNER_SEND = 1;
	public final static int MSG_OWNER_RECV = 2;
	
	//消息类型
	public final static int MSG_TYPE_TEXT = 1;	
	public final static int MSG_TYPE_AUDIO = 2;
	
	private int mMsgType = MSG_UNKNOWN;
	private int mMsgOwner = MSG_UNKNOWN;
	/*
	private String mAudioPath = null;	//
	private int mAudioSize;*/
	private String mMsgText;
	private String mOwnerName;
	
	/**
	 * 发送方用户id
	 */
	private String mSendId;
	
	/**
	 * 接收方用户id
	 */
	private String mRecvId;
	
	/**
	 * 消息id
	 */
	private int mMessageId;
	public YouMeChatMessage(int from, int type, String owner, String content){
		mMsgOwner = from;
		mMsgType = type;
		mMsgText = content;
		mOwnerName = owner;
	}
	
	public void setMsgText(String text){
		mMsgText = text;
	}
	
	public String getMsgText(){
		return mMsgText;
	}
	
	public void setMsgType(int type){
		mMsgType = type;
	}
	
	public int getMsgType(){
		return mMsgType;
	}
	
	public void setMsgOwner(int from){
		mMsgOwner = from;
	}
	
	public int getMsgOwner(){
		return mMsgOwner;
	}
	
	public String getOwnerName(){
		return mOwnerName;
	}
	
	public void setOwnerName(String name){
		mOwnerName = name;
	}
	
}

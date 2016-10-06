package com.youme.ui.entries;

import com.google.gson.annotations.SerializedName;

public class YouMeContactModel {
	/**
	 * 用户ID,唯一标识
	 */
	@SerializedName("userid")
	private String userId;
	
	/**
	 * 用户昵称,用户可修改
	 */
	@SerializedName("nick")
	private String nickName;
	
	/**
	 * 性别：0.男	1.女
	 */
	private int gender;
	
	/**
	 * 用户头像URL
	 */
	private String avatar;
	
	/**
	 * 用户等级
	 */
	private int level;
	
	/**
	 * 用户VIP等级
	 */
	@SerializedName("vip")
	private int vipLevel;
	
	public void setUserId(String id){
		userId = id;
	}
	
	public String getUserId(){
		return userId;
	}
	
	public void setNickName(String nick){
		nickName = nick;
	}
	
	public String getNickName(){
		return nickName;
	}
	
	public void setGender(int gender){
		this.gender = gender;
	}
	
	public int getGender(){
		return gender;
	}
	
	public void setAvatar(String avatar){
		this.avatar = avatar;
	}
	
	public String getAvatar(){
		return avatar;
	}
	
	public void setLevel(int level){
		this.level = level;
	}
	
	public int getLevel(){
		return level;
	}
	
	public void setVipLevel(int level){
		vipLevel = level;
	}
	
	public int getVipLevel(){
		return vipLevel;
	}
}

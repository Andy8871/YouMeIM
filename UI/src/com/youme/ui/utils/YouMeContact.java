package com.youme.ui.utils;
import com.youme.ui.entries.YouMeContactModel;

public class YouMeContact {
	private YouMeContactModel contactModel;
	
	private boolean mIsSelect = false;
	
	public void setUserId(String id){
		contactModel.setUserId(id);
	}
	
	public String getUserId(){
		return contactModel.getUserId();
	}
	
	public void setNickName(String nick){
		contactModel.setNickName(nick);
	}
	
	public String getNickName(){
		return contactModel.getNickName();
	}
	
	public void setGender(int gender){
		contactModel.setGender(gender);
	}
	
	public int getGender(){
		return contactModel.getGender();
	}
	
	public void setAvatar(String avatar){
		contactModel.setAvatar(avatar);
	}
	
	public String getAvatar(){
		return contactModel.getAvatar();
	}
	
	public void setLevel(int level){
		contactModel.setLevel(level);
	}
	
	public int getLevel(){
		return contactModel.getLevel();
	}
	
	public void setVipLevel(int level){
		contactModel.setVipLevel(level);
	}
	
	public int getVipLevel(){
		return contactModel.getVipLevel();
	}
	public void setSelected(boolean bIsSelect){
		mIsSelect = bIsSelect;
	}
	
	public boolean isSelected(){
		return mIsSelect;
	}
}

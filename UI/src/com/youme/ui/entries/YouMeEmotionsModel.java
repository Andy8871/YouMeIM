package com.youme.ui.entries;

import java.util.List;


public class YouMeEmotionsModel {
	private List<Emotion> packages;  
	public void setEmotions(List<Emotion> packages){
		this.packages = packages;
	}
	
	public List<Emotion> getEmotions(){
		return packages;
	}
	
	
	public static class Emotion{
		private String name;
		private String icon;
		private String smallicon;
		private String title;
		private String zip;
		
		public void setName(String name){
			this.name = name;
		}
		
		public String getName(){
			return name;
		}
		
		public void setIcon(String icon){
			this.icon = icon;
		}
		
		public String getIcon(){
			return icon;
		}
		
		public void setSmallIcon(String smallicon){
			this.smallicon = smallicon;
		}
		
		public String getSamllIcon(){
			return smallicon;
		}
		
		public void setTitle(String title){
			this.title = title;
		}
		
		public String getTitle(){
			return title;
		}
		
		public void setZip(String zip){
			this.zip = zip;
		}
		
		public String getZip(){
			return zip;
		}
	}
}

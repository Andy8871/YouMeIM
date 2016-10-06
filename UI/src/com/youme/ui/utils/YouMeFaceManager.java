package com.youme.ui.utils;

import java.util.HashMap;

public class YouMeFaceManager {
	private static YouMeFaceManager instance;

	private YouMeFaceManager() {
		initFaceMap();
	}

	public static YouMeFaceManager getInstance() {
		if (null == instance)
			instance = new YouMeFaceManager();
		return instance;
	}

	private HashMap<String, String> faceHashMap = new HashMap<String, String>();

	private void initFaceMap() {
		faceHashMap.put("[/΢Ц]", "image/024.gif");
		faceHashMap.put("[/̾��]", "image/041.gif");
		faceHashMap.put("[/ɫ]", "image/020.gif");
		faceHashMap.put("[/����]", "image/044.gif");
		faceHashMap.put("[/��Ƥ]", "image/002.gif");
		faceHashMap.put("[/��ͷ]", "image/008.gif");
		faceHashMap.put("[/���]", "image/011.gif");
		faceHashMap.put("[/ί��]", "image/015.gif");
		faceHashMap.put("[/����]", "image/031.gif");
		faceHashMap.put("[/����]", "image/042.gif");
		faceHashMap.put("[/���]", "image/051.gif");
		faceHashMap.put("[/�]", "image/071.gif");
		faceHashMap.put("[/�����]", "image/089.gif");
	}
	
	public String getGifPath(String content){ 
		String filePath = faceHashMap.get(content);
		return filePath;
	}
}

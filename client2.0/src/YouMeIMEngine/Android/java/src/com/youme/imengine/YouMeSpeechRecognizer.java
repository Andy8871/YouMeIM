package com.youme.imengine;

import java.io.File;
import java.io.IOException;
import java.util.UUID;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONTokener;

import com.iflytek.cloud.InitListener;
import com.iflytek.cloud.RecognizerListener;
import com.iflytek.cloud.RecognizerResult;
import com.iflytek.cloud.SpeechConstant;
import com.iflytek.cloud.SpeechError;
import com.iflytek.cloud.SpeechRecognizer;
import com.iflytek.thridparty.k;

import android.content.Context;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.util.JsonToken;
import android.util.Log;

public class YouMeSpeechRecognizer implements InitListener, RecognizerListener, IMediaAudioRecordCallback {
	SpeechRecognizer m_SpeechRecognizer;
	static YouMeSpeechRecognizer s_instance;
	long m_ulCurSerial;
	String m_strWAVPath;
	String m_strCacheDir;
	Context m_Context;
	StringBuilder m_convetText = new StringBuilder();

	static SpeechMediaRecorder speechRecord;
	static boolean isSpeechOnly = false;
	
	public static YouMeSpeechRecognizer Instance()
	{
		if (s_instance == null) {
			s_instance = new YouMeSpeechRecognizer();
		}
		return s_instance;
	}
	
	public static void SetCacheDir(String path){
		if (path.length() == 0) {
			YouMeSpeechRecognizer.Instance().m_strCacheDir = YouMeSpeechRecognizer.Instance().m_Context.getCacheDir().getAbsolutePath() + "/";
		}else{
			File file =new File(path); 
			if (file.exists() && file.isDirectory()) {
				YouMeSpeechRecognizer.Instance().m_strCacheDir = path;
			}
		}
	}
	
	public void Init(Context context) {
		m_Context = context;
		m_SpeechRecognizer = SpeechRecognizer.createRecognizer(context, this);
		m_SpeechRecognizer.setParameter(SpeechConstant.ENGINE_TYPE, SpeechConstant.TYPE_CLOUD);
		m_SpeechRecognizer.setParameter(SpeechConstant.RESULT_TYPE,"json");
		m_SpeechRecognizer.setParameter(SpeechConstant.LANGUAGE,"zh_cn");
		m_SpeechRecognizer.setParameter(SpeechConstant.ACCENT,"mandarin");
		m_SpeechRecognizer.setParameter(SpeechConstant.VAD_BOS,"60000");
		m_SpeechRecognizer.setParameter(SpeechConstant.VAD_EOS,"60000");
		m_SpeechRecognizer.setParameter(SpeechConstant.ASR_PTT, "1");
		m_SpeechRecognizer.setParameter(SpeechConstant.AUDIO_FORMAT,"wav");
		m_SpeechRecognizer.setParameter(SpeechConstant.SAMPLE_RATE,"8000");
		
		speechRecord = new SpeechMediaRecorder();
		speechRecord.SetRecordCallback(this);
		
		SetCacheDir("");
	}

	@Override
	public void onInit(int errorcode) {
		// TODO Auto-generated method stub
		Log.i("YouMeSpeechRecognizer", "onInit:" + errorcode);
		NativeEngine.NotifySpeechFinish(m_ulCurSerial,0, errorcode, "","");
	}

	@Override
	public void onBeginOfSpeech() {
		// TODO Auto-generated method stub
		Log.i("YouMeSpeechRecognizer", "onBeginOfSpeech");
	}

	@Override
	public void onEndOfSpeech() {
		// TODO Auto-generated method stub
		Log.i("YouMeSpeechRecognizer", "onEndOfSpeech");
		
		
	}

	@Override
	public void onError(SpeechError arg0) {
		// TODO Auto-generated method stub
		Log.i("YouMeSpeechRecognizer", "onError:" + arg0);
		NativeEngine.NotifySpeechFinish(m_ulCurSerial,1, arg0.getErrorCode(), "","");
	
	}

	@Override
	public void onEvent(int arg0, int arg1, int arg2, Bundle arg3) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void onResult(RecognizerResult result, boolean bIsLast) {
		// TODO Auto-generated method stub
		String strResult = result.getResultString();
		JSONTokener token = new JSONTokener(strResult);
		try {
			JSONObject jsonObject = new JSONObject(token);
			JSONArray wordsArray = jsonObject.getJSONArray("ws");
			for(int i=0;i<wordsArray.length();i++)
			{
				JSONArray wordItems = wordsArray.getJSONObject(i).getJSONArray("cw");
				//获取第一个
				if (wordItems.length() < 1) {
					continue;
				}
				m_convetText.append(wordItems.getJSONObject(0).getString("w"));
			}
		} catch (JSONException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		if (bIsLast) {
			Log.i("YouMeSpeechRecognizer", "识别结束:" + m_convetText.toString());
			NativeEngine.NotifySpeechFinish(m_ulCurSerial, 1, 0, m_convetText.toString(),m_strWAVPath);
			
		}
	}

	@Override
	public void onVolumeChanged(int arg0, byte[] arg1) {
		// TODO Auto-generated method stub
		
	}

	//提供jni 函数用来给C++层调用
	public static int StartListening(long ulSerial)
	{
		if (YouMeSpeechRecognizer.Instance().m_SpeechRecognizer.isListening()) {
			Log.i("YouMeSpeechRecognizer", "当前已经开始PTT了");
			return -1;
		}
		isSpeechOnly = false;
		YouMeSpeechRecognizer.Instance().m_ulCurSerial = ulSerial;
		YouMeSpeechRecognizer.Instance().m_convetText = new StringBuilder();
		YouMeSpeechRecognizer.Instance().m_strWAVPath = YouMeSpeechRecognizer.Instance().m_strCacheDir + UUID.randomUUID().toString() + ".wav";
		YouMeSpeechRecognizer.Instance().m_SpeechRecognizer.setParameter(SpeechConstant.ASR_AUDIO_PATH, YouMeSpeechRecognizer.Instance().m_strWAVPath);
		return YouMeSpeechRecognizer.Instance().m_SpeechRecognizer.startListening(YouMeSpeechRecognizer.Instance());
	}
	public static int StartListeningOnlyAudio(long ulSerial)
	{	
		if (speechRecord != null && speechRecord.IsRecording()) {
			return -1;
		}
		isSpeechOnly = true;
		YouMeSpeechRecognizer.Instance().m_ulCurSerial = ulSerial;
		YouMeSpeechRecognizer.Instance().m_convetText = new StringBuilder();
		YouMeSpeechRecognizer.Instance().m_strWAVPath = YouMeSpeechRecognizer.Instance().m_strCacheDir + UUID.randomUUID().toString() + ".wav";
		return speechRecord.StartRecord(YouMeSpeechRecognizer.Instance().m_strWAVPath);
	}
	
	public void OnFinishRecord(IMediaAudioRecordCallback.RECORD_ERROR_CODE result, String path){
		if (result == IMediaAudioRecordCallback.RECORD_ERROR_CODE.ERRORCODE_SUCCESS) {
			NativeEngine.NotifySpeechFinish(m_ulCurSerial, 1, 0, "", path);
		} else {
			NativeEngine.NotifySpeechFinish(m_ulCurSerial, 1, -1, "", path);
		}
	}
	
	public static void StopListening() {
		if (isSpeechOnly) {
			if (speechRecord != null) {
				speechRecord.StopSpeech();
			}
		} else {
			if (!YouMeSpeechRecognizer.Instance().m_SpeechRecognizer.isListening()) {
				Log.i("YouMeSpeechRecognizer", "还没开始录音就调用停止了");
				return;
			}
			YouMeSpeechRecognizer.Instance().m_SpeechRecognizer.stopListening();
		}
	}
	
	public static void  CancleListening() {
		if (isSpeechOnly) {
			if (speechRecord != null) {
				speechRecord.CancelSpeech();
			}
		} else {
			if (!YouMeSpeechRecognizer.Instance().m_SpeechRecognizer.isListening()) {
				Log.i("YouMeSpeechRecognizer", "还没开始录音就调用取消了");
				return;
			} 
			YouMeSpeechRecognizer.Instance().m_SpeechRecognizer.cancel();
		}
		
	}
	public static void  PlayAudio(String strPath) {
		Log.i("YouMeSpeechRecognizer", "开始播放:" + strPath);
		
	}
}

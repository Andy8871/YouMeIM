package com.youme.imengine;

import java.io.File;
import android.media.MediaRecorder;
import android.media.MediaRecorder.OnErrorListener;
import android.media.MediaRecorder.OnInfoListener;
import android.util.Log;


interface IMediaAudioRecordCallback{
	public enum RECORD_ERROR_CODE{
	    ERRORCODE_SUCCESS,
	    ERRORCODE_STATUSERROR,
	    ERRORCODE_RECORDING,
	    ERRORCODE_FILENOTEXIST,
	    ERRORCODE_REACHMAXDURATION,
	    ERRORCODE_OTHER
	}
	
	public abstract void OnFinishRecord(RECORD_ERROR_CODE result, String path);
}

public class SpeechMediaRecorder implements OnInfoListener, OnErrorListener{
	private MediaRecorder recorder;
	private String recordPath;
	private boolean isRecording = false;
	private IMediaAudioRecordCallback callback = null;
	
	
	private void Init(){
		if (recorder == null) {
			recorder = new MediaRecorder();
		}
		
		try {
			recorder.setAudioSource(MediaRecorder.AudioSource.MIC);
			recorder.setOutputFormat(MediaRecorder.OutputFormat.AMR_NB);
			
			recorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);
			recorder.setAudioChannels(1);			//声道		
			recorder.setAudioSamplingRate(8000);	//采样率
			recorder.setAudioEncodingBitRate(16);	//采样位宽
			recorder.setMaxDuration(180000);		//最大录制时长（ms）
			
			recorder.setOnInfoListener(this);
			recorder.setOnErrorListener(this);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public void UnInit(){
		if (recorder != null) {
			recorder.release();
			recorder = null;
		}
	}
	
	public void SetRecordCallback(IMediaAudioRecordCallback callback){
		this.callback = callback;
	}
	
	public boolean IsRecording() {
		return isRecording;
	}
	
	@Override
	public void onInfo(MediaRecorder mr, int what, int extra) {
		String msg = String.format("onInfo what:%d extra:%d", what, extra);
		Log.e("SpeechMediaRecorder", msg);
		
		if (recorder != null) {
			isRecording = false;
			try {
				recorder.stop();
			} catch (IllegalStateException e) {
				Log.e("SpeechMediaRecorder", "recorder status error");
				e.printStackTrace();
			}
			if (callback != null) {
				if (MediaRecorder.MEDIA_RECORDER_INFO_MAX_DURATION_REACHED == what || MediaRecorder.MEDIA_RECORDER_INFO_MAX_FILESIZE_REACHED == what) {
					this.callback.OnFinishRecord(IMediaAudioRecordCallback.RECORD_ERROR_CODE.ERRORCODE_REACHMAXDURATION, recordPath);
				} else {
					this.callback.OnFinishRecord(IMediaAudioRecordCallback.RECORD_ERROR_CODE.ERRORCODE_OTHER, recordPath);
				}
			}
			recordPath = "";
		}
	}
	
	@Override
	public void onError(MediaRecorder mr, int what, int extra) {
		String msg = String.format("onError what:%d extra:%d", what, extra);
		Log.e("SpeechMediaRecorder", msg);
		
		isRecording = false;
		try {
			recorder.stop();
		} catch (IllegalStateException e) {
			Log.e("SpeechMediaRecorder", "recorder status error");
			e.printStackTrace();
		}
		if (callback != null) {
			this.callback.OnFinishRecord(IMediaAudioRecordCallback.RECORD_ERROR_CODE.ERRORCODE_OTHER, recordPath);
		}
		recordPath = "";
		if (what == MediaRecorder.MEDIA_ERROR_SERVER_DIED) {
			UnInit();
			Init();
		}
	}
	
	public int StartRecord(final String recordPath){
		Log.i("SpeechMediaRecorder", recordPath);
		if (isRecording) {
			return -1;
		}
		Init();
		
		try{
			this.recordPath = recordPath;
			isRecording = true;
			recorder.setOutputFile(recordPath);
			recorder.prepare();
			recorder.start();  
		} catch (Exception e) {
			e.printStackTrace();
			
			this.recordPath = "";
			isRecording = false;
			return -1;
		}
		
		return 0;
	}
	
	public void StopSpeech(){
		Log.i("SpeechMediaRecorder", "StopSpeech");
		if (!isRecording) {
			return;
		}
		isRecording = false;
		try {
			recorder.stop();
		} catch (IllegalStateException e) {
			Log.e("SpeechMediaRecorder", "recorder status error");
			e.printStackTrace();
			return;
		}
		if (recordPath.length() == 0) {
			return;
		}
		File file = new File(recordPath);
		if (file.exists()){
			this.callback.OnFinishRecord(IMediaAudioRecordCallback.RECORD_ERROR_CODE.ERRORCODE_SUCCESS, this.recordPath);
		} else {
			String msg = String.format("file not exist(%s)", this.recordPath);
			Log.e("SpeechMediaRecorder", msg);
			this.callback.OnFinishRecord(IMediaAudioRecordCallback.RECORD_ERROR_CODE.ERRORCODE_FILENOTEXIST, "");
		}
		recordPath = "";
	}
	
	public void CancelSpeech(){
		Log.i("SpeechMediaRecorder", "CancelSpeech");
		if (!isRecording) {
			return;
		}
		isRecording = false;
		try {
			recorder.stop();
		} catch (IllegalStateException e) {
			Log.e("SpeechMediaRecorder", "recorder status error");
			e.printStackTrace();
			return;
		}
		
		if (recordPath.length() == 0) {
			return;
		}
		File file = new File(this.recordPath);
		if (file.exists()){
			file.delete();
		}
		recordPath = "";
	}
}

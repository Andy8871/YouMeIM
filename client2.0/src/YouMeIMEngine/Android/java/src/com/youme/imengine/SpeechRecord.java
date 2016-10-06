package com.youme.imengine;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.util.Log;


interface IAudioRecordCallback{
	public abstract void OnFinishRecord(int result, String path);
}

class WAVHeadInfo{
	// RIFF chunk
	private byte riffID[] = {'R', 'I', 'F', 'F'};		//"RIFF"
	private int fileSize = 0;
	private byte riffFormat[] = {'W', 'A', 'V', 'E'};	//"WAVE"
	// format chunk
	private byte fmtID[] = {'f', 'm', 't', 0X20};		//"fmt "
	private int fmtSize = 16;
	private short formatTag = 0x0001;					//编码方式
	private short channels = 1;							//声道数
	private int sampleFrequency = 8000;					//采样频率
	private int byteRate = 0;							//比特率
	private short blockAlign = 0;						//块对齐大小
	private short sampleBitSize = 16;					//样本宽度
	// data chunk
	private byte dataID[] = {'d', 'a', 't', 'a'};		//"data"
	private int dataChunkSize = 0;
	
	public void SetAudioProperty(int sampleFrequency, int channel, int sampleBitSize){
		this.channels = (short) channel;
		this.sampleFrequency = sampleFrequency;
		this.sampleBitSize = (short) sampleBitSize;
		this.byteRate = sampleFrequency * channels * sampleBitSize / 8;
		this.blockAlign = (short) (channel * sampleBitSize / 8);
	}
	
	public void WriteHeadInfo(String recordPath){
		try{
			RandomAccessFile raf = new RandomAccessFile(recordPath, "rw");
			int length = (int) raf.length();
			this.fileSize = length - 8;
			this.dataChunkSize = length - 44;
			
			ByteBuffer byteBuf = ByteBuffer.allocate(44);
			//if (ByteOrder.nativeOrder() != ByteOrder.LITTLE_ENDIAN){
				byteBuf.order(ByteOrder.LITTLE_ENDIAN);
			//}
			byteBuf.put(riffID);
			byteBuf.putInt(fileSize);
			byteBuf.put(riffFormat);
			byteBuf.put(fmtID);
			byteBuf.putInt(fmtSize);
			byteBuf.putShort(formatTag);
			byteBuf.putShort(channels);
			byteBuf.putInt(sampleFrequency);
			byteBuf.putInt(byteRate);
			byteBuf.putShort(blockAlign);
			byteBuf.putShort(sampleBitSize);
			byteBuf.put(dataID);
			byteBuf.putInt(dataChunkSize);
			raf.write(byteBuf.array(), 0, 44);
			raf.close();
		}catch(Exception e){
			Log.e("SpeechRecord", "WriteHeadInfo Failed");
			e.printStackTrace();
		}
	}
}


class SpeechRecord{
	private int sampleRate = 8000;										// 采样频率
	private int channelConfig = AudioFormat.CHANNEL_IN_MONO;			// 声道
	private int audioFormat = AudioFormat.ENCODING_PCM_16BIT;			// 样本位宽
	private int recordBufferSize = 0;
	private boolean isRecording = false ;
	private boolean isInit = false;
	private boolean isCanceled = false;
	private String recordPath;
	private WAVHeadInfo wavHeadInfo;
	private AudioRecord audioRecord = null;
	private Thread recordThread;
	private IAudioRecordCallback callback;
	
	public SpeechRecord(){
		wavHeadInfo = new WAVHeadInfo();
		//wavHeadInfo.SetAudioProperty(16000, 1, 16);
	}
	
	/*
	 * frequency:采样频率
	 * channel:1-单声道	2-立体声
	 * sampleBitSize: 8 16
	 */
	public void Init(int frequency, int channel, int sampleBitSize){
		if (isInit) {
			return;
		}
		
		sampleRate = frequency;
		if (channel == 1){
			channelConfig = AudioFormat.CHANNEL_IN_MONO;
		}else if (channel == 2){
			channelConfig = AudioFormat.CHANNEL_IN_STEREO;
		}
		
		if (sampleBitSize == 8){
			audioFormat = AudioFormat.ENCODING_PCM_8BIT;
		}else if (sampleBitSize == 16){
			audioFormat = AudioFormat.ENCODING_PCM_16BIT;
		}else{
			audioFormat = AudioFormat.ENCODING_DEFAULT;
		}
		
		wavHeadInfo.SetAudioProperty(sampleRate, channel, sampleBitSize);
		recordBufferSize = AudioRecord.getMinBufferSize(sampleRate, channelConfig, audioFormat) * 10;
		isInit = true;
	}
	
	public void SetRecordCallback(IAudioRecordCallback callback){
		this.callback = callback;
	}
	
	public boolean IsRecording() {
		return isRecording;
	}
	
	public int StartRecord(final String recordPath){
		if(isRecording || (audioRecord != null && audioRecord.getRecordingState() == AudioRecord.RECORDSTATE_RECORDING)){
			return -1;
		}
		this.recordPath = recordPath;
		
		try {
			 audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, sampleRate, channelConfig, audioFormat, recordBufferSize);
		     if (audioRecord == null || audioRecord.getState() != AudioRecord.STATE_INITIALIZED || audioRecord.getRecordingState() != AudioRecord.RECORDSTATE_STOPPED) {
		    	 return -1;
		     }
		     audioRecord.startRecording();
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
			return -1;
		} catch (IllegalStateException e) {
			e.printStackTrace();
			return -1;
		}
		
        isRecording = true;  
        recordThread = new Thread(new Runnable() {  
            public void run() {  
                Record();  
            }  
        }, "AudioRecorder Thread");  
        recordThread.start();
        
        return 0;
	}
	
	public void StopSpeech(){
		isRecording = false;
		//audioRecord.stop();
		//audioRecord.release();
		recordThread = null;
	}
	
	public void CancelSpeech(){
		isRecording = false;
		isCanceled = true;
		recordThread = null;
	}
	
	private void Record() {
		byte data[] = new byte[recordBufferSize];
		File file = new File(recordPath);
		if (file.exists() == true){
			file.delete();
		}  
        FileOutputStream os = null;
        try {
            os = new FileOutputStream(file);
        } catch (FileNotFoundException e) { 
            e.printStackTrace();  
        }
        OutputStream bos = new BufferedOutputStream(os);
        DataOutputStream dos = new DataOutputStream(bos);
        
        if (null != dos) {
        	byte fillBytes[] = new byte[44];
        	try { 
        		dos.write(fillBytes);
            } catch (IOException e) {
                e.printStackTrace();  
            }
        	int readByte = 0;  
            while (isRecording) {
            	readByte = audioRecord.read(data, 0, recordBufferSize);  
                if (readByte > 0) {
                    try {
                    	dos.write(data);  
                    } catch (IOException e) {
                        e.printStackTrace();  
                    }  
                }
            }
            audioRecord.stop();
    		audioRecord.release();
  
            try {
            	dos.close();
            	bos.close();
                os.close();
            } catch (IOException e) {
                e.printStackTrace();  
            }
            
            if (isCanceled) {
				isCanceled = false;
				return;
			}
            
            wavHeadInfo.WriteHeadInfo(recordPath);
    	    this.callback.OnFinishRecord(0, recordPath);
        }
	}
	
	void Play(String path){
		File file = new File(path);
		if (file.exists() == false){
		  Log.e("SpeechRecord", "record fils not exist");
		  return;
		}
		InputStream is = null;
		try {
		  is = new FileInputStream(file);
		} catch (IOException e) {
		  e.printStackTrace();
		  Log.e("SpeechRecord", "play audio open file error");
		}

		BufferedInputStream bis = new BufferedInputStream(is);
		DataInputStream dis = new DataInputStream(bis);
		
		int bufferSize = 8192;
		byte buffer[] = new byte[bufferSize];
		AudioTrack audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, sampleRate, channelConfig, audioFormat, bufferSize, AudioTrack.MODE_STREAM);
		audioTrack.play();
		try {
			int readBytes = 0;
			while (dis.available() > 0) {
				readBytes = dis.read(buffer, 0, bufferSize);
				audioTrack.write(buffer, 0, readBytes);
			}
			dis.close();
			bis.close();
			is.close();
		} catch (IOException e) {
			e.printStackTrace();
			Log.e("SpeechRecord", "read file error");
		}

		audioTrack.stop();
		audioTrack.release();
	}
}
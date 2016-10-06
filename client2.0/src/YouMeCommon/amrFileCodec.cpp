//
//  amrFileCodec.cpp
//  amrDemoForiOS
//
//  Created by Tang Xiaoping on 9/27/11.
//  Copyright 2011 test. All rights reserved.
//
#include <YouMeCommon/XFile.h>
#include "amrFileCodec.h"
int amrEncodeMode[] = {4750, 5150, 5900, 6700, 7400, 7950, 10200, 12200}; // amr 编码方式
// 从WAVE文件中跳过WAVE文件头，直接到PCM音频数据
void SkipToPCMAudioData(youmecommon::CXFile& fpwave)
{
	RIFFHEADER riff;
	FMTBLOCK fmt;
	XCHUNKHEADER chunk;
	WAVEFORMATX wfx;
	int bDataBlock = 0;
	
	// 1. 读RIFF头
	fpwave.Read((byte*)&riff, sizeof(RIFFHEADER));
	// 2. 读FMT块 - 如果 fmt.nFmtSize>16 说明需要还有一个附属大小没有读
	fpwave.Read((byte*)&chunk, sizeof(XCHUNKHEADER));
	if ( chunk.nChunkSize>16 )
	{
		fpwave.Read((byte*)&wfx, sizeof(WAVEFORMATX));
	}
	else
	{
		memcpy(fmt.chFmtID, chunk.chChunkID, 4);
		fmt.nFmtSize = chunk.nChunkSize;
		fpwave.Read((byte*)&fmt.wf, sizeof(WAVEFORMAT));
	}
	
	// 3.转到data块 - 有些还有fact块等。
	while(!bDataBlock)
	{
		fpwave.Read((byte*)&chunk, sizeof(XCHUNKHEADER));
		if ( !memcmp(chunk.chChunkID, "data", 4) )
		{
			bDataBlock = 1;
			break;
		}
		// 因为这个不是data块,就跳过块数据
		fpwave.Seek(chunk.nChunkSize, SEEK_CUR);
	}
}

// 从WAVE文件读一个完整的PCM音频帧
// 返回值: 0-错误 >0: 完整帧大小
int ReadPCMFrame(short speech[], youmecommon::CXFile& fpwave, int nChannels, int nBitsPerSample)
{
	int nRead = 0;
	int x = 0, y=0;
	unsigned short ush1=0, ush2=0, ush=0;
	
	// 原始PCM音频帧数据
	unsigned char  pcmFrame_8b1[PCM_FRAME_SIZE];
	unsigned char  pcmFrame_8b2[PCM_FRAME_SIZE<<1];
	unsigned short pcmFrame_16b1[PCM_FRAME_SIZE];
	unsigned short pcmFrame_16b2[PCM_FRAME_SIZE<<1];
	
	if (nBitsPerSample==8 && nChannels==1)
	{
		nRead = fpwave.Read((byte*)pcmFrame_8b1, PCM_FRAME_SIZE*nChannels);
		for(x=0; x<PCM_FRAME_SIZE; x++)
		{
			speech[x] =(short)((short)pcmFrame_8b1[x] << 7);
		}
	}
	else
		if (nBitsPerSample==8 && nChannels==2)
		{
			nRead = fpwave.Read((byte*)pcmFrame_8b2, PCM_FRAME_SIZE*nChannels);
			for( x=0, y=0; y<PCM_FRAME_SIZE; y++,x+=2 )
			{
				// 1 - 取两个声道之左声道
				speech[y] =(short)((short)pcmFrame_8b2[x+0] << 7);
				// 2 - 取两个声道之右声道
				//speech[y] =(short)((short)pcmFrame_8b2[x+1] << 7);
				// 3 - 取两个声道的平均值
				//ush1 = (short)pcmFrame_8b2[x+0];
				//ush2 = (short)pcmFrame_8b2[x+1];
				//ush = (ush1 + ush2) >> 1;
				//speech[y] = (short)((short)ush << 7);
			}
		}
		else
			if (nBitsPerSample==16 && nChannels==1)
			{
				nRead = fpwave.Read((byte*)pcmFrame_16b1, PCM_FRAME_SIZE*nChannels *(nBitsPerSample / 8));
				for(x=0; x<PCM_FRAME_SIZE; x++)
				{
					speech[x] = (short)pcmFrame_16b1[x+0];
				}
			}
			else
				if (nBitsPerSample==16 && nChannels==2)
				{
					nRead = fpwave.Read((byte*)pcmFrame_16b2, PCM_FRAME_SIZE*nChannels *(nBitsPerSample / 8));
					for( x=0, y=0; y<PCM_FRAME_SIZE; y++,x+=2 )
					{
						//speech[y] = (short)pcmFrame_16b2[x+0];
						speech[y] = (short)((int)((int)pcmFrame_16b2[x+0] + (int)pcmFrame_16b2[x+1])) >> 1;
					}
				}
	
	// 如果读到的数据不是一个完整的PCM帧, 就返回0
	if (nRead<PCM_FRAME_SIZE*nChannels) return 0;
	
	return nRead;
}

// WAVE音频采样频率是8khz 
// 音频样本单元数 = 8000*0.02 = 160 (由采样频率决定)
// 声道数 1 : 160
//        2 : 160*2 = 320
// bps决定样本(sample)大小
// bps = 8 --> 8位 unsigned char
//       16 --> 16位 unsigned short
int EncodeWAVEFileToAMRFile(const XString& strWAVEFilename, const XString& strAMRFileName, int nChannels, int nBitsPerSample)
{
	
	/* input speech vector */
	short speech[160];
	
	/* counters */
	int byte_counter, frames = 0, bytes = 0;
	
	/* pointer to encoder state structure */
	void *enstate;
	
	/* requested mode */
	enum Mode req_mode = MR122;
	int dtx = 0;
	
	/* bitstream filetype */
	unsigned char amrFrame[MAX_AMR_FRAME_SIZE];
	youmecommon::CXFile  fpwave;
	if (0 != fpwave.LoadFile(strWAVEFilename, youmecommon::CXFile::Mode_OpenExist_ReadOnly))
	{
		return 0;
	}
	youmecommon::CXFile fpamr;
	if (0 != fpamr.LoadFile(strAMRFileName, youmecommon::CXFile::Mode_CREATE_ALWAYS))
	{
		return 0;
	}
	
	/* write magic number to indicate single channel AMR file storage format */
	bytes = fpamr.Write((const byte*)AMR_MAGIC_NUMBER, strlen(AMR_MAGIC_NUMBER));
	
	/* skip to pcm audio data*/
	SkipToPCMAudioData(fpwave);
	
	enstate = Encoder_Interface_init(dtx);
	
	while(1)
	{
		// read one pcm frame
		if (!ReadPCMFrame(speech, fpwave, nChannels, nBitsPerSample)) break;
		
		frames++;
		
		/* call encoder */
		byte_counter = Encoder_Interface_Encode(enstate, req_mode, speech, amrFrame, 0);
		
		bytes += byte_counter;

		fpamr.Write(amrFrame, byte_counter);
	}
	
	Encoder_Interface_exit(enstate);
	
	
	return frames;
}




//#pragma mark - Decode
//decode
void WriteWAVEFileHeader(youmecommon::CXFile& fpwave, int nFrame)
{
	char tag[10] = "";
	
	// 1. 写RIFF头
	RIFFHEADER riff;
	strcpy(tag, "RIFF");
	memcpy(riff.chRiffID, tag, 4);
	riff.nRiffSize = 4                                     // WAVE
	+ sizeof(XCHUNKHEADER)               // fmt 
	+ sizeof(WAVEFORMATX)           // WAVEFORMATX
	+ sizeof(XCHUNKHEADER)               // DATA
	+ nFrame*160*sizeof(short);    // 
	strcpy(tag, "WAVE");
	memcpy(riff.chRiffFormat, tag, 4);
	fpwave.Write((const byte*)&riff, sizeof(RIFFHEADER));
	// 2. 写FMT块
	XCHUNKHEADER chunk;
	WAVEFORMATX wfx;
	strcpy(tag, "fmt ");
	memcpy(chunk.chChunkID, tag, 4);
	chunk.nChunkSize = sizeof(WAVEFORMATX);

	fpwave.Write((const byte*)&chunk, sizeof(XCHUNKHEADER));
	memset(&wfx, 0, sizeof(WAVEFORMATX));
	wfx.nFormatTag = 1;
	wfx.nChannels = 1; // 单声道
	wfx.nSamplesPerSec = 8000; // 8khz
	wfx.nAvgBytesPerSec = 16000;
	wfx.nBlockAlign = 2;
	wfx.nBitsPerSample = 16; // 16位
	fpwave.Write((const byte*)&wfx, sizeof(WAVEFORMATX));
	// 3. 写data块头
	strcpy(tag, "data");
	memcpy(chunk.chChunkID, tag, 4);
	chunk.nChunkSize = nFrame*160*sizeof(short);
	fpwave.Write((const byte*)&chunk, sizeof(XCHUNKHEADER));
}

const int myround(const double x)
{
	return((int)(x+0.5));
} 

// 根据帧头计算当前帧大小
int caclAMRFrameSize(unsigned char frameHeader)
{
	int mode;
	int temp1 = 0;
	int temp2 = 0;
	int frameSize;
	
	temp1 = frameHeader;
	
	// 编码方式编号 = 帧头的3-6位
	temp1 &= 0x78; // 0111-1000
	temp1 >>= 3;
	
	mode = amrEncodeMode[temp1];
	
	// 计算amr音频数据帧大小
	// 原理: amr 一帧对应20ms，那么一秒有50帧的音频数据
	temp2 = myround((double)(((double)mode / (double)AMR_FRAME_COUNT_PER_SECOND) / (double)8));
	
	frameSize = myround((double)temp2 + 0.5);
	return frameSize;
}

// 读第一个帧 - (参考帧)
// 返回值: 0-出错; 1-正确
int ReadAMRFrameFirst(youmecommon::CXFile& fpamr, unsigned char frameBuffer[], int* stdFrameSize, unsigned char* stdFrameHeader)
{
	memset(frameBuffer, 0, sizeof(frameBuffer));
	
	// 先读帧头
	fpamr.Read(stdFrameHeader, sizeof(unsigned char));
	if (fpamr.IsEnd()) return 0;
	
	// 根据帧头计算帧大小
	*stdFrameSize = caclAMRFrameSize(*stdFrameHeader);
	
	// 读首帧
	frameBuffer[0] = *stdFrameHeader;

	fpamr.Read(&(frameBuffer[1]), (*stdFrameSize - 1)*sizeof(unsigned char));
	if (fpamr.IsEnd()) return 0;
	
	return 1;
}

// 返回值: 0-出错; 1-正确
int ReadAMRFrame(youmecommon::CXFile& fpamr, unsigned char frameBuffer[], int stdFrameSize, unsigned char stdFrameHeader)
{
	int bytes = 0;
	unsigned char frameHeader; // 帧头
	
	memset(frameBuffer, 0, sizeof(frameBuffer));
	
	// 读帧头
	// 如果是坏帧(不是标准帧头)，则继续读下一个字节，直到读到标准帧头
	while(1)
	{
		bytes = fpamr.Read((byte*)&frameHeader, sizeof(unsigned char));
		if (fpamr.IsEnd()) return 0;
		if (frameHeader == stdFrameHeader) break;
	}
	
	// 读该帧的语音数据(帧头已经读过)
	frameBuffer[0] = frameHeader;
	bytes = fpamr.Read((byte*)&(frameBuffer[1]), (stdFrameSize - 1)*sizeof(unsigned char));
	if (fpamr.IsEnd()) return 0;
	
	return 1;
}

// 将AMR文件解码成WAVE文件
int DecodeAMRFileToWAVEFile(const XString& strAMRFileName, const XString& strWAVEFilename)
{
	char magic[8];
	void * destate;
	int nFrameCount = 0;
	int stdFrameSize;
	unsigned char stdFrameHeader;
	
	unsigned char amrFrame[MAX_AMR_FRAME_SIZE];
	short pcmFrame[PCM_FRAME_SIZE];
	
	//NSString * path = [[NSBundle mainBundle] pathForResource:  @"test" ofType: @"amr"]; 
	youmecommon::CXFile fpamr;
	if (0 != fpamr.LoadFile(strAMRFileName, youmecommon::CXFile::Mode_OpenExist_ReadOnly))
	{
		return 0;
	}
	
	// 检查amr文件头
	fpamr.Read((byte*)magic, strlen(AMR_MAGIC_NUMBER));
	if (strncmp(magic, AMR_MAGIC_NUMBER, strlen(AMR_MAGIC_NUMBER)))
	{
		return 0;
	}
	
	// 创建并初始化WAVE文件
//	NSArray *paths               = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
//	NSString *documentPath       = [paths objectAtIndex:0];
//	NSString *docFilePath        = [documentPath stringByAppendingPathComponent:[NSString stringWithFormat:@"%s", pchWAVEFilename]];
//	NSLog(@"documentPath=%@", documentPath);
	
//	fpwave = fopen([docFilePath cStringUsingEncoding:NSASCIIStringEncoding], "wb");
//	WriteWAVEFileHeader(fpwave, nFrameCount);

	youmecommon::CXFile fpwave;
	if (0 != fpwave.LoadFile(strWAVEFilename, youmecommon::CXFile::Mode_CREATE_ALWAYS))
	{
		return 0;
	}
	WriteWAVEFileHeader(fpwave, nFrameCount);

    
	/* init decoder */
	destate = Decoder_Interface_init();
	
	// 读第一帧 - 作为参考帧
	memset(amrFrame, 0, sizeof(amrFrame));
	memset(pcmFrame, 0, sizeof(pcmFrame));
	ReadAMRFrameFirst(fpamr, amrFrame, &stdFrameSize, &stdFrameHeader);
	
	// 解码一个AMR音频帧成PCM数据
	Decoder_Interface_Decode(destate, amrFrame, pcmFrame, 0);
	nFrameCount++;
	fpwave.Write((const byte*)pcmFrame, PCM_FRAME_SIZE*sizeof(short));
	// 逐帧解码AMR并写到WAVE文件里
	while(1)
	{
		memset(amrFrame, 0, sizeof(amrFrame));
		memset(pcmFrame, 0, sizeof(pcmFrame));
		if (!ReadAMRFrame(fpamr, amrFrame, stdFrameSize, stdFrameHeader)) break;
		
		// 解码一个AMR音频帧成PCM数据 (8k-16b-单声道)
		Decoder_Interface_Decode(destate, amrFrame, pcmFrame, 0);
		nFrameCount++;

		fpwave.Write((const byte*)pcmFrame, PCM_FRAME_SIZE*sizeof(short));
	}
//	NSLog(@"frame = %d", nFrameCount);
	Decoder_Interface_exit(destate);

	
	// 重写WAVE文件头
	fpwave.Seek(0, SEEK_SET);	
	WriteWAVEFileHeader(fpwave, nFrameCount);

	return nFrameCount;
}
//
//  IATSpeech.m
//  YouMeIM
//
//  Created by 智璋 谢 on 16/6/27.
//
//

#import <Foundation/Foundation.h>
#import "IATSpeech.h"
#import <YouMeCommon/YMApplication.hpp>
#include <YouMeCommon/Log.h>
#include <YouMeIMEngine/Speech/SpeechUtil.h>
#include <YouMeIMEngine/YouMeInterface.h>
@implementation IATSpeech
#pragma mark - IFlySpeechRecognizerDelegate
#pragma mark - IAudioReordDelegate

#define SAMPLERATE 8000
#define CHANNEL_NUMBER 1
#define SAMPLE_BIT_SIZE 16

static IATSpeech *SpeechsharedInstance = nil;
ISpeechListen* g_pListen = nullptr;
+ (IATSpeech *)getInstance
{
    @synchronized (self)
    {
        if (SpeechsharedInstance == nil)
        {
            SpeechsharedInstance = [self alloc];
        }
    }
    
    return SpeechsharedInstance;
}

-(void) SetCacheDir:(NSString*) path
{
    if ([path length] == 0) {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        self.m_cacheDir = [paths objectAtIndex:0];
    } else {
        NSFileManager *fileManager = [NSFileManager defaultManager];
        if (fileManager != nil && [fileManager fileExistsAtPath:path]) {
            self.m_cacheDir = path;
        }
    }
}

-(int) StartOnlySpeech:(long long)Serial
{
    if (self.m_audioRecord == nil) {
        self.m_audioRecord = [[AudioRecord alloc] init];
        [self.m_audioRecord setRecrodDelegate:self];
    }
    if ([self.m_audioRecord IsRecording] == YES) {
        return -1;
    }
    
    m_ulCurSerial = Serial;
    [self.m_curResult setString:@""];
    m_bSpeechOnly = true;
    
    NSString * strFileName = [NSString stringWithFormat:@"%d.pcm",arc4random()];
    self.m_pcmPath = [[NSString alloc] initWithFormat:@"%@",[self.m_cacheDir stringByAppendingPathComponent:strFileName]];
    
    [self.m_audioRecord StartRecord:self.m_pcmPath];
    
    return 0;
}

- (void) OnRecordFinish:(BOOL)result path:(NSString*)path{
    if (result == YES) {
        //把PCM 转换成wav然后上传
        std::string strWavPath= [path UTF8String];
        strWavPath += ".wav";
        bool bSuccess = SpeechUtil::PcmToWav([path UTF8String], SAMPLE_BIT_SIZE, CHANNEL_NUMBER, SAMPLERATE, 1, strWavPath);
        if (bSuccess) {
            g_pListen->OnSpeechResult(m_ulCurSerial, YouMeIMErrorcode_Success, "", strWavPath);
            NSFileManager *fileManager = [NSFileManager defaultManager];
            if (fileManager) {
                [fileManager removeItemAtPath:path error:nil];
            }
        } else {
            YouMe_LOG_Error(__XT("pcm 转wav 失败了:%s->%s"),[path UTF8String],strWavPath.c_str());
            g_pListen->OnSpeechResult(m_ulCurSerial, YouMeIMErrorcode_PTT_Fail, "", "");
        }
    }
}

-(int) StartSpeech:(long long)Serial
{
    YouMe_LOG_Debug("进入startspeech 函数:%lld",Serial);
    BOOL bIsListen = [_m_iFlySpeechRecognizer isListening];
    if (bIsListen) {
        YouMe_LOG_Debug("已经开始录音了，直接退出");
        return  -1;
    }
    m_ulCurSerial = Serial;
    [self.m_curResult setString:@""];
    m_bSpeechOnly = false;
    
    //保存的文件名
    NSString * strFileName = [NSString stringWithFormat:@"%d.pcm",arc4random()];
     [_m_iFlySpeechRecognizer setParameter:strFileName forKey:[IFlySpeechConstant ASR_AUDIO_PATH]];
    self.m_pcmPath = [[NSString alloc] initWithFormat:@"%@",[self.m_cacheDir stringByAppendingPathComponent:strFileName]];

    if (YES == [_m_iFlySpeechRecognizer startListening])
    {
        return 0;
    }
    return -1;
}
-(void) StopSpeech
{
    if (m_bSpeechOnly) {
        if (self.m_audioRecord != nil) {
            [self.m_audioRecord StopRecord];
        }
    } else {
        BOOL bIsListen = [_m_iFlySpeechRecognizer isListening];
        if (!bIsListen) {
            YouMe_LOG_Info("还没开始录音就调用了停止");
            return;
        }
        
        [_m_iFlySpeechRecognizer stopListening];
    }
}
-(void)SetListen:(ISpeechListen*) listen
{
    g_pListen = listen;
}
-(void) CancleSpeech
{
    if (m_bSpeechOnly) {
        if (self.m_audioRecord != nil) {
            [self.m_audioRecord CancelRecord];
        }
    } else {
        BOOL bIsListen = [_m_iFlySpeechRecognizer isListening];
        if (!bIsListen) {
            NSLog(@"还没开始录音就调用了取消");
            return  ;
        }
        
        [_m_iFlySpeechRecognizer cancel];
    }
}

-(void)Init
{
    if (_m_iFlySpeechRecognizer != nil) {
        return;
    }
    
    //创建语音配置,appid必须要传入，仅执行一次则可
    NSString *initString = [[NSString alloc] initWithFormat:@"appid=%@",@"57555017"];
    
    //所有服务启动前，需要确保执行createUtility
    [IFlySpeechUtility createUtility:initString];
    
    
    self.m_curResult = [[NSMutableString alloc]init];
    _m_iFlySpeechRecognizer = [IFlySpeechRecognizer sharedInstance];
    [_m_iFlySpeechRecognizer setParameter:[IFlySpeechConstant TYPE_CLOUD] forKey:[IFlySpeechConstant ENGINE_TYPE]];
    
    [_m_iFlySpeechRecognizer setParameter:@"json" forKey:[IFlySpeechConstant RESULT_TYPE]];
    [_m_iFlySpeechRecognizer setParameter:@"mandarin" forKey:[IFlySpeechConstant ACCENT]];
    [_m_iFlySpeechRecognizer setParameter:@"60000" forKey:[IFlySpeechConstant VAD_BOS]];
    [_m_iFlySpeechRecognizer setParameter:@"60000" forKey:[IFlySpeechConstant VAD_EOS]];
    [_m_iFlySpeechRecognizer setParameter:@"1" forKey:[IFlySpeechConstant ASR_PTT]];
    [_m_iFlySpeechRecognizer setParameter:@"iat" forKey: [IFlySpeechConstant IFLY_DOMAIN]];
    [_m_iFlySpeechRecognizer setParameter:[NSString stringWithFormat:@"%d", SAMPLERATE] forKey:[IFlySpeechConstant SAMPLE_RATE]];
    [_m_iFlySpeechRecognizer setDelegate:self];
   // [_m_iFlySpeechRecognizer setParameter:@"wav" forKey:[IFlySpeechConstant AUDIO_FORMAT]];
    
    [self SetCacheDir:@""];
    
    m_bSpeechOnly = false;
    m_bGetSpeechResult = false;
}
/**
 音量回调函数
 volume 0－30
 ****/
- (void) onVolumeChanged: (int)volume
{
    }



/**
 开始识别回调
 ****/
- (void) onBeginOfSpeech
{
    NSLog(@"onBeginOfSpeech");
    m_bGetSpeechResult = false;
}

/**
 停止录音回调
 ****/
- (void) onEndOfSpeech
{
    NSLog(@"onEndOfSpeech");
}


/**
 听写结束回调（注：无论听写是否正确都会回调）
 error.errorCode =
 0     听写正确
 other 听写出错
 ****/
- (void) onError:(IFlySpeechError *) error
{
    if (!m_bGetSpeechResult && g_pListen != nullptr && [error errorCode] != 0) {
        g_pListen->OnSpeechResult(m_ulCurSerial, YouMeIMErrorcode_PTT_NotSpeech, "", "");
        NSLog(@"speech error(%d)", [error errorCode]);
    }
    
}

+ (NSString *)stringFromJson:(NSString*)params
{
    if (params == NULL) {
        return nil;
    }
    
    NSMutableString *tempStr = [[NSMutableString alloc] init];
    NSDictionary *resultDic  = [NSJSONSerialization JSONObjectWithData:    //返回的格式必须为utf8的,否则发生未知错误
                                [params dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:nil];
    
    if (resultDic!= nil) {
        NSArray *wordArray = [resultDic objectForKey:@"ws"];
        
        for (int i = 0; i < [wordArray count]; i++) {
            NSDictionary *wsDic = [wordArray objectAtIndex: i];
            NSArray *cwArray = [wsDic objectForKey:@"cw"];
            
            for (int j = 0; j < [cwArray count]; j++) {
                NSDictionary *wDic = [cwArray objectAtIndex:j];
                NSString *str = [wDic objectForKey:@"w"];
                [tempStr appendString: str];
            }
        }
    }
    return tempStr;
}
/**
 无界面，听写结果回调
 results：听写结果
 isLast：表示最后一次
 ****/
- (void) onResults:(NSArray *) results isLast:(BOOL)isLast
{
    m_bGetSpeechResult = true;
    NSMutableString *result = [[NSMutableString alloc] init];
    NSMutableString * resultString = [[NSMutableString alloc]init];
    NSDictionary *dic = results[0];
    
    for (NSString *key in dic) {
        
        [result appendFormat:@"%@",key];
        
        NSString * resultFromJson = [IATSpeech stringFromJson:result];
        [resultString appendString:resultFromJson];
        
    }
    [self.m_curResult appendString:resultString];
    if (isLast) {
        NSLog(@"%@",self.m_curResult);
        if (NULL != g_pListen) {
            //把PCM 转换成wav然后上传
            std::string strWavPath= [self.m_pcmPath UTF8String];
            strWavPath += ".wav";
           bool bSuccess = SpeechUtil::PcmToWav([self.m_pcmPath UTF8String], SAMPLE_BIT_SIZE, CHANNEL_NUMBER, SAMPLERATE, 0, strWavPath);
            if (!bSuccess) {
                YouMe_LOG_Error(__XT("pcm 转wav 失败了:%s->%s"),[self.m_pcmPath UTF8String],strWavPath.c_str());
                g_pListen->OnSpeechResult(m_ulCurSerial, YouMeIMErrorcode_PTT_Fail, "", "");
            } else {
                g_pListen->OnSpeechResult(m_ulCurSerial, YouMeIMErrorcode_Success, [self.m_curResult UTF8String], strWavPath);
                NSFileManager *fileManager = [NSFileManager defaultManager];
                if (fileManager) {
                    [fileManager removeItemAtPath:self.m_pcmPath error:nil];
                }
            }
        }
    }
    
}


/**
 听写取消回调
 ****/
- (void) onCancel
{
    NSLog(@"识别取消");
}
@end

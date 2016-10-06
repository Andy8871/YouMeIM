#include "AudioRecord.h"
#include <YouMeCommon/Log.h>
@implementation AudioRecord
{
    bool isRecording;
    bool isCanceled;
}

-(id) init{
    if (self == [super init]) {
        self.audioSettingDic = [[NSMutableDictionary alloc] init];
        [self.audioSettingDic setValue:[NSNumber numberWithInt:kAudioFormatLinearPCM] forKey:AVFormatIDKey];  // 录音格式
        [self.audioSettingDic setValue:[NSNumber numberWithInt:8000] forKey:AVSampleRateKey];                 // 采样率
        [self.audioSettingDic setValue:[NSNumber numberWithInt:1] forKey:AVNumberOfChannelsKey];              // 声道
        [self.audioSettingDic setValue:[NSNumber numberWithInt:16] forKey:AVLinearPCMBitDepthKey];            // 样本位数
        [self.audioSettingDic setValue:[NSNumber numberWithBool:NO] forKey:AVLinearPCMIsFloatKey];            // 是否使用浮点数采样
        [self.audioSettingDic setValue:[NSNumber numberWithBool:NO] forKey:AVLinearPCMIsBigEndianKey];
        
        [self SetAudioSession];
        isRecording = false;
        isCanceled = false;
    }
    return self;
}

-(void) dealloc{
    [self.audioSettingDic release];
    [self.audioRecorder release];
    [self.audioPlayer release];
    [super dealloc];
}

-(void) SetAudioSetting:(int)sampleRate channel:(int)channel sampleBiteSize:(int)sampleBitSize{
    [self.audioSettingDic setObject:@(sampleRate) forKey:AVSampleRateKey];
    [self.audioSettingDic setObject:@(channel) forKey:AVNumberOfChannelsKey];
    [self.audioSettingDic setObject:@(sampleRate) forKey:AVLinearPCMBitDepthKey];
}

-(void) SetDelegate : (id) delegateObj{
    _recrodDelegate = delegateObj;
}

-(void) SetAudioSession{
    AVAudioSession *audioSession=[AVAudioSession sharedInstance];
    //[audioSession setCategory:AVAudioSessionCategoryPlayAndRecord error:nil];
    [audioSession setCategory:AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker error:nil];
    [audioSession setActive:YES error:nil];
}

-(void) StartRecord:(NSString*) path{
    if ([path length] == 0) {
        return;
    }
    NSFileManager *fileManager = [NSFileManager defaultManager];
    if (fileManager && [fileManager fileExistsAtPath:path]) {
        [fileManager removeItemAtPath:path error:nil];
    }
    
    self.recordPath = path;
    NSError *error = nil;
    NSURL *urlAudio = [NSURL fileURLWithPath:path];
    self.audioRecorder = [[AVAudioRecorder alloc]initWithURL:urlAudio settings:self.audioSettingDic error:&error];
    self.audioRecorder.delegate = self;
    //_audioRecorder.meteringEnabled = YES; // 监控声波则为YES
    if (error) {
        YouMe_LOG_Error(__XT("create audio record failed, error：%s"), error.localizedDescription.UTF8String);
    }
    [self.audioRecorder prepareToRecord];
    BOOL bRet = [self.audioRecorder record];
    if (bRet == NO) {
        YouMe_LOG_Error(__XT("record audio error"));
    }
    isRecording = true;
}

-(void) StopRecord{
    if (self.audioRecorder) {
        [self.audioRecorder stop];
    }
}

-(void) CancelRecord{
	isCanceled = true;
    if (self.audioRecorder) {
        [self.audioRecorder stop];
    }
}

-(BOOL) IsRecording{
    if (self.audioRecorder) {
        if (self.audioRecorder.recording == YES || isRecording) {
            return YES;
        } else {
            return NO;
        }
    }
    return NO;
}

-(void)audioRecorderDidFinishRecording:(AVAudioRecorder *)recorder successfully:(BOOL)flag{
    if (isCanceled) {
        isCanceled = false;
        if (self.audioRecorder != nil) {
            [self.audioRecorder deleteRecording];
        }
    } else {
        if (self.recrodDelegate != NULL) {
            [self.recrodDelegate OnRecordFinish:flag path:self.recordPath];
        }
    }
    
    AVAudioSession *audioSession=[AVAudioSession sharedInstance];
    [audioSession setCategory:AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker error:nil];
    if (self.audioRecorder != nil) {
        [self.audioRecorder release];
        self.audioRecorder = nil;
    }
    isRecording = false;
}

- (void)audioPlayerDidFinishPlaying:(AVAudioPlayer *)player successfully:(BOOL)flag{
    if (self.audioPlayer != nil) {
        [self.audioPlayer release];
        self.audioPlayer = nil;
    }
}

- (void)audioRecorderEncodeErrorDidOccur:(AVAudioRecorder *)recorder error:(NSError * __nullable)error{
    isRecording = false;
    isCanceled = false;
    if (self.recrodDelegate != NULL) {
        [self.recrodDelegate OnRecordFinish:NO path:@""];
    }
}

-(void) PlayAudio:(NSString*)path{
    if ([path length] == 0 || ![[NSFileManager defaultManager] fileExistsAtPath:path]) {
        YouMe_LOG_Error(__XT("file not exist:%s"), path.UTF8String);
    }
    NSURL *url = [NSURL fileURLWithPath:path];
    NSError *error = nil;
    self.audioPlayer = [[AVAudioPlayer alloc]initWithContentsOfURL:url error:&error];
    self.audioPlayer.numberOfLoops = 0;
    [self.audioPlayer prepareToPlay];
    if (error) {
        YouMe_LOG_Error(__XT("create audio player failed,error：%s"), error.localizedDescription.UTF8String);
        return;
    }
    
    [self.audioPlayer play];
}

@end
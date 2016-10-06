#import <AVFoundation/AVFoundation.h>

@protocol IAudioReordDelegate <NSObject>

- (void) OnRecordFinish:(BOOL)result path:(NSString*)path;

@end


@interface AudioRecord : NSObject<AVAudioRecorderDelegate, AVAudioPlayerDelegate>

-(void) SetAudioSetting:(int)sampleRate channel:(int)channel sampleBiteSize:(int)sampleBitSize;
-(void) StartRecord:(NSString*)path;
-(void) StopRecord;
-(void) CancelRecord;
-(BOOL) IsRecording;
-(void) PlayAudio:(NSString*)path;

@property (nonatomic, assign) id<IAudioReordDelegate> recrodDelegate;
@property (nonatomic, strong) NSMutableDictionary *audioSettingDic;
@property (nonatomic, strong) AVAudioRecorder *audioRecorder;
@property (nonatomic, strong) AVAudioPlayer *audioPlayer;
@property (nonatomic, copy) NSString *recordPath;

@end
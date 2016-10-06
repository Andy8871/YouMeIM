//
//  IATSpeech.m
//  YouMeIM
//
//  Created by 智璋 谢 on 16/6/22.
//
//

#import <Foundation/Foundation.h>
#import <CoreFoundation/CoreFoundation.h>
#include <YouMeIMEngine/Speech/SpeechListenManager.h>
#import <iflyMSC.framework/Headers/IFlyMSC.h>
#import "AudioRecord.h"


@interface IATSpeech :NSObject<IFlySpeechRecognizerDelegate, IAudioReordDelegate>
{
    long m_ulCurSerial;
    bool m_bSpeechOnly;
    bool m_bGetSpeechResult;
}

+ (IATSpeech *)getInstance;

-(void)Init;
-(void) SetCacheDir:(NSString*) path;

-(void)SetListen:(ISpeechListen*) listen;
-(int) StartSpeech:(long long)Serial;
-(int) StartOnlySpeech:(long long)Serial;

-(void) StopSpeech;
-(void) CancleSpeech;


@property (nonatomic, strong) NSMutableString *m_curResult;//当前session的结果
@property (nonatomic, strong) NSString *m_pcmPath;//当前session的结果
+ (NSString *)stringFromJson:(NSString*)params;//
@property (nonatomic, strong) NSString *m_cacheDir;
@property (nonatomic, strong) IFlySpeechRecognizer *m_iFlySpeechRecognizer;//不带界面的识别对象
@property (nonatomic, strong) AudioRecord *m_audioRecord;
@end

#import <CoreTelephony/CTTelephonyNetworkInfo.h>
#import "NetworkService.h"
#import "ReachabilityService.H"
#import "Reachability.h"

static YouMeCommonReachabilityService *sharedInstance = nil;

@interface YouMeCommonReachabilityService ()

@property (nonatomic) YouMeCommonReachability *hostReachability;
@property (nonatomic) YouMeCommonReachability *internetReachability;
@property (nonatomic) YouMeCommonReachability *wifiReachability;
@property (atomic) youmecommon::INgnNetworkChangCallback *mCallback;

@end


@implementation YouMeCommonReachabilityService

+ (YouMeCommonReachabilityService *)getInstance
{
    @synchronized (self)
    {
        if (sharedInstance == nil)
        {
            sharedInstance = [self alloc];
        }
    }

    return sharedInstance;
}

+ (void)destroy
{
    sharedInstance = nil;
}
- (id)init
{
    self = [super init];
    return self;
}
- (void)uninit
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    self.mCallback=nil;
}
- (void)start:(youmecommon::INgnNetworkChangCallback *)networkChangeCallback
{
    if (self)
    {
        self.mCallback = networkChangeCallback;


        /*
         Observe the kNetworkReachabilityChangedNotification. When that notification is posted, the
         method reachabilityChanged will be called.
         */
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector (reachabilityChanged:) name:kYouMeCommonReachabilityChangedNotification object:nil];

        // Change the host name here to change the server you want to monitor.
        self.hostReachability = [YouMeCommonReachability reachabilityWithHostName];
        [self.hostReachability startNotifier];
        [self updateInterfaceWithReachability:self.hostReachability];

        self.internetReachability = [YouMeCommonReachability reachabilityForInternetConnection];
        [self.internetReachability startNotifier];
        [self updateInterfaceWithReachability:self.internetReachability];

        self.wifiReachability = [YouMeCommonReachability reachabilityForLocalWiFi];
        [self.wifiReachability startNotifier];
        [self updateInterfaceWithReachability:self.wifiReachability];
    }
    return;
}


/*!
 * Called by Reachability whenever status changes.
 */
- (void)reachabilityChanged:(NSNotification *)note
{
    YouMeCommonReachability *curReach = [note object];
    NSParameterAssert ([curReach isKindOfClass:[YouMeCommonReachability class]]);
    [self updateInterfaceWithReachability:curReach];
}


- (void)updateInterfaceWithReachability:(YouMeCommonReachability *)reachability
{

    if (self.mCallback == nil)
    {
        return;
    }


    NetworkStatus netStatus = [reachability currentReachabilityStatus];
    BOOL connectionRequired = [reachability connectionRequired];

    switch (netStatus)
    {
    case NotReachable:
    {
        connectionRequired = NO;
        self.mCallback->onNetWorkChanged (youmecommon::NetworkType_Unknow);
        break;
    }

    case ReachableViaWWAN:
    {
        self.mCallback->onNetWorkChanged (youmecommon::NetworkType_3G);
        break;
    }
    case ReachableViaWiFi:
    {

        self.mCallback->onNetWorkChanged (youmecommon::NetworkType_Wifi);
        break;
    }
    default:
        self.mCallback->onNetWorkChanged (youmecommon::NetworkType_Unknow);
    }
}



@end

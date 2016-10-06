#import <CoreTelephony/CTTelephonyNetworkInfo.h>
#import "NetworkService.h"
#import "ReachabilityService.H"
#import "Reachability.h"

static YouMeReachabilityService *sharedInstance = nil;

@interface YouMeReachabilityService ()

@property (nonatomic) YouMeReachability *hostReachability;
@property (nonatomic) YouMeReachability *internetReachability;
@property (nonatomic) YouMeReachability *wifiReachability;
@property (atomic) youmecommon::INgnNetworkChangCallback *mCallback;

@end


@implementation YouMeReachabilityService

+ (YouMeReachabilityService *)getInstance
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
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector (reachabilityChanged:) name:kYouMeReachabilityChangedNotification object:nil];

        // Change the host name here to change the server you want to monitor.
        self.hostReachability = [YouMeReachability reachabilityWithHostName];
        [self.hostReachability startNotifier];
        [self updateInterfaceWithReachability:self.hostReachability];

        self.internetReachability = [YouMeReachability reachabilityForInternetConnection];
        [self.internetReachability startNotifier];
        [self updateInterfaceWithReachability:self.internetReachability];

        self.wifiReachability = [YouMeReachability reachabilityForLocalWiFi];
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
    YouMeReachability *curReach = [note object];
    NSParameterAssert ([curReach isKindOfClass:[YouMeReachability class]]);
    [self updateInterfaceWithReachability:curReach];
}


- (void)updateInterfaceWithReachability:(YouMeReachability *)reachability
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

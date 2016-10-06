


#import <Foundation/Foundation.h>
#import "NetworkService.h"

@interface YouMeCommonReachabilityService : NSObject

+ (YouMeCommonReachabilityService *)getInstance;
+ (void)destroy;

- (id)init;
- (void)start:(youmecommon::INgnNetworkChangCallback *)networkChangeCallback;
- (void)uninit;

@end

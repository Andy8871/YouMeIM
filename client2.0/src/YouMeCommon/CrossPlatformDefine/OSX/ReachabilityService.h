


#import <Foundation/Foundation.h>
#import "NetworkService.h"

@interface YouMeReachabilityService : NSObject

+ (YouMeReachabilityService *)getInstance;
+ (void)destroy;

- (id)init;
- (void)start:(youmecommon::INgnNetworkChangCallback *)networkChangeCallback;
- (void)uninit;

@end

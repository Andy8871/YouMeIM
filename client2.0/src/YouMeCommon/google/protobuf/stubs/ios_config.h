#include "config.h"

#define __OPTIMIZE__ 1
#define HAVE_NEON 0
#if HAVE_NEON
#include <arm_neon.h>
#endif

#define HAVE_COREAUDIO_AUDIO_UNIT 1
//#define HAVE_COREAUDIO_AUDIO_QUEUE 1
/* Define if <ifaddrs.h> header exist */
#define HAVE_IFADDRS_H 1

#define HAVE_GETIFADDRS 1
/* Define if <netpacket/packet.h> header exist */
#define HAVE_NETPACKET_PACKET_H 0
/* Define if <net/if_types> header exist */
#define HAVE_NET_IF_TYPES_H 0

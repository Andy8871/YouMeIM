#if defined WIN32

#elif defined __APPLE__
#include "curl_config_apple.h"
#elif defined __ANDROID__
#include "curl_config_android.h"
#else
#error "no support system"
#endif

#include "ares_setup.h"
#include "ares.h"

namespace youmecommon {
    
const char *ares_version(int *version)
{
  if(version)
    *version = ARES_VERSION;

  return ARES_VERSION_STR;
}

}  // namespace youmecommon
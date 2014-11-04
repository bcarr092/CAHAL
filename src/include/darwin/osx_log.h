#ifndef __OSX_LOG_H__
#define __OSX_LOG_H__

#include <CoreFoundation/CoreFoundation.h>

#include "cpcommon.h"

void
  osx_print_code  (
                    CPC_LOG_LEVEL log_level,
                    OSStatus      code
                  );

#endif /* __OSX_LOG_H__ */

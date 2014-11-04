#ifndef __OSX_CAHAL_H__
#define __OSX_CAHAL_H__

#include <CoreFoundation/CoreFoundation.h>
#include <AudioToolbox/AudioToolbox.h>

#include "cpcommon.h"

#include "osx_log.h"

OSStatus
  osx_get_audio_objects_handles (
      AudioObjectID** io_device_list,
      UINT32*         out_num_devices
                                );

#endif /* __OSX_CAHAL_H__ */

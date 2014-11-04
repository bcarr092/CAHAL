#ifndef __OSX_CAHAL_DEVICE_H__
#define __OSX_CAHAL_DEVICE_H__

#include <CoreFoundation/CoreFoundation.h>
#include <AudioToolbox/AudioToolbox.h>

#include <darwinhelper.h>

#include "cahal.h"

OSStatus
osx_set_cahal_device_struct(
                            AudioObjectID in_device_id,
                            cahal_device* out_device
                            );

OSStatus
osx_get_device_string_property  (
                                 AudioObjectID               in_device_id,
                                 AudioObjectPropertySelector property,
                                 CHAR**                      out_device_property
                                 );

#endif /* __OSX_CAHAL_DEVICE_H__ */

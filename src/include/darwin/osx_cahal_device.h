#ifndef __OSX_CAHAL_DEVICE_H__
#define __OSX_CAHAL_DEVICE_H__

#include <CoreFoundation/CoreFoundation.h>
#include <AudioToolbox/AudioToolbox.h>

#include <darwin_helper.h>

#include "cahal.h"

void
osx_set_cahal_device_struct(
                            AudioObjectID in_device_id,
                            cahal_device* io_device
                            );

OSStatus
osx_get_device_uint32_property  (
                               AudioObjectID                in_device_id,
                               AudioObjectPropertySelector  in_property,
                               UINT32*                      out_device_property
                                 );

OSStatus
osx_get_device_float64_property (
                               AudioObjectID                in_device_id,
                               AudioObjectPropertySelector  in_property,
                               FLOAT64*                      out_device_property
                                 );

OSStatus
osx_get_device_string_property  (
                                 AudioObjectID               in_device_id,
                                 AudioObjectPropertySelector in_property,
                                 CHAR**                      out_device_property
                                 );

OSStatus
osx_get_device_property_value (
                     AudioObjectID                in_device_id,
                     AudioObjectPropertySelector  in_property,
                     UINT32                       in_device_property_value_size,
                     void*                        out_device_property_value
                               );

OSStatus
osx_get_device_property_size_and_value (
                   AudioObjectID                in_device_id,
                   AudioObjectPropertySelector  in_property,
                   UINT32*                      out_device_property_value_size,
                   void**                       out_device_property_value
                                        );

OSStatus
osx_get_device_supported_sample_rates (
                                       cahal_device* io_device
                                       );

OSStatus
osx_get_device_streams  (
                         cahal_device* io_device
                         );

#endif /* __OSX_CAHAL_DEVICE_H__ */

#ifndef __IOS_CAHAL_DEVICE_H__
#define __IOS_CAHAL_DEVICE_H__

#include <CoreFoundation/CoreFoundation.h>
#include <AudioToolbox/AudioToolbox.h>

#include <darwin_helper.h>

#include "cahal.h"
#include "cahal_device.h"
#include "cahal_device_stream.h"
#include "cahal_audio_format_flags.h"

#define IOS_DEVICE_HANDLE_INPUT   1
#define IOS_DEVICE_HANDLE_OUTPUT  2

#define IOS_DEVICE_STREAM_DEFAULT_HANDLE  1

cahal_device**
ios_set_cahal_device_struct( void );

BOOL
ios_query_input_support( void );

OSStatus
ios_get_device_uint32_property  (
                                 AudioSessionPropertyID  in_property,
                                 UINT32*                 out_device_property
                                 );
OSStatus
ios_get_device_float64_property (
                                 AudioSessionPropertyID  in_property,
                                 FLOAT64*                out_device_property
                                 );

OSStatus
ios_get_device_property_size_and_value (
                          AudioSessionPropertyID  in_property,
                          UINT32*                 out_device_property_value_size,
                          void**                  out_device_property_value
                                        );

OSStatus
ios_get_device_property_value (
                           AudioSessionPropertyID in_property,
                           UINT32*                in_device_property_value_size,
                           void*                  out_device_property_value
                               );

OSStatus
ios_get_device_name (
                     CFStringRef  in_route_direction,
                     CHAR**       out_device_label
                     );

BOOL
ios_get_input_device_info (
                           cahal_device* io_input_device
                           );

BOOL
ios_get_output_device_info (
                            cahal_device* io_output_device
                            );

BOOL
ios_set_device_stream (
                       cahal_device* io_device,
                       cahal_device_stream_direction in_direction
                       );

#endif  /*  __IOS_CAHAL_DEVICE_H__  */

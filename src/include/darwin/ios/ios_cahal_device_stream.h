#ifndef __IOS_CAHAL_DEVICE_STREAM_H__
#define __IOS_CAHAL_DEVICE_STREAM_H__

#include <CoreFoundation/CoreFoundation.h>
#include <AudioToolbox/AudioToolbox.h>

#include <darwin_helper.h>

#include "cahal.h"
#include "cahal_device.h"
#include "cahal_device_stream.h"
#include "cahal_audio_format_flags.h"

#include "darwin/darwin_cahal_device.h"

void
ios_determine_supported_formats (
                                 cahal_device_stream*           out_stream,
                                 cahal_device_stream_direction  in_direction
                                 );

#endif  /*  __IOS_CAHAL_DEVICE_STREAM_H__ */

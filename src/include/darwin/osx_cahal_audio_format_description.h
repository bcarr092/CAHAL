#ifndef __OSX_CAHAL_AUDIO_FORMAT_H__
#define __OSX_CAHAL_AUDIO_FORMAT_H__

#include <CoreFoundation/CoreFoundation.h>
#include <CoreAudio/CoreAudio.h>

#include <darwin_helper.h>

#include "cahal_audio_format_description.h"

OSStatus
osx_set_cahal_audio_format_description_list  (
                                          cahal_device_stream* io_device_stream
                                              );

#endif /* __OSX_CAHAL_AUDIO_FORMAT_H__ */

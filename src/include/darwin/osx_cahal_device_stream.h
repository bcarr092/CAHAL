#ifndef __OSX_CAHAL_DEVICE_STREAM_H__
#define __OSX_CAHAL_DEVICE_STREAM_H__

#include <CoreFoundation/CoreFoundation.h>
#include <CoreAudio/CoreAudio.h>

#include <darwin_helper.h>

#include "cahal.h"

#include "osx_cahal_device.h"
#include "osx_cahal_audio_format_description.h"

/*! \fn     OSStatus osx_get_device_streams  (
 cahal_device* io_device
 )
 \brief  Populate the list of streams supported by the audio device
 identified by the handle in io_device. The caller must free the list
 of streams. The list is null-terminated.
 
 \param  io_device The streams supported by the audio device pointed to by
 the handle in io_device will be logged in the list of
 streams in io_device. This is a null terminated list and
 must be freed by the caller.
 \return Either noErr or an error code. The error code can be passed to
 darwin_helper for printing.
 */
OSStatus
osx_get_device_streams  (
                         cahal_device* io_device
                         );

void
osx_set_cahal_device_stream_struct(
                                   AudioStreamID          in_device_stream_id,
                                   cahal_device_stream*   io_device_stream
                                   );

OSStatus
osx_get_device_stream_supported_virtual_formats (
                                           cahal_device_stream* io_device_stream
                                                 );

OSStatus
osx_get_device_stream_supported_physical_formats (
                                          cahal_device_stream* io_device_stream
                                                  );

#endif  /*  __OSX_CAHAL_DEVICE_STREAM_H__ */

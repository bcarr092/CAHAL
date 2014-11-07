/*! \file   osx_cahal_device_stream.h
    \brief  The platform-specific functions to query the hardware for its
            hardware-supported device streams as well as manage device streams
            are in this file.
 */
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

/*! \fn     void osx_set_cahal_device_stream_struct(
              AudioStreamID          in_device_stream_id,
              cahal_device_stream*   io_device_stream
            )
    \brief  Populates io_device_stream with all its properties by querying the
            streams supported by the audio IO hardware referenced by
            in_device_stream_id.
 
    \param  in_device_stream_id The id whose properties are to be queried and
                                populated in in_device_stream.
    \param  io_device_stream  The struct to be populated.
 */
void
osx_set_cahal_device_stream_struct(
                                   AudioStreamID          in_device_stream_id,
                                   cahal_device_stream*   io_device_stream
                                   );

/*! \fn     OSStatus osx_get_device_stream_supported_formats (
              cahal_device_stream* io_device_stream
            )
    \brief  Queries the hardware stream referenced in io_device_stream for its
            hardware preferred audio format, e.g. lpcm, as well as catalogues
            all the hardware supported parameters, e.g. format, sample rate,
            number of channels, etc as well. A null-terminated list of audio
            formats will be created by this function, which must be freed by the
            caller.
 
    \param  io_device_stream  The device stream whose preferred and supported
                              formats are to be queried and populated. The
                              null-terminated list of supported streams must be
                              freed by the caller.
    \return noErr if both the supported and preferred audio formats cane be
            queired and error code otherwise.
 */
OSStatus
osx_get_device_stream_supported_formats (
                                         cahal_device_stream* io_device_stream
                                         );

#endif  /*  __OSX_CAHAL_DEVICE_STREAM_H__ */

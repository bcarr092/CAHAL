/*! \file   osx_cahal_audio_format_description.h
    \brief  This is the platform-specific file responsible for querying the
            hardware for its supported audio formats as well as creating the
            list of supported formats for each device stream.
 */
#ifndef __OSX_CAHAL_AUDIO_FORMAT_H__
#define __OSX_CAHAL_AUDIO_FORMAT_H__

#include <CoreFoundation/CoreFoundation.h>
#include <CoreAudio/CoreAudio.h>

#include <darwin_helper.h>

#include "cahal_audio_format_description.h"

#include "darwin/osx_cahal_device_stream.h"

/*! \fn     OSStatus osx_set_cahal_audio_format_description_list  (
              cahal_device_stream* io_device_stream
            )
    \brief  Queries the audio formats supported in hardware by io_device_stream
            and generates a null-temrinated list of
            cahal_audio_format_description structs for all supported formats.
 
    \brief  io_device_stream  The device stream whose list of hardware supported
                              formats is to be queried for and whose list of
                              supported formats will be created and populated.
                              It is the responsibility of the caller to free
                              the list of supported formats created.
    \return noErr if the supported formats property could be queried, an error
            code otherwise.
 */
OSStatus
osx_set_cahal_audio_format_description_list  (
                                          cahal_device_stream* io_device_stream
                                              );

/*! \fn     cahal_audio_format_id osx_convert_core_audio_format_id_to_cahal_audio_format_id (
              UINT32 core_audio_format_id
            )
    \brief  This function bridges the core audio format constants to the cahal
            format constants. It takes as input a core audio format id and
            returns a cahal format id.
 
    \param  core_audio_format_id  The core audio format id to convert to a cahal
                                  format id.
    \return The equivalent cahal audio format id for the given core audio format
            id.
 */
cahal_audio_format_id
osx_convert_core_audio_format_id_to_cahal_audio_format_id (
                                                     UINT32 core_audio_format_id
                                                           );

#endif /* __OSX_CAHAL_AUDIO_FORMAT_H__ */

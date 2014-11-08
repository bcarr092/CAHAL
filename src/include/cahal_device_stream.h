/*! \file   cahal_device_stream.h
    \brief  All cahal_device_stream structs and typedefs are defined in this
            file. All functions related to the struct are also defined here.
 */
#ifndef __CAHAL_DEVICE_STREAM_H__
#define __CAHAL_DEVICE_STREAM_H__

#include <cpcommon.h>

#include "cahal_audio_format_description.h"

#define CAHAL_DEVICE_OUTPUT_STREAM  0
#define CAHAL_DEVICE_INPUT_STREAM   1

/*! \var    cahal_device_stream_handle
    \brief  Type definition for device stream handles
 */
typedef UINT32 cahal_device_stream_handle;

/*! \var    cahal_device_stream_direction
    \brief  Type definition for device stream directions.
 */
typedef UINT32 cahal_device_stream_direction;

/*! \var    cahal_device_stream_t
    \brief  Struct definition for device streams. A device stream represents
            either an input our output stream and can be opened using a
            specific format. Note that a device can have multiple streams, e.g.
            a USB headset with a speaker and microphone.
 */
typedef struct cahal_device_stream
{
  /*! \var    handle
      \brief  OS-specific handle for the stream
   */
  cahal_device_stream_handle                  handle;
  
  /*! \var    direction
      \brief  Direction of the stream. Set to either CAHAL_DEVICE_OUTPUT_STREAM
              or CAHAL_DEVICE_INPUT_STREAM.
   */
  cahal_device_stream_direction               direction;
  
  /*! \var    preferred_format
      \brief  The format that the hardware device prefers to work at as defined
              by the hardware. The format is the audio format that the hardware
              records samples in.
   */
  cahal_audio_format_id                       preferred_format;
  
  /*! \var    supported_formats
      \brief  A null-terminated list of format descriptions that are all
              supported in hardware by the audio device.
   */
  cahal_audio_format_description**            supported_formats;
  
} cahal_device_stream;

/*! \fn     void print_cahal_device_stream_list (
              cahal_device_stream** in_device_stream_list
            )
    \brief  Prints all device streams using the logger. The streams are logged
            at CPC_LOG_LEVEL_INFO.
 
    \param  in_device_stream_list The null-terminated list of device streams to
                                  print using the logger.
 */
void
print_cahal_device_stream_list (
                                cahal_device_stream** in_device_stream_list
                                );

/*! \fn     void print_cahal_device_stream (
              cahal_device_stream* in_device_stream
            )
    \brief  Prints the device stream using the logger. The stream is logged at
            CPC_LOG_LEVEL_INFO.
 
    \param  in_device_stream  The stream to log using the logger.
 */
void
print_cahal_device_stream (
                           cahal_device_stream* in_device_stream
                           );

/*! \fn     void free_cahal_device_stream_list (
              cahal_device_stream** in_device_stream_list
            )
    \brief  Frees all device streams in the list and then frees the passed in
            list itself.
 
    \param  in_device_stream_list A null-terminated list of device streams to be
                                  freed. The passed in pointer will be freed as
                                  well.
 */
void
free_cahal_device_stream_list (
                               cahal_device_stream** in_device_stream_list
                               );

#endif  /*  __CAHAL_DEVICE_STREAM_H__ */

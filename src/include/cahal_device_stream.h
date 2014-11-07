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

/*! \var    cahal_device_stream
 \brief  Struct definition for device streams
 */
typedef struct cahal_device_stream_t
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
  
  cahal_audio_format_id                       preferred_format;
  
  cahal_audio_format_description**            supported_formats;
  
} cahal_device_stream;

void
print_cahal_device_stream_list (
                                cahal_device_stream** in_device_stream_list
                                );

void
print_cahal_device_stream (
                           cahal_device_stream* in_device_stream
                           );

void
free_cahal_device_stream_list (
                               cahal_device_stream** in_device_stream_list
                               );

#endif  /*  __CAHAL_DEVICE_STREAM_H__ */

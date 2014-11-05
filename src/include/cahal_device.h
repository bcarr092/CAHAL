#ifndef __CAHAL_DEVICE_H__
#define __CAHAL_DEVICE_H__

#include <cpcommon.h>

#define CAHAL_DEVICE_OUTPUT_STREAM  0
#define CAHAL_DEVICE_INPUT_STREAM   1

typedef UINT32 cahal_device_handle;

typedef UINT32 cahal_device_stream_handle;
typedef UINT32 cahal_device_stream_direction;

typedef struct cahal_device_stream_t
{
  cahal_device_stream_handle      handle;
  cahal_device_stream_direction   direction;
  
} cahal_device_stream;

typedef struct cahal_device_t
{
  cahal_device_handle   handle;
  CHAR*               	device_name;
  CHAR*                 model;
  CHAR*                 manufacturer;
  CHAR*                 serial_number;
  CHAR*                 version;
  CHAR*                 device_uid;
  CHAR*                 model_uid;
  FLOAT64               preferred_sample_rate;
  FLOAT64**             supported_sample_rates;
  cahal_device_stream** device_streams;
  
} cahal_device;

void
print_cahal_device  (
                     cahal_device* device
                     );

void
free_cahal_device_list (
                        cahal_device** device_list
                        );

#endif  /*  _CAHAL_DEVICE_H__ */

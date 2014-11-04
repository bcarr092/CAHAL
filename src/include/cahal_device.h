#ifndef __CAHAL_DEVICE_H__
#define __CAHAL_DEVICE_H__

#include <cpcommon.h>

typedef UINT32 cahal_device_handle;

typedef struct cahal_device_t
{
  cahal_device_handle handle;
  CHAR*               device_name;
  CHAR*               model;
  CHAR*               manufacturer;
  CHAR*               serial_number;
  CHAR*               version;
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

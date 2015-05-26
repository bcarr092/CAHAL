#ifndef _WINDOWS_CAHAL_DEVICE_H__
#define _WINDOWS_CAHAL_DEVICE_H__

#include <mmdeviceapi.h>

#include <Audioclient.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <Propvarutil.h>

#include "cahal.h"
#include "cahal_device.h"

#ifdef __cplusplus
extern "C"
{
#endif

cahal_device**
windows_get_device_list( void );

void
windows_set_device_info(
  cahal_device* out_device,
  IMMDevice*    in_endpoint
);

void
windows_set_device_defaults(
  cahal_device* out_device,
  IMMDevice*    in_endpoint
);

#ifdef __cplusplus
}
#endif

#endif /* _WINDOWS_CAHAL_DEVICE_H__ */

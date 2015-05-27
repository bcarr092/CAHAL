#ifndef _WINDOWS_CAHAL_DEVICE_H__
#define _WINDOWS_CAHAL_DEVICE_H__

#include <mmdeviceapi.h>

#include <Audioclient.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <Propvarutil.h>

#include "cahal.h"
#include "cahal_device.h"

#include "windows_cahal_device_stream.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

cahal_device**
windows_get_device_list( void );

#ifdef __cplusplus
}
#endif

#endif /* _WINDOWS_CAHAL_DEVICE_H__ */

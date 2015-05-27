#ifndef __WINDOWS_CAHAL_DEVICE_STREAM_H__
#define __WINDOWS_CAHAL_DEVICE_STREAM_H__

#include <mmdeviceapi.h>

#include <Audioclient.h>

#include "cahal.h"
#include "cahal_device.h"
#include "cahal_device_stream.h"

#define WINDOWS_DEVICE_STREAM_DEFAULT_HANDLE  1

HRESULT
windows_set_device_streams(
  cahal_device* out_device,
  IMMDevice*    in_endpoint
);

#endif  /*  __WINDOWS_CAHAL_DEVICE_STREAM_H__ */

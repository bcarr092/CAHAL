/*! \file   windows_cahal_device.hpp
    \brief  All APIs related to querying the underlying audio IO hardware are
            in this file.

    \author Brent Carrara
 */

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

/*! \def    cahal_device** windows_get_device_list( void );
    \brief  Enumerates the audio IO devices and creates the device list
            struct.

    \return The populated list of devices and their associated streams.
 */
cahal_device**
windows_get_device_list( void );

#ifdef __cplusplus
}
#endif

#endif /* _WINDOWS_CAHAL_DEVICE_H__ */

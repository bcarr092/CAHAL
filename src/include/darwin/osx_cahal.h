/*! \file   osx_cahal.h
    \brief  Provides access at the top level (the device level) to the platform
            specific audio devices (both input and output). The exposed method
            cahal_get_device_list is defined in the implementation of this
            header in a platform specific way. The prototype however is defined
            in cahal.h, which is an exposed header file. The interface is
            platform agnostic even though the gathering of device information is
            platform specific.
 */
#ifndef __OSX_CAHAL_H__
#define __OSX_CAHAL_H__

#include <CoreFoundation/CoreFoundation.h>
#include <AudioToolbox/AudioToolbox.h>

#include <darwin_helper.h>

#include "cahal.h"
#include "darwin/osx_cahal_device.h"

/*! \fn     OSStatus osx_get_audio_device_handles (
              AudioObjectID** io_device_list,
              UINT32*         out_num_devices
            )
    \brief  Helper function to query the hardware for input/output audio devices
            Populates io_device_list with AudioObjectIDs. The AudioObjectID
            handle is used by OSX/iOS to query properties of audio hardware.
 
    \param  io_device_list  The list of AudioObjectID handles to populate. This
                            list is null-terminated. This structure must be
                            freed by the caller.
    \param  out_num_devices Populated with the number of devices that are
                            contained in the io_device_list. Note that there
                            are out_num_devices + 1 elements in io_device_list
                            with the + 1 for the null terminator.
    \return Either noErr(0) or an error code. The error code can be passed to
            darwin_helper for printing.
 */
OSStatus
osx_get_audio_device_handles (
                              AudioObjectID** io_device_list,
                              UINT32*         out_num_devices
                              );

#endif /* __OSX_CAHAL_H__ */

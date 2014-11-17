#ifndef __IOS_CAHAL_H__
#define __IOS_CAHAL_H__

#include <CoreFoundation/CoreFoundation.h>
#include <AudioToolbox/AudioToolbox.h>

#include <darwin_helper.h>

#include "cahal.h"

#include "ios_cahal_device.h"

void
ios_interrupt_listener  (
                         void* in_user_data,
                         UINT32 in_state
                         );

OSStatus
ios_initialize_audio_session( void );

#endif  /*  __IOS_CAHAL_H__ */

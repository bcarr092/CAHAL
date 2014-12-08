/*! \file   ios_cahal_initialize.h
    \brief  objective-c class that provides access to the underlying playback/
            record hardware via an objective-c inteface.
 */
#ifndef __IOS_CAHAL_INITIALIZE_H__
#define __IOS_CAHAL_INITIALIZE_H__

#import <AudioUnit/AudioUnit.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>

#import "ios_cahal_device.h"

/*! \class  ios_cahal_initialize
    \brief  Static class that polls for recording premissions then configures
            the audio hardware for input/output.
 */
@interface ios_cahal_initialize : NSObject

/*  \method (void) ios_initialize_recording
    \brief  Performs the ios-specific initialization of the hardware. Ensures
            that the system has the appropriate permissions to record as well.
 */
+ (void) ios_initialize_recording;

@end

#endif  /*  __IOS_CAHAL_INITIALIZE_H__  */

#ifndef __IOS_CAHAL_INITIALIZE_H__
#define __IOS_CAHAL_INITIALIZE_H__

#import <AudioUnit/AudioUnit.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>

#import "ios_cahal_device.h"

@interface ios_cahal_initialize : NSObject

+ (void) ios_initialize_recording;

@end

#endif  /*  __IOS_CAHAL_INITIALIZE_H__  */

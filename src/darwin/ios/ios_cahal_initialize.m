/*! \file   ios_cahal_initialize.m
 
    \author Brent Carrara
 */
#import "darwin/ios/ios_cahal_initialize.h"

void ios_initialize_recording()
{
  [ios_cahal_initialize ios_initialize_recording];
}

void ios_terminate_recording()
{
  [ios_cahal_initialize ios_terminate_recording];
}

@implementation ios_cahal_initialize

+ (void) ios_terminate_recording
{
  NSError *err = nil;
  
  [[AVAudioSession sharedInstance] setActive:NO error:&err];
  
  if( err )
  {
    CPC_ERROR (
               "audioSession: %s %d %s",
               [err domain],
               [err code],
               [[err userInfo] description]
               );
    
    cpc_exit( CPC_ERROR_CODE_INVALID_PERMISSIONS );
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "Session terminated successfully." );
  }
}

/*! \fn     ios_initialize_recording
    \brief  Requests permission to access the microphone and exits if the binary 
            doesn't have the necessary permissions.
 */
+ (void) ios_initialize_recording
{
  NSError *err = nil;
  
  [[AVAudioSession sharedInstance] requestRecordPermission:^(BOOL granted)
  {
    if (granted) {
      CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "Permission granted" );
    }
    else {
      CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Permission denied" );
      
      cpc_exit( CPC_ERROR_CODE_INVALID_PERMISSIONS );
    }
  } ];
  
  [[AVAudioSession sharedInstance]
    setCategory:AVAudioSessionCategoryPlayAndRecord error:&err];
  
  if( err )
  {
    CPC_ERROR (
               "audioSession: %s %d %s",
               [err domain],
               [err code],
               [[err userInfo] description]
               );
    
    cpc_exit( CPC_ERROR_CODE_INVALID_PERMISSIONS );
  }
  
  [[AVAudioSession sharedInstance]
   setMode:AVAudioSessionModeVideoRecording error:&err];
  
  if( err )
  {
    CPC_ERROR (
               "audioSession: %s %d %s",
               [err domain],
               [err code],
               [[err userInfo] description]
               );
    
    cpc_exit( CPC_ERROR_CODE_INVALID_PERMISSIONS );
  }
  
  [[AVAudioSession sharedInstance] setActive:YES error:&err];
  
  if( err )
  {
    CPC_ERROR (
               "audioSession: %s %d %s",
               [err domain],
               [err code],
               [[err userInfo] description]
               );
    
    cpc_exit( CPC_ERROR_CODE_INVALID_PERMISSIONS );
  }
  
  NSArray* available_inputs = [[AVAudioSession sharedInstance] availableInputs];
  
  if( 0 == available_inputs.count )
  {
    CPC_LOG_STRING  (
                      CPC_LOG_LEVEL_ERROR,
                     "Audio input hardware is not avilable. Sleeping..."
                     );
    
    [NSThread sleepForTimeInterval:1];
    
    NSArray* available_inputs =
      [[AVAudioSession sharedInstance] availableInputs];
    
    if( 0 == available_inputs.count )
    {
      CPC_LOG_STRING  (
                       CPC_LOG_LEVEL_ERROR,
                       "Audio input hardware is still not avilable. Sleeping..."
                       );
      
      [NSThread sleepForTimeInterval:5];
      
      NSArray* available_inputs =
        [[AVAudioSession sharedInstance] availableInputs];
      
      if( 0 == available_inputs.count )
      {
        CPC_LOG_STRING  (
                         CPC_LOG_LEVEL_ERROR,
                         "Audio input hardware is still"
                         " not avilable. Sleeping..."
                         );
        
        cpc_exit( CPC_ERROR_CODE_INVALID_PERMISSIONS );
      }
    }
  }
}

@end

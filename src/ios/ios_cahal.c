#include "ios/ios_cahal.h"

cahal_device**
cahal_get_device_list( void )
{
  cahal_device** device_list  = NULL;
  
  OSStatus result =
  ios_initialize_audio_session();
  
  if( noErr == result )
  {
    device_list = ios_set_cahal_device_struct();
    
    result =
    AudioSessionSetActive( FALSE );
    
    if( result )
    {
      CPC_ERROR( "Could not set session inactive: 0x%x.", result );
      
      CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
    }
  }

  return( device_list );
}
void
ios_interrupt_listener  (
                         void* in_user_data,
                         UINT32 in_state
                         )
{
  CPC_ERROR( "Interrupted. State = 0x%x.", in_state );
  
  CPC_EXIT( CPC_ERROR_CODE_APPLICATION_INTERRUPTED_IOS );
}

OSStatus
ios_initialize_audio_session( void )
{
  OSStatus result =
  AudioSessionInitialize  (
                           NULL,
                           kCFRunLoopDefaultMode,
                           &ios_interrupt_listener,
                           NULL
                           );
  if( noErr == result )
  {
    UINT32 category = kAudioSessionCategory_PlayAndRecord;

    result =
    AudioSessionSetProperty (
                             kAudioSessionProperty_AudioCategory,
                             sizeof( category ),
                             &category
                             );
    
    if( kAudioSessionNoError == result )
    {
      UINT32 mode = kAudioSessionMode_VideoRecording;
      
      result =
      AudioSessionSetProperty (
                               kAudioSessionProperty_Mode,
                               sizeof( mode ),
                               &mode
                               );
      
      if( kAudioSessionNoError == result )
      {
        result = AudioSessionSetActive( TRUE );
      }
      else
      {
        CPC_ERROR (
                   "Could not set mode (0x%x) to 0x%x: 0x%x.",
                   kAudioSessionProperty_Mode,
                   kAudioSessionMode_Measurement,
                   result
                   );
        
        CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
      }
    }
    else
    {
      CPC_ERROR (
                 "Could not set session category to 0x%x: 0x%x.",
                 category,
                 result
                 );
      
      CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
    }
  }
  else
  {
    CPC_ERROR( "Error initializing Audio Session: 0x%x.", result );
    
    CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
  }
  
  return( result );
}

BOOL
cahal_start_recording (
                       cahal_device*            in_device,
                       cahal_audio_format_id    in_format_id,
                       UINT32                   in_number_of_channels,
                       FLOAT64                  in_sample_rate,
                       UINT32                   in_bit_depth,
                       cahal_recorder_callback  in_recorder,
                       void*                    in_callback_user_data,
                       cahal_audio_format_flag  in_format_flags,
                       UINT32                   in_recording_time
                       )
{
  return( FALSE );
}

BOOL
cahal_start_playback  (
                       cahal_device*            in_device,
                       cahal_audio_format_id    in_format_id,
                       UINT32                   in_number_of_channels,
                       FLOAT64                  in_sample_rate,
                       UINT32                   in_bit_depth,
                       cahal_playback_callback  in_playback,
                       void*                    in_callback_user_data,
                       cahal_audio_format_flag  in_format_flags,
                       UINT32                   in_playback_time
                       )
{
  return( FALSE );
}

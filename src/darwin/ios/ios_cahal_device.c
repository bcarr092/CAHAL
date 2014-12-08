#include "darwin/ios/ios_cahal_device.h"

CPC_BOOL
cahal_set_default_device (
                          cahal_device*                 in_device,
                          cahal_device_stream_direction in_direction
                          )
{
  CPC_BOOL return_value = CPC_TRUE;
  
  if(
     NULL != in_device
     && cahal_test_device_direction_support  (
                                              in_device,
                                              in_direction
                                              )
     )
  {
    CPC_LOG (
             CPC_LOG_LEVEL_INFO,
             "Default device for direction %d set for %s.",
             in_direction,
             in_device->device_name
             );
  }
  else
  {
    CPC_LOG_STRING  (
                     CPC_LOG_LEVEL_ERROR,
                     "Device is null or direction not supported."
                     );
    
    return_value = CPC_FALSE;
  }
  
  return( return_value );
}

cahal_device**
ios_set_cahal_device_struct( void )
{
  CPC_BOOL initialized_device_aray  = CPC_TRUE;
  UINT32 num_devices                = 1;
  cahal_device** device_list        = NULL;
  
  if( ios_query_input_support() )
  {
    num_devices++;
  }
  
  if  ( CPC_ERROR_CODE_NO_ERROR
       ==  cpc_safe_malloc (
                            ( void** ) &device_list,
                            ( num_devices + 1 ) * sizeof( cahal_device* )
                            )
       )
  {
    for( UINT32 i = 0; i < num_devices; i++ )
    {
      if( CPC_ERROR_CODE_NO_ERROR
          != cpc_safe_malloc  (
                               ( void** ) &( device_list[ i ] ),
                               sizeof( cahal_device )
                               )
         )
      {
        initialized_device_aray = CPC_FALSE;
        
        break;
      }
    }
  }
  
  if( initialized_device_aray )
  {
    if  (
         ! ios_get_output_device_info( device_list[ 0 ] )
         || ! ios_set_device_stream (
                                     device_list[ 0 ],
                                     CAHAL_DEVICE_OUTPUT_STREAM
                                     )
         )
    {
      CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Could not initialize output." );
    }
    else
    {
      if  (
           2 > num_devices
           || ! ios_get_input_device_info( device_list[ 1 ] )
           || ! ios_set_device_stream (
                                       device_list[ 1 ],
                                       CAHAL_DEVICE_INPUT_STREAM
                                       )
           )
      {
        CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Could not initialize input." );
      }
    }
  }
  
  return( device_list );
}

CPC_BOOL
ios_set_device_stream (
                       cahal_device* io_device,
                       cahal_device_stream_direction in_direction
)
{
  CPC_BOOL result = CPC_TRUE;
  
  if( NULL != io_device )
  {
    if  (
         CPC_ERROR_CODE_NO_ERROR
         == cpc_safe_malloc (
                             ( void** ) &( io_device->device_streams ),
                             2 * sizeof( cahal_device_stream* )
                             )
         )
    {
      if  (
            CPC_ERROR_CODE_NO_ERROR
           == cpc_safe_malloc (
                               ( void** ) &( io_device->device_streams[ 0 ] ),
                               sizeof( cahal_device_stream )
                               )
          )
      {
        io_device->device_streams[ 0 ]->direction         = in_direction;
        io_device->device_streams[ 0 ]->handle            =
        IOS_DEVICE_STREAM_DEFAULT_HANDLE;

        io_device->device_streams[ 0 ]->preferred_format  =
        CAHAL_AUDIO_FORMAT_LINEARPCM;
      }
    }
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Null device." );
    
    result = CPC_FALSE;
  }
  
  return( result );
}

CPC_BOOL
ios_get_output_device_info (
                           cahal_device* io_output_device
                           )
{
  CPC_BOOL initialized_output = CPC_TRUE;
  
  io_output_device->handle    = IOS_DEVICE_HANDLE_OUTPUT;
  io_output_device->is_alive  = 1;
  
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
  if  (
       ios_get_device_name  (
                             kAudioSession_AudioRouteKey_Outputs,
                             &io_output_device->device_name
                             )
       )
#pragma clang diagnostic pop
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Could not get name." );
    
    initialized_output = CPC_FALSE;
  }
  
  if( ios_get_device_uint32_property (
                      kAudioSessionProperty_CurrentHardwareOutputNumberChannels,
                      &io_output_device->preferred_number_of_channels
                                      )
     )
  {
    CPC_ERROR (
             "Could not read output number of channels proprety (0x%x)",
             kAudioSessionProperty_CurrentHardwareOutputNumberChannels
             );
    
    initialized_output = CPC_FALSE;
  }
  
  if( ios_get_device_float64_property (
                               kAudioSessionProperty_CurrentHardwareSampleRate,
                               &io_output_device->preferred_sample_rate
                                       )
     )
  {
    CPC_ERROR (
             "Could not read current sample rate proprety (0x%x)",
             kAudioSessionProperty_CurrentHardwareSampleRate
             );
    
    initialized_output = CPC_FALSE;
  }
  
  return( initialized_output );
}

CPC_BOOL
ios_get_input_device_info (
                           cahal_device* out_input_device
                           )
{
  CPC_BOOL initialized_input = CPC_TRUE;
  
  out_input_device->handle   = IOS_DEVICE_HANDLE_INPUT;
  out_input_device->is_alive = 1;
  
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
  if  (
       ios_get_device_name  (
                             kAudioSession_AudioRouteKey_Inputs,
                             &out_input_device->device_name
                             )
       )
#pragma clang diagnostic pop
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Could not get name." );
    
    initialized_input = CPC_FALSE;
  }
  
  if( ios_get_device_uint32_property (
                    kAudioSessionProperty_CurrentHardwareInputNumberChannels,
                    &out_input_device->preferred_number_of_channels
                                      )
     )
  {
    CPC_ERROR (
             "Could not read input number of channels proprety (0x%x)",
             kAudioSessionProperty_CurrentHardwareInputNumberChannels
             );
    
    initialized_input = CPC_FALSE;
  }
  
  if( ios_get_device_float64_property (
                             kAudioSessionProperty_CurrentHardwareSampleRate,
                             &out_input_device->preferred_sample_rate
                                       )
     )
  {
    CPC_ERROR (
             "Could not read current sample rate proprety (0x%x)",
             kAudioSessionProperty_CurrentHardwareSampleRate
               );
    
    initialized_input = CPC_FALSE;
  }
  
  return( initialized_input );
}

OSStatus
ios_get_device_name (
                     CFStringRef  in_route_direction,
                     CHAR**       out_device_label
                     )
{
  UINT32 property_size                = 0;
  CFDictionaryRef audio_routing_info  = NULL;
  
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
  OSStatus result =
  AudioSessionGetProperty (
                           kAudioSessionProperty_AudioRouteDescription,
                           &property_size,
                           &audio_routing_info
                           );
#pragma clang diagnostic pop
  
  if( kAudioSessionNoError == result && NULL != audio_routing_info )
  {
    property_size = 0;
    
    CFArrayRef routes =
    CFDictionaryGetValue  (
                           audio_routing_info,
                           in_route_direction
                           );
    
    if( NULL != routes )
    {
      UINT32 num_routes = CFArrayGetCount( routes );
      
      CPC_LOG (
               CPC_LOG_LEVEL_TRACE,
               "Found 0x%x routes",
               num_routes
               );
      
      if( 1 == num_routes )
      {
        CFDictionaryRef input_dictionary =
        CFArrayGetValueAtIndex( routes, 0 );
        
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        CFStringRef device_type =
        CFDictionaryGetValue  (
                               input_dictionary,
                               kAudioSession_AudioRouteKey_Type
                               );
#pragma clang diagnostic pop
        
        *out_device_label = darwin_convert_cfstring_to_char_string( device_type );
        
        CPC_LOG (
                 CPC_LOG_LEVEL_DEBUG,
                 "Found device with label: %s.",
                 *out_device_label
                 );
      }
      else
      {
        CPC_ERROR( "Found 0x%x devices. Expecting 1.", num_routes );
        
        result = kAudioSessionUnspecifiedError;
        
        *out_device_label = NULL;
      }
    }
    else
    {
      CHAR* direction =
      darwin_convert_cfstring_to_char_string( in_route_direction );
      
      CPC_ERROR( "Received null route for direction %s.", direction );
                     
      cpc_safe_free( ( void** ) &direction );
    }
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Received NULL routing info." );
    
    result = kAudioSessionUnspecifiedError;
  }
  
  return( result );
}

CPC_BOOL
ios_query_input_support( void )
{
  CPC_BOOL input_supported  = CPC_FALSE;
  UINT32 audio_support      = 0;
  
  OSStatus result =
  ios_get_device_uint32_property  (
                                   kAudioSessionProperty_AudioInputAvailable,
                                   &audio_support
                                   );
  
  if( kAudioSessionNoError == result )
  {
    CPC_LOG (
             CPC_LOG_LEVEL_INFO,
             "Audio input is available (0x%x).",
             audio_support
             );
    
    input_supported = CPC_TRUE;
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "Audio input not available" );
  }
  
  return( input_supported );
}

OSStatus
ios_get_device_uint32_property  (
                                 AudioSessionPropertyID  in_property,
                                 UINT32*                 out_device_property
                                 )
{
  UINT32 property_size = sizeof( UINT32 );
  
  OSStatus result =
  ios_get_device_property_value (
                                 in_property,
                                 &property_size,
                                 out_device_property
                                 );
  
  if( kAudioSessionNoError == result )
  {
    CPC_LOG (
             CPC_LOG_LEVEL_DEBUG,
             "Device property 0x%x is %d",
             in_property,
             *out_device_property
             );
  }
  
  return( result );
}

OSStatus
ios_get_device_float64_property (
                                 AudioSessionPropertyID  in_property,
                                 FLOAT64*                out_device_property
                                 )
{
  UINT32 property_size = sizeof( FLOAT64 );
  
  OSStatus result =
  ios_get_device_property_value (
                                 in_property,
                                 &property_size,
                                 out_device_property
                                 );
  
  if( kAudioSessionNoError == result )
  {
    CPC_LOG (
             CPC_LOG_LEVEL_DEBUG,
             "Device property 0x%x is %f",
             in_property,
             *out_device_property
             );
  }
  
  return( result );
}
OSStatus
ios_get_device_property_value (
                           AudioSessionPropertyID in_property,
                           UINT32*                io_device_property_value_size,
                           void*                  out_device_property_value
                               )
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
  OSStatus result =
  AudioSessionGetProperty (
                           in_property,
                           io_device_property_value_size,
                           out_device_property_value
                           );
#pragma clang diagnostic pop
  
  if( result )
  {
    CPC_ERROR (
               "Could not get value for property 0x%x: 0x%x.",
               in_property,
               result
               );
    
    CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
  }
  
  return( result );
}


void
cahal_sleep (
             UINT32 in_sleep_time
             )
{
  CFRunLoopRunInMode(kCFRunLoopDefaultMode, in_sleep_time, CPC_FALSE );
}

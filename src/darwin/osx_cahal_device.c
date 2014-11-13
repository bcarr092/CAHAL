#include "darwin/osx_cahal_device.h"

void
osx_set_cahal_device_struct(
                            AudioObjectID  in_device_id,
                            cahal_device*  io_device
                            )
{
  io_device->handle       = in_device_id;
  io_device->device_name  = NULL;
  
  if( osx_get_device_string_property (
                                      io_device->handle,
                                      kAudioObjectPropertyName,
                                      &io_device->device_name
                                      )
     )
  {
    CPC_LOG (
             CPC_LOG_LEVEL_WARN,
             "Could not read name proprety (0x%x)",
             kAudioObjectPropertyName
             );
  }
  
  if( osx_get_device_string_property (
                                      io_device->handle,
                                      kAudioObjectPropertyModelName,
                                      &io_device->model
                                      )
     )
  {
    CPC_LOG (
             CPC_LOG_LEVEL_WARN,
             "Could not read model proprety (0x%x)",
             kAudioObjectPropertyModelName
             );
  }
  
  if( osx_get_device_string_property (
                                      io_device->handle,
                                      kAudioObjectPropertyManufacturer,
                                      &io_device->manufacturer
                                      )
     )
  {
    CPC_LOG (
             CPC_LOG_LEVEL_WARN,
             "Could not read manufacturer proprety (0x%x)",
             kAudioObjectPropertyManufacturer
             );
  }
  
  if( osx_get_device_string_property (
                                      io_device->handle,
                                      kAudioObjectPropertySerialNumber,
                                      &io_device->serial_number
                                      )
     )
  {
    CPC_LOG (
             CPC_LOG_LEVEL_WARN,
             "Could not read serial number proprety (0x%x)",
             kAudioObjectPropertySerialNumber
             );
  }
  
  if( osx_get_device_string_property (
                                      io_device->handle,
                                      kAudioObjectPropertyFirmwareVersion,
                                      &io_device->version
                                      )
     )
  {
    CPC_LOG (
             CPC_LOG_LEVEL_WARN,
             "Could not read firmware version proprety (0x%x)",
             kAudioObjectPropertyFirmwareVersion
             );
  }
  
  if( osx_get_device_string_property (
                                      io_device->handle,
                                      kAudioDevicePropertyDeviceUID,
                                      &io_device->device_uid
                                      )
     )
  {
    CPC_LOG (
             CPC_LOG_LEVEL_WARN,
             "Could not read device UID proprety (0x%x)",
             kAudioDevicePropertyDeviceUID
             );
  }
  
  if( osx_get_device_string_property (
                                      io_device->handle,
                                      kAudioDevicePropertyModelUID,
                                      &io_device->model_uid
                                      )
     )
  {
    CPC_LOG (
             CPC_LOG_LEVEL_WARN,
             "Could not read model UID proprety (0x%x)",
             kAudioDevicePropertyModelUID
             );
  }
  
  if( osx_get_device_float64_property (
                                       io_device->handle,
                                       kAudioDevicePropertyNominalSampleRate,
                                       &io_device->preferred_sample_rate
                                       )
     )
  {
    CPC_LOG (
             CPC_LOG_LEVEL_WARN,
             "Could not read nominal sample rate proprety (0x%x)",
             kAudioDevicePropertyNominalSampleRate
             );
  }
  
  if( osx_get_device_uint32_property (
                                      io_device->handle,
                                      kAudioDevicePropertyDeviceIsAlive,
                                      ( UINT32* ) &io_device->is_alive
                                      )
     )
  {
    CPC_LOG (
             CPC_LOG_LEVEL_WARN,
             "Could not read is alive flag (0x%x)",
             kAudioDevicePropertyDeviceIsAlive
             );
  }
  
  if( osx_get_device_uint32_property (
                                      io_device->handle,
                                      kAudioDevicePropertyDeviceIsRunning,
                                      ( UINT32* ) &io_device->is_running
                                      )
     )
  {
    CPC_LOG (
             CPC_LOG_LEVEL_WARN,
             "Could not read is running flag (0x%x)",
             kAudioDevicePropertyDeviceIsRunning
             );
  }
  
  if( osx_get_device_supported_sample_rates( io_device ) )
  {
    CPC_LOG (
             CPC_LOG_LEVEL_WARN,
             "Could not read available sample rates proprety (0x%x)",
             kAudioDevicePropertyAvailableNominalSampleRates
             );
  }
  
  if( osx_get_device_streams( io_device ) )
  {
    CPC_LOG (
             CPC_LOG_LEVEL_WARN,
             "Could not read available streams (0x%x)",
             kAudioDevicePropertyStreams
             );
  }
  
  if( osx_get_number_of_channels( io_device ) )
  {
    CPC_LOG (
             CPC_LOG_LEVEL_WARN,
             "Could not read number of channels (0x%x)",
             kAudioDevicePropertyPreferredChannelLayout
             );
  }
}

OSStatus
osx_get_number_of_channels  (
                             cahal_device* io_device
                             )
{
  AudioChannelLayout layout_property;
  
  UINT32 property_size = sizeof( AudioChannelLayout );
  
  io_device->preferred_number_of_channels = 0;
  
  OSStatus result =
  osx_get_device_property_value (
                                 io_device->handle,
                                 kAudioDevicePropertyPreferredChannelLayout,
                                 property_size,
                                 &layout_property
                                 );
  
  if( noErr == result )
  {
    CPC_LOG (
             CPC_LOG_LEVEL_TRACE,
             "Channel layout tag: 0x%x.",
             layout_property.mChannelLayoutTag
             );
    
    if( layout_property.mChannelLayoutTag ==
       kAudioChannelLayoutTag_UseChannelDescriptions )
    {
      io_device->preferred_number_of_channels =
      layout_property.mNumberChannelDescriptions;
    }
    else if(  layout_property.mChannelLayoutTag !=
            kAudioChannelLayoutTag_UseChannelBitmap )
    {
      //  Count the number of bits in the bit map to determine the number of
      //  channels
      
      UINT32 channel_bitmap = layout_property.mChannelBitmap;
      
      for( UINT32 i = 0; i < sizeof( UINT32 ); i++ )
      {
        if( channel_bitmap & 0x0001 )
        {
          io_device->preferred_number_of_channels++;
        }
        
        channel_bitmap >>= 1;
      }
    }
    else
    {
      io_device->preferred_number_of_channels =
      AudioChannelLayoutTag_GetNumberOfChannels (
                                               layout_property.mChannelLayoutTag
                                                 );
    }
  }
  else
  {
    AudioObjectPropertyAddress property_address =
    {
      kAudioDevicePropertyPreferredChannelsForStereo,
      kAudioObjectPropertyScopeGlobal,
      kAudioObjectPropertyElementMaster
    };
    
    if( ! AudioObjectHasProperty( io_device->handle, &property_address ) )
    {
      io_device->preferred_number_of_channels = 2;
    }
    else
    {
      result = kAudioHardwareUnknownPropertyError;
      
      CPC_LOG (
               CPC_LOG_LEVEL_WARN,
               "Property 0x%x is not supported.",
               kAudioDevicePropertyPreferredChannelsForStereo
               );
      
      CPC_PRINT_CODE( CPC_LOG_LEVEL_WARN, result );
    }
  }
  
  return( result );
}

OSStatus
osx_get_device_supported_sample_rates (
                                       cahal_device* io_device
                                       )
{
  UINT32 device_value_size                = 0;
  AudioValueRange* supported_sample_rates = NULL;
  
  OSStatus result =
  osx_get_device_property_size_and_value  (
                               io_device->handle,
                               kAudioDevicePropertyAvailableNominalSampleRates,
                               &device_value_size,
                               ( void** ) &supported_sample_rates
                                           );
  
  if( noErr == result )
  {
    UINT32 num_items = device_value_size / sizeof( AudioValueRange );
    
    CPC_LOG( CPC_LOG_LEVEL_TRACE, "Found 0x%x sample rates.", num_items );
    
    if( num_items > 0 )
    {
      if  ( CPC_ERROR_CODE_NO_ERROR
           == cpc_safe_malloc (
                             ( void ** ) &( io_device->supported_sample_rates ),
                             ( num_items + 1 )
                             * sizeof( cahal_sample_rate_range* )
                               )
           )
      {
        for( UINT32 i = 0; i < num_items; i++ )
        {
          AudioValueRange sample_rate = supported_sample_rates[ i ];
          
          CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Supported sample rate:" );
          CPC_LOG( CPC_LOG_LEVEL_TRACE, "\tMin: %.2f", sample_rate.mMinimum );
          CPC_LOG( CPC_LOG_LEVEL_TRACE, "\tMax: %.2f", sample_rate.mMaximum );
          
          if  ( CPC_ERROR_CODE_NO_ERROR
               == cpc_safe_malloc (
                       ( void ** ) &( io_device->supported_sample_rates[ i ] ),
                       sizeof( cahal_sample_rate_range )
                                   )
               )
          {
            io_device->supported_sample_rates[ i ]->minimum_rate =
            sample_rate.mMinimum;
            
            io_device->supported_sample_rates[ i ]->maximum_rate =
            sample_rate.mMaximum;
          }
        }
      }
    }
  }
  
  cpc_safe_free( ( void** ) &supported_sample_rates );
  
  return( result );
}

OSStatus
osx_get_device_property_value (
                   AudioObjectID                in_device_id,
                   AudioObjectPropertySelector  in_property,
                   UINT32                       in_device_property_value_size,
                   void*                        out_device_property_value
                               )
{
  OSStatus result       = noErr;
  UINT32 property_size  = in_device_property_value_size;
  
  AudioObjectPropertyAddress property_address =
  {
    in_property,
    kAudioObjectPropertyScopeGlobal,
    kAudioObjectPropertyElementMaster
  };
  
  if( ! AudioObjectHasProperty( in_device_id, &property_address ) )
  {
    result = kAudioHardwareUnknownPropertyError;
    
    CPC_LOG (
             CPC_LOG_LEVEL_WARN,
             "Property 0x%x is not supported.",
             in_property
             );
    
    CPC_PRINT_CODE( CPC_LOG_LEVEL_WARN, result );
  }
  else
  {
    result =
    AudioObjectGetPropertyData  (
                                 in_device_id,
                                 &property_address,
                                 0,
                                 NULL,
                                 &property_size,
                                 out_device_property_value
                                 );
  }
  
  if( result )
  {
    CPC_ERROR (
               "Error in AudioObjectGetPropertyData: %d",
               result
               );
    
    CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
  }
  
  return( result );
  
}

OSStatus
osx_get_device_property_size_and_value (
                    AudioObjectID                in_device_id,
                    AudioObjectPropertySelector  in_property,
                    UINT32*                      out_device_property_value_size,
                    void**                       out_device_property_value
                                        )
{
  AudioObjectPropertyAddress property_address =
  {
    in_property,
    kAudioObjectPropertyScopeGlobal,
    kAudioObjectPropertyElementMaster
  };
  
  OSStatus result =
  AudioObjectGetPropertyDataSize  (
                                   in_device_id,
                                   &property_address,
                                   0, NULL,
                                   out_device_property_value_size
                                   );
  
  if( result )
  {
    CPC_ERROR (
               "Error in AudioObjectGetPropertyDataSize: %d",
               result
               );
    
    CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
  }
  else
  {
    CPC_LOG(
            CPC_LOG_LEVEL_DEBUG,
            "Device property 0x%x for device 0x%x has size 0x%x",
            in_property,
            in_device_id,
            *out_device_property_value_size
            );
    
    if  ( CPC_ERROR_CODE_NO_ERROR
         == cpc_safe_malloc (
                             ( void ** ) out_device_property_value,
                              *out_device_property_value_size
                            )
         )
    {
      result =
      osx_get_device_property_value (
                                     in_device_id,
                                     in_property,
                                     *out_device_property_value_size,
                                     *out_device_property_value
                                     );
    }
  }
  
  return( result );

}

OSStatus
osx_get_device_uint32_property  (
                               AudioObjectID                in_device_id,
                               AudioObjectPropertySelector  in_property,
                               UINT32*                      out_device_property
                                 )
{
  OSStatus result =
  osx_get_device_property_value (
                                 in_device_id,
                                 in_property,
                                 sizeof( UINT32 ),
                                 out_device_property
                                 );
  
  if( noErr == result )
  {
    CPC_LOG(
            CPC_LOG_LEVEL_DEBUG,
            "Device property 0x%x for device 0x%x is %d",
            in_property,
            in_device_id,
            *out_device_property
            );
    
    CPC_PRINT_CODE( CPC_LOG_LEVEL_DEBUG, *out_device_property );
  }
  
  return( result );
}

OSStatus
osx_get_device_float64_property  (
                                AudioObjectID                in_device_id,
                                AudioObjectPropertySelector  in_property,
                                FLOAT64*                     out_device_property
                                  )
{
  OSStatus result =
  osx_get_device_property_value (
                                 in_device_id,
                                 in_property,
                                 sizeof( FLOAT64 ),
                                 out_device_property
                                 );
  
  if( noErr == result )
  {
    CPC_LOG(
            CPC_LOG_LEVEL_DEBUG,
            "Device property 0x%x for device 0x%x is %f",
            in_property,
            in_device_id,
            *out_device_property
            );
    
    CPC_PRINT_CODE( CPC_LOG_LEVEL_DEBUG, *out_device_property );
  }
  
  return( result );
}

OSStatus
osx_get_device_string_property  (
                                 AudioObjectID               in_device_id,
                                 AudioObjectPropertySelector in_property,
                                 CHAR**                      out_device_property
                                 )
{
  CFStringRef device_property;
  
  OSStatus result =
  osx_get_device_property_value (
                                 in_device_id,
                                 in_property,
                                 sizeof( CFStringRef ),
                                 &device_property
                                 );
  
  *out_device_property = NULL;
  
  if( result )
  {
    CPC_ERROR (
               "Error in AudioObjectGetPropertyData: %d",
               result
               );
    
    CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
  }
  else
  {
    *out_device_property =
    darwin_convert_cfstring_to_char_string( device_property );
    
    if( NULL != device_property )
    {
      CFRelease( device_property );
    }
    
    if( NULL != *out_device_property )
    {
      CPC_LOG(
              CPC_LOG_LEVEL_DEBUG,
              "Device property 0x%x for device 0x%x is %s",
              in_property,
              in_device_id,
              *out_device_property
              );
    }
  }
  
  return( result );
}

BOOL
cahal_set_default_device (
                          cahal_device*                 in_device,
                          cahal_device_stream_direction in_direction
                          )
{
  BOOL return_value = TRUE;
  
  if(
     NULL != in_device
     && cahal_test_device_direction_support  (
                                              in_device,
                                              in_direction
                                              )
     )
  {
    UINT32 osx_property = 0;
    
    CPC_LOG(
            CPC_LOG_LEVEL_TRACE,
            "Setting device (0x%x:%s) as default device for direction %d.",
            in_device->handle,
            in_device->device_name,
            in_direction
            );
    
    switch( in_direction )
    {
      case CAHAL_DEVICE_INPUT_STREAM:
        osx_property = kAudioHardwarePropertyDefaultInputDevice;
        break;
      case CAHAL_DEVICE_OUTPUT_STREAM:
        osx_property = kAudioHardwarePropertyDefaultOutputDevice;
        break;
      default:
        CPC_ERROR( "Unknown direction %d.", in_direction );
        
        return_value = FALSE;
        
        break;
    }
    
    if( return_value )
    {
      CPC_LOG(
              CPC_LOG_LEVEL_TRACE,
              "Setting default device, direction=%d (0x%x), to 0x%x",
              in_direction,
              osx_property,
              in_device->handle
              );
      
      AudioObjectPropertyAddress property_address =
      {
        osx_property,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster
      };
      
      OSStatus result =
      AudioObjectSetPropertyData (
                                  kAudioObjectSystemObject,
                                  &property_address,
                                  0,
                                  NULL,
                                  sizeof( in_device->handle ),
                                  &( in_device->handle )
                                  );
      
      if( result )
      {
        CPC_LOG(
                CPC_LOG_LEVEL_DEBUG,
      "Could not set default device (%d:0x%x) on object 0x%x to device 0x%x.",
                in_direction,
                osx_property,
                kAudioObjectSystemObject,
                in_device->handle
                );
        
        CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
        
        return_value = FALSE;
      }
    }
  }
  else
  {
    return_value = FALSE;
  }
  
  return( return_value );
}

void
osx_playback_callback (
                       void*                in_user_data,
                       AudioQueueRef        in_queue,
                       AudioQueueBufferRef  in_buffer
                       )
{
  CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Received osx playback callback" );
  
  CPC_LOG (
           CPC_LOG_LEVEL_TRACE,
           "in_user_data=0x%x, in_queue=0x%x, in_buffer=0x%x",
           in_user_data,
           in_queue,
           in_buffer
           );
  
  if( NULL != in_user_data )
  {
    UINT32 number_of_bytes              = in_buffer->mAudioDataBytesCapacity;
    cahal_playback_info* playback_info  = ( cahal_playback_info* ) in_user_data;
    
    if  (
         playback_info->playback_callback  (
                                       playback_info->playback_device,
                                       in_buffer->mAudioData,
                                       &number_of_bytes,
                                       playback_info->user_data
                                       )
         )
    {
    
      in_buffer->mAudioDataByteSize = number_of_bytes;
      
      OSStatus result =
      AudioQueueEnqueueBuffer( in_queue, in_buffer, 0, NULL );
      
      if( result )
      {
        CPC_ERROR (
                   "Error enqueuing buffer: %d",
                   result
                   );
        
        CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
      }
      else
      {
        CPC_LOG (
                 CPC_LOG_LEVEL_TRACE,
                 "Received 0x%x bytes of data.",
                 number_of_bytes
                 );
      }
    }
    else
    {
      CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Error filling buffer." );
    }
  }
}

void
osx_recorder_callback  (
                      void*                                in_user_data,
                      AudioQueueRef                        in_queue,
                      AudioQueueBufferRef                  in_buffer,
                      const AudioTimeStamp*                in_start_time,
                      UINT32                               in_number_of_packets,
                      const AudioStreamPacketDescription*  in_packet_description
                        )
{
  CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Received osx recorder callback" );
  
  CPC_LOG (
       CPC_LOG_LEVEL_TRACE,
       "in_user_data=0x%x, in_queue=0x%x, in_buffer=0x%x, in_packet_desc=0x%x,"
        " in_number_of_packets=0x%x",
       in_user_data,
       in_queue,
       in_buffer,
       in_packet_description,
       in_number_of_packets
  );
  
  if( 0 < in_number_of_packets )
  {
    if( NULL == in_packet_description )
    {
      if( NULL != in_queue )
      {
        AudioStreamBasicDescription recorder_format;
        
        UINT32 property_size  = sizeof( AudioStreamBasicDescription );
        
        memset( &recorder_format, 0, sizeof( AudioStreamBasicDescription ) );
        
        OSStatus result =
        AudioQueueGetProperty (
                               in_queue,
                               kAudioConverterCurrentInputStreamDescription,
                               &recorder_format,
                               &property_size
                               );
        
        if( noErr == result )
        {
          UINT32 buffer_size =
          in_number_of_packets * recorder_format.mBytesPerPacket;
          
          if( NULL != in_user_data )
          {
            
            UCHAR* buffer                       = NULL;
            cahal_recorder_info* recorder_info  =
            ( cahal_recorder_info* ) in_user_data;
            
            if  ( CPC_ERROR_CODE_NO_ERROR
                 == cpc_safe_malloc (
                                     ( void ** ) &( buffer ),
                                     sizeof( UCHAR ) * buffer_size
                                     )
                 )
            {
              CPC_LOG (
                       CPC_LOG_LEVEL_TRACE,
                       "Calling function at location 0x%x with user data 0x%x.",
                       recorder_info->recording_callback,
                       recorder_info->user_data
                       );
              
              if  (
                   ! recorder_info->recording_callback (
                                                recorder_info->recording_device,
                                                buffer,
                                                buffer_size,
                                                recorder_info->user_data
                                                        )
                   )
              {
                CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Error returning buffer." );
              }
            }
          }
          else
          {
            CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Null in_user_data!" );
          }
        }
        else
        {
          CPC_ERROR (
                     "Invalid property request (0x%x) for audio queue: 0x%x",
                     kAudioConverterCurrentInputStreamDescription,
                     result
                     );
          
          CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
        }
      }
    }
    else
    {
      CPC_LOG_STRING  (
                       CPC_LOG_LEVEL_ERROR,
                       "Variable rate formats are not supported."
                       );
    }
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Invalid number of packets." );
  }
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
  BOOL return_value = FALSE;
  
  if  (
       cahal_test_device_direction_support  (
                                             in_device,
                                             CAHAL_DEVICE_OUTPUT_STREAM
                                             )
       )
  {
    AudioStreamBasicDescription playback_description;
    
    cahal_playback_info* playback = NULL;
    
    memset( &playback_description, 0, sizeof( AudioStreamBasicDescription ) );
    
    if  ( CPC_ERROR_CODE_NO_ERROR
         == cpc_safe_malloc (
                             ( void ** ) &( playback ),
                             sizeof( cahal_playback_info )
                             )
         )
    {
      playback->playback_device   = in_device;
      playback->playback_callback = in_playback;
      playback->user_data         = in_callback_user_data;
      
      OSStatus result =
      osx_configure_asbd  (
                           in_format_id,
                           in_number_of_channels,
                           in_sample_rate,
                           in_bit_depth,
                           in_format_flags,
                           CAHAL_DEVICE_OUTPUT_STREAM,
                           &playback_description
                           );
      
      if( noErr == result )
      {
        AudioQueueRef audio_queue = NULL;
        
        result =
        osx_configure_output_audio_queue  (
                                           in_device,
                                           playback,
                                           &playback_description,
                                           &audio_queue
                                           );
        
        if( noErr == result )
        {
          result =
          osx_configure_output_audio_queue_buffer (
                                                   &playback_description,
                                                   playback,
                                                   audio_queue
                                                   );
          
          if( noErr == result )
          {
            result = AudioQueueStart( audio_queue, NULL );
            
            if( noErr == result )
            {
              sleep( in_playback_time );
              
              result = AudioQueueStop( audio_queue, true );
              
              if( noErr == result )
              {
                result =
                AudioQueueDispose( audio_queue, true );
                
                if( result )
                {
                  CPC_ERROR( "Could not dispose of queue: 0x%x.", result );
                  
                  CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
                }
              }
              else
              {
                CPC_ERROR( "Could not stop audio queue: 0x%x.", result );
                
                CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
              }
            }
            else
            {
              CPC_ERROR( "Could not start audio queue: 0x%x.", result );
              
              CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
            }
          }
        }
      }
      
      cpc_safe_free( ( void** ) &playback );
      
      if( noErr == result )
      {
        return_value = TRUE;
      }
    }
  }
  else
  {
    CPC_ERROR (
               "Direction (%d) not support by %s.",
               CAHAL_DEVICE_OUTPUT_STREAM,
               in_device->device_name
               );
  }
  
  return( return_value );
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
                       UINT32                   in_record_time
                       )
{
  BOOL return_value = FALSE;
  
  if  (
       cahal_test_device_direction_support  (
                                             in_device,
                                             CAHAL_DEVICE_INPUT_STREAM
                                             )
       )
  {
    AudioStreamBasicDescription recorder_desciption;
    
    cahal_recorder_info* recorder = NULL;
    
    memset( &recorder_desciption, 0, sizeof( AudioStreamBasicDescription ) );
    
    if  ( CPC_ERROR_CODE_NO_ERROR
         == cpc_safe_malloc (
                             ( void ** ) &( recorder ),
                             sizeof( cahal_recorder_info )
                             )
         )
    {
      recorder->recording_device    = in_device;
      recorder->recording_callback  = in_recorder;
      recorder->user_data           = in_callback_user_data;
      
      OSStatus result =
      osx_configure_asbd  (
                           in_format_id,
                           in_number_of_channels,
                           in_sample_rate,
                           in_bit_depth,
                           in_format_flags,
                           CAHAL_DEVICE_INPUT_STREAM,
                           &recorder_desciption
                           );
      
      if( noErr == result )
      {
        AudioQueueRef audio_queue = NULL;
        
        result =
        osx_configure_input_audio_queue (
                                         in_device,
                                         recorder,
                                         &recorder_desciption,
                                         &audio_queue
                                         );
        
        if( noErr == result )
        {
          result =
          osx_configure_input_audio_queue_buffer  (
                                                   &recorder_desciption,
                                                   audio_queue
                                                   );
          
          if( noErr == result )
          {
            result = AudioQueueStart( audio_queue, NULL );
            
            if( noErr == result )
            {
              sleep( in_record_time );
              
              result = AudioQueueStop( audio_queue, true );
              
              if( noErr == result )
              {
                result =
                AudioQueueDispose( audio_queue, true );
                
                if( result )
                {
                  CPC_ERROR( "Could not dispose of queue: 0x%x.", result );
                  
                  CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
                }
              }
              else
              {
                CPC_ERROR( "Could not stop audio queue: 0x%x.", result );
                
                CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
              }
            }
            else
            {
              CPC_ERROR( "Could not start audio queue: 0x%x.", result );
              
              CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
            }
          }
        }
      }
      
      cpc_safe_free( ( void** ) &recorder );
      
      if( noErr == result )
      {
        return_value = TRUE;
      }
    }
  }
  else
  {
    CPC_ERROR (
               "Direction (%d) not support by %s.",
               CAHAL_DEVICE_INPUT_STREAM,
               in_device->device_name
               );
  }
  
  return( return_value );
}

OSStatus
osx_configure_input_audio_queue_buffer  (
                                     AudioStreamBasicDescription* in_asbd,
                                     AudioQueueRef                io_audio_queue
                                         )
{
  UINT32 bytes_per_buffer = 0;
  OSStatus result         = noErr;
  
  if( NULL != in_asbd )
  {
    result =
    osx_compute_bytes_per_buffer  (
                                   in_asbd,
                                   CAHAL_QUEUE_BUFFER_DURATION,
                                   &bytes_per_buffer
                                   );
    
    if( noErr == result )
    {
      for( UINT32 i = 0; i < CAHAL_QUEUE_NUMBER_OF_QUEUES; i++ )
      {
        AudioQueueBufferRef buffer;
        
        result =
        AudioQueueAllocateBuffer  (
                                   io_audio_queue,
                                   bytes_per_buffer,
                                   &buffer
                                   );
        
        if( noErr == result )
        {
          result =
          AudioQueueEnqueueBuffer( io_audio_queue, buffer, 0, NULL );
          
          if( result )
          {
            CPC_ERROR (
                       "Error enqueuing buffer: %d",
                       result
                       );
            
            CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
          }
        }
        else
        {
          CPC_ERROR(
                    "Error allocating a new audio queue buffer: %d",
                    result
                    );
          
          CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
        }
      }
    }
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Invalid basic stream description" );
  }
  
  return( result );
}

OSStatus
osx_compute_bytes_per_buffer  (
                             AudioStreamBasicDescription* in_asbd,
                             FLOAT32                      in_number_of_seconds,
                             UINT32*                      out_bytes_per_buffer
                               )
{
  OSStatus result = noErr;
  
  if( NULL != in_asbd )
  {
    if( 0 < in_asbd->mBytesPerFrame )
    {
      UINT32 num_frames = in_number_of_seconds * in_asbd->mSampleRate;
      
      *out_bytes_per_buffer = num_frames * in_asbd->mBytesPerFrame;
    }
    else
    {
      CPC_LOG_STRING  (
                       CPC_LOG_LEVEL_ERROR,
                       "Only constant bit-rate codecs are supported"
                       );
      
      result = kAudioHardwareUnsupportedOperationError;
    }
  }
  else
  {
    CPC_LOG_STRING  (
                     CPC_LOG_LEVEL_ERROR,
                     "Invalid basic stream description"
                     );
  }
  
  return( result );
}

OSStatus
osx_configure_output_audio_queue_buffer  (
                                    AudioStreamBasicDescription*  in_asbd,
                                    cahal_playback_info*          in_playback,
                                    AudioQueueRef                 io_audio_queue
                                          )
{
  UINT32 bytes_per_buffer = 0;
  OSStatus result         = noErr;
  
  if( NULL != in_asbd )
  {
    result =
    osx_compute_bytes_per_buffer  (
                                   in_asbd,
                                   CAHAL_QUEUE_BUFFER_DURATION,
                                   &bytes_per_buffer
                                   );
    
    if( noErr == result )
    {
      for( UINT32 i = 0; i < CAHAL_QUEUE_NUMBER_OF_QUEUES; i++ )
      {
        AudioQueueBufferRef buffer;
        
        result =
        AudioQueueAllocateBuffer  (
                                   io_audio_queue,
                                   bytes_per_buffer,
                                   &buffer
                                   );
        
        if( noErr == result )
        {
          osx_playback_callback( in_playback, io_audio_queue, buffer );
        }
        else
        {
          CPC_ERROR(
                    "Error allocating a new audio queue buffer: %d",
                    result
                    );
          
          CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
        }
      }
    }
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Invalid basic stream description" );
  }
  
  return( result );
}

OSStatus
osx_configure_output_audio_queue (
                                cahal_device*                  in_device,
                                cahal_playback_info*           in_callback_info,
                                AudioStreamBasicDescription*   in_asbd,
                                AudioQueueRef*                 out_audio_queue
                                  )
{
  OSStatus result = noErr;
  
  if( NULL != in_asbd )
  {
    result =
    AudioQueueNewOutput  (
                         in_asbd,
                         osx_playback_callback,
                         in_callback_info,
                         NULL,
                         NULL,
                         0,
                         out_audio_queue
                         );
    
    if( noErr == result )
    {
      CFStringRef device_uid =
      CFStringCreateWithCString (
                                 NULL,
                                 in_device->device_uid,
                                 kCFStringEncodingASCII
                                 );
      
      CPC_LOG (
               CPC_LOG_LEVEL_TRACE,
               "Setting queue device to %s.",
               in_device->device_uid
               );
      
      result =
      AudioQueueSetProperty (
                             *out_audio_queue,
                             kAudioQueueProperty_CurrentDevice,
                             &device_uid,
                             sizeof( device_uid )
                             );
      
      if( NULL != device_uid )
      {
        CFRelease( device_uid );
      }
      
      if( result )
      {
        CPC_ERROR (
                   "Error setting current device (0x%x) to %s: 0x%x",
                   kAudioQueueProperty_CurrentDevice,
                   in_device->device_uid,
                   result
                   );
        
        CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
      }
    }
    else
    {
      CPC_ERROR (
                 "Error creating AudioQueue: 0x%x.",
                 result
                 );
      
      CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
    }
  }
  else
  {
    CPC_LOG_STRING  (
                     CPC_LOG_LEVEL_ERROR,
                     "Invalid basic stream description"
                     );
  }
  
  return( result );
}

OSStatus
osx_configure_input_audio_queue (
                                 cahal_device*                 in_device,
                                 cahal_recorder_info*          in_callback_info,
                                 AudioStreamBasicDescription*  io_asbd,
                                 AudioQueueRef*                out_audio_queue
                                 )
{
  OSStatus result = noErr;
  
  if( NULL != io_asbd )
  {
    result =
    AudioQueueNewInput  (
                         io_asbd,
                         osx_recorder_callback,
                         in_callback_info,
                         NULL,
                         NULL,
                         0,
                         out_audio_queue
                         );
    
    if( noErr == result )
    {
      CFStringRef device_uid =
      CFStringCreateWithCString (
                                 NULL,
                                 in_device->device_uid,
                                 kCFStringEncodingASCII
                                 );
      
      CPC_LOG (
               CPC_LOG_LEVEL_TRACE,
               "Setting queue device to %s.",
               in_device->device_uid
               );
      
      result =
      AudioQueueSetProperty (
                             *out_audio_queue,
                             kAudioQueueProperty_CurrentDevice,
                             &device_uid,
                             sizeof( device_uid )
                             );
      
      if( NULL != device_uid )
      {
        CFRelease( device_uid );
      }
      
      if( result )
      {
        CPC_ERROR (
                   "Error setting current device (0x%x) to %s: 0x%x",
                   kAudioQueueProperty_CurrentDevice,
                   in_device->device_uid,
                   result
                   );
        
        CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
      }
      else
      {
        UINT32 property_size = sizeof( AudioStreamBasicDescription );
        
        result =
        AudioQueueGetProperty (
                               *out_audio_queue,
                               kAudioConverterCurrentInputStreamDescription,
                               io_asbd,
                               &property_size
                               );
        
        if( result )
        {
          CPC_ERROR(
                    "Error accessing property 0x%x on AudioQueue: %d",
                    kAudioConverterCurrentInputStreamDescription,
                    result
                    );
          
          CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
        }
      }
    }
    else
    {
      CPC_ERROR (
                 "Error creating AudioQueue: 0x%x.",
                 result
                 );
      
      CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
    }
  }
  else
  {
    CPC_LOG_STRING  (
                     CPC_LOG_LEVEL_ERROR,
                     "Invalid basic stream description"
                     );
  }
  
  return( result );
}

OSStatus
osx_configure_asbd  (
                     cahal_audio_format_id          in_format_id,
                     UINT32                         in_number_of_channels,
                     FLOAT64                        in_sample_rate,
                     UINT32                         in_bit_depth,
                     cahal_audio_format_flag        in_format_flags,
                     cahal_device_stream_direction  in_direction,
                     AudioStreamBasicDescription    *out_asbd
                     )
{
  OSStatus result       = kAudioHardwareUnspecifiedError;
  UINT32 property_size  = sizeof( AudioStreamBasicDescription );
  
  if( NULL != out_asbd )
  {
    out_asbd->mChannelsPerFrame = in_number_of_channels;
    out_asbd->mSampleRate       = in_sample_rate;
    out_asbd->mBitsPerChannel   = in_bit_depth;
    out_asbd->mFormatFlags      =
    osx_convert_cahal_audio_format_flags_to_core_audio_format_flags (
                                                                   in_format_flags
                                                                     );
    out_asbd->mFormatID         =
    osx_convert_cahal_audio_format_id_to_core_audio_format_id( in_format_id );
    
    result =
    AudioFormatGetProperty  (
                             kAudioFormatProperty_FormatInfo,
                             0,
                             NULL,
                             &property_size,
                             out_asbd
                             );

    if( result )
    {
      CPC_ERROR (
                 "Error setting ASBD using property 0x%x: 0x%x.",
                 kAudioFormatProperty_FormatInfo,
                 result
                 );
      
      CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
    }
    else
    {
      CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "ASBD:" );
      
      CPC_LOG (
               CPC_LOG_LEVEL_TRACE,
               "\tNumber of channels: %d.",
               out_asbd->mChannelsPerFrame
               );
      
      CPC_LOG (
               CPC_LOG_LEVEL_TRACE,
               "\tSample rate: %.2f.",
               out_asbd->mSampleRate
               );
      
      CPC_LOG (
               CPC_LOG_LEVEL_TRACE,
               "\tBit depth: %d",
               out_asbd->mBitsPerChannel
               );
      
      CPC_LOG (
               CPC_LOG_LEVEL_TRACE,
               "\tFormat flags: 0x%x",
               out_asbd->mFormatFlags
               );
      
      CPC_PRINT_CODE( CPC_LOG_LEVEL_TRACE, out_asbd->mFormatID );
    }
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Null ASBD." );
  }
  
  return( result );
}

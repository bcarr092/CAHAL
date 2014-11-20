#include "darwin/osx/osx_cahal_device.h"

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
    CPC_LOG (
             CPC_LOG_LEVEL_WARN,
             "Error in AudioObjectGetPropertyData: %d",
             result
             );
    
    CPC_PRINT_CODE( CPC_LOG_LEVEL_WARN, result );
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
    CPC_LOG (
             CPC_LOG_LEVEL_WARN,
             "Error in AudioObjectGetPropertyDataSize: %d",
             result
             );
    
    CPC_PRINT_CODE( CPC_LOG_LEVEL_WARN, result );
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
    CPC_LOG (
             CPC_LOG_LEVEL_WARN,
             "Error in AudioObjectGetPropertyData: %d",
             result
             );
    
    CPC_PRINT_CODE( CPC_LOG_LEVEL_WARN, result );
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
        
        return_value = CPC_FALSE;
        
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
        
        CPC_PRINT_CODE( CPC_LOG_LEVEL_DEBUG, result );
        
        return_value = CPC_FALSE;
      }
    }
  }
  else
  {
    return_value = CPC_FALSE;
  }
  
  return( return_value );
}

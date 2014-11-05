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
  
  io_device->number_of_channels = 0;
  
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
      io_device->number_of_channels =
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
          io_device->number_of_channels++;
        }
        
        channel_bitmap >>= 1;
      }
    }
    else
    {
      io_device->number_of_channels =
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
      io_device->number_of_channels = 2;
    }
    else
    {
      result = kAudioHardwareUnknownPropertyError;
      
      CPC_LOG (
               CPC_LOG_LEVEL_WARN,
               "Property 0x%x is not supported.",
               kAudioDevicePropertyPreferredChannelsForStereo
               );
      
      DARWIN_PRINT_CODE( CPC_LOG_LEVEL_WARN, result );
    }
  }
  
  return( result );
}

OSStatus
osx_get_device_streams  (
                         cahal_device* io_device
                         )
{
  UINT32 device_value_size          = 0;
  AudioStreamID* available_streams  = NULL;
  
  OSStatus result =
  osx_get_device_property_size_and_value  (
                                           io_device->handle,
                                           kAudioDevicePropertyStreams,
                                           &device_value_size,
                                           ( void** ) &available_streams
                                           );
  
  if( noErr == result )
  {
    UINT32 num_items = device_value_size / sizeof( AudioStreamID );
    
    CPC_LOG( CPC_LOG_LEVEL_TRACE, "Found 0x%x streams.", num_items );
    
    if( num_items > 0 && NULL != available_streams )
    {
      io_device->device_streams =
      ( cahal_device_stream ** ) malloc (
                                         ( num_items + 1 )
                                         * sizeof( cahal_device_stream* )
                                         );
      
      memset  (
               io_device->device_streams,
               0,
               ( num_items + 1 ) * sizeof( cahal_device_stream* )
               );
      
      CPC_LOG( CPC_LOG_LEVEL_TRACE, "Found 0x%x streams.", num_items );
      
      for( UINT32 i = 0; i < num_items; i++ )
      {
        UINT32 direction = 0;
        
        result = osx_get_device_uint32_property  (
                                                   available_streams[ i ],
                                                   kAudioStreamPropertyDirection,
                                                   &direction
                                                   );
        
        CPC_LOG( CPC_LOG_LEVEL_TRACE, "Stream direction: 0x%x", direction );
        
        io_device->device_streams[ i ] =
        ( cahal_device_stream* ) malloc( sizeof( cahal_device_stream ) );
        
        io_device->device_streams[ i ]->handle = available_streams[ i ];
        
        if( 0 == direction )
        {
          io_device->device_streams[ i ]->direction =
          CAHAL_DEVICE_OUTPUT_STREAM;
        }
        else
        {
          io_device->device_streams[ i ]->direction = CAHAL_DEVICE_INPUT_STREAM;
        }
      }
    }
  }
  
  if( NULL != available_streams )
  {
    free( available_streams );
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
      io_device->supported_sample_rates =
      ( FLOAT64** ) malloc( ( num_items + 1 ) * sizeof( FLOAT64* ) );
      
      memset  (
               io_device->supported_sample_rates,
               0,
               ( num_items + 1 ) * sizeof( FLOAT64* )
               );
      
      for( UINT32 i = 0; i < num_items; i++ )
      {
        AudioValueRange sample_rate = supported_sample_rates[ i ];
        
        CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Supported sample rate:" );
        CPC_LOG( CPC_LOG_LEVEL_TRACE, "\tMin: %.2f", sample_rate.mMinimum );
        CPC_LOG( CPC_LOG_LEVEL_TRACE, "\tMax: %.2f", sample_rate.mMaximum );
        
        io_device->supported_sample_rates[ i ] =
        ( FLOAT64* ) malloc( sizeof( FLOAT64 ) );
        
        *( io_device->supported_sample_rates[ i ] ) = sample_rate.mMinimum;
      }
    }
  }
  
  if( NULL != supported_sample_rates )
  {
    free( supported_sample_rates );
  }
  
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
    
    DARWIN_PRINT_CODE( CPC_LOG_LEVEL_WARN, result );
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
    
    DARWIN_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
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
    
    DARWIN_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
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
    
    *out_device_property_value = malloc( *out_device_property_value_size );
    
    result =
    osx_get_device_property_value (
                                   in_device_id,
                                   in_property,
                                   *out_device_property_value_size,
                                   *out_device_property_value
                                   );
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
    
    DARWIN_PRINT_CODE( CPC_LOG_LEVEL_DEBUG, *out_device_property );
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
    
    DARWIN_PRINT_CODE( CPC_LOG_LEVEL_DEBUG, *out_device_property );
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
    
    DARWIN_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
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

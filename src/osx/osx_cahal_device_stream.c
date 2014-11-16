#include "osx/osx_cahal_device_stream.h"

void
osx_set_cahal_device_stream_struct(
                                   AudioStreamID          in_device_stream_id,
                                   cahal_device_stream*   io_device_stream
                                   )
{
  if( NULL != io_device_stream )
  {
    io_device_stream->handle = in_device_stream_id;
    
    if( osx_get_device_uint32_property  (
                                         in_device_stream_id,
                                         kAudioStreamPropertyDirection,
                                         &( io_device_stream->direction )
                                         )
       )
    {
      CPC_LOG (
               CPC_LOG_LEVEL_WARN,
               "Could not get stream's direction (0x%x)",
               kAudioStreamPropertyDirection
               );
    }
    
    if( osx_get_device_stream_supported_formats( io_device_stream ) )
    {
      CPC_LOG (
               CPC_LOG_LEVEL_WARN,
               "Could not get stream's physical formats (0x%x)",
               kAudioStreamPropertyPhysicalFormat
               );
    }
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Null device stream object." );
  }
}

OSStatus
osx_get_device_stream_supported_formats (
                                         cahal_device_stream* io_device_stream
                                         )
{
  OSStatus result = kAudioHardwareUnspecifiedError;
  
  if( NULL != io_device_stream )
  {
    UINT32 property_size                            =
    sizeof( AudioStreamBasicDescription );
    AudioStreamBasicDescription* format_description = NULL;
    
    result =
    osx_get_device_property_size_and_value  (
                                             io_device_stream->handle,
                                             kAudioStreamPropertyPhysicalFormat,
                                             &property_size,
                                             ( void** ) &format_description
                                             );
    
    if( noErr == result && NULL != format_description )
    {
      UINT32 format =
      osx_convert_core_audio_format_id_to_cahal_audio_format_id (
                                                   format_description->mFormatID
                                                                 );
      
      io_device_stream->preferred_format = format;
      
      result = osx_set_cahal_audio_format_description_list( io_device_stream );
    }
    
    cpc_safe_free( ( void** ) &format_description );
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Null device stream." );
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
  OSStatus result                   = kAudioHardwareUnspecifiedError;
  
  if( NULL != io_device )
  {
    result =
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
        io_device->device_streams = NULL;
        
        if  ( CPC_ERROR_CODE_NO_ERROR
              == cpc_safe_malloc (
                             ( void ** ) &( io_device->device_streams ),
                             ( num_items + 1 ) * sizeof( cahal_device_stream* )
                                 )
             )
        {
          CPC_LOG( CPC_LOG_LEVEL_TRACE, "Found 0x%x streams.", num_items );
          
          for( UINT32 i = 0; i < num_items; i++ )
          {
            if  ( CPC_ERROR_CODE_NO_ERROR
                 == cpc_safe_malloc (
                               ( void ** ) &( io_device->device_streams[ i ] ),
                               sizeof( cahal_device_stream )
                                     )
                 )
            {
              osx_set_cahal_device_stream_struct  (
                                             available_streams[ i ],
                                             io_device->device_streams[ i ]
                                                   );
            }
            else
            {
              result = kAudioHardwareUnspecifiedError;
              
              break;
            }
          }
        }
        else
        {
          result = kAudioHardwareUnspecifiedError;
        }
      }
    }
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Null device." );
  }
  
  cpc_safe_free( ( void** ) &available_streams );
  
  return( result );
}

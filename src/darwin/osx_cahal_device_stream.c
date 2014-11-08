#include "darwin/osx_cahal_device_stream.h"

void
osx_set_cahal_device_stream_struct(
                                   AudioStreamID          in_device_stream_id,
                                   cahal_device_stream*   io_device_stream
                                   )
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

OSStatus
osx_get_device_stream_supported_formats (
                                         cahal_device_stream* io_device_stream
                                         )
{
  UINT32 property_size                            =
  sizeof( AudioStreamBasicDescription );
  AudioStreamBasicDescription* format_description = NULL;
  
  OSStatus result =
  osx_get_device_property_size_and_value  (
                                           io_device_stream->handle,
                                           kAudioStreamPropertyPhysicalFormat,
                                           &property_size,
                                           ( void** ) &format_description
                                           );
  
  if( noErr == result )
  {
    UINT32 format =
    osx_convert_core_audio_format_id_to_cahal_audio_format_id (
                                                   format_description->mFormatID
                                                               );
    
    io_device_stream->preferred_format = format;
    
    result = osx_set_cahal_audio_format_description_list( io_device_stream );
  }
  
  if( NULL != format_description )
  {
    free( format_description );
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
        io_device->device_streams[ i ] =
        ( cahal_device_stream* ) malloc( sizeof( cahal_device_stream ) );
        
        memset  (
                 io_device->device_streams[ i ],
                 0,
                 sizeof( cahal_device_stream )
                 );
        
        
        osx_set_cahal_device_stream_struct  (
                                             available_streams[ i ],
                                             io_device->device_streams[ i ]
                                             );
      }
    }
  }
  
  if( NULL != available_streams )
  {
    free( available_streams );
  }
  
  return( result );
}

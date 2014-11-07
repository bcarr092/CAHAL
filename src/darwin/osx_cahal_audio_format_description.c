#include "darwin/osx_cahal_device_stream.h"

CHAR*
convert_cahal_audio_format_id_to_cstring  (
                                        cahal_audio_format_id in_audio_format_id
                                        )
{
  CHAR* format_cstring = cpc_convert_code_to_cstring( in_audio_format_id );
  
  return( format_cstring );
}

void
log_cahal_audio_format  (
                         CPC_LOG_LEVEL          in_log_level,
                         CHAR*                  in_label,
                         cahal_audio_format_id  in_format
                         )
{
  CHAR* format_cstring = convert_cahal_audio_format_id_to_cstring( in_format );
  
  if( NULL != format_cstring )
  {
    if( NULL == in_label )
    {
      in_label = "";
    }
    
    CPC_LOG (
             in_log_level,
             "%s %s",
             in_label,
             format_cstring
             );
    
    free( format_cstring );
  }
}

OSStatus
osx_set_cahal_audio_format_description_list  (
                                          cahal_device_stream* io_device_stream
                                              )
{
  UINT32 property_size                                  = 0;
  AudioStreamRangedDescription* stream_description_list = NULL;
  
  OSStatus result =
  osx_get_device_property_size_and_value  (
                                   io_device_stream->handle,
                                   kAudioStreamPropertyAvailablePhysicalFormats,
                                   &property_size,
                                   ( void** ) &stream_description_list
                                           );
  if( noErr == result )
  {
    UINT32 num_items = property_size / sizeof( AudioStreamRangedDescription );
    
    CPC_LOG( CPC_LOG_LEVEL_TRACE, "Found 0x%x supported formats", num_items );
    
    io_device_stream->supported_formats =
    ( cahal_audio_format_description** )
    malloc  (
             ( num_items + 1 )
             * sizeof( cahal_audio_format_description* )
             );
    
    memset  (
             io_device_stream->supported_formats,
             0,
             ( num_items + 1 ) * sizeof( cahal_audio_format_description* )
             );
    
    for( UINT32 i = 0; i < num_items; i++ )
    {
      io_device_stream->supported_formats[ i ] =
      ( cahal_audio_format_description* )
      malloc( sizeof( cahal_audio_format_description ) );
      
      memset  (
               io_device_stream->supported_formats[ i ],
               0,
               sizeof( cahal_audio_format_description )
               );
      
      io_device_stream->supported_formats[ i ]->format_id =
      stream_description_list[ i ].mFormat.mFormatID;
      
      io_device_stream->supported_formats[ i ]->bit_depth =
      stream_description_list[ i ].mFormat.mBitsPerChannel;
      
      io_device_stream->supported_formats[ i ]->number_of_channels =
      stream_description_list[ i ].mFormat.mChannelsPerFrame;
      
      if( kAudioStreamAnyRate ==
         stream_description_list[ i ].mFormat.mSampleRate
         )
      {
        io_device_stream->supported_formats[ i ]->sample_rate_range.minimum_rate
        = stream_description_list[ i ].mSampleRateRange.mMinimum;
        
        io_device_stream->supported_formats[ i ]->sample_rate_range.maximum_rate
        = stream_description_list[ i ].mSampleRateRange.mMaximum;
      }
      else
      {
        io_device_stream->supported_formats[ i ]->sample_rate_range.minimum_rate
        = stream_description_list[ i ].mFormat.mSampleRate;
        
        io_device_stream->supported_formats[ i ]->sample_rate_range.maximum_rate
        = stream_description_list[ i ].mFormat.mSampleRate;
      }
      
    }
  }
  
  if( NULL != stream_description_list )
  {
    free( stream_description_list );
  }
  
  return( result );
}

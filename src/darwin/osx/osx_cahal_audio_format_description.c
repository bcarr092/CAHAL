/*! \file   osx_cahal_audio_format_description.h
    \brief  The OSX specific audio format description code.
 
    \author Brent Carrara
 */
#include "darwin/osx/osx_cahal_audio_format_description.h"

OSStatus
osx_set_cahal_audio_format_description_list  (
                                          cahal_device_stream* io_device_stream
                                              )
{
  UINT32 property_size                                  = 0;
  AudioStreamRangedDescription* stream_description_list = NULL;
  OSStatus result                                       =
  kAudioHardwareUnspecifiedError;
  
  if( NULL != io_device_stream )
  {
    result =
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
      
      if  ( CPC_ERROR_CODE_NO_ERROR
           == cpc_safe_malloc (
                           ( void ** ) &( io_device_stream->supported_formats ),
                           ( num_items + 1 )
                           * sizeof( cahal_audio_format_description* )
                               )
           )
      {
        for( UINT32 i = 0; i < num_items; i++ )
        {
          if  ( CPC_ERROR_CODE_NO_ERROR
               == cpc_safe_malloc (
                     ( void ** ) &( io_device_stream->supported_formats[ i ] ),
                     sizeof( cahal_audio_format_description )
                                   )
               )
          {
            io_device_stream->supported_formats[ i ]->format_id =
            darwin_convert_core_audio_format_id_to_cahal_audio_format_id  (
                             stream_description_list[ i ].mFormat.mFormatID
                                                                          );
            
            io_device_stream->supported_formats[ i ]->bit_depth =
            stream_description_list[ i ].mFormat.mBitsPerChannel;
            
            io_device_stream->supported_formats[ i ]->number_of_channels =
            stream_description_list[ i ].mFormat.mChannelsPerFrame;
            
            if( kAudioStreamAnyRate ==
               stream_description_list[ i ].mFormat.mSampleRate
               )
            {
              io_device_stream->supported_formats[ i ]->
                sample_rate_range.minimum_rate
              = stream_description_list[ i ].mSampleRateRange.mMinimum;
              
              io_device_stream->supported_formats[ i ]->
                sample_rate_range.maximum_rate
              = stream_description_list[ i ].mSampleRateRange.mMaximum;
            }
            else
            {
              io_device_stream->supported_formats[ i ]->
                sample_rate_range.minimum_rate
              = stream_description_list[ i ].mFormat.mSampleRate;
              
              io_device_stream->supported_formats[ i ]->
                sample_rate_range.maximum_rate
              = stream_description_list[ i ].mFormat.mSampleRate;
            }
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
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Null device stream." );
    
    result = kAudioHardwareUnspecifiedError;
  }
  
  cpc_safe_free( ( void** ) &stream_description_list );
  
  return( result );
}

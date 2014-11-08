#include "cahal_device_stream.h"

void
print_cahal_device_stream_list (
                                cahal_device_stream** in_device_stream_list
                                )
{
  if( NULL != in_device_stream_list )
  {
    UINT32 index                        = 0;
    cahal_device_stream* device_stream  = in_device_stream_list[ index++ ];
    
    CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "\tAvailable streams:" );
    
    while( NULL != device_stream )
    {
      print_cahal_device_stream( device_stream );
      
      device_stream = in_device_stream_list[ index++ ];
    }
  }
}

void
print_cahal_device_stream (
                           cahal_device_stream* in_device_stream
                           )
{
  if( NULL != in_device_stream )
  {
    CPC_LOG( CPC_LOG_LEVEL_INFO, "\t\tID: 0x%x", in_device_stream->handle );
    
    if( CAHAL_DEVICE_OUTPUT_STREAM == in_device_stream->direction )
    {
      CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "\t\tDirection: OUTPUT" );
    }
    else
    {
      CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "\t\tDirection: INTPUT" );
    }
    
    print_cahal_audio_format_id (
                                 "\t\tPreferred physical format:",
                                 in_device_stream->preferred_format
                                 );
    
    if( NULL != in_device_stream->supported_formats )
    {
      UINT32 index                          = 0;
      cahal_audio_format_description* supported_format  =
      in_device_stream->supported_formats[ index++ ];
      
      CPC_LOG_STRING (
                      CPC_LOG_LEVEL_INFO,
                      "\t\tSupported formats:"
                      );
      
      while( NULL != supported_format )
      {
        print_cahal_audio_format_description( supported_format );
        
        supported_format =
        in_device_stream->supported_formats[ index++ ];
      }
    }
  }
}

void
free_cahal_device_stream_list (
                               cahal_device_stream** in_device_stream_list
                               )
{
  if( NULL != in_device_stream_list )
  {
    UINT32 index                        = 0;
    cahal_device_stream* device_stream  = in_device_stream_list[ index++ ];
    
    while( NULL != device_stream )
    {
      if( NULL != device_stream->supported_formats )
      {
        UINT32 index = 0;
        cahal_audio_format_description* format =
        device_stream->supported_formats[ index++ ];
        
        while( NULL != format )
        {
          free( format );
          
          format = device_stream->supported_formats[ index++ ];
        }
        
        free( device_stream->supported_formats);
      }
      
      free( device_stream );
      
      device_stream = in_device_stream_list[ index++ ];
    }
    
    free( in_device_stream_list );
    
    in_device_stream_list = NULL;
  }
}

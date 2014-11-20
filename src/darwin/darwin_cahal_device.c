#include "darwin/darwin_cahal_device.h"

static
void
darwin_playback_callback (
                       void*                in_user_data,
                       AudioQueueRef        in_queue,
                       AudioQueueBufferRef  in_buffer
                       )
{
  CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Received darwin playback callback" );
  
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
      CPC_LOG (
               CPC_LOG_LEVEL_TRACE,
               "Number of bytes is 0x%x.",
               number_of_bytes
               );
      
      in_buffer->mAudioDataByteSize = number_of_bytes;
      
      if( 0 != number_of_bytes)
      {
        CPC_LOG_BUFFER  (
                         CPC_LOG_LEVEL_TRACE,
                         "Playback buffer",
                         in_buffer->mAudioData,
                         80,
                         8
                         );
      }
      else
      {
        CPC_MEMSET  (
                     in_buffer->mAudioData,
                     0,
                     in_buffer->mAudioDataBytesCapacity
                     );
      }
      
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
                 "Played back 0x%x bytes of data.",
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

static
void
darwin_recorder_callback  (
                      void*                                in_user_data,
                      AudioQueueRef                        in_queue,
                      AudioQueueBufferRef                  in_buffer,
                      const AudioTimeStamp*                in_start_time,
                      UINT32                               in_number_of_packets,
                      const AudioStreamPacketDescription*  in_packet_description
                        )
{
  CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Received darwin recorder callback" );
  
  CPC_LOG (
       CPC_LOG_LEVEL_TRACE,
       "in_user_data=0x%x, in_queue=0x%x, in_buffer=0x%x, in_packet_desc=0x%x,"
        " in_number_of_packets=0x%x, buffer_size=0x%x",
       in_user_data,
       in_queue,
       in_buffer,
       in_packet_description,
       in_number_of_packets,
       in_buffer->mAudioDataByteSize
  );
  
  if( 0 < in_number_of_packets )
  {
    if( NULL == in_packet_description )
    {
      if( NULL != in_queue )
      {
        if( NULL != in_user_data )
        {
          UCHAR* buffer                       = NULL;
          cahal_recorder_info* recorder_info  =
          ( cahal_recorder_info* ) in_user_data;
          
          if  ( CPC_ERROR_CODE_NO_ERROR
               == cpc_safe_malloc (
                                 ( void ** ) &( buffer ),
                                 sizeof( UCHAR ) * in_buffer->mAudioDataByteSize
                                   )
               )
          {
            CPC_LOG (
                     CPC_LOG_LEVEL_TRACE,
                     "Calling function at location 0x%x with user data 0x%x.",
                     recorder_info->recording_callback,
                     recorder_info->user_data
                     );
            
            memcpy  (
                     buffer,
                     in_buffer->mAudioData,
                     in_buffer->mAudioDataByteSize
                     );
            
            CPC_LOG_BUFFER  (
                             CPC_LOG_LEVEL_TRACE,
                             "Recorded buffer",
                             in_buffer->mAudioData,
                             80,
                             8
                             );
            
            if  (
                 ! recorder_info->recording_callback (
                                              recorder_info->recording_device,
                                              buffer,
                                              in_buffer->mAudioDataByteSize,
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
  
  OSStatus result =
  AudioQueueEnqueueBuffer( in_queue, in_buffer, 0, NULL );
  
  if( result )
  {
    CPC_ERROR (
               "Error re-enqueuing buffer: %d",
               result
               );
    
    CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
  }
}

CPC_BOOL
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
  CPC_BOOL return_value = CPC_FALSE;
  
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
      darwin_configure_asbd  (
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
        
        CPC_LOG (
                 CPC_LOG_LEVEL_TRACE,
                 "ASDB Info: sr=%.2f, nc=0x%x, bd=0x%x, bpf=0x%x"
                 ", bpp=0x%x, fpp=0x%x",
                 playback_description.mSampleRate,
                 playback_description.mChannelsPerFrame,
                 playback_description.mBitsPerChannel,
                 playback_description.mBytesPerFrame,
                 playback_description.mBytesPerPacket,
                 playback_description.mFramesPerPacket
                 );
        
        result =
        darwin_configure_output_audio_queue  (
                                           in_device,
                                           playback,
                                           &playback_description,
                                           &audio_queue
                                           );
        
        if( noErr == result )
        {
          result =
          darwin_configure_output_audio_queue_buffer (
                                                   &playback_description,
                                                   playback,
                                                   audio_queue
                                                   );
          
          CPC_LOG (
                   CPC_LOG_LEVEL_TRACE,
                   "ASDB Info: sr=%.2f, nc=0x%x, bd=0x%x, bpf=0x%x"
                   ", bpp=0x%x, fpp=0x%x",
                   playback_description.mSampleRate,
                   playback_description.mChannelsPerFrame,
                   playback_description.mBitsPerChannel,
                   playback_description.mBytesPerFrame,
                   playback_description.mBytesPerPacket,
                   playback_description.mFramesPerPacket
                   );
          
          if( noErr == result )
          {
            result = AudioQueueStart( audio_queue, NULL );
            
            if( noErr == result )
            {
//              sleep( in_playback_time );
              CFRunLoopRunInMode(kCFRunLoopDefaultMode, in_playback_time, false);
              
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
        return_value = CPC_TRUE;
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

CPC_BOOL
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
  CPC_BOOL return_value = CPC_FALSE;
  
  CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "In start recording!" );
  
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
      darwin_configure_asbd  (
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
        
        CPC_LOG (
                 CPC_LOG_LEVEL_TRACE,
                 "ASDB Info: sr=%.2f, nc=0x%x, bd=0x%x, bpf=0x%x"
                 ", bpp=0x%x, fpp=0x%x",
                 recorder_desciption.mSampleRate,
                 recorder_desciption.mChannelsPerFrame,
                 recorder_desciption.mBitsPerChannel,
                 recorder_desciption.mBytesPerFrame,
                 recorder_desciption.mBytesPerPacket,
                 recorder_desciption.mFramesPerPacket
                 );
        
        result =
        darwin_configure_input_audio_queue (
                                         in_device,
                                         recorder,
                                         &recorder_desciption,
                                         &audio_queue
                                         );
        
        CPC_LOG (
                 CPC_LOG_LEVEL_TRACE,
                 "ASDB Info: sr=%.2f, nc=0x%x, bd=0x%x, bpf=0x%x"
                 ", bpp=0x%x, fpp=0x%x",
                 recorder_desciption.mSampleRate,
                 recorder_desciption.mChannelsPerFrame,
                 recorder_desciption.mBitsPerChannel,
                 recorder_desciption.mBytesPerFrame,
                 recorder_desciption.mBytesPerPacket,
                 recorder_desciption.mFramesPerPacket
                 );
        
        if( noErr == result )
        {
          result =
          darwin_configure_input_audio_queue_buffer  (
                                                   &recorder_desciption,
                                                   audio_queue
                                                   );
          
          if( noErr == result )
          {
            result = AudioQueueStart( audio_queue, NULL );
            
            if( noErr == result )
            {
              CFRunLoopRunInMode  (
                                   kCFRunLoopDefaultMode,
                                   in_record_time,
                                   CPC_FALSE
                                   );
              
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
        return_value = CPC_TRUE;
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
darwin_configure_input_audio_queue_buffer  (
                                     AudioStreamBasicDescription* in_asbd,
                                     AudioQueueRef                io_audio_queue
                                         )
{
  UINT32 bytes_per_buffer = 0;
  OSStatus result         = noErr;
  
  if( NULL != in_asbd )
  {
    result =
    darwin_compute_bytes_per_buffer  (
                                   in_asbd,
                                   CAHAL_QUEUE_BUFFER_DURATION,
                                   &bytes_per_buffer
                                   );
    
    CPC_LOG (
             CPC_LOG_LEVEL_TRACE,
             "Bytes per AudioQueue buffer is 0x%x",
             bytes_per_buffer
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
darwin_compute_bytes_per_buffer  (
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
      
      result = kAudio_UnimplementedError;
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
darwin_configure_output_audio_queue_buffer  (
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
    darwin_compute_bytes_per_buffer  (
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
          darwin_playback_callback( in_playback, io_audio_queue, buffer );
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
darwin_configure_output_audio_queue (
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
                         darwin_playback_callback,
                         in_callback_info,
                         NULL,
                         kCFRunLoopCommonModes,
                         0,
                         out_audio_queue
                         );
    
    if( noErr == result )
    {
      if( NULL != in_device->device_uid )
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
darwin_configure_input_audio_queue (
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
                         darwin_recorder_callback,
                         in_callback_info,
                         NULL,
                         kCFRunLoopCommonModes,
                         0,
                         out_audio_queue
                         );
    
    if( noErr == result )
    {
      if( NULL != in_device->device_uid )
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
                               kAudioQueueProperty_StreamDescription,
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
darwin_configure_asbd  (
                     cahal_audio_format_id          in_format_id,
                     UINT32                         in_number_of_channels,
                     FLOAT64                        in_sample_rate,
                     UINT32                         in_bit_depth,
                     cahal_audio_format_flag        in_format_flags,
                     cahal_device_stream_direction  in_direction,
                     AudioStreamBasicDescription    *out_asbd
                     )
{
  OSStatus result       = kAudio_UnimplementedError;
  UINT32 property_size  = sizeof( AudioStreamBasicDescription );
  
  if( NULL != out_asbd )
  {
    out_asbd->mChannelsPerFrame = in_number_of_channels;
    out_asbd->mSampleRate       = in_sample_rate;
    out_asbd->mBitsPerChannel   = in_bit_depth;
    out_asbd->mFormatFlags      =
    darwin_convert_cahal_audio_format_flags_to_core_audio_format_flags (
                                                                 in_format_flags
                                                                     );
    out_asbd->mFormatID         =
    darwin_convert_cahal_audio_format_id_to_core_audio_format_id  (
                                                                   in_format_id
                                                                   );
    
    CPC_LOG (
             CPC_LOG_LEVEL_TRACE,
             "ASDB Info: sr=%.2f, nc=0x%x, bd=0x%x, bpf=0x%x"
             ", bpp=0x%x, fpp=0x%x",
             out_asbd->mSampleRate,
             out_asbd->mChannelsPerFrame,
             out_asbd->mBitsPerChannel,
             out_asbd->mBytesPerFrame,
             out_asbd->mBytesPerPacket,
             out_asbd->mFramesPerPacket
             );
    
    CPC_PRINT_CODE( CPC_LOG_LEVEL_TRACE, out_asbd->mFormatID );
    
    result =
    AudioFormatGetProperty  (
                             kAudioFormatProperty_FormatInfo,
                             0,
                             NULL,
                             &property_size,
                             out_asbd
                             );
    
    CPC_LOG (
             CPC_LOG_LEVEL_TRACE,
             "ASDB Info: sr=%.2f, nc=0x%x, bd=0x%x, bpf=0x%x"
             ", bpp=0x%x, fpp=0x%x",
             out_asbd->mSampleRate,
             out_asbd->mChannelsPerFrame,
             out_asbd->mBitsPerChannel,
             out_asbd->mBytesPerFrame,
             out_asbd->mBytesPerPacket,
             out_asbd->mFramesPerPacket
             );
    
    CPC_PRINT_CODE( CPC_LOG_LEVEL_TRACE, out_asbd->mFormatID );

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

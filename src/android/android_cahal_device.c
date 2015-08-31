/*! \file   android_cahal_device.c

    \author Brent Carrara
 */
#include "android/android_cahal_device.h"

cahal_recorder_info* g_recorder_callback_info = NULL;
cahal_playback_info* g_playback_callback_info = NULL;

/*! \fn     cpc_error_code android_configure_volume_level  (
              FLOAT32                         in_volume,
              SLObjectItf*                    io_playback_object
                                                            )
    \brief  Sets the volume of the playback object to in_volume.

    \param  in_volume Volume to set the playback device to. This is a value
                      between 0 and 1 (inclusive). This function handles
                      converting the in_volume parameter to millibels.
    \param  in_playback_object  OpenSLES interface to the playback device.
    \return Error code upon error, CPC_ERRROR_CODE_NO_ERROR upon success.
 */
cpc_error_code
android_configure_volume_level  (
    FLOAT32                         in_volume,
    SLObjectItf*                    in_playback_object
                                );

/*! \fn     void android_free_callback_buffers (
              android_callback_info*  io_callback_info
            )

    \brief  Safely frees all members of the callback struct.

    \param  io_callback_info  Struct whos members need to be freed.
 */
void
android_free_callback_buffers (
    android_callback_info*  io_callback_info
                              );

CPC_BOOL
cahal_start_playback  (
                       cahal_device*            in_device,
                       cahal_audio_format_id    in_format_id,
                       UINT32                   in_number_of_channels,
                       FLOAT64                  in_sample_rate,
                       UINT32                   in_bit_depth,
                       FLOAT32                  in_volume,
                       cahal_playback_callback  in_playback,
                       void*                    in_callback_user_data,
                       cahal_audio_format_flag  in_format_flags
                       )
{
  CPC_BOOL return_value = CPC_FALSE;

  SLDataFormat_PCM*             audio_format        = NULL;
  SLDataSource*                 output_source       = NULL;
  SLDataSink*                   output_sink         = NULL;
  SLObjectItf                   playback_object     = NULL;
  SLPlayItf                     playback_interface  = NULL;
  SLAndroidSimpleBufferQueueItf buffer_interface    = NULL;

  CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "In start playback!" );

  if( 0.0 > in_volume || 1.0 < in_volume )
  {
    CPC_ERROR( "Volume (%.02f) must be in the range [ 0, 1 ].", in_volume );
  }
  else if  (
       cahal_test_device_direction_support  (
                                             in_device,
                                             CAHAL_DEVICE_OUTPUT_STREAM
                                             )
       && CAHAL_STATE_INITIALIZED == g_cahal_state
       )
  {
    if  (
        CPC_ERROR_CODE_NO_ERROR ==
            cpc_safe_malloc (
                ( void** ) &audio_format, sizeof( SLDataFormat_PCM )
                            )
        && CPC_ERROR_CODE_NO_ERROR ==
            cpc_safe_malloc( ( void** ) &output_source, sizeof( SLDataSource ) )
        && CPC_ERROR_CODE_NO_ERROR ==
            cpc_safe_malloc( ( void** ) &output_sink, sizeof( SLDataSink ) )
        )
    {
      cpc_error_code result =
          android_initialize_playback_structs  (
              in_number_of_channels,
              in_sample_rate,
              in_bit_depth,
              audio_format,
              output_source,
              output_sink
              );

      if( CPC_ERROR_CODE_NO_ERROR == result )
      {
        result =
            android_register_playback (
              in_device,
              in_volume,
              output_source,
              output_sink,
              &playback_object,
              &playback_interface,
              &buffer_interface
              );

        if( CPC_ERROR_CODE_NO_ERROR == result )
        {
          result =
            android_register_playback_callback  (
                in_device,
                in_playback,
                in_callback_user_data,
                playback_interface,
                buffer_interface,
                &g_playback_callback_info
                );

          if( CPC_ERROR_CODE_NO_ERROR == result )
          {
            CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Registered callback" );

            result =
                android_enqueue_playback_buffers  (
                    audio_format,
                    buffer_interface,
                    g_playback_callback_info
                    );

            if( CPC_ERROR_CODE_NO_ERROR == result )
            {
              CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Enqueued buffers." );

              SLresult opensl_result =
              ( *playback_interface )->SetPlayState (
                  playback_interface,
                  SL_PLAYSTATE_PLAYING
                  );

              if( SL_RESULT_SUCCESS == opensl_result )
              {
                android_save_playback_context  (
                    audio_format,
                    output_source,
                    output_sink,
                    playback_object,
                    playback_interface,
                    buffer_interface
                    );

                return_value = CPC_TRUE;
              }
            }
            else
            {
              CPC_ERROR (
                  "Could not enqueue buffers: %d.",
                  result
                  );
            }
          }
          else
          {
            CPC_ERROR (
                "Could not register callback: %d.",
                result
                );
          }
        }
        else
        {
          CPC_ERROR (
              "Could not register recorder: %d.",
              result
              );
        }
      }
      else
      {
        CPC_ERROR( "Could not initialize structures: %d.", result );
      }
    }
    else
    {
      CPC_LOG_STRING  (
          CPC_LOG_LEVEL_ERROR,
          "Could not malloc platform structs"
          );
    }
  }

  return( return_value );
}

cpc_error_code
android_initialize_playback_structs  (
    UINT32            in_number_of_channels,
    FLOAT64           in_sample_rate,
    UINT32            in_bit_depth,
    SLDataFormat_PCM* out_audio_format,
    SLDataSource*     out_output_source,
    SLDataSink*       out_output_sink
                                      )
{
  UINT32 int_sample_rate = in_sample_rate * 1000;

  cpc_error_code result =
      android_set_audio_format_struct (
          in_number_of_channels,
          int_sample_rate,
          in_bit_depth,
          out_audio_format
                                      );

  if( CPC_ERROR_CODE_NO_ERROR == result )
  {
    result = android_create_output_source (
        out_audio_format,
        out_output_source
        );

    if( CPC_ERROR_CODE_NO_ERROR == result )
    {
      result = android_create_output_sink( out_output_sink );

      if( CPC_ERROR_CODE_NO_ERROR != result )
      {
        CPC_ERROR( "Could not create output sink: %d.", result );
      }
    }
    else
    {
      CPC_ERROR( "Could not create output source: %d.", result );
    }
  }
  else
  {
    CPC_ERROR( "Could not initialize format struct: %d.", result );
  }

  return( result );
}

void
android_free_callback_buffers (
    android_callback_info*  io_callback_info
                              )
{
  if( NULL != io_callback_info )
  {
    for( UINT32 i = 0; i < io_callback_info->number_of_buffers; i++ )
    {
      if( NULL != io_callback_info->buffers[ i ] )
      {
        CPC_LOG (
            CPC_LOG_LEVEL_TRACE,
            "Freeing buffer=0x%x",
            io_callback_info->buffers[ i ]
            );

        cpc_safe_free( ( void** ) &( io_callback_info->buffers[ i ] ) );
      }
    }
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Received null callback info." );
  }
}

CPC_BOOL
cahal_stop_recording( void )
{
  CPC_BOOL result = CPC_FALSE;

  if  (
        CAHAL_STATE_INITIALIZED == g_cahal_state
        && NULL != g_recorder_callback_info
      )
  {
    android_callback_info* callback_info =
        ( android_callback_info* ) g_recorder_callback_info->platform_data;

    if( NULL != callback_info )
    {
      android_recorder_context* context =
          ( android_recorder_context* ) callback_info->context;

      if( NULL != context )
      {
        CPC_LOG (
            CPC_LOG_LEVEL_TRACE,
            "Stopping recording. ci=0x%x, pd=0x%x, ctx=0x%x",
            g_recorder_callback_info,
            callback_info,
            context
            );

        SLresult opensl_result =
            ( *( context->recorder_interface ) )->SetRecordState (
                context->recorder_interface,
                SL_RECORDSTATE_STOPPED
                );

        if( SL_RESULT_SUCCESS == opensl_result )
        {
          CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "Stopped recording" );

          opensl_result =
              ( *( context->buffer_interface ) )->Clear (
                  context->buffer_interface
                  );

          if( SL_RESULT_SUCCESS == opensl_result )
          {
            CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Cleared buffer queue" );
          }
        }
        else
        {
          CPC_ERROR( "Could not stop recording: %d.", result );
        }

//        ( *( context->recorder_object ) )->Destroy( context->recorder_object );

        CPC_LOG (
            CPC_LOG_LEVEL_TRACE,
            "Freeing. af=0x%x, is->pL=0x%x, is=0x%x, isi->pL=0x%x,"
            " isi=0x%x, ctx=0x%x, cbi=0x%x",
            context->audio_format,
            context->input_source->pLocator,
            context->input_source,
            context->input_sink->pLocator,
            context->input_sink,
            context,
            callback_info
            );

        cpc_safe_free( ( void** ) &( context->audio_format ) );

        cpc_safe_free( &( context->input_source->pLocator ) );
        cpc_safe_free( ( void** ) &( context->input_source ) );

        cpc_safe_free( &( context->input_sink->pLocator ) );
        cpc_safe_free( ( void** ) &( context->input_sink ) );

        cpc_safe_free( ( void** ) &context );
      }
      else
      {
        CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Context is null." );
      }

      android_free_callback_buffers( callback_info );

      cpc_safe_free( ( void** ) &( callback_info ) );
    }
    else
    {
      CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Callback info is null." );
    }

    cpc_safe_free( ( void** ) &( g_recorder_callback_info ) );

    result = CPC_TRUE;
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Recorder info is null." );
  }

  return( result );
}

CPC_BOOL
cahal_stop_playback( void )
{
  CPC_BOOL result = CPC_FALSE;

  if  (
        CAHAL_STATE_INITIALIZED == g_cahal_state
        && NULL != g_playback_callback_info
      )
  {
    android_callback_info* callback_info =
        ( android_callback_info* ) g_playback_callback_info->platform_data;

    if( NULL != callback_info )
    {
      android_playback_context* context =
          ( android_playback_context* ) callback_info->context;

      if( NULL != context )
      {
        CPC_LOG (
            CPC_LOG_LEVEL_TRACE,
            "Stopping recording. ci=0x%x, pd=0x%x, ctx=0x%x",
            g_playback_callback_info,
            callback_info,
            context
            );

        SLresult opensl_result =
            ( *( context->playback_interface ) )->SetPlayState (
                context->playback_interface,
                SL_PLAYSTATE_STOPPED
                );

        if( SL_RESULT_SUCCESS == opensl_result )
        {
          CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "Stopped playback" );

          opensl_result =
              ( *( context->buffer_interface ) )->Clear (
                  context->buffer_interface
                  );

          if( SL_RESULT_SUCCESS == opensl_result )
          {
            CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Cleared buffer queue" );
          }
        }
        else
        {
          CPC_ERROR( "Could not stop playback: %d.", result );
        }

        ( *( context->playback_object ) )->Destroy( context->playback_object );

        CPC_LOG (
            CPC_LOG_LEVEL_TRACE,
            "Freeing. af=0x%x, is->pL=0x%x, is=0x%x, isi->pL=0x%x,"
            " isi=0x%x, ctx=0x%x, cbi=0x%x",
            context->audio_format,
            context->input_source->pLocator,
            context->input_source,
            context->input_sink->pLocator,
            context->input_sink,
            context,
            callback_info
            );

        cpc_safe_free( ( void** ) &( context->audio_format ) );

        cpc_safe_free( &( context->input_source->pLocator ) );
        cpc_safe_free( ( void** ) &( context->input_source ) );

        cpc_safe_free( &( context->input_sink->pLocator ) );
        cpc_safe_free( ( void** ) &( context->input_sink ) );

        cpc_safe_free( ( void** ) &context );
      }
      else
      {
        CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Context is null." );
      }

      android_free_callback_buffers( callback_info );

      cpc_safe_free( ( void** ) &( callback_info ) );

      result = CPC_TRUE;
    }
    else
    {
      CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Callback info is null." );
    }

    cpc_safe_free( ( void** ) &( g_playback_callback_info ) );
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Playback info is null." );
  }

  return( result );
}

cpc_error_code
android_initialize_recording_structs  (
    UINT32            in_number_of_channels,
    FLOAT64           in_sample_rate,
    UINT32            in_bit_depth,
    SLDataFormat_PCM* out_audio_format,
    SLDataSource*     out_input_source,
    SLDataSink*       out_input_sink
                                      )
{
  UINT32 int_sample_rate = in_sample_rate * 1000;

  cpc_error_code result =
      android_set_audio_format_struct (
          in_number_of_channels,
          int_sample_rate,
          in_bit_depth,
          out_audio_format
                                      );

  if( CPC_ERROR_CODE_NO_ERROR == result )
  {
    result = android_create_input_source( out_input_source );

    if( CPC_ERROR_CODE_NO_ERROR == result )
    {
      result = android_create_input_sink( out_audio_format, out_input_sink );

      if( CPC_ERROR_CODE_NO_ERROR != result )
      {
        CPC_ERROR( "Could not create input sink: %d.", result );
      }
    }
    else
    {
      CPC_ERROR( "Could not create input source: %d.", result );
    }
  }
  else
  {
    CPC_ERROR( "Could not initialize format struct: %d.", result );
  }

  return( result );
}

cpc_error_code
android_configure_volume_level  (
    FLOAT32                         in_volume,
    SLObjectItf*                    in_playback_object
                                )
{
  cpc_error_code result = CPC_ERROR_CODE_API_ERROR;

  SLVolumeItf volume_interface;
  SLmillibel max_volume;
  SLmillibel current_volume;

  SLmillibel min_volume = SL_MILLIBEL_MIN;

  SLmillibel new_volume = SL_MILLIBEL_MIN;

  if( 0.0 != in_volume )
  {
    new_volume = ( SLmillibel ) ( 1000 * CPC_LOG_10_FLOAT32( in_volume ) );
  }

  CPC_LOG (
      CPC_LOG_LEVEL_TRACE,
      "Converted volume %.02f to %d.",
      in_volume,
      new_volume
          );

  SLresult opensl_result =
      ( **in_playback_object )->GetInterface  (
          *in_playback_object,
          SL_IID_VOLUME,
          &volume_interface
          );

  if( SL_RESULT_SUCCESS == opensl_result )
  {
    result = CPC_ERROR_CODE_NO_ERROR;

    ( *volume_interface )->GetMaxVolumeLevel( volume_interface, &max_volume );
    ( *volume_interface )->GetVolumeLevel( volume_interface, &current_volume );

    CPC_LOG (
        CPC_LOG_LEVEL_TRACE,
        "Volume range is [%d,%d].",
        min_volume,
        max_volume
            );

    CPC_LOG (
        CPC_LOG_LEVEL_TRACE,
        "Current volume is %d.",
        current_volume
            );

    ( *volume_interface )->SetVolumeLevel( volume_interface, new_volume );
    ( *volume_interface )->GetVolumeLevel( volume_interface, &current_volume );

    CPC_LOG (
        CPC_LOG_LEVEL_TRACE,
        "Set volume to %d. Current volume is %d.",
        new_volume,
        current_volume
            );
  }
  else
  {
    CPC_ERROR (
        "Could not get volume interface: %d.",
        opensl_result
        );
  }

  return( result );
}

cpc_error_code
android_register_playback (
    cahal_device*                   in_device,
    FLOAT32                         in_volume,
    SLDataSource*                   in_output_source,
    SLDataSink*                     in_output_sink,
    SLObjectItf*                    io_playback_object,
    SLPlayItf*                      out_playback_interface,
    SLAndroidSimpleBufferQueueItf*  out_buffer_interface
                          )
{
  cpc_error_code result = CPC_ERROR_CODE_API_ERROR;

  SLboolean interface_requirements[ 3 ] =
  {
      SL_BOOLEAN_TRUE,
      SL_BOOLEAN_TRUE,
      SL_BOOLEAN_TRUE
  };
  SLInterfaceID interface_ids[ 3 ]      =
  {
      SL_IID_PLAY,
      SL_IID_VOLUME,
      SL_IID_ANDROIDSIMPLEBUFFERQUEUE
  };

  CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Creating playback" );
  CPC_LOG (
      CPC_LOG_LEVEL_DEBUG,
      "source=0x%x, sink=0x%x, engine=0x%x",
      in_output_source,
      in_output_sink,
      g_engine_interface
      );

  SLresult opensl_result =
      ( *g_engine_interface )->CreateAudioPlayer  (
          g_engine_interface,
          io_playback_object,
          in_output_source,
          in_output_sink,
          3,
          interface_ids,
          interface_requirements
                                                  );

  if( SL_RESULT_SUCCESS == opensl_result )
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Created Audio Player" );

    opensl_result =
        ( **io_playback_object )->Realize (
            *io_playback_object,
            SL_BOOLEAN_FALSE
            );

    if( SL_RESULT_SUCCESS == opensl_result )
    {
      CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Realized player" );

      opensl_result =
          ( **io_playback_object )->GetInterface  (
              *io_playback_object,
              SL_IID_PLAY,
              out_playback_interface
              );

      if( SL_RESULT_SUCCESS == opensl_result )
      {
        CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Got playback interface" );

        opensl_result =
            ( **io_playback_object )->GetInterface  (
                *io_playback_object,
                SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                out_buffer_interface
                );

        if( SL_RESULT_SUCCESS == opensl_result )
        {
          CPC_LOG (
              CPC_LOG_LEVEL_TRACE,
              "Got buffer interface: 0x%x",
              *out_buffer_interface
              );

          result =
              android_configure_volume_level( in_volume, io_playback_object );
        }
        else
        {
          CPC_ERROR (
              "Could not get buffer queue interface: %d.",
              opensl_result
              );
        }
      }
      else
      {
        CPC_ERROR( "Could not get recorder interface: %d.", opensl_result );
      }
    }
    else
    {
      CPC_ERROR( "Could not realize recorder: %d.", opensl_result );
    }
  }
  else
  {
    CPC_ERROR( "Could not create audio recorder: %d.", opensl_result );
  }

  return( result );
}

cpc_error_code
android_register_recorder (
    cahal_device*                   in_device,
    SLDataSource*                   in_input_source,
    SLDataSink*                     in_input_sink,
    SLObjectItf*                    io_recorder_object,
    SLRecordItf*                    out_recorder_interface,
    SLAndroidSimpleBufferQueueItf*  out_buffer_interface
                          )
{
  cpc_error_code result = CPC_ERROR_CODE_API_ERROR;

  SLboolean interface_requirements[ 3 ] =
  {
      SL_BOOLEAN_TRUE,
      SL_BOOLEAN_TRUE,
      SL_BOOLEAN_TRUE
  };
  SLInterfaceID interface_ids[ 3 ]      =
  {
      SL_IID_RECORD,
      SL_IID_ANDROIDCONFIGURATION,
      SL_IID_ANDROIDSIMPLEBUFFERQUEUE
  };

  CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Creating recorder" );
  CPC_LOG (
      CPC_LOG_LEVEL_DEBUG,
      "source=0x%x, sink=0x%x, engine=0x%x",
      in_input_source,
      in_input_sink,
      g_engine_interface
      );

  SLresult opensl_result =
      ( *g_engine_interface )->CreateAudioRecorder  (
          g_engine_interface,
          io_recorder_object,
          in_input_source,
          in_input_sink,
          3,
          interface_ids,
          interface_requirements
                                                  );

  if( SL_RESULT_SUCCESS == opensl_result )
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Created Audio Recorder" );

    opensl_result = android_set_config (
        *io_recorder_object,
        in_device->handle
        );

    if( SL_RESULT_SUCCESS == opensl_result )
    {
      CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Set configuration" );

      opensl_result =
          ( **io_recorder_object )->Realize (
              *io_recorder_object,
              SL_BOOLEAN_FALSE
              );

      if( SL_RESULT_SUCCESS == opensl_result )
      {
        CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Realized recorder" );

        opensl_result =
            ( **io_recorder_object )->GetInterface  (
                *io_recorder_object,
                SL_IID_RECORD,
                out_recorder_interface
                );

        if( SL_RESULT_SUCCESS == opensl_result )
        {
          CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Got recorder interface" );

          opensl_result =
              ( **io_recorder_object )->GetInterface  (
                  *io_recorder_object,
                  SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                  out_buffer_interface
                  );

          if( SL_RESULT_SUCCESS == opensl_result )
          {
            CPC_LOG (
                CPC_LOG_LEVEL_TRACE,
                "Got buffer interface: 0x%x",
                *out_buffer_interface
                );

            result = CPC_ERROR_CODE_NO_ERROR;
          }
          else
          {
            CPC_ERROR (
                "Could not get buffer queue interface: %d.",
                opensl_result
                );
          }
        }
        else
        {
          CPC_ERROR( "Could not get recorder interface: %d.", opensl_result );
        }
      }
      else
      {
        CPC_ERROR( "Could not realize recorder: %d.", opensl_result );
      }
    }
    else
    {
      CPC_ERROR (
          "Could not set configuration to 0x%x: %d.",
          in_device->handle,
          opensl_result
          );
    }
  }
  else
  {
    CPC_ERROR( "Could not create audio recorder: %d.", opensl_result );
  }

  return( result );
}

cpc_error_code
android_register_playback_callback  (
    cahal_device*                 in_device,
    cahal_playback_callback       in_playback,
    void*                         in_callback_user_data,
    SLPlayItf                     in_playback_interface,
    SLAndroidSimpleBufferQueueItf in_buffer_interface,
    cahal_playback_info**         out_playback_callback_info
                                    )
{
  cpc_error_code result =
      cpc_safe_malloc (
        ( void ** ) out_playback_callback_info,
        sizeof( cahal_playback_info )
        );

  if( CPC_ERROR_CODE_NO_ERROR == result )
  {
    ( *out_playback_callback_info )->playback_device    = in_device;
    ( *out_playback_callback_info )->playback_callback  = in_playback;
    ( *out_playback_callback_info )->user_data          =
        in_callback_user_data;
    ( *out_playback_callback_info )->platform_data      = NULL;

    CPC_LOG (
        CPC_LOG_LEVEL_TRACE,
        "Registering callback: buffer=0x%x, callback=0x%x, recorder=0x%x",
        in_buffer_interface,
        android_playback_callback,
        *out_playback_callback_info
        );

    SLresult opensl_result =
        ( *in_buffer_interface )->RegisterCallback  (
            in_buffer_interface,
            android_playback_callback,
            *out_playback_callback_info
            );

    if( SL_RESULT_SUCCESS == opensl_result )
    {
      CPC_LOG_STRING  (
          CPC_LOG_LEVEL_INFO,
          "Registered playback callback!"
          );

      opensl_result =
        ( *in_playback_interface )->SetCallbackEventsMask (
            in_playback_interface,
            SL_PLAYEVENT_HEADATEND
            );

      if( SL_RESULT_SUCCESS == opensl_result )
      {
        CPC_LOG (
            CPC_LOG_LEVEL_TRACE,
            "Set callback event mask to 0x%x (event_full)",
            SL_PLAYEVENT_HEADATEND
            );

        opensl_result =
            ( *in_playback_interface )->SetPlayState  (
                in_playback_interface,
                SL_PLAYSTATE_STOPPED
                );

        if( SL_RESULT_SUCCESS == opensl_result )
        {
          CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Stopped playback" );

          opensl_result =
              ( *in_buffer_interface )->Clear( in_buffer_interface );

          if( SL_RESULT_SUCCESS == opensl_result )
          {
            CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Cleared buffers" );
          }
          else
          {
            CPC_ERROR (
                "Could not clear buffer: %d.",
                opensl_result
                );

            result = CPC_ERROR_CODE_API_ERROR;
          }
        }
        else
        {
          CPC_ERROR (
              "Could not set playback state to stopped: %d.",
              opensl_result
              );

          result = CPC_ERROR_CODE_API_ERROR;
        }
      }
      else
      {
        CPC_ERROR (
            "Could not set callback event mask: %d.",
            opensl_result
            );

        result = CPC_ERROR_CODE_API_ERROR;
      }
    }
    else
    {
      CPC_ERROR (
          "Could not register callback: %d.",
          opensl_result
          );

      result = CPC_ERROR_CODE_API_ERROR;
    }
  }
  else
  {
    CPC_ERROR (
        "Could not malloc playback info: %d.",
        result
        );

    result = CPC_ERROR_CODE_API_ERROR;
  }

  return( result );
}

cpc_error_code
android_register_recorder_callback  (
    cahal_device*                 in_device,
    cahal_recorder_callback       in_recorder,
    void*                         in_callback_user_data,
    SLRecordItf                   in_recorder_interface,
    SLAndroidSimpleBufferQueueItf in_buffer_interface,
    cahal_recorder_info**         out_recorder_callback_info
                                    )
{
  cpc_error_code result =
      cpc_safe_malloc (
        ( void ** ) out_recorder_callback_info,
        sizeof( cahal_recorder_info )
        );

  if( CPC_ERROR_CODE_NO_ERROR == result )
  {
    ( *out_recorder_callback_info )->recording_device    = in_device;
    ( *out_recorder_callback_info )->recording_callback  = in_recorder;
    ( *out_recorder_callback_info )->user_data           =
        in_callback_user_data;
    ( *out_recorder_callback_info )->platform_data       = NULL;

    CPC_LOG (
        CPC_LOG_LEVEL_TRACE,
        "Registering callback: buffer=0x%x, callback=0x%x, recorder=0x%x",
        in_buffer_interface,
        android_recorder_callback,
        *out_recorder_callback_info
        );

    SLresult opensl_result =
        ( *in_buffer_interface )->RegisterCallback  (
            in_buffer_interface,
            android_recorder_callback,
            *out_recorder_callback_info
            );

    if( SL_RESULT_SUCCESS == opensl_result )
    {
      CPC_LOG_STRING  (
          CPC_LOG_LEVEL_INFO,
          "Registered record callback!"
          );

      opensl_result =
        ( *in_recorder_interface )->SetCallbackEventsMask (
            in_recorder_interface,
            SL_RECORDEVENT_BUFFER_FULL
            );

      if( SL_RESULT_SUCCESS == opensl_result )
      {
        CPC_LOG (
            CPC_LOG_LEVEL_TRACE,
            "Set callback event mask to 0x%x (event_full)",
            SL_RECORDEVENT_BUFFER_FULL
            );

        opensl_result =
            ( *in_recorder_interface )->SetRecordState  (
                in_recorder_interface,
                SL_RECORDSTATE_STOPPED
                );

        if( SL_RESULT_SUCCESS == opensl_result )
        {
          CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Stopped recording" );

          opensl_result =
              ( *in_buffer_interface )->Clear( in_buffer_interface );

          if( SL_RESULT_SUCCESS == opensl_result )
          {
            CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Cleared buffers" );
          }
          else
          {
            CPC_ERROR (
                "Could not clear buffer: %d.",
                opensl_result
                );

            result = CPC_ERROR_CODE_API_ERROR;
          }
        }
        else
        {
          CPC_ERROR (
              "Could not set record state to stopped: %d.",
              opensl_result
              );

          result = CPC_ERROR_CODE_API_ERROR;
        }
      }
      else
      {
        CPC_ERROR (
            "Could not set callback event mask: %d.",
            opensl_result
            );

        result = CPC_ERROR_CODE_API_ERROR;
      }
    }
    else
    {
      CPC_ERROR (
          "Could not register callback: %d.",
          opensl_result
          );

      result = CPC_ERROR_CODE_API_ERROR;
    }
  }
  else
  {
    CPC_ERROR (
        "Could not malloc recorder info: %d.",
        result
        );

    result = CPC_ERROR_CODE_API_ERROR;
  }

  return( result );
}

cpc_error_code
android_enqueue_playback_buffers  (
    SLDataFormat_PCM*             in_audio_format,
    SLAndroidSimpleBufferQueueItf in_buffer_interface,
    cahal_playback_info*          out_playback_callback_info
                                )
{
  cpc_error_code result = CPC_ERROR_CODE_NO_ERROR;

  result =
      cpc_safe_malloc (
          &( out_playback_callback_info->platform_data ),
          sizeof( android_callback_info )
          );

  if( CPC_ERROR_CODE_NO_ERROR == result )
  {

    UINT32 buffer_size                    = 0;
    android_callback_info* callback_info  =
        ( android_callback_info* ) out_playback_callback_info->platform_data;

    callback_info->current_buffer_index = 0;
    callback_info->number_of_buffers    = CAHAL_QUEUE_NUMBER_OF_QUEUES;

    result =
        cpc_safe_malloc (
            ( void** ) &( callback_info->buffers ),
            sizeof( UCHAR* ) * callback_info->number_of_buffers
            );

    if( CPC_ERROR_CODE_NO_ERROR == result )
    {
      result =
          android_compute_bytes_per_buffer  (
              in_audio_format,
              CAHAL_QUEUE_BUFFER_DURATION,
              &buffer_size
              );

      if( CPC_ERROR_CODE_NO_ERROR == result )
      {
        UCHAR* buffer = NULL;

        callback_info->buffer_size = buffer_size;

        for( UINT32 i = 0;i < CAHAL_QUEUE_NUMBER_OF_QUEUES; i++ )
        {
          result = cpc_safe_malloc( ( void** ) &buffer, buffer_size );

          if( CPC_ERROR_CODE_NO_ERROR == result )
          {
            CPC_LOG (
                CPC_LOG_LEVEL_TRACE,
                "Calling playback callback with buffer 0x%x, size 0x%x",
                buffer,
                buffer_size
                );

            callback_info->buffers[ i ] = buffer;

            android_playback_callback (
                in_buffer_interface,
                out_playback_callback_info
                );
          }
          else
          {
            CPC_ERROR (
                "Could not malloc buffer: %d.",
                result
                );
          }
        }
      }
      else
      {
        CPC_ERROR (
            "Could not compute bytes per buffer: %d.",
            result
            );
      }
    }
    else
    {
      CPC_ERROR( "Could not malloc array of buffers: %d.", result );
    }
  }
  else
  {
    CPC_ERROR( "Could not malloc callback info: %d", result );
  }

  return( result );
}

cpc_error_code
android_enqueue_record_buffers  (
    SLDataFormat_PCM*             in_audio_format,
    SLAndroidSimpleBufferQueueItf in_buffer_interface,
    cahal_recorder_info*          out_recorder_callback_info
                                )
{
  cpc_error_code result = CPC_ERROR_CODE_NO_ERROR;

  result =
      cpc_safe_malloc (
          &( out_recorder_callback_info->platform_data ),
          sizeof( android_callback_info )
          );

  if( CPC_ERROR_CODE_NO_ERROR == result )
  {

    UINT32 buffer_size                    = 0;
    android_callback_info* callback_info  =
        ( android_callback_info* ) out_recorder_callback_info->platform_data;

    callback_info->current_buffer_index = 0;
    callback_info->number_of_buffers    = CAHAL_QUEUE_NUMBER_OF_QUEUES;

    result =
        cpc_safe_malloc (
            ( void** ) &( callback_info->buffers ),
            sizeof( UCHAR* ) * callback_info->number_of_buffers
            );

    if( CPC_ERROR_CODE_NO_ERROR == result )
    {
      result =
          android_compute_bytes_per_buffer  (
              in_audio_format,
              CAHAL_QUEUE_BUFFER_DURATION,
              &buffer_size
              );

      if( CPC_ERROR_CODE_NO_ERROR == result )
      {
        UCHAR* buffer = NULL;

        callback_info->buffer_size = buffer_size;

        for( UINT32 i = 0;i < CAHAL_QUEUE_NUMBER_OF_QUEUES; i++ )
        {
          result = cpc_safe_malloc( ( void** ) &buffer, buffer_size );

          if( CPC_ERROR_CODE_NO_ERROR == result )
          {
            CPC_LOG (
                CPC_LOG_LEVEL_TRACE,
                "Enqueuing buffers of size 0x%x (0x%x)",
                buffer_size,
                buffer
                );

            SLresult opensl_result =
                ( *in_buffer_interface )->Enqueue (
                    in_buffer_interface,
                    buffer,
                    buffer_size
                    );

            if( SL_RESULT_SUCCESS == opensl_result )
            {
              CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Enqueued buffer" );

              callback_info->buffers[ i ] = buffer;
            }
            else
            {
              CPC_ERROR (
                  "Could not enqueue buffer: %d.",
                  opensl_result
                  );

              result = CPC_ERROR_CODE_API_ERROR;
            }
          }
          else
          {
            CPC_ERROR (
                "Could not malloc buffer: %d.",
                result
                );
          }
        }
      }
      else
      {
        CPC_ERROR (
            "Could not compute bytes per buffer: %d.",
            result
            );
      }
    }
    else
    {
      CPC_ERROR( "Could not malloc array of buffers: %d.", result );
    }
  }
  else
  {
    CPC_ERROR( "Could not malloc callback info: %d", result );
  }

  return( result );
}

cpc_error_code
android_compute_bytes_per_buffer  (
    SLDataFormat_PCM* in_audio_format,
    FLOAT32           in_number_of_seconds,
    UINT32*           out_bytes_per_buffer
                                  )
{
  cpc_error_code result = CPC_ERROR_CODE_NO_ERROR;

  if( NULL != in_audio_format )
  {

    UINT32 num_frames =
        in_number_of_seconds
        * ( in_audio_format->samplesPerSec / 1000 );

    *out_bytes_per_buffer =
        num_frames
        * in_audio_format->numChannels
        * ( in_audio_format->containerSize / 8 );

    CPC_LOG (
        CPC_LOG_LEVEL_TRACE,
        "Computing buffer sizes: sr=0x%x, nc=0x%x, cs=0x%x,"
        " nf=0x%x, bpb=0x%x",
        ( in_audio_format->samplesPerSec / 1000 ),
        in_audio_format->numChannels,
        in_audio_format->containerSize,
        num_frames,
        *out_bytes_per_buffer
        );
  }
  else
  {
    result = CPC_ERROR_CODE_NULL_POINTER;
  }

  return( result );
}

cpc_error_code
android_save_playback_context  (
    SLDataFormat_PCM*             in_audio_format,
    SLDataSource*                 in_input_source,
    SLDataSink*                   in_input_sink,
    SLObjectItf                   in_playback_object,
    SLPlayItf                     in_playback_interface,
    SLAndroidSimpleBufferQueueItf in_buffer_interface
                                )
{
  cpc_error_code result = CPC_ERROR_CODE_NO_ERROR;

  if  (
      NULL != g_playback_callback_info
      && NULL != g_playback_callback_info->platform_data
      )
  {
    android_callback_info* platform_data =
        ( android_callback_info* ) g_playback_callback_info->platform_data;

    result =
        cpc_safe_malloc (
            ( void** ) &( platform_data->context ),
            sizeof( android_playback_context )
            );

    if( CPC_ERROR_CODE_NO_ERROR == result )
    {
      android_playback_context* context =
          ( android_playback_context* ) platform_data->context;

      context->audio_format       = in_audio_format;
      context->input_sink         = in_input_sink;
      context->input_source       = in_input_source;
      context->playback_object    = in_playback_object;
      context->playback_interface = in_playback_interface;
      context->buffer_interface   = in_buffer_interface;
    }
  }

  return( result );
}

cpc_error_code
android_save_recording_context  (
    SLDataFormat_PCM*             in_audio_format,
    SLDataSource*                 in_input_source,
    SLDataSink*                   in_input_sink,
    SLObjectItf                   in_recorder_object,
    SLRecordItf                   in_recorder_interface,
    SLAndroidSimpleBufferQueueItf in_buffer_interface
                                )
{
  cpc_error_code result = CPC_ERROR_CODE_NO_ERROR;

  if  (
      NULL != g_recorder_callback_info
      && NULL != g_recorder_callback_info->platform_data
      )
  {
    android_callback_info* platform_data =
        ( android_callback_info* ) g_recorder_callback_info->platform_data;

    result =
        cpc_safe_malloc (
            ( void** ) &( platform_data->context ),
            sizeof( android_recorder_context )
            );

    if( CPC_ERROR_CODE_NO_ERROR == result )
    {
      android_recorder_context* context =
          ( android_recorder_context* ) platform_data->context;

      context->audio_format       = in_audio_format;
      context->input_sink         = in_input_sink;
      context->input_source       = in_input_source;
      context->recorder_object    = in_recorder_object;
      context->recorder_interface = in_recorder_interface;
      context->buffer_interface   = in_buffer_interface;
    }
  }

  return( result );
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
                       cahal_audio_format_flag  in_format_flags
                       )
{
  CPC_BOOL return_value = CPC_FALSE;

  SLDataFormat_PCM*             audio_format        = NULL;
  SLDataSource*                 input_source        = NULL;
  SLDataSink*                   input_sink          = NULL;
  SLObjectItf                   recorder_object     = NULL;
  SLRecordItf                   recorder_interface  = NULL;
  SLAndroidSimpleBufferQueueItf buffer_interface    = NULL;

  CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "In start recording!" );

  if  (
       cahal_test_device_direction_support  (
                                             in_device,
                                             CAHAL_DEVICE_INPUT_STREAM
                                             )
       && CAHAL_STATE_INITIALIZED == g_cahal_state
       && NULL == g_recorder_callback_info
       )
  {
    if  (
        CPC_ERROR_CODE_NO_ERROR ==
            cpc_safe_malloc (
                ( void** ) &audio_format, sizeof( SLDataFormat_PCM )
                            )
        && CPC_ERROR_CODE_NO_ERROR ==
            cpc_safe_malloc( ( void** ) &input_source, sizeof( SLDataSource ) )
        && CPC_ERROR_CODE_NO_ERROR ==
            cpc_safe_malloc( ( void** ) &input_sink, sizeof( SLDataSink ) )
        )
    {
      cpc_error_code result =
          android_initialize_recording_structs  (
              in_number_of_channels,
              in_sample_rate,
              in_bit_depth,
              audio_format,
              input_source,
              input_sink
              );

      if( CPC_ERROR_CODE_NO_ERROR == result )
      {
        result =
          android_register_recorder (
              in_device,
              input_source,
              input_sink,
              &recorder_object,
              &recorder_interface,
              &buffer_interface
              );

        if( CPC_ERROR_CODE_NO_ERROR == result )
        {
          result =
            android_register_recorder_callback  (
                in_device,
                in_recorder,
                in_callback_user_data,
                recorder_interface,
                buffer_interface,
                &g_recorder_callback_info
                );

          if( CPC_ERROR_CODE_NO_ERROR == result )
          {
            CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Registered callback" );

            result =
                android_enqueue_record_buffers  (
                    audio_format,
                    buffer_interface,
                    g_recorder_callback_info
                    );

            if( CPC_ERROR_CODE_NO_ERROR == result )
            {
              SLresult opensl_result =
              ( *recorder_interface )->SetRecordState (
                  recorder_interface,
                  SL_RECORDSTATE_RECORDING
                  );

              if( SL_RESULT_SUCCESS == opensl_result )
              {
                android_save_recording_context  (
                    audio_format,
                    input_source,
                    input_sink,
                    recorder_object,
                    recorder_interface,
                    buffer_interface
                    );

                return_value = CPC_TRUE;
              }
            }
            else
            {
              CPC_ERROR (
                  "Could not enqueue buffers: %d.",
                  result
                  );
            }
          }
          else
          {
            CPC_ERROR (
                "Could not register callback: %d.",
                result
                );
          }
        }
        else
        {
          CPC_ERROR (
              "Could not register recorder: %d.",
              result
              );
        }
      }
      else
      {
        CPC_ERROR( "Could not initialize structures: %d.", result );
      }
    }
    else
    {
      CPC_LOG_STRING  (
          CPC_LOG_LEVEL_ERROR,
          "Could not malloc platform structs"
          );
    }
  }

  return( return_value );
}

void
android_playback_callback (
    SLAndroidSimpleBufferQueueItf in_playback_buffer,
    void*                         in_user_data
                          )
{
  CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "In android playback callback!" );

  if( NULL != in_user_data )
  {
    cahal_playback_info* callback_info    =
        ( cahal_playback_info* ) in_user_data;
    android_callback_info* platform_info  =
        ( android_callback_info* ) callback_info->platform_data;

    CPC_LOG (
        CPC_LOG_LEVEL_INFO,
        "in_recorder_buffer=0x%x, callback_info=0x%x, platform_info=0x%x",
        in_playback_buffer,
        callback_info,
        platform_info
        );

    if( NULL != platform_info )
    {
      UINT32 buffer_size = platform_info->buffer_size;

      CPC_LOG (
          CPC_LOG_LEVEL_INFO,
          "cb=0x%x, nb=0x%x, bs=0x%x, buffer=0x%x",
          platform_info->current_buffer_index,
          platform_info->number_of_buffers,
          platform_info->buffer_size,
          platform_info->buffers[ platform_info->current_buffer_index ]
          );

      CPC_LOG (
               CPC_LOG_LEVEL_TRACE,
               "Calling function at location 0x%x with user data 0x%x.",
               callback_info->playback_callback,
               callback_info->user_data
               );

      if  (
           callback_info->playback_callback (
               callback_info->playback_device,
               platform_info->buffers[ platform_info->current_buffer_index ],
               &buffer_size,
               callback_info->user_data
                                             )
           )
      {

        CPC_LOG_BUFFER  (
                         CPC_LOG_LEVEL_TRACE,
                         "Playback buffer",
                         platform_info->buffers [
                                             platform_info->current_buffer_index
                                                ],
                         80,
                         8
                         );

        SLresult opensl_result =
            ( *in_playback_buffer )->Enqueue (
                in_playback_buffer,
                platform_info->buffers[ platform_info->current_buffer_index ],
                buffer_size
                );

        if( SL_RESULT_SUCCESS == opensl_result )
        {
          CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "Enqueued buffer" );
        }
        else
        {
          CPC_LOG (
              CPC_LOG_LEVEL_WARN,
              "Could not enqueue buffer: %d.",
              opensl_result
              );
        }

        platform_info->current_buffer_index++;
        platform_info->current_buffer_index =
            (
                platform_info->current_buffer_index
                % platform_info->number_of_buffers
            );
      }
      else
      {
        CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Error returning buffer." );
      }
    }
    else
    {
      CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Null platform info" );
    }
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Null user data." );
  }
}

void
android_recorder_callback (
    SLAndroidSimpleBufferQueueItf in_recorder_buffer,
    void*                         in_user_data
                          )
{
  CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "In android recorder callback!" );

  if( NULL != in_user_data )
  {
    cahal_recorder_info* callback_info    =
        ( cahal_recorder_info* ) in_user_data;
    android_callback_info* platform_info  =
        ( android_callback_info* ) callback_info->platform_data;

    CPC_LOG (
        CPC_LOG_LEVEL_INFO,
        "in_recorder_buffer=0x%x, callback_info=0x%x, platform_info=0x%x",
        in_recorder_buffer,
        callback_info,
        platform_info
        );

    if( NULL != platform_info )
    {
      UCHAR* buffer                       = NULL;

      CPC_LOG (
          CPC_LOG_LEVEL_INFO,
          "cb=0x%x, nb=0x%x, bs=0x%x, buffer=0x%x",
          platform_info->current_buffer_index,
          platform_info->number_of_buffers,
          platform_info->buffer_size,
          platform_info->buffers[ platform_info->current_buffer_index ]
          );

      if  ( CPC_ERROR_CODE_NO_ERROR
           == cpc_safe_malloc (
                             ( void ** ) &( buffer ),
                             sizeof( UCHAR ) * platform_info->buffer_size
                               )
           )
      {
        CPC_LOG (
                 CPC_LOG_LEVEL_TRACE,
                 "Calling function at location 0x%x with user data 0x%x.",
                 callback_info->recording_callback,
                 callback_info->user_data
                 );

        memcpy  (
                 buffer,
                 platform_info->buffers[ platform_info->current_buffer_index ],
                 platform_info->buffer_size
                 );

        CPC_LOG_BUFFER  (
                         CPC_LOG_LEVEL_TRACE,
                         "Recorded buffer",
                         platform_info->buffers [
                                             platform_info->current_buffer_index
                                                ],
                         80,
                         8
                         );

        if  (
             callback_info->recording_callback (
                 callback_info->recording_device,
                 buffer,
                 platform_info->buffer_size,
                 callback_info->user_data
                                               )
             )
        {
          CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Called callback" );

          memset  (
              platform_info->buffers[ platform_info->current_buffer_index ],
              0,
              platform_info->buffer_size
              );

          SLresult opensl_result =
              ( *in_recorder_buffer )->Enqueue (
                  in_recorder_buffer,
                  platform_info->buffers[ platform_info->current_buffer_index ],
                  platform_info->buffer_size
                  );

          if( SL_RESULT_SUCCESS == opensl_result )
          {
            CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "Enqueued buffer" );
          }
          else
          {
            CPC_ERROR (
                "Could not enqueue buffer: %d.",
                opensl_result
                );
          }

          platform_info->current_buffer_index++;
          platform_info->current_buffer_index =
              (
                  platform_info->current_buffer_index
                  % platform_info->number_of_buffers
              );

          if( NULL != buffer )
          {
            cpc_safe_free( ( void** ) &buffer );
          }
        }
        else
        {
          CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Error returning buffer." );
        }
      }
    }
    else
    {
      CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Null platform info" );
    }
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Null user data." );
  }
}

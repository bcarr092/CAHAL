#include "android/android_cahal.h"

SLObjectItf g_engine_object     = NULL;
SLEngineItf g_engine_interface  = NULL;

#define NUM_CHANNELS_TO_TEST                        2
#define NUM_OPENSLES_SUPPORTED_BITS_PER_SAMPLE      6
#define NUM_OPENSLES_SUPPORTED_SAMPLE_RATES         8
#define NUM_OPENSLES_SUPPORTED_INPUT_CONFIGURATIONS 5
#define INPUT_CONFIGURATION_MAX_LENGTH              0x20
#define OUTPUT_CONFIGURATION_MAX_LENGTH             0x20

#define DEFAULT_OUTPUT_DEVICE_NAME                  "DefaultOutput"

static UINT32 opensles_supported_bits_per_sample  [
                                         NUM_OPENSLES_SUPPORTED_BITS_PER_SAMPLE
                                                   ] =
{
  SL_PCMSAMPLEFORMAT_FIXED_8,
  SL_PCMSAMPLEFORMAT_FIXED_16,
  SL_PCMSAMPLEFORMAT_FIXED_20,
  SL_PCMSAMPLEFORMAT_FIXED_24,
  SL_PCMSAMPLEFORMAT_FIXED_28,
  SL_PCMSAMPLEFORMAT_FIXED_32
};

static UINT32 opensles_supported_sample_rates [
                                             NUM_OPENSLES_SUPPORTED_SAMPLE_RATES
                                             ]  =
{
  SL_SAMPLINGRATE_8,
  SL_SAMPLINGRATE_11_025,
//  SL_SAMPLINGRATE_12,
  SL_SAMPLINGRATE_16,
  SL_SAMPLINGRATE_22_05,
  SL_SAMPLINGRATE_24,
  SL_SAMPLINGRATE_32,
  SL_SAMPLINGRATE_44_1,
  SL_SAMPLINGRATE_48,
//  SL_SAMPLINGRATE_64,
//  SL_SAMPLINGRATE_88_2,
//  SL_SAMPLINGRATE_96,
//  SL_SAMPLINGRATE_192
};

static UINT32 opensles_supported_input_configurations [
                                     NUM_OPENSLES_SUPPORTED_INPUT_CONFIGURATIONS
                                     ]  =
{
  SL_ANDROID_RECORDING_PRESET_NONE,
  SL_ANDROID_RECORDING_PRESET_GENERIC,
  SL_ANDROID_RECORDING_PRESET_CAMCORDER,
  SL_ANDROID_RECORDING_PRESET_VOICE_RECOGNITION,
  SL_ANDROID_RECORDING_PRESET_VOICE_COMMUNICATION,
};

static CHAR* input_configuration_strings[ 5 ] =
{
    "MicDefault",
    "MicGeneric",
    "MicCamcorder",
    "MicVoiceRecognition",
    "MicVoiceCommunication"
};

void
cahal_terminate( void )
{
  switch( g_cahal_state )
  {
    case CAHAL_STATE_NOT_INITIALIZED:
      CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "CAHAL has not been initialized" );
      break;
    case CAHAL_STATE_TERMINATED:
      CPC_LOG_STRING  (
          CPC_LOG_LEVEL_ERROR,
          "CAHAL has already been terminated"
          );
      break;
    case CAHAL_STATE_INITIALIZED:
      android_terminate();
      break;
  }
}

void
android_terminate( void )
{
  if( NULL != g_engine_object )
  {
    ( *g_engine_object )->Destroy( g_engine_object );

    g_engine_object     = NULL;
    g_engine_interface  = NULL;

    cahal_free_device_list();

    g_cahal_state = CAHAL_STATE_TERMINATED;

    CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "CAHAL has terminated" );
  }
  else
  {
    CPC_LOG_STRING  (
        CPC_LOG_LEVEL_WARN,
        "CAHAL is not initialized properly. Did you terminate already?"
        );
  }
}

void
cahal_initialize( void )
{
  if( CAHAL_STATE_NOT_INITIALIZED == g_cahal_state )
  {
    SLresult result;
    SLEngineOption engine_option;

    memset( &engine_option, 0, sizeof( SLEngineOption ) );

    engine_option.feature = SL_ENGINEOPTION_THREADSAFE;
    engine_option.data    = SL_BOOLEAN_TRUE;

    result = slCreateEngine (
        &g_engine_object,
        1,
        &engine_option,
        0,
        NULL,
        NULL
                            );

    if( SL_RESULT_SUCCESS == result )
    {
      result = ( *g_engine_object )->Realize  (
          g_engine_object,
          SL_BOOLEAN_FALSE
          );

      if( SL_RESULT_SUCCESS == result )
      {
        result =
            ( *g_engine_object )->GetInterface  (
                g_engine_object,
                SL_IID_ENGINE,
                &g_engine_interface
                                        );

        if( SL_RESULT_SUCCESS != result )
        {
          CPC_ERROR( "Error attempting to initialize interface: %d.", result );
        }
        else
        {
          CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "Initialized CAHAL." );

          g_cahal_state = CAHAL_STATE_INITIALIZED;
        }
      }
      else
      {
        CPC_ERROR( "Error attempting to realize engine: %d.", result );
      }
    }
    else
    {
      CPC_ERROR( "Error attempting to create engine: %d.", result );
    }
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_WARN, "CAHAL is already intialized" );
  }
}

cpc_error_code
android_create_output_sink  (
    SLDataSink* out_output_sink
                            )
{
  cpc_error_code return_result  = CPC_ERROR_CODE_NO_ERROR;
  SLObjectItf output_mix        = NULL;

  SLresult result =
      ( *g_engine_interface )->CreateOutputMix  (
          g_engine_interface,
          &output_mix,
          0,
          NULL,
          NULL
          );

  if( SL_RESULT_SUCCESS == result )
  {
    result = ( *output_mix )->Realize( output_mix, SL_BOOLEAN_FALSE );

    if( SL_RESULT_SUCCESS == result )
    {
      SLDataLocator_OutputMix* output_locator = NULL;

      return_result =
          cpc_safe_malloc (
              ( void** ) &output_locator,
              sizeof( SLDataLocator_OutputMix)
              );

      if( CPC_ERROR_CODE_NO_ERROR == return_result )
      {
        output_locator->locatorType = SL_DATALOCATOR_OUTPUTMIX;
        output_locator->outputMix   = output_mix;

        out_output_sink->pLocator = output_locator;
        out_output_sink->pFormat  = NULL;
      }
      else
      {
        CPC_ERROR( "Error mallocing output_locator: %d.", return_result );
      }
    }
    else
    {
      CPC_ERROR( "Error realizing output mix: %d.", result );

      return_result = CPC_ERROR_CODE_API_ERROR;
    }
  }
  else
  {
    CPC_ERROR( "Error creating output mix: %d.", result );

    return_result = CPC_ERROR_CODE_API_ERROR;
  }

  if( CPC_ERROR_CODE_NO_ERROR != result && NULL != output_mix )
  {
    ( *output_mix )->Destroy( output_mix );
  }

  return( return_result );
}

cpc_error_code
android_create_input_source (
    SLDataSource *out_input_source
                            )
{
  cpc_error_code result = CPC_ERROR_CODE_NO_ERROR;

  if( NULL != out_input_source )
  {
    SLDataLocator_IODevice *input_locator;

    result = cpc_safe_malloc  (
        ( void** ) &input_locator,
        sizeof( SLDataLocator_IODevice )
                              );

    if( CPC_ERROR_CODE_NO_ERROR == result )
    {
      input_locator->locatorType  = SL_DATALOCATOR_IODEVICE;
      input_locator->deviceType   = SL_IODEVICE_AUDIOINPUT;
      input_locator->deviceID     = SL_DEFAULTDEVICEID_AUDIOINPUT;
      input_locator->device       = NULL;

      memset( out_input_source, 0, sizeof( SLDataSource ) );

      out_input_source->pLocator = input_locator;
      out_input_source->pFormat  = NULL;

      CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Created input source." );
    }
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Null input source." );

    result = CPC_ERROR_CODE_NULL_POINTER;
  }

  return( result );
}

cpc_error_code
android_create_output_source  (
    SLDataFormat_PCM* in_audio_format,
    SLDataSource*     out_output_source
                              )
{
  cpc_error_code result = CPC_ERROR_CODE_NO_ERROR;

  if( NULL != in_audio_format && NULL != out_output_source )
  {
    SLDataLocator_AndroidSimpleBufferQueue* buffer_locator;

    result = cpc_safe_malloc  (
        ( void** ) &buffer_locator,
        sizeof( SLDataLocator_AndroidSimpleBufferQueue )
                              );

    if( CPC_ERROR_CODE_NO_ERROR == result )
    {
      buffer_locator->locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
      buffer_locator->numBuffers  = CAHAL_QUEUE_NUMBER_OF_QUEUES;

      memset( out_output_source, 0, sizeof( SLDataSource ) );

      out_output_source->pLocator = buffer_locator;
      out_output_source->pFormat  = in_audio_format;

      CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Created input sink." );
    }
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Null audio format/input sink." );

    result = CPC_ERROR_CODE_NULL_POINTER;
  }

  return( result );
}

cpc_error_code
android_create_input_sink (
    SLDataFormat_PCM* in_audio_format,
    SLDataSink*       out_input_sink
                          )
{
  cpc_error_code result = CPC_ERROR_CODE_NO_ERROR;

  if( NULL != in_audio_format && NULL != out_input_sink )
  {
    SLDataLocator_AndroidSimpleBufferQueue* buffer_locator;

    result = cpc_safe_malloc  (
        ( void** ) &buffer_locator,
        sizeof( SLDataLocator_AndroidSimpleBufferQueue )
                              );

    if( CPC_ERROR_CODE_NO_ERROR == result )
    {
      buffer_locator->locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
      buffer_locator->numBuffers  = CAHAL_QUEUE_NUMBER_OF_QUEUES;

      memset( out_input_sink, 0, sizeof( SLDataSink ) );

      out_input_sink->pLocator = buffer_locator;
      out_input_sink->pFormat  = in_audio_format;

      CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Created input sink." );
    }
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Null audio format/input sink." );

    result = CPC_ERROR_CODE_NULL_POINTER;
  }

  return( result );
}

CPC_BOOL
android_test_output_configuration (
    SLDataFormat_PCM* in_audio_format
                                  )
{
  CPC_BOOL configuration_supported = CPC_FALSE;

  SLDataSource output_source;
  SLDataSink output_sink;

  if(
      CPC_ERROR_CODE_NO_ERROR ==
          android_create_output_source( in_audio_format, &output_source )
      && CPC_ERROR_CODE_NO_ERROR == android_create_output_sink( &output_sink ) )
  {
    SLObjectItf player_object = NULL;

    SLresult result =
        ( *g_engine_interface )->CreateAudioPlayer  (
            g_engine_interface,
            &player_object,
            &output_source,
            &output_sink,
            0,
            NULL,
            NULL
                                                    );

    if( SL_RESULT_SUCCESS == result )
    {
      result =
          ( *player_object )->Realize( player_object, SL_BOOLEAN_FALSE );

      if( SL_RESULT_SUCCESS == result )
      {
        configuration_supported = CPC_TRUE;
      }
      else
      {
        CPC_LOG (
            CPC_LOG_LEVEL_DEBUG,
            "Could not realize player: %d.",
            result
            );
      }

      ( *player_object )->Destroy( player_object );
    }
    else
    {
      CPC_LOG (
          CPC_LOG_LEVEL_DEBUG,
          "Could not create audio player: %d",
          result
          );
    }

    cpc_safe_free( &( output_source.pLocator ) );

    SLDataLocator_OutputMix* output_locator =
        ( SLDataLocator_OutputMix* ) output_sink.pLocator;

    SLObjectItf output_mix = output_locator->outputMix;

    ( *output_mix )->Destroy( output_mix );

    cpc_safe_free( &( output_sink.pLocator ) );
  }

  return( configuration_supported );
}

CPC_BOOL
android_test_input_configuration (
    SLDataFormat_PCM* in_audio_format,
    UINT32            in_configuration
                                )
{
  CPC_BOOL configuration_supported = CPC_FALSE;

  SLDataSource input_source;
  SLDataSink input_sink;

  if  (
      CPC_ERROR_CODE_NO_ERROR == android_create_input_source( &input_source )
      && CPC_ERROR_CODE_NO_ERROR ==
          android_create_input_sink( in_audio_format, &input_sink )
      )
  {
    SLObjectItf recorder_object           = NULL;
    SLboolean interface_requirements[ 1 ] =
    {
        SL_BOOLEAN_TRUE
    };
    SLInterfaceID interface_ids[ 1 ]      =
    {
        SL_IID_ANDROIDCONFIGURATION,
    };

    SLresult result =
        ( *g_engine_interface )->CreateAudioRecorder  (
            g_engine_interface,
            &recorder_object,
            &input_source,
            &input_sink,
            1,
            interface_ids,
            interface_requirements
                                                    );

    if( SL_RESULT_SUCCESS == result )
    {
      result = android_set_config( recorder_object, in_configuration );

      if( SL_RESULT_SUCCESS == result )
      {
        result =
            ( *recorder_object )->Realize( recorder_object, SL_BOOLEAN_FALSE );

        if( SL_RESULT_SUCCESS == result )
        {
          configuration_supported = CPC_TRUE;
        }
        else
        {
          CPC_LOG (
              CPC_LOG_LEVEL_DEBUG,
              "Could not realize recorder: %d.",
              result
              );
        }
      }

      CPC_LOG (
          CPC_LOG_LEVEL_TRACE, "Destroying recorder: 0x%x",
          recorder_object
          );

      ( *recorder_object )->Destroy( recorder_object );

      CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Destroyed recorder" );
    }
    else
    {
      CPC_LOG (
          CPC_LOG_LEVEL_DEBUG,
          "Could not create audio recorder: %d",
          result
          );
    }

    CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Freeing sink and source" );

    cpc_safe_free( &( input_source.pLocator ) );
    cpc_safe_free( &( input_sink.pLocator ) );

    CPC_LOG_STRING( CPC_LOG_LEVEL_TRACE, "Freed sink and source" );
  }

  return( configuration_supported );
}

SLresult
android_set_config  (
    SLObjectItf in_recorder_object,
    UINT32      in_configuration
                    )
{
  SLAndroidConfigurationItf recorder_config = NULL;

  SLresult result =
      ( *in_recorder_object )->GetInterface  (
          in_recorder_object,
          SL_IID_ANDROIDCONFIGURATION,
          &recorder_config
                                          );

  if( SL_RESULT_SUCCESS == result )
  {
    result = ( *recorder_config )->SetConfiguration (
        recorder_config,
        SL_ANDROID_KEY_RECORDING_PRESET,
        &in_configuration,
        sizeof( UINT32 )
                                                    );

    if( SL_RESULT_SUCCESS == result )
    {
      CPC_LOG (
          CPC_LOG_LEVEL_TRACE,
          "Configuration is set to 0x%x.",
          in_configuration
              );
    }
    else
    {
      CPC_LOG (
          CPC_LOG_LEVEL_WARN,
          "Could not set configuration to 0x%x: %d.",
          in_configuration,
          result
                );
    }
  }
  else
  {
    CPC_LOG (
        CPC_LOG_LEVEL_WARN,
        "Could not get configuration interface: %d.",
        result
        );
  }

  return( result );
}

cpc_error_code
android_set_audio_format_struct (
    UINT32            in_num_channels,
    UINT32            in_sample_rate,
    UINT32            in_bits_per_sample,
    SLDataFormat_PCM* out_audio_format
                                )
{
  cpc_error_code result = CPC_ERROR_CODE_NO_ERROR;

  if( NULL != out_audio_format )
  {
    UINT32 container_size = in_num_channels * in_bits_per_sample;

    if( 8 >= container_size )
      container_size = 8;
    else if( 16 >= container_size )
      container_size = 16;
    else if( 32 >= container_size )
      container_size = 32;
    else if( 64 >= container_size )
      container_size = 64;

    UINT32 channel_mask = 0;

    for( UINT32 i = 0; i < in_num_channels; i++ )
    {
      channel_mask <<= 1;
      channel_mask |= 0x00000001;
    }

    CPC_LOG (
        CPC_LOG_LEVEL_TRACE,
        "Container size is 0x%x.",
        container_size
        );

    CPC_LOG (
        CPC_LOG_LEVEL_TRACE,
        "Channel mask is 0x%x.",
        channel_mask
        );

    out_audio_format->formatType     = SL_DATAFORMAT_PCM;
    out_audio_format->numChannels    = in_num_channels;
    out_audio_format->samplesPerSec  = in_sample_rate;
    out_audio_format->bitsPerSample  = in_bits_per_sample;
    out_audio_format->containerSize  = container_size;
    out_audio_format->channelMask    = channel_mask;
    out_audio_format->endianness     = SL_BYTEORDER_LITTLEENDIAN;
  }
  else
  {
    result = CPC_ERROR_CODE_NULL_POINTER;
  }

  return( result );
}

CPC_BOOL
android_init_and_test_output_configuration (
    UINT32 in_num_channels,
    UINT32 in_sample_rate,
    UINT32 in_bits_per_sample
                                            )
{
  SLDataFormat_PCM audio_format;

  CPC_BOOL result = CPC_FALSE;

  if  (
      CPC_ERROR_CODE_NO_ERROR ==
          android_set_audio_format_struct (
              in_num_channels,
              in_sample_rate,
              in_bits_per_sample,
              &audio_format
                                          )
      )
  {
    result = android_test_output_configuration( &audio_format );
  }

  return( result );
}

CPC_BOOL
android_init_and_test_input_configuration (
    UINT32 in_num_channels,
    UINT32 in_sample_rate,
    UINT32 in_bits_per_sample,
    UINT32 in_configuration
                                    )
{
  SLDataFormat_PCM audio_format;

  CPC_BOOL result = CPC_FALSE;

  if  (
      CPC_ERROR_CODE_NO_ERROR ==
          android_set_audio_format_struct (
              in_num_channels,
              in_sample_rate,
              in_bits_per_sample,
              &audio_format
                                          )
      )
  {
    result =
        android_test_input_configuration  (
            &audio_format,
            in_configuration
        );
  }

  return( result );
}

cahal_device**
cahal_get_device_list( void )
{
  UINT32 num_devices          = 0;
  cahal_device** device_list  = NULL;

  if( CAHAL_STATE_INITIALIZED == g_cahal_state )
  {
    if( NULL != g_device_list )
    {
      CPC_LOG (
          CPC_LOG_LEVEL_DEBUG,
          "Returning existing device list: 0x%x.",
          g_device_list
          );

      device_list = g_device_list;
    }
    else
    {
      cpc_error_code result =
          android_set_input_devices( &device_list, &num_devices );

      if( CPC_ERROR_CODE_NO_ERROR == result )
      {
        result = android_set_output_device( &device_list, &num_devices );

        if( CPC_ERROR_CODE_NO_ERROR == result )
        {
          g_device_list = device_list;

          CPC_LOG (
              CPC_LOG_LEVEL_DEBUG,
              "Generated device list: 0x%x.",
              g_device_list
              );
        }
        else
        {
          CPC_ERROR( "Could not generate list of output devices: %d.", result );
        }
      }
      else
      {
        CPC_ERROR( "Could not generate list of input devices: %d.", result );
      }
    }
  }
  else
  {
    CPC_ERROR( "CAHAL has not been initialized: %d.", g_cahal_state );
  }

  return( device_list );
}

cpc_error_code
android_init_output_device_struct (
    cahal_device**        out_device,
    cahal_device_stream** out_stream
                                  )
{
  cpc_error_code result = CPC_ERROR_CODE_NO_ERROR;

  if( NULL != out_device || NULL != out_stream )
  {
    result = cpc_safe_malloc( ( void** ) out_device, sizeof( cahal_device ) );

    if( CPC_ERROR_CODE_NO_ERROR == result )
    {
      ( *out_device )->handle = ANDROID_DEVICE_HANDLE_OUTPUT;

      SSIZE configuration_string_length = OUTPUT_CONFIGURATION_MAX_LENGTH;

      result =
          cpc_strnlen (
              DEFAULT_OUTPUT_DEVICE_NAME,
              &configuration_string_length
                       );

      if( CPC_ERROR_CODE_NO_ERROR == result )
      {
        result =
            cpc_safe_malloc (
                ( void** ) &( ( *out_device )->device_name ),
                ( configuration_string_length + 1 ) * sizeof( CHAR )
                );

        if( CPC_ERROR_CODE_NO_ERROR == result )
        {
          result =
              cpc_memcpy  (
                  ( *out_device )->device_name,
                  DEFAULT_OUTPUT_DEVICE_NAME,
                  configuration_string_length
                  );

          if( CPC_ERROR_CODE_NO_ERROR == result )
          {
            result =
                cpc_safe_malloc (
                    ( void** ) out_stream,
                    sizeof( cahal_device_stream )
                    );

            if( CPC_ERROR_CODE_NO_ERROR == result )
            {
              ( *out_stream )->handle    = ANDROID_DEVICE_STREAM_DEFAULT_HANDLE;
              ( *out_stream )->direction = CAHAL_DEVICE_OUTPUT_STREAM;
              ( *out_stream )->preferred_format  =
            android_convert_android_audio_format_id_to_cahal_audio_format_id  (
                SL_DATAFORMAT_PCM
                );
            }
            else
            {
              CPC_ERROR( "Could not malloc stream: %d.", result );
            }
          }
        }
        else
        {
          CPC_ERROR( "Could not malloc device name: %d.", result );
        }
      }
      else
      {
        CPC_ERROR( "Could not get string length: %d.", result );
      }
    }
    else
    {
      CPC_ERROR( "Could not malloc device: %d.", result );
    }
  }
  else
  {
    result = CPC_ERROR_CODE_NULL_POINTER;
  }

  return( result );
}

cpc_error_code
android_init_input_device_struct  (
    UINT32                in_configuration_index,
    cahal_device**        out_device,
    cahal_device_stream** out_stream
                                  )
{
  cpc_error_code result = CPC_ERROR_CODE_NO_ERROR;

  if( NULL != out_device || NULL != out_stream )
  {
    result = cpc_safe_malloc( ( void** ) out_device, sizeof( cahal_device ) );

    if( CPC_ERROR_CODE_NO_ERROR == result )
    {
      ( *out_device )->handle =
          opensles_supported_input_configurations[ in_configuration_index ];

      SSIZE configuration_string_length = INPUT_CONFIGURATION_MAX_LENGTH;

      result =
          cpc_strnlen (
              input_configuration_strings[ in_configuration_index ],
              &configuration_string_length
                       );

      if( CPC_ERROR_CODE_NO_ERROR == result )
      {
        result =
            cpc_safe_malloc (
                ( void** ) &( ( *out_device )->device_name ),
                ( configuration_string_length + 1 ) * sizeof( CHAR )
                );

        if( CPC_ERROR_CODE_NO_ERROR == result )
        {
          result =
              cpc_memcpy  (
                  ( *out_device )->device_name,
                  input_configuration_strings[ in_configuration_index ],
                  configuration_string_length
                  );

          if( CPC_ERROR_CODE_NO_ERROR == result )
          {
            result =
                cpc_safe_malloc (
                    ( void** ) out_stream,
                    sizeof( cahal_device_stream )
                    );

            if( CPC_ERROR_CODE_NO_ERROR == result )
            {
              ( *out_stream )->handle    = ANDROID_DEVICE_STREAM_DEFAULT_HANDLE;
              ( *out_stream )->direction = CAHAL_DEVICE_INPUT_STREAM;
              ( *out_stream )->preferred_format  =
            android_convert_android_audio_format_id_to_cahal_audio_format_id  (
                SL_DATAFORMAT_PCM
                );
            }
            else
            {
              CPC_ERROR( "Could not malloc stream: %d.", result );
            }
          }
        }
        else
        {
          CPC_ERROR( "Could not malloc device name: %d.", result );
        }
      }
      else
      {
        CPC_ERROR( "Could not get string length: %d.", result );
      }
    }
    else
    {
      CPC_ERROR( "Could not malloc device: %d.", result );
    }
  }
  else
  {
    result = CPC_ERROR_CODE_NULL_POINTER;
  }

  return( result );
}

cpc_error_code
android_add_format_description  (
    UINT32                            in_bits_per_sample,
    UINT32                            in_num_channels,
    UINT32                            in_sample_rate,
    UINT32                            in_num_supported_formats,
    cahal_device_stream*              io_device_stream
                                  )
{
  cahal_audio_format_description* description;

  cpc_error_code result =
      cpc_safe_malloc (
          ( void** ) &description,
          sizeof( cahal_audio_format_description )
                      );

  if( CPC_ERROR_CODE_NO_ERROR == result )
  {
    description->bit_depth          = in_bits_per_sample;
    description->format_id          =
  android_convert_android_audio_format_id_to_cahal_audio_format_id  (
          SL_DATAFORMAT_PCM
          );
    description->number_of_channels = in_num_channels;

    description->sample_rate_range.minimum_rate =
        ( in_sample_rate / 1000.0 );
    description->sample_rate_range.maximum_rate =
        ( in_sample_rate / 1000.0 );

    result =
        cpc_safe_realloc  (
            ( void** ) &( io_device_stream->supported_formats ),
            in_num_supported_formats
              * sizeof( cahal_audio_format_description* ),
            ( in_num_supported_formats + 2 )
              * sizeof( cahal_audio_format_description* )
                          );

    if( CPC_ERROR_CODE_NO_ERROR == result )
    {
      io_device_stream->supported_formats[ in_num_supported_formats ] =
          description;
    }
    else
    {
      cpc_safe_free( ( void** ) &description );

      CPC_ERROR( "Could not realloc supported formats: %d.", result );
    }
  }
  else
  {
    CPC_ERROR( "Could not malloc description:%d.", result );
  }

  return( result );
}

cpc_error_code
android_set_output_device (
    cahal_device*** io_device_list,
    UINT32*         io_num_devices
                          )
{
  cpc_error_code result = CPC_ERROR_CODE_NO_ERROR;

  UINT32 num_supported_formats  = 0;
  cahal_device* device          = NULL;
  cahal_device_stream* stream   = NULL;

  result =
      android_init_output_device_struct  (
          &device,
          &stream
          );

  if( CPC_ERROR_CODE_NO_ERROR == result )
  {
    for (
          UINT32 num_channels = 0;
          num_channels < NUM_CHANNELS_TO_TEST;
          num_channels++
        )
    {
      for (
          UINT32 rate_index = 0;
          rate_index < NUM_OPENSLES_SUPPORTED_SAMPLE_RATES;
          rate_index++
          )
      {
        for (
            UINT32 bits_index = 0;
            bits_index < NUM_OPENSLES_SUPPORTED_BITS_PER_SAMPLE;
            bits_index++
            )
        {
          if  (
            android_init_and_test_output_configuration (
                num_channels,
                opensles_supported_sample_rates[ rate_index ],
                opensles_supported_bits_per_sample[ bits_index ]
                                                )
              )
          {
            CPC_LOG (
                CPC_LOG_LEVEL_DEBUG,
                "Supported nc=%d, sr=%d, bps=%d",
                num_channels,
                opensles_supported_sample_rates[ rate_index ],
                opensles_supported_bits_per_sample[ bits_index ]
                );

            result =
                android_add_format_description (
                    opensles_supported_bits_per_sample[ bits_index ],
                    num_channels,
                    opensles_supported_sample_rates[ rate_index ],
                    num_supported_formats,
                    stream
                    );

            if( CPC_ERROR_CODE_NO_ERROR == result )
            {
              num_supported_formats++;
            }
            else
            {
              CPC_ERROR( "Could not initialize description: %d.", result );
            }
          }
        }
      }
    }
  }
  else
  {
    CPC_ERROR( "Could not malloc device and stream: %d.", result );
  }

  if( 0 < num_supported_formats )
  {
    result =
        cpc_safe_malloc (
            ( void** ) &( device->device_streams ),
            2 * sizeof( cahal_device_stream* )
    );

    if( CPC_ERROR_CODE_NO_ERROR == result )
    {
      device->device_streams[ 0 ] = stream;

      android_add_device_to_list  (
          device,
          io_device_list,
          io_num_devices
          );
    }
    else
    {
      CPC_ERROR( "Could not malloc device streams: %d.", result );
    }
  }
  else
  {
    cpc_safe_free( ( void** ) &stream );
    cpc_safe_free( ( void** ) &device );
  }

  return( result );
}

cpc_error_code
android_set_input_devices (
    cahal_device*** io_device_list,
    UINT32*         io_num_devices
                          )
{
  cpc_error_code result = CPC_ERROR_CODE_NO_ERROR;

  for (
      UINT32 configuration_index = 0;
      configuration_index < NUM_OPENSLES_SUPPORTED_INPUT_CONFIGURATIONS;
      configuration_index++
      )
  {
    UINT32 num_supported_formats  = 0;
    cahal_device* device          = NULL;
    cahal_device_stream* stream   = NULL;

    result =
        android_init_input_device_struct  (
            configuration_index,
            &device,
            &stream
            );

    if( CPC_ERROR_CODE_NO_ERROR == result )
    {
      for (
            UINT32 num_channels = 0;
            num_channels < NUM_CHANNELS_TO_TEST;
            num_channels++
          )
      {
        for (
            UINT32 rate_index = 0;
            rate_index < NUM_OPENSLES_SUPPORTED_SAMPLE_RATES;
            rate_index++
            )
        {
          for (
              UINT32 bits_index = 0;
              bits_index < NUM_OPENSLES_SUPPORTED_BITS_PER_SAMPLE;
              bits_index++
              )
          {
            if  (
              android_init_and_test_input_configuration (
                  num_channels,
                  opensles_supported_sample_rates[ rate_index ],
                  opensles_supported_bits_per_sample[ bits_index ],
                  opensles_supported_input_configurations[ configuration_index ]
                                                  )
                )
            {
              CPC_LOG (
                  CPC_LOG_LEVEL_DEBUG,
                  "Supported nc=%d, sr=%d, bps=%d, c=%d",
                  num_channels,
                  opensles_supported_sample_rates[ rate_index ],
                  opensles_supported_bits_per_sample[ bits_index ],
                  opensles_supported_input_configurations[ configuration_index ]
                  );

              result =
                  android_add_format_description (
                      opensles_supported_bits_per_sample[ bits_index ],
                      num_channels,
                      opensles_supported_sample_rates[ rate_index ],
                      num_supported_formats,
                      stream
                      );

              if( CPC_ERROR_CODE_NO_ERROR == result )
              {
                num_supported_formats++;
              }
              else
              {
                CPC_ERROR( "Could not initialize description: %d.", result );
              }
            }
          }
        }
      }
    }
    else
    {
      CPC_ERROR( "Could not malloc device and stream: %d.", result );
    }

    if( 0 < num_supported_formats )
    {
      result =
          cpc_safe_malloc (
              ( void** ) &( device->device_streams ),
              2 * sizeof( cahal_device_stream* )
      );

      if( CPC_ERROR_CODE_NO_ERROR == result )
      {
        device->device_streams[ 0 ] = stream;

        android_add_device_to_list  (
            device,
            io_device_list,
            io_num_devices
            );
      }
      else
      {
        CPC_ERROR( "Could not realloc device list: %d.", result );
      }
    }
    else
    {
      cpc_safe_free( ( void** ) &stream );
      cpc_safe_free( ( void** ) &device );
    }
  }

  return( result );
}

cpc_error_code
android_add_device_to_list  (
    cahal_device*   in_device,
    cahal_device*** io_device_list,
    UINT32*         io_number_of_devices
                            )
{
  CPC_LOG( CPC_LOG_LEVEL_TRACE, "Reallocing: 0x%x.", io_device_list );

  cpc_error_code result =
      cpc_safe_realloc  (
          ( void** ) io_device_list,
          *io_number_of_devices * sizeof( cahal_device* ),
          ( *io_number_of_devices + 2 ) * sizeof( cahal_device* )
          );

  if( CPC_ERROR_CODE_NO_ERROR == result )
  {
    CPC_LOG (
        CPC_LOG_LEVEL_TRACE,
        "Realloc'd, device count=%d (0x%x).",
        *io_number_of_devices,
        io_device_list
        );

    ( *io_device_list )[ ( *io_number_of_devices )++ ] = in_device;
  }
  else
  {
    CPC_ERROR( "Could not realloc device list: %d.", result );
  }

  return( result );
}

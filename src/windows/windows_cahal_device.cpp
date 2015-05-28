/*! \file   windows_cahal_device.cpp
    \brief  Implementation that queries the underlying Windows hardware and
            creates the device_list struct.

    \author Brent Carrara
 */

#include "windows/windows_cahal_device.hpp"

cahal_recorder_info* g_recorder_callback_info = NULL;
cahal_playback_info* g_playback_callback_info = NULL;

HANDLE g_recorder_thread            = INVALID_HANDLE_VALUE;
HANDLE g_recorder_data_ready_event  = INVALID_HANDLE_VALUE;
HANDLE g_recorder_terminate_event   = INVALID_HANDLE_VALUE;

/*! \def    DWORD WINAPI  windows_recorder_thread_entry (
                            LPVOID in_callback_info
                                                        )
    \brief  Entry point for the recorder thread. This function will block
            on the data ready and terminate events and handle them when
            signalled. When the terminate event is signalled the thread will
            exit.

    \param  in_callback_info  Contains the callback function to call when data
                              is read (after receiving the data ready event).
    \return Exit code (always 0).
 */
DWORD
WINAPI
windows_recorder_thread_entry (
  LPVOID in_callback_info
                              );

/*! \def    UINT32  windows_recorder_read_data  (
                      IAudioCaptureClient*  in_capture_client,
                      WAVEFORMATEX*         in_format,
                      cahal_recorder_info*  in_callback_info
                                                )
    \brief  Reads a buffer full of data from the capture hardware and makes
            the data available to the callback.

    \param  in_capture_client The capture client to record the data from.
    \param  in_format The format the data is in.
    \param  in_callback_info  The callback to call with the new data.
    \return The amount of data read from in_capture_client.
 */
UINT32
windows_recorder_read_data  (
  IAudioCaptureClient*  in_capture_client,
  WAVEFORMATEX*         in_format,
  cahal_recorder_info*  in_callback_info
                            );

/*! \def    HRESULT windows_initialize_recorder_event_thread(
                      IAudioClient*         io_audio_client,
                      cahal_recorder_info*  in_callback_info
                                                            )
    \brief  Creates the recorder events (data ready, terminate), the recorder
            thread and adds the data ready event to the audio client (so it
            can be signalled when data is available).

    \param  io_audio_client The objet to set the data ready event on.
    \param  in_callback_info  The recorder thread created in this function is
                              configured with this callback.
    \return S_OK iff the audio_client is configured to call the data ready
            event. An error code otherwise.
 */
HRESULT
windows_initialize_recorder_event_thread(
  IAudioClient*         io_audio_client,
  cahal_recorder_info*  in_callback_info
                                        );

/*! \def    void  windows_handle_recorder_data(
                    cahal_recorder_info* in_callback_info
                                              )
    \brief  Called by the recording thread when signaled that a new capture
            buffer is ready. This function will get the capture client and
            read a buffer full of data from it then pass the data to the
            callback in in_callback_info.

    \param  in_callback_info  The callback to call with a buffer full of data.
 */
void
windows_handle_recorder_data(
  cahal_recorder_info* in_callback_info
                            );

/*! \def    HRESULT windows_configure_capture_device(
                      cahal_device*   in_device,
                      UINT32          in_number_of_channels,
                      FLOAT64         in_sample_rate,
                      UINT32          in_bit_depth,
                      IAudioClient**  out_audio_client
                                                    )
    \breif  Entry point to configure the WASAPI to capture audio samples. This
            function will enumerate the audio devices, select the appropriate
            device (using in_device->handle) and initialize an audio client
            to use for capture.

    \param  in_device The cahal device struct that contains the handle of the
                      capture device.
    \param  in_number_of_channels The number of channels to capture on.
    \param  in_sample_rate  The sample rate to record at.
    \param  in_bit_depth  The number of bits per sample to capture at.
    \param  out_audio_client  A created, initialized audio client that can be
                              used to read recorded data buffers.
    \return S_OK iff out_audio_client is created, activated and initialized.
            An error code otherwise.
 */
HRESULT
windows_configure_capture_device(
  cahal_device*   in_device,
  UINT32          in_number_of_channels,
  FLOAT64         in_sample_rate,
  UINT32          in_bit_depth,
  IAudioClient**  out_audio_client
                                );

/*! \def    HRESULT windows_initialize_device (
                      IMMDevice*      in_device,
                      IAudioClient**  io_audio_client,
                      WAVEFORMATEX    in_format
                                              )
    \brief  Activates a new audio client from in_device and initializes it.
            Calling reinitialize if required.

    \param  in_device The device to activate.
    \param  io_audio_client The newly created audio client, intialized from
                            in_device.
    \param  in_format The data format to configure the device to use.
    \return S_OK iff the audio_client is properly activated and initialized.
            An error code otherwise.
 */
HRESULT
windows_initialize_device (
  IMMDevice*      in_device,
  IAudioClient**  io_audio_client,
  WAVEFORMATEX    in_format
                          );

/*! \def    HRESULT windows_reinitialize_device (
                      IMMDevice*      in_device,
                      IAudioClient**  io_audio_client,
                      WAVEFORMATEX    in_format
                                                )
    \breif  Reinitializes the audio_client with a buffer size that is aligned
            to the block size in the format. This is required when devices are
            opened in exclusive mode (it is required to align the expected
            buffer size with the hardware buffer size).

    \param  in_device The endpoint device that the audio client is attached
                      to.
    \param  io_audio_client The audio client to reinitialize. Note that the
                            passed in audio client will be released and a new
                            one will be initialized.
    \param  in_format The data format that the device is configured to capture
                      in.
    \return S_OK iff the audio_client is reinitialized with the appropriate
            sized buffer. An error code otherwise.
 */
HRESULT
windows_reinitialize_device (
  IMMDevice*      in_device,
  IAudioClient**  io_audio_client,
  WAVEFORMATEX    in_format
                            );

/*! \def    HRESULT windows_set_device_info(
              cahal_device* out_device,
              IMMDevice*    in_endpoint
            );
    \brief  Sets out_device's name, ID, and state by querying the hardware
            pointed to by in_endpoint. A call is made from this function
            to set the streams. Success is returned if this device has streams.

    \param  out_device  The device whos name, ID, streams and state are to be
                        set.
    \param  in_endpoint Pointer to the underlying hardware that is to be
            queried.
    \return S_OK iff the devices streams are set correctly. An error code
            otherwise.
 */
HRESULT
windows_set_device_info(
  cahal_device* out_device,
  IMMDevice*    in_endpoint
);

/*! \def    HRESULT windows_set_device_defaults(
              cahal_device* out_device,
              IMMDevice*    in_endpoint
            );
    \brief  Sets the preferred sample rate and number of channels for the
            device pointed to by in_endpoint.

    \param  out_device  The device whos preferred sample rate and number of
                        channels is ot be set.
    \param  in_endpoint Pointer to the underlying hardware that is to be
                        queried.
    \return S_OK iff the default format for in_endpoint can be queried.
 */
HRESULT
windows_set_device_defaults(
  cahal_device* out_device,
  IMMDevice*    in_endpoint
);

/*! \def    HRESULT windows_set_device_id(
              cahal_device* out_device,
              IMMDevice*    in_endpoint
            );
    \brief  Sets the device's ID by querying the underlying hardware.

    \param  out_device  The device whos ID is to be set (this is the
                        device_uid)
    \param  in_endpoint Pointer to the underlying hardware that is to be
                        queried.
    \return S_OK iff the devices ID can be queried.
 */
HRESULT
windows_set_device_id(
  cahal_device* out_device,
  IMMDevice*    in_endpoint
);

/*! \def    HRESULT windows_set_device_state(
              cahal_device* out_device,
              IMMDevice*    in_endpoint
            );
    \brief  Sets the device's state (active or not) by querying the underlying
            hardware.

    \param  out_device  The device whos state is to be set
    \param  in_endpoint Pointer to the underlying hardware that is to be
                        queried.
    \return S_OK iff the devices state can be queried.
*/
HRESULT
windows_set_device_state(
  cahal_device* out_device,
  IMMDevice*    in_endpoint
);

/*! \def    HRESULT windows_set_device_name(
              cahal_device* out_device,
              IMMDevice*    in_endpoint
            );
    \brief  Sets the device's name by querying the underlying hardware.

    \param  out_device  The device whos name is to be sets
    \param  in_endpoint Pointer to the underlying hardware that is to be
                        queried.
    \return S_OK iff the devices name can be queried.
*/
HRESULT
windows_set_device_name(
  cahal_device* out_device,
  IMMDevice*    in_endpoint
);

HRESULT
windows_set_device_id(
  cahal_device* out_device,
  IMMDevice*    in_endpoint
                      )
{
  LPWSTR device_id  = NULL;
  HRESULT result    = S_OK;
  
  result = in_endpoint->GetId( &device_id );

  if( S_OK == result )
  {
    if(
      CPC_ERROR_CODE_NO_ERROR
      == cpc_safe_malloc(
      ( void** )&( out_device->device_uid ),
      sizeof( CHAR )* ( wcslen( device_id ) + 1 )
      )
      )
    {
      USIZE number_converted = 0;

      wcstombs_s(
        &number_converted,
        out_device->device_uid,
        wcslen( device_id ) + 1,
        device_id,
        wcslen( device_id )
        );

      CPC_LOG(
        CPC_LOG_LEVEL_DEBUG,
        "Device identifier is %s.",
        out_device->device_uid
        );
    }

    CoTaskMemFree( device_id );
  }

  return( result );
}

HRESULT
windows_set_device_state  (
  cahal_device* out_device,
  IMMDevice*    in_endpoint
                          )
{
  DWORD state     = NULL;
  HRESULT result  = S_OK;

  result = in_endpoint->GetState( &state );

  if( S_OK == result )
  {
    if( DEVICE_STATE_ACTIVE == state )
    {
      out_device->is_alive = 1;
    }
    else
    {
      out_device->is_alive = 0;
    }
  }

  return( result );
}

HRESULT
windows_set_device_name(
  cahal_device* out_device,
  IMMDevice*    in_endpoint
                        )
{
  PROPVARIANT value;

  IPropertyStore *properties  = NULL;
  HRESULT result              = S_OK;

  result = in_endpoint->OpenPropertyStore( STGM_READ, &properties );

  if( S_OK == result )
  {
    PropVariantInit( &value );

    result = properties->GetValue( PKEY_Device_DeviceDesc, &value );

    if( S_OK == result && VT_EMPTY != value.vt )
    {
      if(
        CPC_ERROR_CODE_NO_ERROR
        == cpc_safe_malloc(
        ( void** )&( out_device->device_name ),
        sizeof( CHAR )* ( wcslen( value.pwszVal ) + 1 )
        )
        )
      {
        USIZE number_converted = 0;

        wcstombs_s(
          &number_converted,
          out_device->device_name,
          wcslen( value.pwszVal ) + 1,
          value.pwszVal,
          wcslen( value.pwszVal )
          );

        CPC_LOG(
          CPC_LOG_LEVEL_DEBUG,
          "Device name is %s.",
          out_device->device_name
          );
      }
    }

    PropVariantClear( &value );

    properties->Release( );
  }

  return( result );
}

HRESULT
windows_set_device_info (
  cahal_device* out_device,
  IMMDevice*    in_endpoint
                        )
{
  HRESULT result = S_OK;

  result = windows_set_device_id( out_device, in_endpoint );

  if( S_OK != result )
  {
    CPC_LOG( CPC_LOG_LEVEL_WARN, "Could not set device id: 0x%x.", result );
  }

  result = windows_set_device_name( out_device, in_endpoint );

  if( S_OK != result )
  {
    CPC_LOG( CPC_LOG_LEVEL_WARN, "Could not set device name: 0x%x.", result );
  }

  result = windows_set_device_state( out_device, in_endpoint );

  if( S_OK != result )
  {
    CPC_LOG( CPC_LOG_LEVEL_WARN, "Could not set device state: 0x%x.", result );
  }

  result = windows_set_device_defaults( out_device, in_endpoint );

  if( S_OK != result )
  {
    CPC_LOG (
      CPC_LOG_LEVEL_WARN,
      "Could not set device defaults: 0x%x.",
      result
            );
  }

  result = windows_set_device_streams( out_device, in_endpoint );

  if( S_OK != result )
  {
    CPC_LOG (
      CPC_LOG_LEVEL_WARN, 
      "Could not set device stream: 0x%x.", 
      result
            );
  }

  return( result );
}

HRESULT
windows_set_device_defaults(
  cahal_device* out_device,
  IMMDevice*    in_endpoint
                            )
{
  IAudioClient *audio_client  = NULL;
  WAVEFORMATEX *format        = NULL;
  HRESULT result              = S_OK;

  result = in_endpoint->Activate (
    __uuidof( IAudioClient ),
    CLSCTX_ALL, 
    NULL, 
    ( void** ) &audio_client
                        );

  if( S_OK == result )
  {
    result = audio_client->GetMixFormat( &format );

    if( S_OK == result )
    {
      out_device->preferred_number_of_channels  = format->nChannels;
      out_device->preferred_sample_rate         = format->nSamplesPerSec;

      CPC_LOG (
        CPC_LOG_LEVEL_DEBUG, 
        "Preferred number of channels is %d and sample rate is %.0f.", 
        out_device->preferred_number_of_channels, 
        out_device->preferred_sample_rate
              );

      CoTaskMemFree( format );
    }
  }

  return( result );
}

cahal_device**
windows_get_device_list( void )
{
  cahal_device** device_list              = NULL;
  HRESULT result                          = S_OK;
  IMMDeviceEnumerator *enumerator         = NULL;
  IMMDeviceCollection *device_collection  = NULL;

  CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Enumerating devices" );

  result =
      CoCreateInstance  (
        __uuidof( MMDeviceEnumerator ),
        NULL, 
        CLSCTX_ALL, 
        __uuidof( IMMDeviceEnumerator ),
        ( void** )&enumerator
                        );

  if( S_OK == result )
  {
    result =
        enumerator->EnumAudioEndpoints  (
          eAll, 
          DEVICE_STATE_ACTIVE, 
          &device_collection
                                        );

    if( S_OK == result )
    {
      UINT32 number_of_devices;
      
      result = device_collection->GetCount( &number_of_devices );

      if( S_OK == result )
      {

        CPC_LOG( CPC_LOG_LEVEL_DEBUG, "Found %d devices.", number_of_devices );

        if( 
          CPC_ERROR_CODE_NO_ERROR
          == cpc_safe_malloc  (
              ( void** )&device_list,
              sizeof( cahal_device* ) * ( number_of_devices + 1 )
                              )
          )
        {
          for( UINT32 i = 0; i < number_of_devices; i++ )
          {
            if(
              CPC_ERROR_CODE_NO_ERROR
              == cpc_safe_malloc  (
                  ( void** )&( device_list[i] ),
                  sizeof( cahal_device )
                                  )
              )
            {
              IMMDevice *endpoint = NULL;

              CPC_LOG_BUFFER(
                CPC_LOG_LEVEL_TRACE,
                "Device",
                ( UCHAR* )device_list[i],
                sizeof( cahal_device ),
                8
              );

              device_list[ i ]->handle = i;

              result = device_collection->Item( i, &endpoint );

              if( S_OK == result )
              {
                windows_set_device_info( device_list[i], endpoint );

                endpoint->Release();
              }
            }
          }
        }
      }
      else
      {
        CPC_ERROR( "Could not get number of devices: %d.", result );
      }

      device_collection->Release( );
    }
    else
    {
      CPC_ERROR( "Could not enumerate devices: %d.", result );
    }

    enumerator->Release();
  }
  else
  {
    CPC_ERROR( "Could not initialize MMDevice enumerator: %d.", result );
  }

  return( device_list );
}

HRESULT
windows_initialize_device (
  IMMDevice*      in_device,
  IAudioClient**  io_audio_client,
  WAVEFORMATEX    in_format
                          )
{
  HRESULT result = S_OK;

  if( NULL != in_device )
  {
    result =
      in_device->Activate(
        __uuidof( IAudioClient ),
        CLSCTX_ALL,
        NULL,
        ( void** )io_audio_client
      );

    if( S_OK == result )
    {
      REFERENCE_TIME requested_latency = 0;

      result =
        ( *io_audio_client )->GetDevicePeriod( NULL, &requested_latency );

      CPC_LOG(
        CPC_LOG_LEVEL_DEBUG,
        "Hardware sample rate: %lld.",
        requested_latency
      );

      if( S_OK == result )
      {
        result =
          ( *io_audio_client )->Initialize(
            AUDCLNT_SHAREMODE_EXCLUSIVE,
            AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
            requested_latency,
            requested_latency,
            &in_format,
            NULL
          );

        if( AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED == result )
        {
          result =
            windows_reinitialize_device (
              in_device, 
              io_audio_client, 
              in_format
            );
        }
        else
        {
          CPC_ERROR( "Could not intialize audio client: 0x%x.", result );
        }
      }
      else
      {
        CPC_ERROR( "Could not get device period: 0x%x.", result );
      }
    }
    else
    {
      CPC_ERROR( "Could not activate audio client: 0x%x.", result );
    }
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Device is null." );
  }
  
  return( result );
}

HRESULT
windows_reinitialize_device (
  IMMDevice*      in_device,
  IAudioClient**  io_audio_client,
  WAVEFORMATEX    in_format
                            )
{
  UINT32 number_of_frames = 0;
  HRESULT result          = S_OK;

  if( NULL != in_device && NULL != ( *io_audio_client ) )
  {
    result = ( *io_audio_client )->GetBufferSize( &number_of_frames );

    if( S_OK == result )
    {
      REFERENCE_TIME requested_latency =
        ( REFERENCE_TIME )
        ( ( 10000.0 * 1000 / in_format.nSamplesPerSec * number_of_frames )
        + 0.5 );

      CPC_LOG(
        CPC_LOG_LEVEL_DEBUG,
        "New hardware sample rate is %lld.",
        requested_latency
      );

      ( *io_audio_client )->Release();

      result =
        in_device->Activate(
          __uuidof( IAudioClient ),
          CLSCTX_ALL,
          NULL,
          ( void** )io_audio_client
        );

      if( S_OK == result )
      {
        result =
          ( *io_audio_client )->Initialize(
            AUDCLNT_SHAREMODE_EXCLUSIVE,
            AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
            requested_latency,
            requested_latency,
            &in_format,
            NULL
          );
      }
      else
      {
        CPC_ERROR( "Could not reactivate device: 0x%x.", result );
      }
    }
    else
    {
      CPC_ERROR( "Could not get buffer size: 0x%x.", result );
    }
  }
  else
  {
    CPC_ERROR (
      "Device (0x%x) or audio client (0x%x) are null.", 
      in_device, 
      io_audio_client
    );
  }

  return( result );
}

HRESULT
windows_configure_capture_device  (
  cahal_device*   in_device,
  UINT32          in_number_of_channels,
  FLOAT64         in_sample_rate,
  UINT32          in_bit_depth,
  IAudioClient**  out_audio_client
                                  )
{
  HRESULT result                          = S_OK;
  IMMDeviceEnumerator *enumerator         = NULL;
  IMMDeviceCollection *device_collection  = NULL;

  if( NULL != in_device )
  {
    result =
      CoCreateInstance(
        __uuidof( MMDeviceEnumerator ),
        NULL,
        CLSCTX_ALL,
        __uuidof( IMMDeviceEnumerator ),
        ( void** )&enumerator
      );

    if( S_OK == result )
    {
      result =
        enumerator->EnumAudioEndpoints(
          eAll,
          DEVICE_STATE_ACTIVE,
          &device_collection
        );

      if( S_OK == result )
      {
        IMMDevice* device = NULL;

        result = device_collection->Item( in_device->handle, &device );

        if( S_OK == result )
        {
          WAVEFORMATEX format;

          CPC_MEMSET( &format, 0x0, sizeof( WAVEFORMATEX ) );

          format.wFormatTag       = WAVE_FORMAT_PCM;
          format.nChannels        = in_number_of_channels;
          format.nSamplesPerSec   = ( DWORD )in_sample_rate;
          format.wBitsPerSample   = in_bit_depth;
          format.nBlockAlign      =
            ( format.nChannels * format.wBitsPerSample ) / 8;
          format.nAvgBytesPerSec  = format.nSamplesPerSec * format.nBlockAlign;
          format.cbSize = 0;

          CPC_LOG(
            CPC_LOG_LEVEL_DEBUG,
            "Format info: sr=%d, nc=0x%x, bd=0x%x",
            format.nSamplesPerSec,
            format.nChannels,
            format.wBitsPerSample
          );

          result =
            windows_initialize_device( device, out_audio_client, format );
        }
        else
        {
          CPC_ERROR(
            "Could not find item 0x%x in collection: 0x%x.",
            in_device->handle,
            result
          );
        }
      }
      else
      {
        CPC_ERROR( "Could not enumerate endpoints: 0x%x.", result );
      }
    }
    else
    {
      CPC_ERROR( "Could not create COM object: 0x%x.", result );
    }
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Device is null." );
  }

  if( NULL != enumerator )
  {
    enumerator->Release();
  }

  if( NULL != device_collection )
  {
    device_collection->Release();
  }

  return( result );
}

UINT32
windows_recorder_read_data(
  IAudioCaptureClient*  in_capture_client,
  WAVEFORMATEX*         in_format,
  cahal_recorder_info*  in_callback_info
                          )
{
  UINT32 number_of_frames   = 0;
  DWORD flags               = 0;
  BYTE *data                = NULL;
  BYTE *buffer              = NULL;
  UINT32 buffer_length      = 0;
  HRESULT result            = S_OK;

  result =
    in_capture_client->GetBuffer(
      &data,
      &number_of_frames,
      &flags,
      NULL,
      NULL
    );

  CPC_LOG(
    CPC_LOG_LEVEL_DEBUG,
    "Number of frames is 0x%x.",
    number_of_frames
  );

  if( 0 != number_of_frames )
  {
    buffer_length = number_of_frames * in_format->nBlockAlign;

    CPC_LOG(
      CPC_LOG_LEVEL_DEBUG,
      "Copying 0x%x bytes of data.",
      buffer_length
    );

    if(
      CPC_ERROR_CODE_NO_ERROR
      == cpc_safe_malloc( ( void** )&buffer, buffer_length )
      )
    {
      if( !( AUDCLNT_BUFFERFLAGS_SILENT & flags ) )
      {
        cpc_memcpy( buffer, data, buffer_length );
      }
    }

    if( NULL != buffer )
    {
      CPC_LOG (
        CPC_LOG_LEVEL_DEBUG, 
        "Sending 0x%x bytes of data to caller.", 
        buffer_length
      );

      in_callback_info->recording_callback(
        in_callback_info->recording_device,
        buffer,
        buffer_length,
        in_callback_info->user_data
      );

      cpc_safe_free( ( void** )&buffer );
    }

    result = in_capture_client->ReleaseBuffer( number_of_frames );

    if( S_OK != result )
    {
      CPC_ERROR( "Could not release buffer: 0x%x.", result );
    }
  }

  return( number_of_frames );
}

void
windows_handle_recorder_data(
  cahal_recorder_info* in_callback_info
                            )
{
  HRESULT result                      = S_OK;
  IAudioCaptureClient* capture_client = NULL;
  WAVEFORMATEX *format                = NULL;

  if( NULL != in_callback_info && NULL != in_callback_info->platform_data )
  {
    IAudioClient* audio_client =
      ( IAudioClient* )in_callback_info->platform_data;

    result =
      audio_client->GetService(
        __uuidof( IAudioCaptureClient ),
        ( void** )&capture_client
      );

    if( S_OK == result )
    {
      result = audio_client->GetMixFormat( &format );

      if( S_OK == result )
      {
        if( S_OK == result )
        {
          UINT32 number_of_frames =
            windows_recorder_read_data  (
              capture_client, 
              format, 
              in_callback_info 
            );

          CPC_LOG(
            CPC_LOG_LEVEL_DEBUG,
            "Number of frames is 0x%x.",
            number_of_frames
          );
        }
        else
        {
          CPC_ERROR( "Could not get packet size: 0x%x.", result );
        }
      }
      else
      {
        CPC_ERROR( "Could not get format: 0x%x.", result );
      }

      CoTaskMemFree( format );
    }
    else
    {
      CPC_ERROR( "Could not get capture client: 0x%x.", result );
    }
  }
  else
  {
    CPC_ERROR (
      "Callback (0x%x) or platform data are null.", 
      in_callback_info
    );
  }

  if( NULL != capture_client )
  {
    capture_client->Release();
  }
}

DWORD
WINAPI
windows_recorder_thread_entry (
  LPVOID in_callback_info
                              ) 
{
  CPC_BOOL done                       = CPC_FALSE;
  HANDLE events[]                     =
    { g_recorder_data_ready_event, g_recorder_terminate_event };
  cahal_recorder_info* callback_info  =
    ( cahal_recorder_info* )in_callback_info;

  CPC_LOG (
    CPC_LOG_LEVEL_DEBUG, 
    "Capture thread (%d) started. Waiting to be signaled.", 
    GetCurrentThreadId()
  );

  if( NULL != callback_info && NULL != callback_info->platform_data )
  {
    IAudioClient* audio_client = ( IAudioClient* )callback_info->platform_data;

    while( !done )
    {
      DWORD wait_result =
        WaitForMultipleObjects( ARRAYSIZE( events ), events, FALSE, INFINITE );

      CPC_LOG(
        CPC_LOG_LEVEL_DEBUG,
        "Capture thread (%d) awoken: 0x%x.",
        GetCurrentThreadId(),
        wait_result
      );

      switch( wait_result )
      {
        case WAIT_OBJECT_0:
          CPC_LOG(
            CPC_LOG_LEVEL_DEBUG,
            "Capture thread (%d) awoken with data present.",
            GetCurrentThreadId()
          );

          windows_handle_recorder_data( callback_info );

          break;

        case WAIT_OBJECT_0 + 1:
          CPC_LOG(
            CPC_LOG_LEVEL_DEBUG,
            "Capture thread (%d) awoken with terminate event.",
            GetCurrentThreadId()
          );

          audio_client->Stop();

          audio_client->Release();

          done = CPC_TRUE;

          break;
        default:
          CPC_ERROR( "Wait error: 0x%x.", wait_result );

          done = CPC_TRUE;

          break;
      }
    }
  }
  else
  {
    CPC_ERROR( "Callback (0x%x) or platform data are null.", callback_info );
  }

  CPC_LOG (
    CPC_LOG_LEVEL_DEBUG, 
    "Capture thread (%d) terminating.", 
    GetCurrentThreadId()
    );

  return( 1 );
}

HRESULT
windows_initialize_recorder_event_thread  (
  IAudioClient*         io_audio_client,
  cahal_recorder_info*  in_callback_info
                                          )
{
  HRESULT result = S_OK;

  if( NULL != in_callback_info && NULL != io_audio_client )
  {
    g_recorder_data_ready_event = CreateEvent( NULL, FALSE, FALSE, NULL );
    g_recorder_terminate_event  = CreateEvent( NULL, FALSE, FALSE, NULL );

    if(
      NULL != g_recorder_data_ready_event
      && NULL != g_recorder_terminate_event
      )
    {
      DWORD thread_id = 0;

      result = io_audio_client->SetEventHandle( g_recorder_data_ready_event );

      g_recorder_thread =
        CreateThread(
          NULL,
          0,
          windows_recorder_thread_entry,
          ( LPVOID )in_callback_info,
          0,
          &thread_id
        );

      if( NULL == g_recorder_thread )
      {
        result = GetLastError();

        CPC_ERROR( "Could not create recorder thread: 0x%x.", result );
      }
    }
    else
    {
      result = GetLastError();

      CPC_ERROR( "Could not create event: 0x%x.", result );
    }
  }
  else
  {
    CPC_ERROR (
      "Callback (0x%x) or audio client (0x%x) are null.", 
      in_callback_info, 
      io_audio_client
    );
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

  CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "In start recording!" );

  if(
    cahal_test_device_direction_support(
      in_device,
      CAHAL_DEVICE_INPUT_STREAM
    )
    && CAHAL_STATE_INITIALIZED == g_cahal_state
    && NULL == g_recorder_callback_info
    )
  {
    IAudioClient* audio_client = NULL;

    HRESULT result =
      windows_configure_capture_device  (
        in_device, 
        in_number_of_channels, 
        in_sample_rate, 
        in_bit_depth, 
        &audio_client
      );

    if( S_OK == result )
    {
      if( CPC_ERROR_CODE_NO_ERROR
        == cpc_safe_malloc(
            ( void ** )&( g_recorder_callback_info ),
            sizeof( cahal_recorder_info )
                          )
        )
      {
        g_recorder_callback_info->recording_device    = in_device;
        g_recorder_callback_info->recording_callback  = in_recorder;
        g_recorder_callback_info->user_data           = in_callback_user_data;
        g_recorder_callback_info->platform_data       = audio_client;

        result =
          windows_initialize_recorder_event_thread  (
            audio_client,
            g_recorder_callback_info
          );

        if( S_OK == result )
        {
          result = audio_client->Start();

          return_value = CPC_TRUE;
        }
        else
        {
          CPC_ERROR( "Could not set event: 0x%x.", result );
        }
      }
    }
    else
    {
      CPC_ERROR( "Could not configure capture device: 0x%x.", result );
    }
  }
  else
  {
    CPC_ERROR(
      "Direction (%d) not support by %s.",
      CAHAL_DEVICE_INPUT_STREAM,
      in_device->device_name
      );
  }

  return( return_value );
}

CPC_BOOL
cahal_stop_recording( void )
{
  CPC_BOOL return_value = CPC_FALSE;

  if( INVALID_HANDLE_VALUE != g_recorder_terminate_event )
  {
    CPC_LOG (
        CPC_LOG_LEVEL_DEBUG, 
        "Sending terminate signal to recorder thread."
      );

    if( SetEvent( g_recorder_terminate_event ) )
    {
      DWORD wait_result = WaitForSingleObject( g_recorder_thread, 1000 );

      switch( wait_result )
      {
        case WAIT_OBJECT_0:
          return_value = CPC_TRUE;

          g_recorder_thread = INVALID_HANDLE_VALUE;

          CPC_LOG_STRING  (
            CPC_LOG_LEVEL_DEBUG, 
            "Capture thread successfully terminated."
            );

          break;
        default:
          CPC_ERROR (
            "Failed while waiting for thread to temrinate: 0x%x.", 
            wait_result
          );

          break;
      }
    }
  }

  if( INVALID_HANDLE_VALUE != g_recorder_data_ready_event )
  {
    CloseHandle( g_recorder_data_ready_event );

    g_recorder_data_ready_event = INVALID_HANDLE_VALUE;
  }

  if( INVALID_HANDLE_VALUE != g_recorder_terminate_event )
  {
    CloseHandle( g_recorder_terminate_event );

    g_recorder_terminate_event = INVALID_HANDLE_VALUE;
  }

  if( INVALID_HANDLE_VALUE != g_recorder_thread )
  {
    TerminateThread( g_recorder_thread, 0 );

    g_recorder_thread = INVALID_HANDLE_VALUE;
  }

  return( return_value );
}

void
cahal_sleep(
UINT32 in_sleep_time
)
{
  Sleep( in_sleep_time * 1000 );
}

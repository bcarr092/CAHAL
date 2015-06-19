/*! \file   windows_cahal_device.cpp
    \brief  Implementation that queries the underlying Windows hardware and
            creates the device_list struct.

    \author Brent Carrara
 */

#include "windows/windows_cahal_device.hpp"

#define LABEL_CAPTURE   "captur"
#define LABEL_RENDER    "render"

cahal_recorder_info* g_recorder_callback_info = NULL;
cahal_playback_info* g_playback_callback_info = NULL;

HANDLE g_recorder_thread            = NULL;
HANDLE g_recorder_data_ready_event  = NULL;
HANDLE g_recorder_terminate_event   = NULL;

HANDLE g_playback_thread            = NULL;
HANDLE g_playback_data_ready_event  = NULL;
HANDLE g_playback_terminate_event   = NULL;

typedef struct windows_context_t
{
  IAudioClient* audio_client;

  WAVEFORMATEX* format;

} windows_context;

typedef void( *windows_data_handler_routine ) (
  void* in_callback_info
);

typedef struct data_handler_t
{
  void* callback_info;

  PHANDLE data_ready_event;

  PHANDLE terminate_event;

  windows_data_handler_routine  data_handler;

  UCHAR label[7];

} data_handler;

void
windows_configure_format(
UINT32          in_number_of_channels,
FLOAT64         in_sample_rate,
UINT32          in_bit_depth,
WAVEFORMATEX**  out_format
);

void
windows_handle_playback_data(
cahal_playback_info* in_callback_info
);

CPC_BOOL
windows_stop_thread(
PHANDLE io_terminate_event,
PHANDLE io_thread
);

/*! \def    DWORD WINAPI  windows_thread_entry  (
                            LPVOID in_handler_info
                                                )
    \brief  Entry point for the processing threads. This function will block
            on the data ready and terminate events and handle them when
            signalled. When the terminate event is signalled the thread will
            exit.

    \param  in_handler_info Contains all the events, callback function and
                            label for the thread handling processing events
                            from the kernel.
    \return Exit code (always 0).
 */
DWORD
WINAPI
windows_thread_entry  (
  LPVOID in_handler_info
                      );

/*! \def    void  windows_recorder_read_data  (
                      IAudioCaptureClient*  in_capture_client,
                      WAVEFORMATEX*         in_format,
                      cahal_recorder_info*  in_callback_info
                                                )
    \brief  Reads a buffer full of data from the capture hardware and makes
            the data available to the callback.

    \param  in_capture_client The capture client to record the data from.
    \param  in_format The format the data is in.
    \param  in_callback_info  The callback to call with the new data.
 */
void
windows_recorder_read_data  (
  IAudioCaptureClient*  in_capture_client,
  WAVEFORMATEX*         in_format,
  cahal_recorder_info*  in_callback_info
                            );

/*! \def    HRESULT windows_initialize_events_thread  (
                      IAudioClient*           io_audio_client,
                      void*                   in_callback_info
                      PHANDLE                 out_data_ready_event,
                      PHANDLE                 out_terminate_event,
                      PHANDLE                 out_thread,
                      LPTHREAD_START_ROUTINE  in_thread_entry,
                      CHAR*                   in_label
                                                      )
    \brief  Creates the events (data ready, terminate), a thread and adds the
            data ready event to the audio client (so it can be signalled when
            a buffer needs to be processed). This function sets up both the
            recorder and playback events and threads.

    \param  io_audio_client The objet to set the data ready event on.
    \param  in_callback_info  The thread created in this function is configured
                              to call this callback when data needs to be
                              processed.
    \param  out_data_ready_event  The newly created data ready event, which is
                                  passed to the audio client so it can signal
                                  the event when data is ready.
    \param  out_terminate_event The newly created terminate event, which is
                                signalled when stop_(playback||record) is
                                called.
    \param  out_thread  The newly created thread, either a recorder or playback
                        thread.
    \param  in_thread_entry The handler that is called when out_thread is 
                            awoken with data to process.
    \param  in_label  Label to be given to the thread (eitehr captur or render)
    \return S_OK iff the audio_client is configured to call the data ready
            event. An error code otherwise.
 */
HRESULT
windows_initialize_events_thread  (
  IAudioClient*                 io_audio_client,
  void*                         in_callback_info,
  PHANDLE                       out_data_ready_event,
  PHANDLE                       out_terminate_event,
  PHANDLE                       out_thread,
  windows_data_handler_routine  in_data_handler_routine,
  CHAR*                         in_label
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

/*! \def    HRESULT windows_configure_device  (
                      cahal_device*   in_device,
                      WAVEFORMATEX*   in_format,
                      IAudioClient**  out_audio_client
                                              )
    \breif  Entry point to configure the WASAPI to process audio samples. This
            function will enumerate the audio devices, select the appropriate
            device (using in_device->handle) and initialize an audio client
            to use for capture or render.

    \param  in_device The cahal device struct that contains the handle of the
                      device to be configured.
    \param  in_format The format of the stream.
    \param  out_audio_client  A created, initialized audio client that can be
                              used to process data buffers.
    \return S_OK iff out_audio_client is created, activated and initialized.
            An error code otherwise.
 */
HRESULT
windows_configure_device  (
  cahal_device*   in_device,
  WAVEFORMATEX*   in_format,
  IAudioClient**  out_audio_client
                          );

/*! \def    HRESULT windows_initialize_device (
                      IMMDevice*      in_device,
                      IAudioClient**  io_audio_client,
                      WAVEFORMATEX*   in_format
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
  WAVEFORMATEX*   in_format
                          );

/*! \def    HRESULT windows_reinitialize_device (
                      IMMDevice*      in_device,
                      IAudioClient**  io_audio_client,
                      WAVEFORMATEX*   in_format
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
  WAVEFORMATEX*   in_format
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
  WAVEFORMATEX*   in_format
                          )
{
  HRESULT result = S_OK;

  if( NULL != in_device && NULL != in_format )
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
      REFERENCE_TIME requested_latency = WINDOWS_DEFAULT_RECORD_DURATION_IN_100_NANOS;

      result =
        ( *io_audio_client )->Initialize(
          AUDCLNT_SHAREMODE_EXCLUSIVE,
          AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
          requested_latency,
          requested_latency,
          in_format,
          NULL
        );

      if( AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED == result )
      {
        result =
          windows_reinitialize_device(
            in_device, 
            io_audio_client, 
            in_format
          );
      }
      else if( S_OK != result )
      {
        CPC_ERROR( "Could not intialize audio client: 0x%x.", result );
      }
    }
    else
    {
      CPC_ERROR( "Could not activate audio client: 0x%x.", result );
    }
  }
  else
  {
    CPC_ERROR (
      "Device (0x%x) or format (0x%x) are null.", 
      in_device, 
      in_format
    );
  }
  
  return( result );
}

HRESULT
windows_reinitialize_device (
  IMMDevice*      in_device,
  IAudioClient**  io_audio_client,
  WAVEFORMATEX*   in_format
                            )
{
  UINT32 number_of_frames = 0;
  HRESULT result          = S_OK;

  if( NULL != in_device && NULL != ( *io_audio_client ) && NULL != in_format )
  {
    result = ( *io_audio_client )->GetBufferSize( &number_of_frames );

    CPC_LOG( CPC_LOG_LEVEL_DEBUG, "Buffer size is 0x%x.", number_of_frames );

    if( S_OK == result )
    {
      REFERENCE_TIME requested_latency =
        ( REFERENCE_TIME )
        (
          10000.0 * 1000 * number_of_frames / in_format->nSamplesPerSec + 0.5
        );

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
            in_format,
            NULL
          );

        if( S_OK != result )
        {
          CPC_ERROR( "Could not reinitialize device: 0x%x.", result );
        }
        else
        {
          CPC_LOG (
            CPC_LOG_LEVEL_DEBUG, 
            "Client's latency is: 0x%x.", 
            requested_latency
            );

          result = ( *io_audio_client )->GetBufferSize( &number_of_frames );

          CPC_LOG (
            CPC_LOG_LEVEL_DEBUG, 
            "Buffer size is 0x%x.", 
            number_of_frames
            );
        }
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
      "Device (0x%x), audio client (0x%x), or format (0x%x) are null.", 
      in_device, 
      io_audio_client,
      in_format
    );
  }

  return( result );
}

HRESULT
windows_configure_device  (
  cahal_device*   in_device,
  WAVEFORMATEX*   in_format,
  IAudioClient**  out_audio_client
                          )
{
  HRESULT result                          = S_OK;
  IMMDeviceEnumerator *enumerator         = NULL;
  IMMDeviceCollection *device_collection  = NULL;

  if( NULL != in_device && NULL != in_format )
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
          result =
            windows_initialize_device( device, out_audio_client, in_format );
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
    CPC_ERROR (
      "Device (0x%x) or format (0x%x) are null.", 
      in_device, 
      in_format
    );
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

void
windows_playback_send_data  (
  IAudioClient*         in_audio_client,
  IAudioRenderClient*   in_render_client,
  WAVEFORMATEX*         in_format,
  cahal_playback_info*  in_callback_info
                            )
{
  UINT32 number_of_frames = 0;
  HRESULT result          = S_OK;
  BYTE *data              = NULL;
  BYTE *buffer            = NULL;
  UINT32 buffer_length    = 0;

  result = in_audio_client->GetBufferSize( &number_of_frames );

  CPC_LOG (
    CPC_LOG_LEVEL_INFO,
    "Render hardware supports buffer of size 0x%x frames (0x%x).", 
    number_of_frames,
    in_format->nBlockAlign
  );

  if( S_OK == result )
  {
    buffer_length = number_of_frames * in_format->nBlockAlign;

    CPC_LOG (
      CPC_LOG_LEVEL_INFO,
      "Requesting 0x%x bytes of data.", 
      buffer_length
    );
    
    if(
      CPC_ERROR_CODE_NO_ERROR 
      == cpc_safe_malloc( ( void** )&buffer, sizeof( BYTE )* buffer_length )
      )
    {
      if(
          ! in_callback_info->playback_callback(
              in_callback_info->playback_device,
              buffer,
              &buffer_length,
              in_callback_info->user_data
            )
        )
      {
        CPC_LOG_STRING  (
          CPC_LOG_LEVEL_ERROR, 
          "Error reading data from callback."
        );
      }
      else
      {
        CPC_LOG (
          CPC_LOG_LEVEL_INFO,
          "Buffer length is 0x%x.", 
          buffer_length
        );

        number_of_frames = buffer_length / in_format->nBlockAlign;

        CPC_LOG (
          CPC_LOG_LEVEL_INFO,
          "Requesting buffer of length 0x%x frames from render client.", 
          number_of_frames
         );

        result = in_render_client->GetBuffer( number_of_frames, &data );

        if( S_OK == result )
        {
          CPC_LOG (
            CPC_LOG_LEVEL_INFO, 
            "Copying 0x%x bytes of data to render hardware (0x%x).", 
            buffer_length,
            data
          );

          cpc_memcpy( data, buffer, buffer_length );

          result = in_render_client->ReleaseBuffer( number_of_frames, 0 );
            
          if( S_OK == result )
          {
            CPC_LOG (
              CPC_LOG_LEVEL_INFO, 
              "Releasing buffer of length 0x%x frames.", 
              number_of_frames
            );
          }
          else
          {
            CPC_ERROR( "Could not release buffer: 0x%x.", result );
          }
        }
        else
        {
          CPC_LOG(
            CPC_LOG_LEVEL_WARN,
            "Could not get buffer: 0x%x.",
            result
            );
        }
      }

      if( NULL != buffer )
      {
        cpc_safe_free( ( void** )&buffer );
      }
    }
  }
  else
  {
    CPC_ERROR( "Could not read buffer size: 0x%x.", result );
  }
}

void
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
      else
      {
        CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Received buffer of silence." );
      }
    }
    
    // Release the buffer back to the endpoint hardware as soon as it's copied,
    // i.e., before the data is passed to the callback.
    result = in_capture_client->ReleaseBuffer( number_of_frames );
    
    if( S_OK != result )
    {
      CPC_ERROR( "Could not release buffer: 0x%x.", result );
    }

    if( NULL != buffer )
    {
      CPC_LOG (
        CPC_LOG_LEVEL_DEBUG, 
        "Sending 0x%x bytes of data to caller.", 
        buffer_length
        );

      if(
          ! in_callback_info->recording_callback(
              in_callback_info->recording_device,
              buffer,
              buffer_length,
              in_callback_info->user_data
            )
        )
      {
        CPC_LOG_STRING  (
          CPC_LOG_LEVEL_ERROR, 
          "Error sending data to callback."
        );
      }

      cpc_safe_free( ( void** )&buffer );
    }
  }
}

void
windows_handle_playback_data  (
  cahal_playback_info* in_callback_info
                              )
{
  HRESULT result                      = S_OK;
  IAudioRenderClient * render_client  = NULL;

  if( NULL != in_callback_info && NULL != in_callback_info->platform_data )
  {
    IAudioClient* audio_client  =
      ( ( windows_context* )in_callback_info->platform_data )->audio_client;
    WAVEFORMATEX *format        =
      ( ( windows_context* )in_callback_info->platform_data )->format;

    result =
      audio_client->GetService(
        __uuidof( IAudioRenderClient ),
        ( void** )&render_client
      );

    if( S_OK == result )
    {
      windows_playback_send_data(
        audio_client,
        render_client,
        format,
        in_callback_info
      );
    }
    else
    {
      CPC_ERROR( "Could not get render client: 0x%x.", result );
    }
  }
  else
  {
    CPC_ERROR(
      "Callback (0x%x) or platform data are null.",
      in_callback_info
    );
  }

  if( NULL != render_client )
  {
    render_client->Release( );
  }
}

void
windows_handle_recorder_data(
  cahal_recorder_info* in_callback_info
                            )
{
  HRESULT result                      = S_OK;
  IAudioCaptureClient* capture_client = NULL;

  if( NULL != in_callback_info && NULL != in_callback_info->platform_data )
  {
    IAudioClient* audio_client  =
      ( ( windows_context* )in_callback_info->platform_data )->audio_client;
    WAVEFORMATEX *format        =
      ( ( windows_context* )in_callback_info->platform_data )->format;

    result =
      audio_client->GetService(
        __uuidof( IAudioCaptureClient ),
        ( void** )&capture_client
      );

    if( S_OK == result )
    {
      windows_recorder_read_data  (
        capture_client, 
        format, 
        in_callback_info 
      );
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
windows_thread_entry  (
  LPVOID in_handler_info
                      ) 
{
  CPC_BOOL done               = CPC_FALSE;
  data_handler* handler_info  = ( data_handler* ) in_handler_info;

  if( NULL != handler_info )
  {
    HANDLE events[] =
      {
        *( handler_info->data_ready_event ),
        *( handler_info->terminate_event )
      };

    windows_data_handler_routine handler_routine = handler_info->data_handler;

    cahal_recorder_info* callback_info =
      ( cahal_recorder_info* )handler_info->callback_info;

    CPC_LOG(
      CPC_LOG_LEVEL_DEBUG,
      "%s thread (%d) started. Waiting to be signaled.",
      handler_info->label,
      GetCurrentThreadId()
    );

    if(
      NULL != callback_info
      && NULL != callback_info->platform_data
      && NULL != handler_routine
      )
    {
      IAudioClient* audio_client =
        ( ( windows_context* )callback_info->platform_data )->audio_client;

      while( !done )
      {
        DWORD wait_result =
          WaitForMultipleObjects  (
            ARRAYSIZE( events ), 
            events, 
            FALSE, 
            INFINITE
          );

        CPC_LOG(
          CPC_LOG_LEVEL_DEBUG,
          "%s thread (%d) awoken: 0x%x.",
          handler_info->label,
          GetCurrentThreadId(),
          wait_result
        );

        switch( wait_result )
        {
        case WAIT_OBJECT_0:
          CPC_LOG(
            CPC_LOG_LEVEL_DEBUG,
            "%s thread (%d) awoken with data present.",
            handler_info->label,
            GetCurrentThreadId()
          );

          handler_routine( callback_info );

          break;

        case WAIT_OBJECT_0 + 1:
          CPC_LOG(
            CPC_LOG_LEVEL_DEBUG,
            "%s thread (%d) awoken with terminate event.",
            handler_info->label,
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
      CPC_ERROR (
        "Callback (0x%x), platform data, or handler routine (0x%x) are null.", 
        callback_info, 
        handler_routine
      );
    }

    if( NULL != in_handler_info )
    {
      cpc_safe_free( ( void** )&in_handler_info );
    }
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Handler info is null." );
  }

  CPC_LOG (
    CPC_LOG_LEVEL_DEBUG, 
    "Thread (%d) terminating.", 
    GetCurrentThreadId()
  );

  return( 1 );
}

HRESULT
windows_initialize_events_thread  (
  IAudioClient*                 io_audio_client,
  void*                         in_callback_info,
  PHANDLE                       out_data_ready_event,
  PHANDLE                       out_terminate_event,
  PHANDLE                       out_thread,
  windows_data_handler_routine  in_data_handler_routine,
  CHAR*                         in_label
                                  )
{
  HRESULT result = S_OK;

  if(
    NULL != in_callback_info 
    && NULL != io_audio_client
    && NULL != out_data_ready_event
    && NULL != out_terminate_event
    && NULL != out_thread
    )
  {
    *out_data_ready_event = CreateEvent( NULL, FALSE, FALSE, NULL );
    *out_terminate_event  = CreateEvent( NULL, FALSE, FALSE, NULL );

    if(
      NULL != *out_data_ready_event
      && NULL != *out_terminate_event
      )
    {
      DWORD thread_id = 0;

      result = io_audio_client->SetEventHandle( *out_data_ready_event );

      if( S_OK == result )
      {
        data_handler* handler_info = NULL;

        if(
          CPC_ERROR_CODE_NO_ERROR
          == cpc_safe_malloc( ( void** )&handler_info, sizeof( data_handler ) )
          )
        {
          handler_info->callback_info     = in_callback_info;
          handler_info->data_ready_event  = out_data_ready_event;
          handler_info->terminate_event   = out_terminate_event;
          handler_info->data_handler      = in_data_handler_routine;

          cpc_memcpy  (
            handler_info->label, 
            in_label, 
            CPC_STRNLEN( in_label, 6 )
          );

          *out_thread =
            CreateThread(
              NULL,
              0,
              windows_thread_entry,
              ( LPVOID )handler_info,
              0,
              &thread_id
            );

          if( NULL == *out_thread )
          {
            result = GetLastError();

            CPC_ERROR( "Could not create recorder thread: 0x%x.", result );
          }
        }
      }
      else
      {
        CPC_ERROR( "Could not set event: 0x%x.", result );
      }
    }
    else
    {
      result = GetLastError();

      *out_data_ready_event = NULL;
      *out_terminate_event  = NULL;

      CPC_ERROR( "Could not create event: 0x%x.", result );
    }
  }
  else
  {
    CPC_ERROR (
      "Callback (0x%x), audio client (0x%x), data read event, (0x%x),"
      " terminate event, or thread (0x%x) are null.", 
      in_callback_info, 
      io_audio_client,
      out_data_ready_event,
      out_terminate_event,
      out_thread
    );
  }

  return( result );
}

void
windows_configure_format  (
  UINT32          in_number_of_channels,
  FLOAT64         in_sample_rate,
  UINT32          in_bit_depth,
  WAVEFORMATEX**  out_format
                          )
{
  if( NULL != out_format )
  {
    if( CPC_ERROR_CODE_NO_ERROR == cpc_safe_malloc( ( void** )out_format, sizeof( WAVEFORMATEX ) ) )
    {
      CPC_MEMSET( *out_format, 0x0, sizeof( WAVEFORMATEX ) );

      ( *out_format )->wFormatTag     = WAVE_FORMAT_PCM;
      ( *out_format )->nChannels      = in_number_of_channels;
      ( *out_format )->nSamplesPerSec = ( DWORD )in_sample_rate;
      ( *out_format )->wBitsPerSample = in_bit_depth;
      ( *out_format )->nBlockAlign    =
        ( ( *out_format )->nChannels * ( *out_format )->wBitsPerSample ) / 8;
      ( *out_format )->nAvgBytesPerSec =
        ( *out_format )->nSamplesPerSec * ( *out_format )->nBlockAlign;
      ( *out_format )->cbSize          = 0;

      CPC_LOG(
        CPC_LOG_LEVEL_INFO,
        "Set format info: f=0x%x, sr=%d, nc=0x%x, bd=0x%x, fs=0x%x",
        ( *out_format )->wFormatTag,
        ( *out_format )->nSamplesPerSec,
        ( *out_format )->nChannels,
        ( *out_format )->wBitsPerSample,
        ( *out_format )->nBlockAlign
        );
    }
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Format is null." );
  }
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
    IAudioClient* audio_client  = NULL;
    WAVEFORMATEX* format = NULL;

    CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Configuring for record." );

    windows_configure_format  (
      in_number_of_channels, 
      in_sample_rate, 
      in_bit_depth, 
      &format
    );

    if( NULL != format )
    {
      HRESULT result =
        windows_configure_device(
          in_device,
          format,
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
          g_recorder_callback_info->user_data           =
            in_callback_user_data;

          if(
            CPC_ERROR_CODE_NO_ERROR 
            == cpc_safe_malloc  (
                ( void** )&( g_recorder_callback_info->platform_data ), 
                sizeof( windows_context )
               )
             )
          {
            (
              ( windows_context* )g_recorder_callback_info->platform_data
            )->audio_client = audio_client;
            (
              ( windows_context* )g_recorder_callback_info->platform_data
            )->format = format;

            result =
              windows_initialize_events_thread(
                audio_client,
                g_recorder_callback_info,
                &g_recorder_data_ready_event,
                &g_recorder_terminate_event,
                &g_recorder_thread,
                ( windows_data_handler_routine )windows_handle_recorder_data,
                LABEL_CAPTURE
              );

            if( S_OK == result )
            {
              result = audio_client->Start();

              return_value = CPC_TRUE;
            }
            else
            {
              CPC_ERROR(
                "Could not initialize recorder threads and events: 0x%x.",
                result
              );
            }
          }
        }
      }
      else
      {
        CPC_ERROR( "Could not configure capture device: 0x%x.", result );
      }
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
  CPC_BOOL return_value =
    windows_stop_thread( &g_recorder_terminate_event, &g_recorder_thread );

  if( NULL != g_recorder_callback_info )
  {
    if( NULL != g_recorder_callback_info->platform_data )
    {
      if(
          NULL
          !=
          (
            ( windows_context* )
            g_recorder_callback_info->platform_data
          )->format
        )
      {
        cpc_safe_free(
            ( void** )
            &(
              ( windows_context* )
              g_recorder_callback_info->platform_data
            )->format
          );
      }

      cpc_safe_free( ( void** ) &( g_recorder_callback_info->platform_data ) );
    }

    cpc_safe_free( ( void** )&g_recorder_callback_info );
  }

  if( NULL != g_recorder_data_ready_event )
  {
    CloseHandle( g_recorder_data_ready_event );

    g_recorder_data_ready_event = NULL;
  }

  if( NULL != g_recorder_terminate_event )
  {
    CloseHandle( g_recorder_terminate_event );

    g_recorder_terminate_event = NULL;
  }

  if( NULL != g_recorder_thread )
  {
    TerminateThread( g_recorder_thread, 0 );

    g_recorder_thread = NULL;
  }

  return( return_value );
}

CPC_BOOL
cahal_stop_playback( void )
{
  CPC_BOOL return_value =
    windows_stop_thread( &g_playback_terminate_event, &g_playback_thread );

  if( NULL != g_playback_callback_info )
  {
    if( NULL != g_playback_callback_info->platform_data )
    {
      if(
        NULL
        !=
          (
            ( windows_context* )
            g_playback_callback_info->platform_data
          )->format
        )
      {
        cpc_safe_free (
          ( void** )
          &( ( windows_context* )
            g_playback_callback_info->platform_data
          )->format
        );
      }

      cpc_safe_free( ( void** ) &( g_playback_callback_info->platform_data ) );
    }

    cpc_safe_free( ( void** )&g_playback_callback_info );
  }

  if( NULL != g_playback_data_ready_event )
  {
    CloseHandle( g_playback_data_ready_event );

    g_playback_data_ready_event = NULL;
  }

  if( NULL != g_playback_terminate_event )
  {
    CloseHandle( g_playback_terminate_event );

    g_playback_terminate_event = NULL;
  }

  if( NULL != g_playback_thread )
  {
    TerminateThread( g_playback_thread, 0 );

    g_playback_thread = NULL;
  }

  return( return_value );
}

CPC_BOOL
windows_stop_thread (
  PHANDLE io_terminate_event,
  PHANDLE io_thread
                    )
{
  CPC_BOOL return_value = CPC_FALSE;

  if( NULL != io_terminate_event && NULL != *io_terminate_event )
  {
    CPC_LOG (
        CPC_LOG_LEVEL_DEBUG, 
        "Sending terminate signal to thread."
      );

    if( SetEvent( *io_terminate_event ) )
    {
      DWORD wait_result = WaitForSingleObject( *io_thread, INFINITE );

      switch( wait_result )
      {
        case WAIT_OBJECT_0:
          return_value = CPC_TRUE;

          *io_thread = NULL;

          CPC_LOG_STRING  (
            CPC_LOG_LEVEL_DEBUG, 
            "Thread successfully terminated."
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
    else
    {
      CPC_ERROR (
        "Could not set terminate event: 0x%x.", 
        GetLastError()
      );
    }
  }
  else
  {
    CPC_ERROR (
      "Terminate event (0x%x) or thread (0x%x) are null.", 
      io_terminate_event, 
      io_thread
    );
  }

  return( return_value );
}

void
cahal_sleep(
UINT32 in_sleep_time
)
{
  Sleep( in_sleep_time );
}

CPC_BOOL
cahal_start_playback(
  cahal_device*            in_device,
  cahal_audio_format_id    in_format_id,
  UINT32                   in_number_of_channels,
  FLOAT64                  in_sample_rate,
  UINT32                   in_bit_depth,
  cahal_playback_callback  in_playback,
  void*                    in_callback_user_data,
  cahal_audio_format_flag  in_format_flags
                    )
{
  CPC_BOOL return_value = CPC_FALSE;

  CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "In start playback!" );

  if(
    cahal_test_device_direction_support(
      in_device,
      CAHAL_DEVICE_OUTPUT_STREAM
    )
    && CAHAL_STATE_INITIALIZED == g_cahal_state
    && NULL == g_recorder_callback_info
    )
  {
    IAudioClient* audio_client  = NULL;
    WAVEFORMATEX* format        = NULL;

    CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Configuring for playback." );

    windows_configure_format(
      in_number_of_channels,
      in_sample_rate,
      in_bit_depth,
      &format
    );

    if( NULL != format )
    {
      HRESULT result =
        windows_configure_device(
          in_device,
          format,
          &audio_client
        );

      if( S_OK == result )
      {
        if( CPC_ERROR_CODE_NO_ERROR
          == cpc_safe_malloc(
          ( void ** )&( g_playback_callback_info ),
          sizeof( cahal_playback_info )
          )
          )
        {
          g_playback_callback_info->playback_device   = in_device;
          g_playback_callback_info->playback_callback = in_playback;
          g_playback_callback_info->user_data         = in_callback_user_data;

          if(
            CPC_ERROR_CODE_NO_ERROR
            == cpc_safe_malloc(
            ( void** )&( g_playback_callback_info->platform_data ),
            sizeof( windows_context )
            )
            )
          {
            (
              ( windows_context* )g_playback_callback_info->platform_data
              )->audio_client = audio_client;
            (
              ( windows_context* )g_playback_callback_info->platform_data
              )->format = format;

            result =
              windows_initialize_events_thread(
                audio_client,
                g_playback_callback_info,
                &g_playback_data_ready_event,
                &g_playback_terminate_event,
                &g_playback_thread,
                ( windows_data_handler_routine )windows_handle_playback_data,
                LABEL_RENDER
              );

            if( S_OK == result )
            {
              windows_handle_playback_data( g_playback_callback_info );

              result = audio_client->Start( );

              return_value = CPC_TRUE;
            }
            else
            {
              CPC_ERROR(
                "Could not initialize playback threads and events: 0x%x.",
                result
              );
            }
          }
        }
      }
      else
      {
        CPC_ERROR( "Could not configure playback device: 0x%x.", result );
      }
    }
  }
  else
  {
    CPC_ERROR(
      "Direction (%d) not support by %s.",
      CAHAL_DEVICE_OUTPUT_STREAM,
      in_device->device_name
    );
  }

  return( return_value );
}

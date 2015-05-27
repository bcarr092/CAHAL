/*! \file   windows_cahal_device.cpp
    \brief  Implementation that queries the underlying Windows hardware and
            creates the device_list struct.

    \author Brent Carrara
 */

#include "windows/windows_cahal_device.hpp"

cahal_recorder_info* g_recorder_callback_info = NULL;
cahal_playback_info* g_playback_callback_info = NULL;

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
    && NULL == g_playback_callback_info
    )
  {
    CPC_BOOL return_value = CPC_FALSE;
    IMMDeviceEnumerator *enumerator = NULL;
    IMMDeviceCollection *device_collection = NULL;

    CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Enumerating devices" );

    HRESULT result =
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

          IAudioClient *audio_client = NULL;

          CPC_MEMSET( &format, 0x0, sizeof( WAVEFORMATEX ) );

          format.wFormatTag = WAVE_FORMAT_PCM;
          format.nChannels = in_number_of_channels;
          format.nSamplesPerSec = ( DWORD ) in_sample_rate;
          format.wBitsPerSample = in_bit_depth;
          format.nBlockAlign =
            ( format.nChannels * format.wBitsPerSample ) / 8;
          format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
          format.cbSize = 0;

          CPC_LOG (
            CPC_LOG_LEVEL_DEBUG,
            "Format info: sr=%d, nc=0x%x, bd=0x%x",
            format.nSamplesPerSec,
            format.nChannels,
            format.wBitsPerSample
                  );

          result =
            device->Activate  (
              __uuidof( IAudioClient ), 
              CLSCTX_ALL, 
              NULL, 
              ( void** )&audio_client
                              );

          if( S_OK == result )
          {
            REFERENCE_TIME requested_latency = 0;

            result = audio_client->GetDevicePeriod( NULL, &requested_latency );

            CPC_LOG (
              CPC_LOG_LEVEL_DEBUG, 
              "Hardware sample rate: %lld.", 
              requested_latency
                    );

            if( S_OK == result )
            {
              result =
                audio_client->Initialize  (
                  AUDCLNT_SHAREMODE_EXCLUSIVE, 
                  AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 
                  requested_latency, 
                  requested_latency, 
                  &format, 
                  NULL
                                          );

              if( AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED == result )
              {
                UINT32 number_of_frames = 0;

                result = audio_client->GetBufferSize( &number_of_frames );

                if( S_OK == result )
                {
                  requested_latency =
                    ( REFERENCE_TIME )
                      ( 
                        (
                          10000.0 * 1000
                          / format.nSamplesPerSec * number_of_frames
                        )
                        + 0.5
                      );

                  CPC_LOG (
                    CPC_LOG_LEVEL_DEBUG, 
                    "New hardware sample rate is %lld.", 
                    requested_latency
                          );

                  audio_client->Release();

                  result =
                    device->Activate(
                      __uuidof( IAudioClient ),
                      CLSCTX_ALL,
                      NULL,
                      ( void** )&audio_client
                    );

                  if( S_OK == result )
                  {
                    result =
                      audio_client->Initialize  (
                        AUDCLNT_SHAREMODE_EXCLUSIVE,
                        AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                        requested_latency,
                        requested_latency,
                        &format,
                        NULL
                                                );

                    if( S_OK == result )
                    {
                      HANDLE event = CreateEvent( NULL, FALSE, FALSE, NULL );

                      if( NULL != event )
                      {
                        result = audio_client->SetEventHandle( event );

                        if( S_OK == result )
                        {
                          result = audio_client->Start();

                          CPC_ERROR( "Start: 0x%x.", result );
                        }
                        else
                        {
                          CPC_ERROR( "Could not set event: 0x%x.", result );
                        }
                      }
                      else
                      {
                        CPC_LOG_STRING  (
                          CPC_LOG_LEVEL_ERROR, 
                          "Could not create event."
                                        );
                      }
                    }
                    else
                    {
                      CPC_ERROR( "Could not initialize client: 0x%x.", result );
                    }
                  }
                  else
                  {
                    CPC_ERROR (
                      "Could not active client (again): 0x%x.", 
                      result
                      );
                  }
                }
                else
                {
                  CPC_ERROR( "Could not read buffer size: 0x%x.", result );
                }
              }
            }
            else
            {
              CPC_ERROR( "Could not requrest device period: 0x%x.", result );
            }
          }
          else
          {
            CPC_ERROR( "Could not active device: 0x%x.", result );
          }
        }
        else
        {
          CPC_ERROR(
            "Could not retrieve item 0x%x: 0x%x.",
            in_device->handle,
            result
            );
        }
      }
      else
      {
        CPC_ERROR( "Could not enumerate devices: 0x%x.", result );
      }
    }
    else
    {
      CPC_ERROR( "Could not create enumerator COM object: 0x%x.", result );
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

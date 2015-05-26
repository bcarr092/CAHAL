#include "windows/windows_cahal_device.hpp"

const CLSID CLSID_MMDeviceEnumerator  = __uuidof( MMDeviceEnumerator );
const IID IID_IMMDeviceEnumerator     = __uuidof( IMMDeviceEnumerator );
const IID IID_IAudioClient            = __uuidof( IAudioClient );

void
windows_set_device_info (
  cahal_device* out_device,
  IMMDevice*    in_endpoint
                        )
{
  PROPVARIANT value;

  DWORD state                 = NULL;
  LPWSTR device_id            = NULL;
  IPropertyStore *properties  = NULL;

  HRESULT result = in_endpoint->GetId( &device_id );

  if( S_OK == result )
  {
    if(
      CPC_ERROR_CODE_NO_ERROR
        == cpc_safe_malloc  (
            ( void** )&( out_device->device_uid ),
            sizeof( CHAR )* ( wcslen( device_id ) + 1 )
                            )
      )
    {
      USIZE number_converted = 0;

      wcstombs_s  (
        &number_converted, 
        out_device->device_uid, 
        wcslen( device_id ) + 1, 
        device_id, 
        wcslen( device_id )
                  );

      CPC_LOG (
        CPC_LOG_LEVEL_DEBUG,
        "Device identifier is %s.",
        out_device->device_uid
              );
    }

    CoTaskMemFree( device_id );
  }

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

  result = in_endpoint->OpenPropertyStore( STGM_READ, &properties );

  if( S_OK == result )
  {
    PropVariantInit( &value );

    result = properties->GetValue( PKEY_Device_FriendlyName, &value );

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

    properties->Release();
  }

  windows_set_device_defaults( out_device, in_endpoint );
}

void
windows_set_device_defaults(
  cahal_device* out_device,
  IMMDevice*    in_endpoint
                            )
{
  IAudioClient *audio_client  = NULL;
  WAVEFORMATEX *format        = NULL;
  HRESULT result              = S_OK;

  result = in_endpoint->Activate (
    IID_IAudioClient,
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
}

cahal_device**
windows_get_device_list( void )
{
  cahal_device** device_list              = NULL;
  HRESULT result                          = S_OK;
  IMMDeviceEnumerator *enumerator         = NULL;
  IMMDeviceCollection *device_collection  = NULL;

  CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Enumerating devices" );

  result =
      CoCreateInstance  (
        CLSID_MMDeviceEnumerator, 
        NULL, 
        CLSCTX_ALL, 
        IID_IMMDeviceEnumerator, 
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
              CPC_LOG_BUFFER(
                CPC_LOG_LEVEL_TRACE,
                "Device",
                ( UCHAR* )device_list[i],
                sizeof( cahal_device ),
                8
                );

              IMMDevice *endpoint = NULL; 

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

#include "cahal.h"
#include "darwin/osx_cahal.h"

void list_devices( void )
{
  AudioObjectID* device_list  = NULL;
  UINT32 num_devices          = 0;

  if( noErr == osx_get_audio_objects_handles( &device_list, &num_devices ) )
  {
    CFStringRef device_name;

    for( UINT32 i = 0; i < num_devices; i++ )
    {
      UINT32 property_size = sizeof( CFStringRef );

      AudioObjectPropertyAddress property_address =
      {
          kAudioObjectPropertyName,
          kAudioObjectPropertyScopeGlobal,
          kAudioObjectPropertyElementMaster
      };

      OSStatus result =
          AudioObjectGetPropertyData  (
              device_list[ i ],
              &property_address,
              0,
              NULL,
              &property_size,
              &device_name
                                      );

      if( result )
      {
        cpc_log (
            CPC_LOG_LEVEL_ERROR,
            "Error in AudioObjectGetPropertyDataSize: %d\n",
            result
                );

        osx_print_code( CPC_LOG_LEVEL_ERROR, result );
      }
      else
      {
        CFShow( device_name );
      }
    }
  }
}

OSStatus
  osx_get_audio_objects_handles (
      AudioObjectID** io_device_list,
      UINT32*         out_num_devices
                            )
{
  UINT32    property_size;
  OSStatus  result          = noErr;

  *io_device_list = NULL;

  AudioObjectPropertyAddress property_address =
    {
        kAudioHardwarePropertyDevices,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster
    };

  result =
        AudioObjectGetPropertyDataSize  (
            kAudioObjectSystemObject,
            &property_address,
            0,
            NULL,
            &property_size
                                        );

  if( result )
  {
    cpc_log (
        CPC_LOG_LEVEL_ERROR,
        "Error in AudioObjectGetPropertyDataSize: %d\n",
        result
            );

    osx_print_code( CPC_LOG_LEVEL_ERROR, result );
  }
  else
  {
    *out_num_devices  = property_size / sizeof( AudioDeviceID );
    *io_device_list   =
        ( AudioDeviceID* ) malloc( *out_num_devices * sizeof( AudioDeviceID ) );

    result =
        AudioObjectGetPropertyData  (
            kAudioObjectSystemObject,
            &property_address,
            0,
            NULL,
            &property_size,
            *io_device_list
                                    );

    if( result )
    {
      cpc_log (
          CPC_LOG_LEVEL_ERROR,
          "Error in AudioObjectGetPropertyData: %d\n",
          result
              );

      osx_print_code( CPC_LOG_LEVEL_ERROR, result );

      if( NULL != *io_device_list )
      {
        free( *io_device_list );
      }

      *out_num_devices = 0;
    }
  }

  return( result );
}



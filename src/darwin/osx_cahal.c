#include "darwin/osx_cahal.h"

cahal_device**
cahal_get_device_list( void )
{
  AudioObjectID* device_ids   = NULL;
  cahal_device** device_list  = NULL;
  UINT32 num_devices          = 0;

  if( noErr == osx_get_audio_device_handles( &device_ids, &num_devices ) )
  {
    device_list =
    ( cahal_device** ) malloc( ( num_devices + 1 ) * sizeof( cahal_device* ) );
    
    memset( device_list, 0, ( num_devices + 1 ) * sizeof( cahal_device* ) );
    
    for( UINT32 i = 0; i < num_devices; i++ )
    {
      device_list[ i ] = ( cahal_device* ) malloc( sizeof( cahal_device ) );
      
      memset( device_list[ i ], 0, sizeof( cahal_device ) );
      
      osx_set_cahal_device_struct( device_ids[ i ], device_list[ i ] );
      
      if( cpc_log_get_current_log_level() <= CPC_LOG_LEVEL_DEBUG )
      {
        cahal_print_device( device_list[ i ] );
      }
    }
    
    if( NULL != device_ids )
    {
      free( device_ids );
    }
  }
  
  return( device_list );
}

OSStatus
osx_get_audio_device_handles (
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
    CPC_ERROR (
               "Error in AudioObjectGetPropertyDataSize: %d",
               result
               );

    CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
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
      CPC_ERROR (
                 "Error in AudioObjectGetPropertyData: %d",
                 result
                 );

      CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );

      if( NULL != *io_device_list )
      {
        free( *io_device_list );
      }

      *out_num_devices = 0;
    }
  }

  return( result );
}



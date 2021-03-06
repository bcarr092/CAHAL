/*! \file osx_cahal.h
 
    \author Brent Carrara
 */
#include "darwin/osx/osx_cahal.h"

void
cahal_initialize( void )
{
  switch( g_cahal_state )
  {
    case CAHAL_STATE_NOT_INITIALIZED:
      g_cahal_state = CAHAL_STATE_INITIALIZED;
      
      break;
    case CAHAL_STATE_INITIALIZED:
    case CAHAL_STATE_TERMINATED:
      CPC_LOG_STRING  (
                       CPC_LOG_LEVEL_WARN,
                       "CAHAL has already been initialized."
                       );
      break;
  }
}

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
                       CPC_LOG_LEVEL_WARN,
                       "CAHAL has already been terminated"
                       );
      break;
    case CAHAL_STATE_INITIALIZED:
      g_cahal_state = CAHAL_STATE_TERMINATED;
      
      break;
  }
}

cahal_device**
cahal_get_device_list( void )
{
  cahal_device** device_list = NULL;
  
  if( CAHAL_STATE_INITIALIZED == g_cahal_state )
  {
    if( NULL == g_device_list )
    {
      g_device_list = osx_get_device_list();
      
      CPC_LOG (
               CPC_LOG_LEVEL_DEBUG,
               "Generated device list: 0x%x.",
               g_device_list
               );
    }
    else
    {
      CPC_LOG (
               CPC_LOG_LEVEL_DEBUG,
               "Returning existing device list: 0x%x.",
               g_device_list
               );
    }
    
    device_list = g_device_list;
  }
  else
  {
    CPC_ERROR( "CAHAL has not been initialized: %d.", g_cahal_state );
  }
  
  return( device_list );
}

cahal_device**
osx_get_device_list( void )
{
  AudioObjectID* device_ids   = NULL;
  cahal_device** device_list  = NULL;
  UINT32 num_devices          = 0;

  if( noErr == osx_get_audio_device_handles( &device_ids, &num_devices ) )
  {
    CPC_LOG_BUFFER  (
                     CPC_LOG_LEVEL_TRACE,
                     "ObjectIDs",
                     ( UCHAR* ) device_ids,
                     sizeof( AudioObjectID ) * num_devices,
                     8
                     );
    
    if  ( CPC_ERROR_CODE_NO_ERROR
         == cpc_safe_malloc (
                             ( void ** ) &( device_list ),
                             ( num_devices + 1 ) * sizeof( cahal_device* )
                             )
         )
    {
      for( UINT32 i = 0; i < num_devices; i++ )
      {
        if  ( CPC_ERROR_CODE_NO_ERROR
             == cpc_safe_malloc (
                                 ( void ** ) &( device_list[ i ] ),
                                 sizeof( cahal_device )
                                 )
             )
        {
          CPC_LOG_BUFFER  (
                           CPC_LOG_LEVEL_TRACE,
                           "Device",
                           ( UCHAR* ) device_list[ i ],
                           sizeof( cahal_device ),
                           8
                           );
          
          osx_set_cahal_device_struct( device_ids[ i ], device_list[ i ] );
          
          if( cpc_log_get_current_log_level() <= CPC_LOG_LEVEL_DEBUG )
          {
            cahal_print_device( device_list[ i ] );
          }
          
          CPC_LOG_BUFFER  (
                           CPC_LOG_LEVEL_TRACE,
                           "Device",
                           ( UCHAR* ) device_list[ i ],
                           sizeof( cahal_device ),
                           8
                           );
        }
        else
        {
          break;
        }
      }
    }
  }
  
  cpc_safe_free( ( void** ) &device_ids );
  
  return( device_list );
}

OSStatus
osx_get_audio_device_handles (
                              AudioObjectID** io_device_list,
                              UINT32*         out_num_devices
                              )
{
  UINT32    property_size;
  OSStatus  result          = kAudioHardwareUnspecifiedError;

  if( NULL != io_device_list )
  {
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
      
      if  ( CPC_ERROR_CODE_NO_ERROR
           == cpc_safe_malloc (
                               ( void ** ) &( *io_device_list ),
                               *out_num_devices * sizeof( AudioDeviceID )
                               )
           )
      {
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
          
          cpc_safe_free( ( void** ) &( *io_device_list ) );

          *out_num_devices = 0;
        }
      }
      else
      {
        result = kAudioHardwareUnspecifiedError;
      }
    }
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_ERROR, "Null device list." );
  }

  return( result );
}



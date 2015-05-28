/*! \file   windows_cahal.c

    \author Brent Carrara
 */

#include "windows/windows_cahal.h"

void
cahal_initialize( void )
{
  switch( g_cahal_state )
  {
  case CAHAL_STATE_NOT_INITIALIZED:
    CoInitialize( NULL );

    g_cahal_state = CAHAL_STATE_INITIALIZED;

    break;
  case CAHAL_STATE_INITIALIZED:
  case CAHAL_STATE_TERMINATED:
    CPC_LOG_STRING(
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
    CPC_LOG_STRING(
      CPC_LOG_LEVEL_WARN,
      "CAHAL has already been terminated"
      );
    break;
  case CAHAL_STATE_INITIALIZED:
    CoUninitialize();

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
      g_device_list = windows_get_device_list();

      CPC_LOG(
        CPC_LOG_LEVEL_DEBUG,
        "Generated device list: 0x%x.",
        g_device_list
        );
    }
    else
    {
      CPC_LOG(
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

CPC_BOOL
cahal_stop_playback( void )
{
  return( CPC_FALSE );
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
  return( CPC_FALSE );
}

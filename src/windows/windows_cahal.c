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
    g_cahal_state = CAHAL_STATE_INITIALIZED;

    CoInitialize( NULL );

    if( ! cpc_is_initialized() )
    {
      cpc_initialize();
    }

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
    g_cahal_state = CAHAL_STATE_TERMINATED;

    cahal_free_device_list();

    CoUninitialize();

    if( cpc_is_initialized() )
    {
      cpc_terminate();
    }

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

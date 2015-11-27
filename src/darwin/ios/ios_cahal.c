/*! \file   ios_cahal.c
 
    \author Brent Carrara
 */
#include "darwin/ios/ios_cahal.h"

void
cahal_initialize( void )
{
  switch( g_cahal_state )
  {
    case CAHAL_STATE_NOT_INITIALIZED:
      ios_initialize_recording();
      
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
  CPC_LOG( CPC_LOG_LEVEL_INFO, "In terminate: %d!", g_cahal_state );
  
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
      ios_terminate_recording();
      
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
      g_device_list = ios_set_cahal_device_struct();
      
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
void
ios_interrupt_listener  (
                         void* in_user_data,
                         UINT32 in_state
                         )
{
  CPC_ERROR( "Interrupted. State = 0x%x.", in_state );
  
  CPC_EXIT( CPC_ERROR_CODE_APPLICATION_INTERRUPTED_IOS );
}

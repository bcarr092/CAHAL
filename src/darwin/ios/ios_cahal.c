#include "darwin/ios/ios_cahal.h"

void
cahal_initialize( void )
{
  ios_initialize_recording();
}

cahal_device**
cahal_get_device_list( void )
{
  cahal_device** device_list  = NULL;
  
  OSStatus result = noErr;
  
  if( noErr == result )
  {
    device_list = ios_set_cahal_device_struct();
    
    if( result )
    {
      CPC_ERROR( "Could not set session inactive: 0x%x.", result );
      
      CPC_PRINT_CODE( CPC_LOG_LEVEL_ERROR, result );
    }
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

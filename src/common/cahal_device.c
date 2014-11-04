#include "cahal_device.h"

void
print_cahal_device  (
                     cahal_device* device
                     )
{
  if( NULL == device )
  {
    return;
  }
  
  cpc_log( CPC_LOG_LEVEL_INFO, "CAHAL Device:" );
  cpc_log( CPC_LOG_LEVEL_INFO, "\tID: 0x%x", device->handle );
  
  if( NULL != device->device_name )
  {
    cpc_log( CPC_LOG_LEVEL_INFO, "\tDevice name: %s", device->device_name );
  }
  
  if( NULL != device->model )
  {
    cpc_log( CPC_LOG_LEVEL_INFO, "\tModel: %s", device->model );  }
  
  if( NULL != device->manufacturer )
  {
    cpc_log( CPC_LOG_LEVEL_INFO, "\tManufacturer: %s", device->manufacturer );
  }
  
  if( NULL != device->serial_number )
  {
    cpc_log( CPC_LOG_LEVEL_INFO, "\tSerial #: %s", device->serial_number );
  }
  
  if( NULL != device->version )
  {
    cpc_log( CPC_LOG_LEVEL_INFO, "\tVersion: %s", device->version );
  }
}

void
free_cahal_device_list (
                        cahal_device** device_list
                        )
{
  if( NULL == device_list )
  {
    return;
  }
  else
  {
    UINT32 device_index   = 0;
    cahal_device* device  = device_list[ 0 ];
    
    while( NULL != device )
    {
      if( NULL != device->device_name )
      {
        free( device->device_name );
      }
      
      if( NULL != device->model )
      {
        free( device->model );
      }
      
      if( NULL != device->manufacturer )
      {
        free( device->manufacturer );
      }
      
      if( NULL != device->serial_number )
      {
        free( device->serial_number );
      }
      
      if( NULL != device->version )
      {
        free( device->version );
      }
      
      free( device );
      
      device_index++;
      
      device = device_list[ device_index ];
    }
    
    free( device_list );
  }
}

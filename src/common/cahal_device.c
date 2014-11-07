#include "cahal_device.h"

void
print_cahal_device_list (
                         cahal_device** in_device_list
                         )
{
  if( NULL != in_device_list )
  {
    UINT32 index          = 0;
    cahal_device* device  = in_device_list[ index++ ];
    
    while( NULL != device )
    {
      print_cahal_device( device );
      
      device  = in_device_list[ index++ ];
    }
  }
}

void
print_cahal_device  (
                     cahal_device* in_device
                     )
{
  if( NULL == in_device )
  {
    return;
  }
  
  CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "CAHAL Device:" );
  CPC_LOG( CPC_LOG_LEVEL_INFO, "\tID: 0x%x", in_device->handle );
  
  if( NULL != in_device->device_name )
  {
    CPC_LOG( CPC_LOG_LEVEL_INFO, "\tDevice name: %s", in_device->device_name );
  }
  
  if( NULL != in_device->model )
  {
    CPC_LOG( CPC_LOG_LEVEL_INFO, "\tModel: %s", in_device->model );  }
  
  if( NULL != in_device->manufacturer )
  {
    CPC_LOG (
             CPC_LOG_LEVEL_INFO,
             "\tManufacturer: %s",
             in_device->manufacturer
             );
  }
  
  if( NULL != in_device->serial_number )
  {
    CPC_LOG( CPC_LOG_LEVEL_INFO, "\tSerial #: %s", in_device->serial_number );
  }
  
  if( NULL != in_device->version )
  {
    CPC_LOG( CPC_LOG_LEVEL_INFO, "\tVersion: %s", in_device->version );
  }
  
  if( NULL != in_device->device_uid )
  {
    CPC_LOG( CPC_LOG_LEVEL_INFO, "\tDevice UID: %s", in_device->device_uid );
  }
  
  if( NULL != in_device->model_uid )
  {
    CPC_LOG( CPC_LOG_LEVEL_INFO, "\tModel UID: %s", in_device->model_uid );
  }
  
  CPC_LOG (
           CPC_LOG_LEVEL_INFO,
           "\tPreferred sample rate: %.2f",
           in_device->preferred_sample_rate
           );
  
  if( NULL != in_device->supported_sample_rates )
  {
    UINT32 index                          = 0;
    cahal_sample_rate_range* sample_rate  =
    in_device->supported_sample_rates[ index++ ];
    
    CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "\tSupported sample rates:" );
    
    while( NULL != sample_rate )
    {
      CPC_LOG( CPC_LOG_LEVEL_INFO, "\t\tSample rate range %d", index );
      
      CPC_LOG (
               CPC_LOG_LEVEL_INFO,
               "\t\t\tMinimum> %.2f",
               sample_rate->minimum_rate
               );
      
      CPC_LOG (
               CPC_LOG_LEVEL_INFO,
               "\t\t\tMaximum> %.2f",
               sample_rate->maximum_rate
               );
      
      sample_rate = in_device->supported_sample_rates[ index++ ];
    }
  }
  
  if( NULL != in_device->device_streams )
  {
    print_cahal_device_stream_list( in_device->device_streams );

  }
  
  CPC_LOG (
           CPC_LOG_LEVEL_INFO,
           "\tPreferred number of channels: %d",
           in_device->preferred_number_of_channels
           );
  
  CPC_LOG(
          CPC_LOG_LEVEL_INFO,
          "\tIs alive?: %d",
          in_device->is_alive
          );
  
  CPC_LOG(
          CPC_LOG_LEVEL_INFO,
          "\tIs running?: %d",
          in_device->is_running
          );
}

void
free_cahal_device_list (
                        cahal_device** in_device_list
                        )
{
  if( NULL == in_device_list )
  {
    return;
  }
  else
  {
    UINT32 device_index   = 0;
    cahal_device* device  = in_device_list[ 0 ];
    
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
      
      if( NULL != device->device_uid )
      {
        free( device->device_uid );
      }
      
      if( NULL != device->model_uid )
      {
        free( device->model_uid );
      }
      
      if( NULL != device->supported_sample_rates )
      {
        UINT32 index                          = 0;
        cahal_sample_rate_range* sample_rate  =
        device->supported_sample_rates[ index++ ];
        
        while( NULL != sample_rate )
        {
          free( sample_rate );
          
          sample_rate = device->supported_sample_rates[ index++ ];
        }
        
        free( device->supported_sample_rates );
      }
      
      if( NULL != device->device_streams )
      {
        free_cahal_device_stream_list( device->device_streams );
      }
      
      free( device );
      
      device_index++;
      
      device = in_device_list[ device_index ];
    }
    
    free( in_device_list );
    
    in_device_list = NULL;
  }
}

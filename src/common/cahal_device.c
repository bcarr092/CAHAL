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
  
  CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "CAHAL Device:" );
  CPC_LOG( CPC_LOG_LEVEL_INFO, "\tID: 0x%x", device->handle );
  
  if( NULL != device->device_name )
  {
    CPC_LOG( CPC_LOG_LEVEL_INFO, "\tDevice name: %s", device->device_name );
  }
  
  if( NULL != device->model )
  {
    CPC_LOG( CPC_LOG_LEVEL_INFO, "\tModel: %s", device->model );  }
  
  if( NULL != device->manufacturer )
  {
    CPC_LOG( CPC_LOG_LEVEL_INFO, "\tManufacturer: %s", device->manufacturer );
  }
  
  if( NULL != device->serial_number )
  {
    CPC_LOG( CPC_LOG_LEVEL_INFO, "\tSerial #: %s", device->serial_number );
  }
  
  if( NULL != device->version )
  {
    CPC_LOG( CPC_LOG_LEVEL_INFO, "\tVersion: %s", device->version );
  }
  
  if( NULL != device->device_uid )
  {
    CPC_LOG( CPC_LOG_LEVEL_INFO, "\tDevice UID: %s", device->device_uid );
  }
  
  if( NULL != device->model_uid )
  {
    CPC_LOG( CPC_LOG_LEVEL_INFO, "\tModel UID: %s", device->model_uid );
  }
  
  CPC_LOG (
           CPC_LOG_LEVEL_INFO,
           "\tPreferred sample rate: %.2f",
           device->preferred_sample_rate
           );
  
  if( NULL != device->supported_sample_rates )
  {
    UINT32 index          = 0;
    FLOAT64* sample_rate  = device->supported_sample_rates[ index++ ];
    
    CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "\tSupported sample rates:" );
    
    while( NULL != sample_rate )
    {
      CPC_LOG( CPC_LOG_LEVEL_INFO, "\t\t%.2f", *sample_rate );
      
      sample_rate = device->supported_sample_rates[ index++ ];
    }
  }
  
  if( NULL != device->device_streams )
  {
    UINT32 index                        = 0;
    cahal_device_stream* device_stream  = device->device_streams[ index++ ];
    
    CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "\tAvailable streams:" );
    
    while( NULL != device_stream )
    {
      CPC_LOG( CPC_LOG_LEVEL_INFO, "\t\tID: 0x%x", device_stream->handle );
      
      if( CAHAL_DEVICE_OUTPUT_STREAM == device_stream->direction )
      {
        CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "\t\tDirection: OUTPUT" );
      }
      else
      {
        CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "\t\tDirection: INTPUT" );
      }
      
      device_stream = device->device_streams[ index++ ];
    }
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
        UINT32 index          = 0;
        FLOAT64* sample_rate  = device->supported_sample_rates[ index++ ];
        
        while( NULL != sample_rate )
        {
          free( sample_rate );
          
          sample_rate = device->supported_sample_rates[ index++ ];
        }
        
        free( device->supported_sample_rates );
      }
      
      if( NULL != device->device_streams )
      {
        UINT32 index                        = 0;
        cahal_device_stream* device_stream  = device->device_streams[ index++ ];
        
        while( NULL != device_stream )
        {
          free( device_stream );
          
          device_stream = device->device_streams[ index++ ];
        }
        
        free( device->device_streams );
      }
      
      free( device );
      
      device_index++;
      
      device = device_list[ device_index ];
    }
    
    free( device_list );
  }
}

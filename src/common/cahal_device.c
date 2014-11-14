#include "cahal_device.h"

void
cahal_print_device_list (
                         cahal_device** in_device_list
                         )
{
  if( NULL != in_device_list )
  {
    UINT32 index          = 0;
    cahal_device* device  = in_device_list[ index++ ];
    
    while( NULL != device )
    {
      cahal_print_device( device );
      
      device  = in_device_list[ index++ ];
    }
  }
}

void
cahal_print_device  (
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
    cahal_print_device_stream_list( in_device->device_streams );

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
cahal_free_device_list (
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
      CPC_LOG_BUFFER  (
                       CPC_LOG_LEVEL_TRACE,
                       "device",
                       ( UCHAR* ) device,
                       sizeof( cahal_device ),
                       8
                       );
      
      cpc_safe_free( ( void** ) &( device->device_name ) );
      cpc_safe_free( ( void** ) &( device->model ) );
      cpc_safe_free( ( void** ) &( device->manufacturer ) );
      cpc_safe_free( ( void** ) &( device->serial_number ) );
      cpc_safe_free( ( void** ) &( device->version ) );
      cpc_safe_free( ( void** ) &( device->device_uid ) );
      cpc_safe_free( ( void** ) &( device->model_uid ) );
      
      
      if( NULL != device->supported_sample_rates )
      {
        UINT32 index                          = 0;
        cahal_sample_rate_range* sample_rate  =
        device->supported_sample_rates[ index++ ];
        
        while( NULL != sample_rate )
        {
          cpc_safe_free( ( void** ) &( sample_rate ) );
          
          sample_rate = device->supported_sample_rates[ index++ ];
        }
        
        cpc_safe_free( ( void** ) &( device->supported_sample_rates ) );
      }
      
      if( NULL != device->device_streams )
      {
        cahal_free_device_stream_list( device->device_streams );
        
        cpc_safe_free( ( void** ) &( device->device_streams ) );
      }
      
      CPC_LOG_BUFFER  (
                       CPC_LOG_LEVEL_TRACE, "device",
                       ( UCHAR* ) device,
                       sizeof( cahal_device ),
                       8
                       );
      
      cpc_safe_free( ( void** ) &( device ) );
      
      device_index++;
      
      device = in_device_list[ device_index ];
    }
    
    cpc_safe_free( ( void** ) &( in_device_list ) );
  }
}

BOOL
cahal_test_device_direction_support  (
                                      cahal_device*                 in_device,
                                      cahal_device_stream_direction in_direction
                                      )
{
  BOOL result = FALSE;
  
  if( NULL != in_device )
  {
    UINT32 index                = 0;
    cahal_device_stream* stream = in_device->device_streams[ index++ ];
    
    while( NULL != stream && ! result )
    {
      if( in_direction == stream->direction )
      {
        result = TRUE;
      }
      
      stream = in_device->device_streams[ index++ ];
    }
    
    CPC_LOG (
             CPC_LOG_LEVEL_TRACE,
             "Testing direction %d on device %d: %d",
             in_direction,
             in_device->handle,
             result
             );
  }
  else
  {
    CPC_ERROR( "Attempting to test direction %d on null device", in_direction );
  }
  
  return( result );
}

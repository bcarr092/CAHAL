#!/usr/local/bin/python -W error

import cahal_tests
import unittest
import string
import types

class TestsCAHALDevice( unittest.TestCase ):
  def test_cahal_set_default_device( self ):
    self.assertFalse(                                  \
      cahal_tests.cahal_set_default_device( None, 0 )  \
                    )

    device_list = cahal_tests.cahal_get_device_list()
    index       = 0;                                                                    
    device      = cahal_tests.cahal_device_list_get( device_list, index )               
                                                                                
    while( device ):                                                                
      stream_index  = 0                                                             
      stream        =                               \
        cahal_tests.cahal_device_stream_list_get  ( \
          device.device_streams,                    \
          stream_index                              \
                                                  )
      self.assertFalse(                                    \
        cahal_tests.cahal_set_default_device( device, 2 )  \
                      )

      self.assertFalse(                                      \
        cahal_tests.cahal_set_default_device( device, 20 )   \
                      )
                                                                               
      supports_input  = False
      supports_output = False

      while( stream ):                                                              
        if( cahal_tests.CAHAL_DEVICE_OUTPUT_STREAM == stream.direction ):
          self.assertTrue (                           \
            cahal_tests.cahal_set_default_device  (   \
              device,                                 \
              cahal_tests.CAHAL_DEVICE_OUTPUT_STREAM  \
                                                  )   \
                          )

          supports_output = True

        if( cahal_tests.CAHAL_DEVICE_INPUT_STREAM == stream.direction ):
          self.assertTrue (                         \
            cahal_tests.cahal_set_default_device (  \
              device,                               \
              cahal_tests.CAHAL_DEVICE_INPUT_STREAM \
                                                 )  \
                          )

          supports_input = True

        stream_index += 1
        stream        =                               \
          cahal_tests.cahal_device_stream_list_get  ( \
            device.device_streams,                    \
            stream_index                              \
                                                    )

      if( not supports_input ):
        self.assertFalse (                         \
          cahal_tests.cahal_set_default_device (   \
            device,                                \
            cahal_tests.CAHAL_DEVICE_INPUT_STREAM  \
                                               )   \
                          )       

      if( not supports_output ):
        self.assertFalse  (                        \
          cahal_tests.cahal_set_default_device (   \
            device,                                \
            cahal_tests.CAHAL_DEVICE_OUTPUT_STREAM \
                                               )   \
                          )

      index   += 1
      device  = cahal_tests.cahal_device_list_get( device_list, index )

"""
  def test_cahal_test_device_direction_support( self ):
    self.assertFalse(                                             \
      cahal_tests.cahal_test_device_direction_support( None, 0 )  \
                    )

    device_list = cahal_tests.cahal_get_device_list()
    index       = 0;                                                                    
    device      = cahal_tests.cahal_device_list_get( device_list, index )               
                                                                                
    while( device ):                                                                
      stream_index  = 0                                                             
      stream        =                               \
        cahal_tests.cahal_device_stream_list_get  ( \
          device.device_streams,                    \
          stream_index                              \
                                                  )
      self.assertFalse(                                               \
        cahal_tests.cahal_test_device_direction_support( device, 2 )  \
                      )

      self.assertFalse(                                                 \
        cahal_tests.cahal_test_device_direction_support( device, 20 )   \
                      )
                                                                               
      supports_input  = False
      supports_output = False

      while( stream ):                                                              
        if( cahal_tests.CAHAL_DEVICE_OUTPUT_STREAM == stream.direction ):
          self.assertTrue (                                   \
            cahal_tests.cahal_test_device_direction_support ( \
              device,                                         \
              cahal_tests.CAHAL_DEVICE_OUTPUT_STREAM          \
                                                            ) \
                          )

          supports_output = True

        if( cahal_tests.CAHAL_DEVICE_INPUT_STREAM == stream.direction ):
          self.assertTrue (                                   \
            cahal_tests.cahal_test_device_direction_support ( \
              device,                                         \
              cahal_tests.CAHAL_DEVICE_INPUT_STREAM           \
                                                            ) \
                          )

          supports_input = True

        stream_index += 1
        stream        =                               \
          cahal_tests.cahal_device_stream_list_get  ( \
            device.device_streams,                    \
            stream_index                              \
                                                    )

      if( not supports_input ):
        self.assertFalse (                                    \
          cahal_tests.cahal_test_device_direction_support (   \
            device,                                           \
            cahal_tests.CAHAL_DEVICE_INPUT_STREAM             \
                                                          )   \
                          )       

      if( not supports_output ):
        self.assertFalse  (                                   \
          cahal_tests.cahal_test_device_direction_support (   \
            device,                                           \
            cahal_tests.CAHAL_DEVICE_OUTPUT_STREAM            \
                                                          )   \
                          )

      index   += 1
      device  = cahal_tests.cahal_device_list_get( device_list, index )

  def test_get_device_list( self ):
    device_list = cahal_tests.cahal_get_device_list()

    self.assertNotEqual( device_list, None )

    index   = 0;                                                                    
    device  = cahal_tests.cahal_device_list_get( device_list, index )               
                                                                                
    while( device ):                                                                
      self.assertNotEqual( device, None )

      index += 1                                                                    

      device = cahal_tests.cahal_device_list_get( device_list, index )

  def test_free_cahal_device_list( self ):
    cahal_tests.cahal_free_device_list( None )

    device_list = cahal_tests.cahal_get_device_list()

    self.assertNotEqual( device_list, None )

    cahal_tests.cahal_free_device_list( device_list )

  def test_print_cahal_device_list( self ):
    cahal_tests.cahal_print_device_list( None )

    device_list = cahal_tests.cahal_get_device_list()

    cahal_tests.cahal_print_device_list( device_list )

  def test_print_cahal_device( self ):
    cahal_tests.cahal_print_device( None )

    device_list = cahal_tests.cahal_get_device_list()
    index       = 0;                                                                    
    device      = cahal_tests.cahal_device_list_get( device_list, index )               
                                                                                
    while( device ):                                                                
      self.assertNotEqual( device, None )

      cahal_tests.cahal_print_device( device )

      index += 1                                                                    

      device = cahal_tests.cahal_device_list_get( device_list, index )

  def test_access_cahal_device_members( self ):
    device_list = cahal_tests.cahal_get_device_list()
    index       = 0;                                                                    
    device      = cahal_tests.cahal_device_list_get( device_list, index )               
                                                                                
    while( device ):                                                                
      self.assertNotEqual( device, None )

      device.handle
      device.device_name
      device.model                                               
      device.manufacturer                                 
      device.serial_number                                    
      device.version                                           
      device.device_uid                                     
      device.model_uid                                       
      device.preferred_sample_rate
      device.preferred_number_of_channels
      device.is_alive
      device.is_running
      device.supported_sample_rates
      device.device_streams

      sample_rate_index = 0
      sample_rate       =                               \
        cahal_tests.cahal_sample_rate_range_list_get  ( \
          device.supported_sample_rates,                \
          sample_rate_index                             \
                                                      )

      while( sample_rate ):
        sample_rate.minimum_rate
        sample_rate.maximum_rate

        sample_rate_index += 1

        sample_rate =                                     \
          cahal_tests.cahal_sample_rate_range_list_get  ( \
            device.supported_sample_rates,                \
            sample_rate_index                             \
                                                        )

      stream_index  = 0                                                             
      stream        =                               \
        cahal_tests.cahal_device_stream_list_get  ( \
          device.device_streams,                    \
          stream_index                              \
                                                  )
                                                                                
      while( stream ):                                                              
        stream.handle
        stream.direction
        stream.preferred_format
        stream.supported_formats

        format_description_index  = 0
        format_description        =                             \
          cahal_tests.cahal_audio_format_description_list_get ( \
            stream.supported_formats,                           \
            format_description_index                            \
                                                              )

        while( format_description ):
          format_description.format_id
          format_description.number_of_channels
          format_description.bit_depth
          format_description.sample_rate_range
          format_description.sample_rate_range.minimum_rate
          format_description.sample_rate_range.maximum_rate

          format_description_index  += 1 
          format_description        =                             \
            cahal_tests.cahal_audio_format_description_list_get ( \
              stream.supported_formats,                           \
              format_description_index                            \
                                                                )

        stream_index += 1
        stream        =                               \
          cahal_tests.cahal_device_stream_list_get  ( \
            device.device_streams,                    \
            stream_index                              \
                                                    )

      index += 1                                                                    

      device = cahal_tests.cahal_device_list_get( device_list, index )
"""

if __name__ == '__main__':
  #cahal_tests.cpc_log_set_log_level( cahal_tests.CPC_LOG_LEVEL_NO_LOGGING )

  unittest.main()

#!/usr/local/bin/python -W error

import cahal_tests
import unittest
import string
import types

class TestsCAHAL( unittest.TestCase ):
  def test_get_version( self ):
    version = cahal_tests.get_version() 

    self.assertIsInstance( version, types.LongType )
    self.assertEqual( version, 256 )

    major = ( version >> 8 ) & 0x00FF
    minor = version & 0x00FF

    self.assertEqual( major, 1 )
    self.assertEqual( minor, 0 )
    
  def test_get_version_string( self ):
    version = cahal_tests.get_version_string()
    
    self.assertIsInstance( version, types.StringType )
    self.assertNotEqual( version, '' )

  def test_free_cahal_device_list( self ):
    cahal_tests.free_cahal_device_list( None )

    device_list = cahal_tests.get_device_list()

    self.assertNotEqual( device_list, None )

    cahal_tests.free_cahal_device_list( device_list )

  def test_get_device_list( self ):
    device_list = cahal_tests.get_device_list()

    self.assertNotEqual( device_list, None )

    index   = 0;                                                                    
    device  = cahal_tests.cahal_device_list_get( device_list, index )               
                                                                                
    while( device ):                                                                
      self.assertNotEqual( device, None )

      index += 1                                                                    

      device = cahal_tests.cahal_device_list_get( device_list, index )

  def test_print_cahal_device( self ):
    cahal_tests.print_cahal_device( None )

    device_list = cahal_tests.get_device_list()
    index       = 0;                                                                    
    device      = cahal_tests.cahal_device_list_get( device_list, index )               
                                                                                
    while( device ):                                                                
      self.assertNotEqual( device, None )

      cahal_tests.print_cahal_device( device )

      index += 1                                                                    

      device = cahal_tests.cahal_device_list_get( device_list, index )

  def test_access_cahal_device_members( self ):
    device_list = cahal_tests.get_device_list()
    index       = 0;                                                                    
    device      = cahal_tests.cahal_device_list_get( device_list, index )               
                                                                                
    while( device ):                                                                
      self.assertNotEqual( device, None )

      device.handle
      device.device_name
      device.handle                                              
      device.device_name                                           
      device.model                                               
      device.manufacturer                                 
      device.serial_number                                    
      device.version                                           
      device.device_uid                                     
      device.model_uid                                       
      device.preferred_sample_rate

      sample_rate_index = 0                                                         
      sample_rate       = cahal_tests.cahal_float_array_get( device.supported_sample_rates, sample_rate_index )
                                                                                
      while( sample_rate ):                                                         
        cahal_tests.doubleP_value( sample_rate )                      
                                                                                    
        sample_rate_index += 1                                                      
                                                                                
        sample_rate = cahal_tests.cahal_float_array_get( device.supported_sample_rates, sample_rate_index )

      stream_index  = 0                                                             
      stream        = cahal_tests.cahal_device_stream_list_get( device.device_streams, stream_index )
                                                                                
      while( stream ):                                                              
        stream.handle
        stream.direction

        stream_index += 1
        stream        = cahal_tests.cahal_device_stream_list_get( device.device_streams, stream_index )

      index += 1                                                                    

      device = cahal_tests.cahal_device_list_get( device_list, index )

if __name__ == '__main__':
    unittest.main()

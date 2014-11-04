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

if __name__ == '__main__':
    unittest.main()

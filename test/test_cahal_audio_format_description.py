#!/usr/local/bin/python -W error

import cahal_tests
import unittest
import string
import types

class TestsCAHALAudioFormatDescription( unittest.TestCase ):
  def test_convert_cahal_audio_format_id_to_cstring( self ):
    cahal_tests.convert_cahal_audio_format_id_to_cstring( 0 )

    cahal_tests.convert_cahal_audio_format_id_to_cstring  ( \
      int( cahal_tests.CAHAL_AUDIO_FORMAT_MACE6 )           \
                                                          ) \

    cahal_tests.convert_cahal_audio_format_id_to_cstring  ( \
      int( cahal_tests.CAHAL_AUDIO_FORMAT_LINEARPCM )       \
                                                          ) \

    cahal_tests.convert_cahal_audio_format_id_to_cstring  (       \
      int( cahal_tests.CAHAL_AUDIO_FORMAT_PARAMETERVALUESTREAM )  \
                                                          )

  def test_print_cahal_audio_format_id( self ):
    cahal_tests.print_cahal_audio_format_id( None, 0 )

    cahal_tests.print_cahal_audio_format_id  (    \
      None,                                       \
      int( cahal_tests.CAHAL_AUDIO_FORMAT_MACE6 ) \
                                             )

    cahal_tests.print_cahal_audio_format_id  (        \
      "Format:",                                      \
      int( cahal_tests.CAHAL_AUDIO_FORMAT_LINEARPCM ) \
                                             )        \

    cahal_tests.print_cahal_audio_format_id  (                    \
      "",                                                         \
      int( cahal_tests.CAHAL_AUDIO_FORMAT_PARAMETERVALUESTREAM )  \
                                             )

  def test_print_cahal_audio_format_description( self ):
    device_list = cahal_tests.get_device_list()
    index       = 0;                                                                    
    device      = cahal_tests.cahal_device_list_get( device_list, index )               
                                                                                
    while( device ):                                                                
      stream_index  = 0                                                             
      stream        =                               \
        cahal_tests.cahal_device_stream_list_get  ( \
          device.device_streams,                    \
          stream_index                              \
                                                  )
                                                                                
      while( stream ):                                                              
        format_description_index  = 0
        format_description        =                             \
          cahal_tests.cahal_audio_format_description_list_get ( \
            stream.supported_formats,                           \
            format_description_index                            \
                                                              )

        while( format_description ):
          cahal_tests.print_cahal_audio_format_description  ( \
            format_description                                \
                                                            )

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

if __name__ == '__main__':
    unittest.main()

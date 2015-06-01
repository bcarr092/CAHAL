import cahal_tests
import wave

wave_read   = None
wave_write  = wave.open( "test.wav", "wb" )

global_input_device_name  = "Microphone"
global_output_device_name = "Speakers"
global_number_of_channels = 2
global_sample_rate        = 44100
global_bit_depth          = 16
global_flags              = 0

wave_write.setnchannels( global_number_of_channels )
wave_write.setsampwidth( global_bit_depth / 8 )
wave_write.setframerate( global_sample_rate )

def playback( in_device, in_buffer_length ):                                    
  global wave_read

  #print "Buffer length", in_buffer_length
  
  number_of_frames = \
    in_buffer_length / ( wave_read.getsampwidth() * wave_read.getnchannels() )

  #print "Number of frames in buffer", number_of_frames

  data = wave_read.readframes( number_of_frames )

  #print "Read length", len( data )

  return( data )

def recorder( in_device, in_buffer, in_buffer_length ):                         
  global wave_write

  wave_write.writeframes( in_buffer )

cahal_tests.cpc_log_set_log_level( cahal_tests.CPC_LOG_LEVEL_ERROR )

cahal_tests.cahal_initialize()

device_list = cahal_tests.cahal_get_device_list()                           
index       = 0                                                             
device      = cahal_tests.cahal_device_list_get( device_list, index )       
                                                                            
built_in_output_device  = None                                              
built_in_input_device   = None                                              
                                                                                
while( device ):                                                            
  print device.device_name

  if( device.device_name == global_input_device_name          \
      and cahal_tests.cahal_test_device_direction_support (   \
            device,                                           \
            cahal_tests.CAHAL_DEVICE_INPUT_STREAM             \
                                                          )   \
    ):                                                         
    built_in_input_device = device                             

  if( device.device_name == global_output_device_name         \
      and cahal_tests.cahal_test_device_direction_support (   \
            device,                                           \
            cahal_tests.CAHAL_DEVICE_OUTPUT_STREAM            \
                                                          )   \
    ):                                                         
    built_in_output_device = device                            

                                                                            
  index   += 1                                                              
  device  = cahal_tests.cahal_device_list_get( device_list, index )         
                                                                            
if( built_in_input_device is None or built_in_output_device is None ):
  print "Could not find input or output device."

  quit()

cahal_tests.start_recording (                   \
  built_in_input_device,                        \
  cahal_tests.CAHAL_AUDIO_FORMAT_LINEARPCM,     \
  global_number_of_channels,                    \
  global_sample_rate,                           \
  global_bit_depth,                             \
  recorder,                                     \
  global_flags                                  \
                            )
                                                                            
cahal_tests.cahal_sleep( 3 )                                                

cahal_tests.cahal_stop_recording()

wave_write.close()

wave_read = wave.open( "test.wav", "rb" )

print "Channels:", wave_read.getnchannels()
print "Sample bit depth:", wave_read.getsampwidth() * 8
print "Sample rate:", wave_read.getframerate()

cahal_tests.start_playback  (                   \
  built_in_output_device,                       \
  cahal_tests.CAHAL_AUDIO_FORMAT_LINEARPCM,     \
  global_number_of_channels,                    \
  global_sample_rate,                           \
  global_bit_depth,                             \
  playback,                                     \
  global_flags                                  \
                            )                   \
                                                                                
cahal_tests.cahal_sleep( 3 )

cahal_tests.cahal_stop_playback()

wave_read.close()

cahal_tests.cahal_terminate()
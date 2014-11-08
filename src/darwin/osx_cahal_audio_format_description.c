#include "darwin/osx_cahal_audio_format_description.h"

OSStatus
osx_set_cahal_audio_format_description_list  (
                                          cahal_device_stream* io_device_stream
                                              )
{
  UINT32 property_size                                  = 0;
  AudioStreamRangedDescription* stream_description_list = NULL;
  
  OSStatus result =
  osx_get_device_property_size_and_value  (
                                   io_device_stream->handle,
                                   kAudioStreamPropertyAvailablePhysicalFormats,
                                   &property_size,
                                   ( void** ) &stream_description_list
                                           );
  if( noErr == result )
  {
    UINT32 num_items = property_size / sizeof( AudioStreamRangedDescription );
    
    CPC_LOG( CPC_LOG_LEVEL_TRACE, "Found 0x%x supported formats", num_items );
    
    io_device_stream->supported_formats =
    ( cahal_audio_format_description** )
    malloc  (
             ( num_items + 1 )
             * sizeof( cahal_audio_format_description* )
             );
    
    memset  (
             io_device_stream->supported_formats,
             0,
             ( num_items + 1 ) * sizeof( cahal_audio_format_description* )
             );
    
    for( UINT32 i = 0; i < num_items; i++ )
    {
      io_device_stream->supported_formats[ i ] =
      ( cahal_audio_format_description* )
      malloc( sizeof( cahal_audio_format_description ) );
      
      memset  (
               io_device_stream->supported_formats[ i ],
               0,
               sizeof( cahal_audio_format_description )
               );
      
      io_device_stream->supported_formats[ i ]->format_id =
      osx_convert_core_audio_format_id_to_cahal_audio_format_id (
                                 stream_description_list[ i ].mFormat.mFormatID
                                                                 );
      
      io_device_stream->supported_formats[ i ]->bit_depth =
      stream_description_list[ i ].mFormat.mBitsPerChannel;
      
      io_device_stream->supported_formats[ i ]->number_of_channels =
      stream_description_list[ i ].mFormat.mChannelsPerFrame;
      
      if( kAudioStreamAnyRate ==
         stream_description_list[ i ].mFormat.mSampleRate
         )
      {
        io_device_stream->supported_formats[ i ]->sample_rate_range.minimum_rate
        = stream_description_list[ i ].mSampleRateRange.mMinimum;
        
        io_device_stream->supported_formats[ i ]->sample_rate_range.maximum_rate
        = stream_description_list[ i ].mSampleRateRange.mMaximum;
      }
      else
      {
        io_device_stream->supported_formats[ i ]->sample_rate_range.minimum_rate
        = stream_description_list[ i ].mFormat.mSampleRate;
        
        io_device_stream->supported_formats[ i ]->sample_rate_range.maximum_rate
        = stream_description_list[ i ].mFormat.mSampleRate;
      }
      
    }
  }
  
  if( NULL != stream_description_list )
  {
    free( stream_description_list );
  }
  
  return( result );
}

cahal_audio_format_id
osx_convert_core_audio_format_id_to_cahal_audio_format_id (
                                                     UINT32 core_audio_format_id
                                                           )
{
  switch( core_audio_format_id )
  {
    case kAudioFormatMACE3:
      return( CAHAL_AUDIO_FORMAT_MACE3 );
    case kAudioFormatMPEG4HVXC:
      return( CAHAL_AUDIO_FORMAT_MPEG4HVXC );
    case kAudioFormatAppleIMA4:
      return( CAHAL_AUDIO_FORMAT_APPLEIMA4 );
    case kAudioFormatMPEG4AAC_ELD_V2:
      return( CAHAL_AUDIO_FORMAT_MPEG4AAC_ELD_V2 );
    case kAudioFormatAES3:
      return( CAHAL_AUDIO_FORMAT_AES3 );
    case kAudioFormatMPEG4AAC_Spatial:
      return( CAHAL_AUDIO_FORMAT_MPEG4AAC_SPATIAL );
    case kAudioFormatMPEGLayer3:
      return( CAHAL_AUDIO_FORMAT_MPEGLAYER3 );
    case kAudioFormatMPEG4AAC:
      return( CAHAL_AUDIO_FORMAT_MPEG4AAC );
    case kAudioFormatQUALCOMM:
      return( CAHAL_AUDIO_FORMAT_QUALCOMM );
    case kAudioFormatMicrosoftGSM:
      return( CAHAL_AUDIO_FORMAT_MICROSOFTGSM );
    case kAudioFormatAMR:
      return( CAHAL_AUDIO_FORMAT_AMR );
    case kAudioFormatMPEGLayer1:
      return( CAHAL_AUDIO_FORMAT_MPEGLAYER1 );
    case kAudioFormatLinearPCM:
      return( CAHAL_AUDIO_FORMAT_LINEARPCM );
    case kAudioFormatMPEG4CELP:
      return( CAHAL_AUDIO_FORMAT_MPEG4CELP );
    case kAudioFormatALaw:
      return( CAHAL_AUDIO_FORMAT_ALAW );
    case kAudioFormatMACE6:
      return( CAHAL_AUDIO_FORMAT_MACE6 );
    case kAudioFormatiLBC:
      return( CAHAL_AUDIO_FORMAT_ILBC );
    case kAudioFormatMPEG4AAC_HE_V2:
      return( CAHAL_AUDIO_FORMAT_MPEG4AAC_HE_V2 );
    case kAudioFormatMPEG4TwinVQ:
      return( CAHAL_AUDIO_FORMAT_MPEG4TWINVQ );
    case kAudioFormatAudible:
      return( CAHAL_AUDIO_FORMAT_AUDIBLE );
    case kAudioFormatMPEG4AAC_LD:
      return( CAHAL_AUDIO_FORMAT_MPEG4AAC_LD );
    case kAudioFormatAC3:
      return( CAHAL_AUDIO_FORMAT_AC3 );
    case kAudioFormatMPEG4AAC_HE:
      return( CAHAL_AUDIO_FORMAT_MPEG4AAC_HE );
    case kAudioFormatMPEG4AAC_ELD_SBR:
      return( CAHAL_AUDIO_FORMAT_MPEG4AAC_ELD_SBR );
    case kAudioFormatAppleLossless:
      return( CAHAL_AUDIO_FORMAT_APPLELOSSLESS );
    case kAudioFormatMPEGLayer2:
      return( CAHAL_AUDIO_FORMAT_MPEGLAYER2 );
    case kAudioFormatMPEG4AAC_ELD:
      return( CAHAL_AUDIO_FORMAT_MPEG4AAC_ELD );
    case kAudioFormatULaw:
      return( CAHAL_AUDIO_FORMAT_ULAW );
    case kAudioFormatQDesign2:
      return( CAHAL_AUDIO_FORMAT_QDESIGN2 );
    case kAudioFormatParameterValueStream:
      return( CAHAL_AUDIO_FORMAT_PARAMETERVALUESTREAM );
    case kAudioFormatDVIIntelIMA:
      return( CAHAL_AUDIO_FORMAT_DVIINTELIMA );
    case kAudioFormatTimeCode:
      return( CAHAL_AUDIO_FORMAT_TIMECODE );
    case kAudioFormatMIDIStream:
      return( CAHAL_AUDIO_FORMAT_MIDISTREAM );
    case kAudioFormat60958AC3:
      return( CAHAL_AUDIO_FORMAT_60958AC3 );
    case kAudioFormatQDesign:
      return( CAHAL_AUDIO_FORMAT_QDESIGN );
    default:
      return( CAHAL_AUDIO_FORMAT_UNKNOWN );
  }
}

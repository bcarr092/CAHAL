#include "cahal_audio_format_description.h"

/*! \fn     void log_cahal_audio_format  (
              CPC_LOG_LEVEL          in_log_level,
              CHAR*                  in_label,
              cahal_audio_format_id  in_format_id
            )
    \brief  Logs a string representation of in_format_id using the logger. The
            log is logged at in_log_level and in_label is prepended to the
            string representation of in_format_id before logging.

    \param  in_log_level  The log level to log in_label and a string
            representation of in_format_id.
    \param  in_label  The label to prepend to the string representation of
            in_format_id.
    \param  in_format_id  The format identifier to be logged.
 */
void
log_cahal_audio_format  (
                           CPC_LOG_LEVEL          in_log_level,
                           CHAR*                  in_label,
                           cahal_audio_format_id  in_format_id
                         )
{
  if( NULL == in_label )
  {
    in_label = "";
  }
  
  CPC_LOG (
           CPC_LOG_LEVEL_INFO,
           "%s %s",
           in_label,
           convert_cahal_audio_format_id_to_cstring( in_format_id )
           );
}


CHAR*
convert_cahal_audio_format_id_to_cstring  (
                                       cahal_audio_format_id in_audio_format_id
                                           )
{
  switch( in_audio_format_id )
  {
    case CAHAL_AUDIO_FORMAT_MPEG4AAC_ELD:
      return( CAHAL_AUDIO_FORMAT_STRING_MPEG4AAC_ELD );
    case CAHAL_AUDIO_FORMAT_APPLEIMA4:
      return( CAHAL_AUDIO_FORMAT_STRING_APPLEIMA4 );
    case CAHAL_AUDIO_FORMAT_MPEG4AAC_ELD_SBR:
      return( CAHAL_AUDIO_FORMAT_STRING_MPEG4AAC_ELD_SBR );
    case CAHAL_AUDIO_FORMAT_QUALCOMM:
      return( CAHAL_AUDIO_FORMAT_STRING_QUALCOMM );
    case CAHAL_AUDIO_FORMAT_ILBC:
      return( CAHAL_AUDIO_FORMAT_STRING_ILBC );
    case CAHAL_AUDIO_FORMAT_MPEG4AAC_SPATIAL:
      return( CAHAL_AUDIO_FORMAT_STRING_MPEG4AAC_SPATIAL );
    case CAHAL_AUDIO_FORMAT_ALAW:
      return( CAHAL_AUDIO_FORMAT_STRING_ALAW );
    case CAHAL_AUDIO_FORMAT_MPEG4AAC:
      return( CAHAL_AUDIO_FORMAT_STRING_MPEG4AAC );
    case CAHAL_AUDIO_FORMAT_QDESIGN:
      return( CAHAL_AUDIO_FORMAT_STRING_QDESIGN );
    case CAHAL_AUDIO_FORMAT_QDESIGN2:
      return( CAHAL_AUDIO_FORMAT_STRING_QDESIGN2 );
    case CAHAL_AUDIO_FORMAT_MPEG4AAC_LD:
      return( CAHAL_AUDIO_FORMAT_STRING_MPEG4AAC_LD );
    case CAHAL_AUDIO_FORMAT_TIMECODE:
      return( CAHAL_AUDIO_FORMAT_STRING_TIMECODE );
    case CAHAL_AUDIO_FORMAT_MPEG4AAC_HE_V2:
      return( CAHAL_AUDIO_FORMAT_STRING_MPEG4AAC_HE_V2 );
    case CAHAL_AUDIO_FORMAT_AUDIBLE:
      return( CAHAL_AUDIO_FORMAT_STRING_AUDIBLE );
    case CAHAL_AUDIO_FORMAT_DVIINTELIMA:
      return( CAHAL_AUDIO_FORMAT_STRING_DVIINTELIMA );
    case CAHAL_AUDIO_FORMAT_MACE6:
      return( CAHAL_AUDIO_FORMAT_STRING_MACE6 );
    case CAHAL_AUDIO_FORMAT_MICROSOFTGSM:
      return( CAHAL_AUDIO_FORMAT_STRING_MICROSOFTGSM );
    case CAHAL_AUDIO_FORMAT_AES3:
      return( CAHAL_AUDIO_FORMAT_STRING_AES3 );
    case CAHAL_AUDIO_FORMAT_AC3:
      return( CAHAL_AUDIO_FORMAT_STRING_AC3 );
    case CAHAL_AUDIO_FORMAT_MPEGLAYER2:
      return( CAHAL_AUDIO_FORMAT_STRING_MPEGLAYER2 );
    case CAHAL_AUDIO_FORMAT_LINEARPCM:
      return( CAHAL_AUDIO_FORMAT_STRING_LINEARPCM );
    case CAHAL_AUDIO_FORMAT_APPLELOSSLESS:
      return( CAHAL_AUDIO_FORMAT_STRING_APPLELOSSLESS );
    case CAHAL_AUDIO_FORMAT_MPEG4AAC_HE:
      return( CAHAL_AUDIO_FORMAT_STRING_MPEG4AAC_HE );
    case CAHAL_AUDIO_FORMAT_PARAMETERVALUESTREAM:
      return( CAHAL_AUDIO_FORMAT_STRING_PARAMETERVALUESTREAM );
    case CAHAL_AUDIO_FORMAT_AMR:
      return( CAHAL_AUDIO_FORMAT_STRING_AMR );
    case CAHAL_AUDIO_FORMAT_ULAW:
      return( CAHAL_AUDIO_FORMAT_STRING_ULAW );
    case CAHAL_AUDIO_FORMAT_MACE3:
      return( CAHAL_AUDIO_FORMAT_STRING_MACE3 );
    case CAHAL_AUDIO_FORMAT_MPEG4HVXC:
      return( CAHAL_AUDIO_FORMAT_STRING_MPEG4HVXC );
    case CAHAL_AUDIO_FORMAT_MPEG4CELP:
      return( CAHAL_AUDIO_FORMAT_STRING_MPEG4CELP );
    case CAHAL_AUDIO_FORMAT_MPEGLAYER3:
      return( CAHAL_AUDIO_FORMAT_STRING_MPEGLAYER3 );
    case CAHAL_AUDIO_FORMAT_MPEGLAYER1:
      return( CAHAL_AUDIO_FORMAT_STRING_MPEGLAYER1 );
    case CAHAL_AUDIO_FORMAT_MPEG4TWINVQ:
      return( CAHAL_AUDIO_FORMAT_STRING_MPEG4TWINVQ );
    case CAHAL_AUDIO_FORMAT_60958AC3:
      return( CAHAL_AUDIO_FORMAT_STRING_60958AC3 );
    case CAHAL_AUDIO_FORMAT_MIDISTREAM:
      return( CAHAL_AUDIO_FORMAT_STRING_MIDISTREAM );
    case CAHAL_AUDIO_FORMAT_MPEG4AAC_ELD_V2:
      return( CAHAL_AUDIO_FORMAT_STRING_MPEG4AAC_ELD_V2 );
    default:
      return( "Unknown format" );
  }
}

void
print_cahal_audio_format_id (
                             CHAR*                  in_label,
                             cahal_audio_format_id  in_format_id
                             )
{
  log_cahal_audio_format  (
                           CPC_LOG_LEVEL_INFO,
                           in_label,
                           in_format_id
                           );
}

void
print_cahal_audio_format_description  (
                     cahal_audio_format_description* in_audio_format_description
                                       )
{
  if( NULL != in_audio_format_description )
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_INFO, "\t\t\tFormat description:" );
    
    log_cahal_audio_format  (
                             CPC_LOG_LEVEL_INFO,
                             "\t\t\t\tCodec:",
                             in_audio_format_description->format_id
                             );
    
    CPC_LOG (
             CPC_LOG_LEVEL_INFO,
             "\t\t\t\tNumber of channels: %d",
             in_audio_format_description->number_of_channels
             );
    
    CPC_LOG (
             CPC_LOG_LEVEL_INFO,
             "\t\t\t\tBit depth: %d",
             in_audio_format_description->bit_depth
             );
    
    CPC_LOG (
             CPC_LOG_LEVEL_INFO,
             "\t\t\t\tMinimum supported sample rate: %.2f",
             in_audio_format_description->sample_rate_range.minimum_rate
             );
    
    CPC_LOG (
             CPC_LOG_LEVEL_INFO,
             "\t\t\t\tMaximum supported sample rate: %.2f",
             in_audio_format_description->sample_rate_range.maximum_rate
             );
  }
}

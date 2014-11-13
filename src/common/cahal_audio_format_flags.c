#include "cahal_audio_format_flags.h"

void
log_cahal_audio_format_flags  (
                               cpc_log_level            in_log_level,
                               CHAR*                    in_label,
                               cahal_audio_format_flag  in_format_flag
                               )
{
  if( NULL == in_label )
  {
    in_label = "";
  }
  
  CPC_LOG (
           in_log_level,
           "%s %s",
           in_label,
           cahal_convert_audio_format_flag_to_cstring( in_format_flag )
           );
}

CHAR*
cahal_convert_audio_format_flag_to_cstring  (
                                    cahal_audio_format_flag in_audio_format_flag
                                              )
{
  switch( in_audio_format_flag )
  {
    case CAHAL_AUDIO_FORMAT_FLAGISNONMIXABLE:
      return( CAHAL_AUDIO_FORMAT_STRING_FLAGISNONMIXABLE );
    case CAHAL_AUDIO_FORMAT_FLAGISALIGNEDHIGH:
      return( CAHAL_AUDIO_FORMAT_STRING_FLAGISALIGNEDHIGH );
    case CAHAL_AUDIO_FORMAT_FLAGISPACKED:
      return( CAHAL_AUDIO_FORMAT_STRING_FLAGISPACKED );
    case CAHAL_AUDIO_FORMAT_FLAGSAREALLCLEAR:
      return( CAHAL_AUDIO_FORMAT_STRING_FLAGSAREALLCLEAR );
    case CAHAL_AUDIO_FORMAT_FLAGISNONINTERLEAVED:
      return( CAHAL_AUDIO_FORMAT_STRING_FLAGISNONINTERLEAVED );
    case CAHAL_AUDIO_FORMAT_FLAGISSIGNEDINTEGER:
      return( CAHAL_AUDIO_FORMAT_STRING_FLAGISSIGNEDINTEGER );
    case CAHAL_AUDIO_FORMAT_FLAGISFLOAT:
      return( CAHAL_AUDIO_FORMAT_STRING_FLAGISFLOAT );
    case CAHAL_AUDIO_FORMAT_FLAGISBIGENDIAN:
      return( CAHAL_AUDIO_FORMAT_STRING_FLAGISBIGENDIAN );
    default:
      return( "Unknown format" );
  }
}

void
cahal_print_audio_format_flag (
                               CHAR*                    in_label,
                               cahal_audio_format_flag  in_format_flag
                               )
{
  log_cahal_audio_format_flags  (
                                 CPC_LOG_LEVEL_INFO,
                                 in_label,
                                 in_format_flag
                                 );
}

#include "cahal_audio_format_description.h"

void
log_cahal_audio_format_id  (
                            CPC_LOG_LEVEL          in_log_level,
                            CHAR*                  in_label,
                            cahal_audio_format_id  in_format
                            )
{
  CHAR* format_cstring = convert_cahal_audio_format_to_cstring( in_format );
  
  if( NULL != format_cstring )
  {
    if( NULL == in_label )
    {
      in_label = "";
    }
    
    CPC_LOG (
             CPC_LOG_LEVEL_INFO,
             "%s %s",
             in_label,
             format_cstring
             );
    
    free( format_cstring );
  }
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
             "\t\t\t\tSample rate: %.2f",
             in_audio_format_description->sample_rate
             );
  }
}

#ifndef __CAHAL_AUDIO_FORMAT_H__
#define __CAHAL_AUDIO_FORMAT_H__

#include <cpcommon.h>

typedef UINT32 cahal_audio_format_id;

typedef struct cahal_audio_format_description_t
{
  cahal_audio_format_id format_id;
  
  UINT32                number_of_channels;
  
  UINT32                bit_depth;
  
  FLOAT32               sample_rate;
  
} cahal_audio_format_description;

CHAR*
convert_cahal_audio_format_to_cstring  (
                                        cahal_audio_format_id in_audio_format_id
                                        );

void
print_cahal_audio_format_description  (
                     cahal_audio_format_description* in_audio_format_description
                                       );

void
log_cahal_audio_format  (
                         CPC_LOG_LEVEL          in_log_level,
                         CHAR*                  in_label,
                         cahal_audio_format_id  in_format
                         );

#endif  /*  __CAHAL_AUDIO_FORMAT_H__  */

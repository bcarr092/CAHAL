#include "android/android_cahal_audio_format_description.h"

cahal_audio_format_id
android_convert_android_audio_format_id_to_cahal_audio_format_id (
                                                     UINT32 android_format_id
                                                                 )
{
  switch( android_format_id )
  {
    case SL_DATAFORMAT_PCM:
      return( CAHAL_AUDIO_FORMAT_LINEARPCM );
    default:
      return( CAHAL_AUDIO_FORMAT_UNKNOWN );
  }
}

UINT32
android_convert_cahal_audio_format_id_to_android_audio_format_id (
                                             cahal_audio_format_id in_format_id
                                                                 )
{
  switch( in_format_id )
  {
    case CAHAL_AUDIO_FORMAT_LINEARPCM:
      return( SL_DATAFORMAT_PCM );
    default:
      return( 0 );
  }
}

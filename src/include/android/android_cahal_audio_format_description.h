#ifndef __ANDROID_CAHAL_AUDIO_FORMAT_DESCRIPTION_H__
#define __ANDROID_CAHAL_AUDIO_FORMAT_DESCRIPTION_H__

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include "cahal_audio_format_description.h"

cahal_audio_format_id
android_convert_android_audio_format_id_to_cahal_audio_format_id (
                                                     UINT32 android_format_id
                                                                 );

UINT32
android_convert_cahal_audio_format_id_to_android_audio_format_id (
                                             cahal_audio_format_id in_format_id
                                                                 );

#endif /* __ANDROID_CAHAL_AUDIO_FORMAT_DESCRIPTION_H__ */

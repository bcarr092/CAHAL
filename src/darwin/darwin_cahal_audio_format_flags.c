/*! \file   darwin_cahal_audio_format_flags.c
 
    \author Brent Carrara
 */
#include "darwin/darwin_cahal_audio_format_flags.h"

cahal_audio_format_flag
darwin_convert_core_audio_format_flags_to_cahal_audio_format_flags (
                                                 UINT32 core_audio_format_flags
                                                                 )
{
  cahal_audio_format_flag flags = 0;
  
  if( ( kAudioFormatFlagIsAlignedHigh & core_audio_format_flags )
      == kAudioFormatFlagIsAlignedHigh )
  {
    flags |= CAHAL_AUDIO_FORMAT_FLAGISALIGNEDHIGH;
  }
  
  if( ( kAudioFormatFlagIsNonInterleaved & core_audio_format_flags )
      == kAudioFormatFlagIsNonInterleaved )
  {
    flags |= CAHAL_AUDIO_FORMAT_FLAGISNONINTERLEAVED;
  }
  
  if( ( kAudioFormatFlagIsPacked & core_audio_format_flags )
      == kAudioFormatFlagIsPacked )
  {
    flags |= CAHAL_AUDIO_FORMAT_FLAGISPACKED;
  }
  
  if( ( kAudioFormatFlagIsBigEndian & core_audio_format_flags )
      == kAudioFormatFlagIsBigEndian )
  {
    flags |= CAHAL_AUDIO_FORMAT_FLAGISBIGENDIAN;
  }
  
  if( ( kAudioFormatFlagsAreAllClear & core_audio_format_flags )
      == kAudioFormatFlagsAreAllClear )
  {
    flags |= CAHAL_AUDIO_FORMAT_FLAGSAREALLCLEAR;
  }
  
  if( ( kAudioFormatFlagIsNonMixable & core_audio_format_flags )
      == kAudioFormatFlagIsNonMixable )
  {
    flags |= CAHAL_AUDIO_FORMAT_FLAGISNONMIXABLE;
  }
  
  if( ( kAudioFormatFlagIsSignedInteger & core_audio_format_flags )
      == kAudioFormatFlagIsSignedInteger )
  {
    flags |= CAHAL_AUDIO_FORMAT_FLAGISSIGNEDINTEGER;
  }
  
  if( ( kAudioFormatFlagIsFloat & core_audio_format_flags )
     == kAudioFormatFlagIsFloat )
  {
    flags |= CAHAL_AUDIO_FORMAT_FLAGISFLOAT;
  }
  
  return( flags );
}

UINT32
darwin_convert_cahal_audio_format_flags_to_core_audio_format_flags (
                                         cahal_audio_format_flag in_format_flags
                                                                 )
{
  UINT32 flags = 0;
  
  if( CAHAL_AUDIO_FORMAT_FLAGISALIGNEDHIGH & in_format_flags )
  {
    flags |= kAudioFormatFlagIsAlignedHigh;
  }
  
  if( CAHAL_AUDIO_FORMAT_FLAGISNONINTERLEAVED & in_format_flags )
  {
    flags |= kAudioFormatFlagIsNonInterleaved;
  }
  
  if( CAHAL_AUDIO_FORMAT_FLAGISPACKED & in_format_flags )
  {
    flags |= kAudioFormatFlagIsPacked;
  }
  
  if( CAHAL_AUDIO_FORMAT_FLAGISBIGENDIAN & in_format_flags )
  {
    flags |= kAudioFormatFlagIsBigEndian;
  }
  
  if( CAHAL_AUDIO_FORMAT_FLAGSAREALLCLEAR & in_format_flags )
  {
    flags |= kAudioFormatFlagsAreAllClear;
  }
  
  if( CAHAL_AUDIO_FORMAT_FLAGISNONMIXABLE & in_format_flags )
  {
    flags |= kAudioFormatFlagIsNonMixable;
  }
  
  if( CAHAL_AUDIO_FORMAT_FLAGISSIGNEDINTEGER & in_format_flags )
  {
    flags |= kAudioFormatFlagIsSignedInteger;
  }
  
  if( CAHAL_AUDIO_FORMAT_FLAGISFLOAT & in_format_flags )
  {
    flags |= kAudioFormatFlagIsFloat;
  }
  
  return( flags );
}

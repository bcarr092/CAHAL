/*! \file   cahal_audio_format_description.h
    \brief  All constants, typedefs and functions related to audio formats and
            sample rates are defined in this file.
 
    \todo   The constants defined in this file for the audio formats are very
            OSX specific. This needs to be cleaned up so that they are cross-
            platform and so that a pragm does not need to be used to disable
            the compiler warning.
 */
#ifndef __CAHAL_AUDIO_FORMAT_H__
#define __CAHAL_AUDIO_FORMAT_H__

#include <cpcommon.h>

/*! \var    cahal_audio_format_id
    \brief  Definition for the audio format identifiers.
 */
typedef UINT32 cahal_audio_format_id;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfour-char-constants"

/*! \enum   cahal_audio_format_ids
    \brief  These are the recognized audio format constants in their string
            representation form.
 
    \todo   This enum is currently OSX specific and needs to be made cross-
            platform.
 */
enum cahal_audio_format_ids
{
  CAHAL_AUDIO_FORMAT_LINEARPCM              =	'lpcm',
  CAHAL_AUDIO_FORMAT_AC3                    =	'ac-3',
  CAHAL_AUDIO_FORMAT_60958AC3               =	'cac3',
  CAHAL_AUDIO_FORMAT_APPLEIMA4              =	'ima4',
  CAHAL_AUDIO_FORMAT_MPEG4AAC               =	'aac ',
  CAHAL_AUDIO_FORMAT_MPEG4CELP              =	'celp',
  CAHAL_AUDIO_FORMAT_MPEG4HVXC              =	'hvxc',
  CAHAL_AUDIO_FORMAT_MPEG4TWINVQ            =	'twvq',
  CAHAL_AUDIO_FORMAT_MACE3                  =	'MAC3',
  CAHAL_AUDIO_FORMAT_MACE6                  =	'MAC6',
  CAHAL_AUDIO_FORMAT_ULAW                   =	'ulaw',
  CAHAL_AUDIO_FORMAT_ALAW                   =	'alaw',
  CAHAL_AUDIO_FORMAT_QDESIGN                =	'QDMC',
  CAHAL_AUDIO_FORMAT_QDESIGN2               =	'QDM2',
  CAHAL_AUDIO_FORMAT_QUALCOMM               =	'Qclp',
  CAHAL_AUDIO_FORMAT_MPEGLAYER1             =	'.mp1',
  CAHAL_AUDIO_FORMAT_MPEGLAYER2             =	'.mp2',
  CAHAL_AUDIO_FORMAT_MPEGLAYER3             =	'.mp3',
  CAHAL_AUDIO_FORMAT_TIMECODE               =	'time',
  CAHAL_AUDIO_FORMAT_MIDISTREAM             =	'midi',
  CAHAL_AUDIO_FORMAT_PARAMETERVALUESTREAM		=	'apvs',
  CAHAL_AUDIO_FORMAT_APPLELOSSLESS          =	'alac',
  CAHAL_AUDIO_FORMAT_MPEG4AAC_HE            =	'aach',
  CAHAL_AUDIO_FORMAT_MPEG4AAC_LD            =	'aacl',
  CAHAL_AUDIO_FORMAT_MPEG4AAC_ELD           =	'aace',
  CAHAL_AUDIO_FORMAT_MPEG4AAC_ELD_SBR       =	'aacf',
  CAHAL_AUDIO_FORMAT_MPEG4AAC_ELD_V2        =	'aacg',
  CAHAL_AUDIO_FORMAT_MPEG4AAC_HE_V2         =	'aacp',
  CAHAL_AUDIO_FORMAT_MPEG4AAC_SPATIAL       =	'aacs',
  CAHAL_AUDIO_FORMAT_AMR                    =	'samr',
  CAHAL_AUDIO_FORMAT_AUDIBLE                =	'AUDB',
  CAHAL_AUDIO_FORMAT_ILBC                   =	'ilbc',
  CAHAL_AUDIO_FORMAT_DVIINTELIMA            =	 0x6D730011,
  CAHAL_AUDIO_FORMAT_MICROSOFTGSM           =	 0x6D730031,
  CAHAL_AUDIO_FORMAT_AES3                   =	 'aes3',
};

#pragma clang diagnostic pop

/*! \var    cahal_sample_rate_range_t
    \brief  This struct is used to represent the sample rate ranges supported
            by various audio IO hardware devices. Sample rates are generally
            defined in the form of ranges.
 */
typedef struct cahal_sample_rate_range_t
{
  /*! \var    minimum_rate
      \brief  The minimum rate supported by the device in the associated
              configuration.
   */
  FLOAT64     minimum_rate;
  
  /*! \var    maximum_rate
      \brief  The maximum rate supported by the device in the associated
              configuration.
   */
  FLOAT64     maximum_rate;
  
} cahal_sample_rate_range;

/*! \var    cahal_audio_format_description_t
    \brief  Struct used to model the configuration parameters for an audio
            stream, both input and output. This struct captures the minimum
            parameters required to define when establishing an audio stream.
 */
typedef struct cahal_audio_format_description_t
{
  /*! \var    format_id
      \brief  Constant defining the audio format in this description.
   */
  cahal_audio_format_id   format_id;
  
  /*! \var    number_of_channels
      \brief  The number of channels supported in this configuration.
   */
  UINT32                  number_of_channels;
  
  /*! \var    bit_depth
      \brief  The number of bits used to quantize samples in this configuration.
   */
  UINT32                  bit_depth;
  
  /*! \var    sample_rate_range
      \brief  The range of sample rates supported in this configuration.
   */
  cahal_sample_rate_range sample_rate_range;
  
} cahal_audio_format_description;

/*! \fn      CHAR* convert_cahal_audio_format_id_to_cstring  (
              cahal_audio_format_id in_audio_format_id
            )
    \brief  Converts in_audio_format_id to a string representation and returns
            the newly created string. It is up to the caller to free the
            returned string.
 
    \param  in_audio_format_id  The audio format identifier whose string
                                representation is to be returned.
    \return A newly created string representation of the input audio format id.
            The returned string must be freed by the caller.
 */
CHAR*
convert_cahal_audio_format_id_to_cstring  (
                                        cahal_audio_format_id in_audio_format_id
                                        );

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
                         );

/*! \fn     void print_cahal_audio_format_description  (
              cahal_audio_format_description* in_audio_format_description
            )
    \brief  Logs a string representation of in_audio_format_description.
 
    \param  in_audio_format_description The format description to be converted
                                        to a string and logged.
 */
void
print_cahal_audio_format_description  (
                     cahal_audio_format_description* in_audio_format_description
                                       );

#endif  /*  __CAHAL_AUDIO_FORMAT_H__  */

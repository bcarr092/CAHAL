/*! \file   ios_cahal_device_stream.c
 
    \author Brent Carrara
 */
#include "darwin/ios/ios_cahal_device_stream.h"

/*! \def    NUM_CHANNELS_TO_TEST
    \brief  The number of channels to test for support is in the range [ 1,
            NUM_CHANNELS_TO_TEST ]
 */
#define NUM_CHANNELS_TO_TEST                2

/*! \def    NUM_SUPPORTED_BITS_PER_SAMPLE
    \brief  The length of the supported_bits_per_sample array.
 */
#define NUM_SUPPORTED_BITS_PER_SAMPLE       5

/*! \def    NUM_SUPPORTED_SAMPLE_RATES
    \brief  The length of the supported_sample_rates array.
 */
#define NUM_SUPPORTED_SAMPLE_RATES          13

/*! \var    supported_bits_per_sample
    \brief  The bit depths to test for support for.
 */
static UINT32 supported_bits_per_sample [
                                         NUM_SUPPORTED_BITS_PER_SAMPLE
                                         ] =
{
  8,
  16,
  20,
  24,
  32
};

/*! \var    supported_sample_rates
    \brief  The sample rates to test for support for.
 */
static UINT32 supported_sample_rates[
                                     NUM_SUPPORTED_SAMPLE_RATES
                                     ] =
{
  8000,
  11025,
  12000,
  16000,
  22050,
  24000,
  32000,
  44100,
  48000,
  64000,
  88200,
  96000,
  192000
};

/*! \fn     cpc_error_code ios_add_format_description  (
             UINT32                in_bits_per_sample,
             UINT32                in_num_channels,
             UINT32                in_sample_rate,
             UINT32                in_num_supported_formats,
             cahal_device_stream*  out_stream
            )
    \brief  Mallocs a new format description struct and populates it with the
            the necessary parameters. One block is allocated per supported
            format.
 
    \param  in_bits_per_sample  The bit depth of the supported format.
    \param  in_num_channels The number of channels supported in this format.
    \param  in_sample_rate  The sample rate supported in this format.
    \param  in_num_supported_formats  The number of supported formats currently
                                      in out_stream.
    \param  out_stream  The cahal_device_stream struct that holds the null
                        terminated list of supported formats.
    \return Returns NO_ERROR upon succesful execution
            (see cpc_safe_malloc for other possible errors).
 */
cpc_error_code
ios_add_format_description  (
                             UINT32                in_bits_per_sample,
                             UINT32                in_num_channels,
                             UINT32                in_sample_rate,
                             UINT32                in_num_supported_formats,
                             cahal_device_stream*  out_stream
                             );

void
ios_determine_supported_formats (
                                 cahal_device_stream*           out_stream,
                                 cahal_device_stream_direction  in_direction
                                 )
{
  UINT32 num_supported_formats  = 0;
  
  for(
      UINT32 num_channels = 0;
      num_channels < NUM_CHANNELS_TO_TEST;
      num_channels++
      )
  {
    for(
        UINT32 bits_per_sample_index = 0;
        bits_per_sample_index < NUM_SUPPORTED_BITS_PER_SAMPLE;
        bits_per_sample_index++
        )
    {
      for(
          UINT32 sample_rate_index = 0;
          sample_rate_index < NUM_SUPPORTED_SAMPLE_RATES;
          sample_rate_index++
          )
      {
        AudioStreamBasicDescription description;
        
        CPC_MEMSET  (
                     &description,
                     0,
                     sizeof( AudioStreamBasicDescription )
                     );
        
        cahal_audio_format_flag flags =
          CAHAL_AUDIO_FORMAT_FLAGISSIGNEDINTEGER
          | CAHAL_AUDIO_FORMAT_FLAGISPACKED;
        
        OSStatus result =
        darwin_configure_asbd  (
                            CAHAL_AUDIO_FORMAT_LINEARPCM,
                            ( num_channels + 1 ),
                            supported_sample_rates[ sample_rate_index ],
                            supported_bits_per_sample[ bits_per_sample_index ],
                            flags,
                            in_direction,
                            &description
                                );
        
        if( noErr == result )
        {
          cpc_error_code format_added =
            ios_add_format_description  (
                                         description.mBitsPerChannel,
                                         description.mChannelsPerFrame,
                                         description.mSampleRate,
                                         num_supported_formats,
                                         out_stream
                                         );
          
          if( CPC_ERROR_CODE_NO_ERROR == format_added )
          {
            num_supported_formats++;
          }
          else
          {
            CPC_ERROR (
                       "Could not initialize description: %d.",
                       format_added
                       );
          }
        }
      }
    }
  }
}

cpc_error_code
ios_add_format_description  (
                             UINT32                in_bits_per_sample,
                             UINT32                in_num_channels,
                             UINT32                in_sample_rate,
                             UINT32                in_num_supported_formats,
                             cahal_device_stream*  io_device_stream
                             )
{
  cahal_audio_format_description* description;
  
  cpc_error_code result =
  cpc_safe_malloc(
                  ( void** )&description,
                  sizeof( cahal_audio_format_description )
                  );
  
  if( CPC_ERROR_CODE_NO_ERROR == result )
  {
    description->bit_depth                      = in_bits_per_sample;
    description->format_id                      = CAHAL_AUDIO_FORMAT_LINEARPCM;
    description->number_of_channels             = in_num_channels;
    description->sample_rate_range.minimum_rate = in_sample_rate;
    description->sample_rate_range.maximum_rate = in_sample_rate;
    
    result =
    cpc_safe_realloc(
                     ( void** )&( io_device_stream->supported_formats ),
                     in_num_supported_formats
                      * sizeof( cahal_audio_format_description* ),
                     ( in_num_supported_formats + 2 )
                      * sizeof( cahal_audio_format_description* )
                     );
    
    if( CPC_ERROR_CODE_NO_ERROR == result )
    {
      io_device_stream->supported_formats[in_num_supported_formats] =
        description;
    }
    else
    {
      cpc_safe_free( ( void** )&description );
      
      CPC_ERROR( "Could not realloc supported formats: %d.", result );
    }
  }
  else
  {
    CPC_ERROR( "Could not malloc description:%d.", result );
  }
  
  return( result );
}

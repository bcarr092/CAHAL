/*! \file   windows_cahal_device_stream.hpp
    \brief  Implementation of functions that query the underlying hardware for
            its supported formats.

    \author Brent Carrara
 */

#include "windows/windows_cahal_device_stream.hpp"

#define NUM_CHANNELS_TO_TEST                2
#define NUM_SUPPORTED_BITS_PER_SAMPLE       2
#define NUM_SUPPORTED_SAMPLE_RATES          13

static UINT32 supported_bits_per_sample [
  NUM_SUPPORTED_BITS_PER_SAMPLE
                                        ] =
{
  8,
  16
};

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

/*! \def    cpc_error_code  windows_add_format_description(
                              UINT32                in_bits_per_sample,
                              UINT32                in_num_channels,
                              UINT32                in_sample_rate,
                              UINT32                in_num_supported_formats,
                              cahal_device_stream*  io_device_stream
                            );
    \brief  Creates a new audio description with the passed in parameters and
            adds the newly created description to the list of supported formats
            pointed to by io_device_stream.

    \param  in_bits_per_sample  The number of bits per sample supported.
    \param  in_num_channels The number of channels supported.
    \param  in_sample_rate  The sample rate supported.
    \param  in_num_supported_formats  The number of supported formats currently
                                      in the list pointed to by
                                      io_device_stream.
    \param  io_device_stream  The list to add the supported format to.
    \return CPC_ERROR_CODE_NO_ERROR if the description is added. An error code
            otherwise.
 */
cpc_error_code
windows_add_format_description(
  UINT32                in_bits_per_sample,
  UINT32                in_num_channels,
  UINT32                in_sample_rate,
  UINT32                in_num_supported_formats,
  cahal_device_stream*  io_device_stream
);

/*! \def    HRESULT windows_set_device_stream_direction(
                        cahal_device_stream*  out_stream,
                        IMMDevice*            in_endpoint
                      );
    \brief  Queries the underlying hardware's direction (render or capture)
            and sets the stream appropriately.

    \param  out_stream  The stream who's direction needs to be set.
    \param  in_endpoint Pointer to the underlying hardware to be queried.
    \return S_OK iff the endpoint's direction can be queried. An error code
            otherwise.
 */
HRESULT
windows_set_device_stream_direction(
  cahal_device_stream*  out_stream,
  IMMDevice*            in_endpoint
);

/*! \def    HRESULT windows_determine_supported_formats(
                      cahal_device_stream*  out_stream,
                      IMMDevice*            in_endpoint
                    );
    \brief  Queries the endpoint for its supported formats (i.e., channels,
            sample rate, bit depth). Supported formats are added to out_stream.

    \param  out_stream Supported formats are added to this stream.
    \param  in_endpoint Pointer to the underlying hardware whos formats are to
                        be queried.
    \return S_OK iff the endpoint's formats can be queried and added to the
            list in out_stream.
 */
HRESULT
windows_determine_supported_formats(
  cahal_device_stream*  out_stream,
  IMMDevice*            in_endpoint
);

HRESULT
windows_set_device_stream_direction (
  cahal_device_stream*  out_stream,
  IMMDevice*            in_endpoint
                                    )
{
  HRESULT result                  = S_OK;
  IMMEndpoint *endpoint_interface = NULL;

  result =
    in_endpoint->QueryInterface (
      __uuidof( IMMEndpoint ),
      ( void** ) &endpoint_interface
                                );

  if( S_OK == result )
  {
    EDataFlow direction;

    result = endpoint_interface->GetDataFlow( &direction );

    if( S_OK == result )
    {
      if( eRender == direction )
      {
        out_stream->direction = CAHAL_DEVICE_OUTPUT_STREAM;
      }
      else if( eCapture == direction )
      {
        out_stream->direction = CAHAL_DEVICE_INPUT_STREAM;
      }
      else
      {
        CPC_LOG_STRING( CPC_LOG_LEVEL_WARN, "Unknown stream direction." );
      }

      CPC_LOG (
        CPC_LOG_LEVEL_DEBUG, 
        "Stream direction is 0x%x.", 
        out_stream->direction
              );
    }
  }

  return( result );
}

HRESULT
windows_determine_supported_formats(
  cahal_device_stream*  out_stream,
  IMMDevice*            in_endpoint
                                    )
{
  HRESULT result                = S_OK;
  IAudioClient *audio_client    = NULL;
  UINT32 num_supported_formats  = 0;

  result = in_endpoint->Activate(
    __uuidof( IAudioClient ),
    CLSCTX_ALL,
    NULL,
    ( void** )&audio_client
    );

  if( S_OK == result )
  {
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
          WAVEFORMATEX format;

          CPC_MEMSET( &format, 0x0, sizeof( WAVEFORMATEX ) );

          format.wFormatTag = WAVE_FORMAT_PCM;
          format.nChannels = ( num_channels + 1 );
          format.nSamplesPerSec = supported_sample_rates[sample_rate_index];
          format.wBitsPerSample =
            supported_bits_per_sample[bits_per_sample_index];
          format.nBlockAlign =
            ( format.nChannels * format.wBitsPerSample ) / 8;
          format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
          format.cbSize = 0;

          HRESULT supported_check = 
            audio_client->IsFormatSupported ( 
              AUDCLNT_SHAREMODE_EXCLUSIVE, 
              &format, 
              NULL
                                            );

          if( S_OK == supported_check )
          {
            cpc_error_code format_added =
              windows_add_format_description  (
                format.wBitsPerSample, 
                format.nChannels, 
                format.nSamplesPerSec, 
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

  return( result );
}

cpc_error_code
windows_add_format_description(
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

HRESULT
windows_set_device_streams(
  cahal_device* out_device,
  IMMDevice*    in_endpoint
                          )
{
  HRESULT result = S_OK;

  if(
    CPC_ERROR_CODE_NO_ERROR
    == cpc_safe_malloc(
        ( void** )&( out_device->device_streams ),
        sizeof( cahal_device_stream* )* 2
                        )
    )
  {
    if(
      CPC_ERROR_CODE_NO_ERROR
      == cpc_safe_malloc(
          ( void** )&( out_device->device_streams[0] ),
          sizeof( cahal_device_stream )
                          )
      )
    {
      out_device->device_streams[0]->handle =
          WINDOWS_DEVICE_STREAM_DEFAULT_HANDLE;
      out_device->device_streams[0]->preferred_format =
          CAHAL_AUDIO_FORMAT_LINEARPCM;

      result =
        windows_set_device_stream_direction(
          out_device->device_streams[0],
          in_endpoint
                                            );

      if( S_OK != result )
      {
        CPC_LOG(
          CPC_LOG_LEVEL_WARN,
          "Could not direction of stream: 0x%x.",
          result
                );
      }

      result =
        windows_determine_supported_formats (
          out_device->device_streams[0], 
          in_endpoint
                                            );

      if( S_OK != result )
      {
        CPC_LOG( CPC_LOG_LEVEL_WARN, "Could not set formats: 0x%x.", result );
      }
    }
  }

  return( result );
}

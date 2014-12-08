#ifndef __ANDROID_CAHAL_H__
#define __ANDROID_CAHAL_H__

#include <unistd.h>
#include <stdlib.h>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include <cpcommon.h>

#include "cahal.h"

#include "android_cahal_audio_format_description.h"

#define ANDROID_DEVICE_HANDLE_OUTPUT          1

#define ANDROID_DEVICE_STREAM_DEFAULT_HANDLE  1

extern SLObjectItf g_engine_object;
extern SLEngineItf g_engine_interface;

void
android_terminate( void );

cpc_error_code
android_create_output_source  (
    SLDataFormat_PCM* in_audio_format,
    SLDataSource*     out_output_source
                              );

cpc_error_code
android_create_input_source (
    SLDataSource *out_input_source
                            );

cpc_error_code
android_create_output_sink  (
    SLDataSink* out_output_sink
                            );

cpc_error_code
android_create_input_sink (
    SLDataFormat_PCM* in_audio_format,
    SLDataSink*       out_input_sink
                          );

SLresult
android_set_config  (
    SLObjectItf in_recorder_object,
    UINT32      in_configuration
                    );

CPC_BOOL
android_test_input_configuration (
    SLDataFormat_PCM* in_audio_format,
    UINT32            in_configuration
                                );

cpc_error_code
android_init_input_device_struct  (
    UINT32                in_configuration_index,
    cahal_device**        out_device,
    cahal_device_stream** out_stream
                                  );

cpc_error_code
android_set_audio_format_struct (
    UINT32            in_num_channels,
    UINT32            in_sample_rate,
    UINT32            in_bits_per_sample,
    SLDataFormat_PCM* out_audio_format
                                );

CPC_BOOL
android_init_and_test_output_configuration (
    UINT32 in_num_channels,
    UINT32 in_sample_rate,
    UINT32 in_bits_per_sample
                                            );

CPC_BOOL
android_test_output_configuration (
    SLDataFormat_PCM* in_audio_format
                                  );

cpc_error_code
android_set_output_device (
    cahal_device*** io_device_list,
    UINT32*         io_num_devices
                          );

cpc_error_code
android_set_input_devices (
    cahal_device*** io_device_list,
    UINT32*         io_num_devices
                          );

cpc_error_code
android_add_device_to_list  (
    cahal_device*   in_device,
    cahal_device*** io_device_list,
    UINT32*         io_number_of_devices
                            );

#endif /* __ANDROID_CAHAL_H__ */

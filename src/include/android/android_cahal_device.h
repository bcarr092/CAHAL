#ifndef __ANDROID_CAHAL_DEVICE_H__
#define __ANDROID_CAHAL_DEVICE_H__

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include <cpcommon.h>

#include "cahal.h"

#include "android_cahal.h"

typedef struct android_callback_info_t
{
  UINT32  current_buffer_index;

  UINT32  number_of_buffers;

  UINT32  buffer_size;

  UCHAR** buffers;

  void*   context;

} android_callback_info;

typedef struct android_recoder_context_t
{
  SLDataFormat_PCM*             audio_format;

  SLDataSource*                 input_source;
  SLDataSink*                   input_sink;

  SLObjectItf                   recorder_object;
  SLRecordItf                   recorder_interface;

  SLAndroidSimpleBufferQueueItf buffer_interface;

} android_recorder_context;

typedef struct android_playback_context_t
{
  SLDataFormat_PCM*             audio_format;

  SLDataSource*                 input_source;
  SLDataSink*                   input_sink;

  SLObjectItf                   playback_object;
  SLPlayItf                     playback_interface;

  SLAndroidSimpleBufferQueueItf buffer_interface;

} android_playback_context;

cpc_error_code
android_save_playback_context  (
    SLDataFormat_PCM*             in_audio_format,
    SLDataSource*                 in_input_source,
    SLDataSink*                   in_input_sink,
    SLObjectItf                   in_playback_object,
    SLPlayItf                     in_playback_interface,
    SLAndroidSimpleBufferQueueItf in_buffer_interface
                                );

cpc_error_code
android_save_recording_context  (
    SLDataFormat_PCM*             in_audio_format,
    SLDataSource*                 in_input_source,
    SLDataSink*                   in_input_sink,
    SLObjectItf                   in_recorder_object,
    SLRecordItf                   in_recorder_interface,
    SLAndroidSimpleBufferQueueItf in_buffer_interface
                                );

void android_recorder_callback (
    SLAndroidSimpleBufferQueueItf in_recorder_buffer,
    void*                         in_user_data
                                    );

void
android_playback_callback (
    SLAndroidSimpleBufferQueueItf in_playback_buffer,
    void*                         in_user_data
                          );

cpc_error_code
android_initialize_playback_structs  (
    UINT32            in_number_of_channels,
    FLOAT64           in_sample_rate,
    UINT32            in_bit_depth,
    SLDataFormat_PCM* out_audio_format,
    SLDataSource*     out_input_source,
    SLDataSink*       out_input_sink
                                      );

cpc_error_code
android_initialize_recording_structs  (
    UINT32            in_number_of_channels,
    FLOAT64           in_sample_rate,
    UINT32            in_bit_depth,
    SLDataFormat_PCM* out_audio_format,
    SLDataSource*     out_input_source,
    SLDataSink*       out_input_sink
                                      );

cpc_error_code
android_register_playback (
    cahal_device*                   in_device,
    SLDataSource*                   in_output_source,
    SLDataSink*                     in_output_sink,
    SLObjectItf*                    io_playback_object,
    SLPlayItf*                      out_playback_interface,
    SLAndroidSimpleBufferQueueItf*  out_buffer_interface
                          );

cpc_error_code
android_register_recorder (
    cahal_device*                   in_device,
    SLDataSource*                   in_input_source,
    SLDataSink*                     in_input_sink,
    SLObjectItf*                    io_recorder_object,
    SLRecordItf*                    out_recorder_interface,
    SLAndroidSimpleBufferQueueItf*  out_buffer_interface
                          );

cpc_error_code
android_register_playback_callback  (
    cahal_device*                 in_device,
    cahal_playback_callback       in_playback,
    void*                         in_callback_user_data,
    SLPlayItf                     in_playback_interface,
    SLAndroidSimpleBufferQueueItf in_buffer_interface,
    cahal_playback_info**         out_playback_callback_info
                                    );

cpc_error_code
android_register_recorder_callback  (
    cahal_device*                 in_device,
    cahal_recorder_callback       in_recorder,
    void*                         in_callback_user_data,
    SLRecordItf                   in_recorder_interface,
    SLAndroidSimpleBufferQueueItf in_buffer_interface,
    cahal_recorder_info**         out_recorder_callback_info
                                    );

cpc_error_code
android_compute_bytes_per_buffer  (
    SLDataFormat_PCM* in_audio_format,
    FLOAT32           in_number_of_seconds,
    UINT32*           out_bytes_per_buffer
                                  );

cpc_error_code
android_enqueue_playback_buffers  (
    SLDataFormat_PCM*             in_audio_format,
    SLAndroidSimpleBufferQueueItf in_buffer_interface,
    cahal_playback_info*          out_playback_callback_info
                                );

cpc_error_code
android_enqueue_record_buffers  (
    SLDataFormat_PCM*             in_audio_format,
    SLAndroidSimpleBufferQueueItf in_buffer_interface,
    cahal_recorder_info*          out_recorder_callback_info
                                );

#endif /* __ANDROID_CAHAL_DEVICE_H__ */

/*! \file   osx_cahal_device.h
    \brief  This interface takes care of creating the platform-agnostic device
            structs as well as the implementation for the audio hardware
            controls, i.e. playback and record.
 */
#ifndef __OSX_CAHAL_DEVICE_H__
#define __OSX_CAHAL_DEVICE_H__

#include <CoreFoundation/CoreFoundation.h>
#include <CoreAudio/CoreAudio.h>

#include <AudioToolbox/AudioToolbox.h>

#include <darwin_helper.h>

#include "cahal.h"
#include "cahal_device.h"
#include "cahal_device_stream.h"
#include "cahal_audio_format_flags.h"

#include "osx_cahal_audio_format_flags.h"
#include "osx_cahal_device_stream.h"

/*! \fn     void osx_set_cahal_device_struct(
              AudioObjectID in_device_id,
              cahal_device* io_device
            )
    \brief  Populate the given io_device struct with the parameters of the
            audio hardware object defined by in_device_id. Note that this 
            function will catalogue all the supported streams of the device
            as well as all of the supported formats.
 
    \param  in_device_id  The device handle whose properties are to be populated
                          in io_device.
    \param  io_device     The cahal_device struct to populate with the
                          properties of in_device_id.
 */
void
osx_set_cahal_device_struct(
                            AudioObjectID in_device_id,
                            cahal_device* io_device
                            );

/*! \fn     OSStatus osx_get_device_uint32_property  (
              AudioObjectID                in_device_id,
              AudioObjectPropertySelector  in_property,
              UINT32*                      out_device_property
            )
    \brief  Extract the UINT32 value of in_property from the audio hardware
            in_device_id. The extracted value is stored in out_device_property.
            This is a helper function to wrap the OSX interface for audio 
            hardware.
 
    \param  in_device_id  The device handle whose property is to be extracted.
    \param  in_property   The property to query on in_device_id
    \param  out_device_property The location to store the extracted property
                                value.
    \return Either noErr or an error code. The error code can be passed to
            darwin_helper for printing.
 */
OSStatus
osx_get_device_uint32_property  (
                               AudioObjectID                in_device_id,
                               AudioObjectPropertySelector  in_property,
                               UINT32*                      out_device_property
                                 );

/*! \fn     OSStatus osx_get_device_float64_property  (
              AudioObjectID                in_device_id,
              AudioObjectPropertySelector  in_property,
              FLOAT64*                     out_device_property
            )
    \brief  Extract the FLOAT64 value of in_property from the audio hardware
            in_device_id. The extracted value is stored in out_device_property.
            This is a helper function to wrap the OSX interface for audio
            hardware.

    \param  in_device_id  The device handle whose property is to be extracted.
    \param  in_property   The property to query on in_device_id
    \param  out_device_property The location to store the extracted property
                                value.
    \return Either noErr or an error code. The error code can be passed to
            darwin_helper for printing.
 */
OSStatus
osx_get_device_float64_property (
                               AudioObjectID                in_device_id,
                               AudioObjectPropertySelector  in_property,
                               FLOAT64*                     out_device_property
                                 );

/*! \fn     OSStatus osx_get_device_string_property  (
              AudioObjectID                in_device_id,
              AudioObjectPropertySelector  in_property,
              CHAR**                       out_device_property
            )
    \brief  Extract the CHAR value of in_property from the audio hardware
            in_device_id. The extracted value is stored in out_device_property.
            This is a helper function to wrap the OSX interface for audio
            hardware.

    \param  in_device_id  The device handle whose property is to be extracted.
    \param  in_property   The property to query on in_device_id
    \param  out_device_property The location to store the extracted property
                                value.
    \return Either noErr or an error code. The error code can be passed to
            darwin_helper for printing.
 */
OSStatus
osx_get_device_string_property  (
                                 AudioObjectID               in_device_id,
                                 AudioObjectPropertySelector in_property,
                                 CHAR**                      out_device_property
                                 );

/*! \fn     OSStatus osx_get_device_property_value  (
              AudioObjectID                in_device_id,
              AudioObjectPropertySelector  in_property,
              UINT32                       in_device_property_value_size,
              void*                        out_device_property_value
            )
    \brief  Generic function to extract the value for in_property from the audio
            hardware in_device_id. The extracted value is stored in
            out_device_property_value.
            This is a helper function to wrap the OSX interface for audio
            hardware.

    \param  in_device_id  The device handle whose property is to be extracted.
    \param  in_property   The property to query on in_device_id
    \param  in_device_property_value_size The number of bytes that will be
                                          stored in out_device_property_value.
    \param  out_device_property_value The location to store the extracted
                                      property value.
    \return Either noErr or an error code. The error code can be passed to
            darwin_helper for printing.
 */
OSStatus
osx_get_device_property_value (
                     AudioObjectID                in_device_id,
                     AudioObjectPropertySelector  in_property,
                     UINT32                       in_device_property_value_size,
                     void*                        out_device_property_value
                               );

/*! \fn     OSStatus osx_get_device_property_size_and_value  (
              AudioObjectID                in_device_id,
              AudioObjectPropertySelector  in_property,
              UINT32*                      out_device_property_value_size,
              void**                       out_device_property_value
            )
    \brief  Generic function to extract both the size and value for in_property
            from the audio hardware in_device_id. The extracted size is stored
            in out_device_property_value_size and value is stored in
            out_device_property_value. This is a helper function to wrap the OSX
            interface for audio hardware.

    \param  in_device_id  The device handle whose property is to be extracted.
    \param  in_property   The property to query on in_device_id
    \param  out_device_property_value_size  The location to store the number of
                                            bytes required to store the property
                                            value.
    \param  out_device_property_value The location to store the extracted
                                      property value. The size of
                                      out_device_property_value is
                                      out_device_property_value_size. The caller
                                      must free this parameter.
    \return Either noErr or an error code. The error code can be passed to
            darwin_helper for printing.
 */
OSStatus
osx_get_device_property_size_and_value (
                   AudioObjectID                in_device_id,
                   AudioObjectPropertySelector  in_property,
                   UINT32*                      out_device_property_value_size,
                   void**                       out_device_property_value
                                        );

/*! \fn     OSStatus osx_get_device_supported_sample_rates (
              cahal_device* io_device
            )
    \brief  Populate the list of supported sample rates in io_device. The
            caller must free the list of supported sample rates. The list is
            null-terminated. This is a helper function to wrap the OSX
            interface for audio hardware.
 
    \param  io_device The sample rates supported by the audio device pointed to
                      by the handle in io_device will be logged in the list of
                      sample rates in io_device. This is a null terminated list
                      and must be freed by the caller.
    \return Either noErr or an error code. The error code can be passed to
            darwin_helper for printing.
 */
OSStatus
osx_get_device_supported_sample_rates (
                                       cahal_device* io_device
                                       );

/*! \fn     OSStatus osx_get_number_of_channels  (
              cahal_device* io_device
            )
    \brief  Populate the number of supported channels in the cahal_device. We
            query a number of different properties to find the correct number of
            channels supported by the device.
 
    \param  io_device The number of channels supported by the device will be
                      logged in the numeber_of_channels field of io_device.
    \return Either noErr or an error code. The error code can be passed to
            darwin_helper for printing.
 */
OSStatus
osx_get_number_of_channels  (
                             cahal_device* io_device
                             );

/*! \fn     void osx_recorder_callback  (
              void*                                in_user_data,
              AudioQueueRef                        in_queue,
              AudioQueueBufferRef                  in_buffer,
              const AudioTimeStamp*                in_start_time,
              UINT32                               in_number_of_packets,
              const AudioStreamPacketDescription*  in_packet_description
            )
    \brief  This the callback that is passed to the Core Audio library, which is
            called when a buffer of recorded samples is ready for processing.
    
    \param  in_user_data  This is a pointer to a cahal_recorder_info struct. It
                          contains the device that the recording was made from
                          as well as the callback function that should be called
                          with the recorded buffer.
    \param  in_queue  A reference to the queue containing the buffer of recorded
                      samples.
    \param  in_buffer The buffer of recorded samples. Note that the buffer
                      contains samples encoded in the format provided when
                      cahal_start_recording was called.
    \param  in_start_time Timestamp the recording was made at.
    \param  in_number_of_packets  The number of packets in the recording. Note
                                  that this is not the same as the number of
                                  bytes in the packet. At this point only
                                  constant bit rate codes are supported so this
                                  value is always used to calculate the number
                                  of bytes in in_buffer.
    \param  in_packet_description Format information for the packet. Only used
                                  with VBR codecs. Always null in CBR cases.
 */
void
osx_recorder_callback  (
                      void*                                in_user_data,
                      AudioQueueRef                        in_queue,
                      AudioQueueBufferRef                  in_buffer,
                      const AudioTimeStamp*                in_start_time,
                      UINT32                               in_number_of_packets,
                      const AudioStreamPacketDescription*  in_packet_description
                        );

/*! \fn     void osx_playback_callback (
              void*                in_user_data,
              AudioQueueRef        in_queue,
              AudioQueueBufferRef  in_buffer
            )
    \brief  This the callback that is passed to the Core Audio library, which is
            called when a buffer of samples needs to be populated for playback.

    \param  in_user_data  This is a pointer to a cahal_playback_info struct. It
                          contains the device that the playback is to made on
                          as well as the callback function that should be called
                          to fill in_buffer.
    \param  in_queue  A reference to the queue containing the buffer of playback
                      samples.
    \param  in_buffer The empty buffer to be filled with samples. Note that the
                      buffer must be filled with samples encoded in the format
                      provided when cahal_start_playback was called.
 */
void
osx_playback_callback (
                       void*                in_user_data,
                       AudioQueueRef        in_queue,
                       AudioQueueBufferRef  in_buffer
                       );

/*! \fn     OSStatus osx_configure_asbd  (
              cahal_audio_format_id          in_format_id,
              UINT32                         in_number_of_channels,
              FLOAT64                        in_sample_rate,
              UINT32                         in_bit_depth,
              cahal_audio_format_flag        in_format_flags,
              cahal_device_stream_direction  in_direction,
              AudioStreamBasicDescription    *out_asbd
            )
    \brief  Helper function to create the AudioStreamBasicDescrition. This is
            the platform specific model that is populated with the format
            information and parameters. The ASBD is used in both input and
            output operations.
 
    \param  in_format_id  The CAHAL format that the samples will be encoded in
                          for playback/recording, e.g. lpcm.
    \param  in_number_of_channels The number of channels in the stream.
    \param  in_sample_rate  The number of samples to capture per second.
    \param  in_bit_depth  The quantization level, i.e. the number of bits per
                          sample.
    \param  in_format_flags Flags to further describe the encoding method. These
                            flags are CAHAL flags.
    \param  in_direction  The direction the ASBD is being built for, i.e. input
                          or output.
    \param  out_asbd  The newly populated ASBD if no error occurs.
 */
OSStatus
osx_configure_asbd  (
                     cahal_audio_format_id          in_format_id,
                     UINT32                         in_number_of_channels,
                     FLOAT64                        in_sample_rate,
                     UINT32                         in_bit_depth,
                     cahal_audio_format_flag        in_format_flags,
                     cahal_device_stream_direction  in_direction,
                     AudioStreamBasicDescription    *out_asbd
                     );

/*! \fn     OSStatus osx_compute_bytes_per_buffer  (
              AudioStreamBasicDescription* in_asbd,
              FLOAT32                      in_number_of_seconds,
              UINT32*                      out_bytes_per_buffer
            )
    \brief  Helper function to calculate number of bytes to put in each buffer
            based on the configured ASBD and in_number_of_seconds.
 
    \param  in_asbd The ASBD that contains the relevant encoding information,
                    e.g. number of channels, bit depth, sample rate.
    \param  in_number_of_seconds  The number of seconds that each buffer will
                                  represent.
    \param  out_bytes_per_buffer  The set number of bytes to put in each buffer.
    \return noErr(0) if no error occurs or an appropriate error code.
 */
OSStatus
osx_compute_bytes_per_buffer  (
                             AudioStreamBasicDescription* in_asbd,
                             FLOAT32                      in_number_of_seconds,
                             UINT32*                      out_bytes_per_buffer
                               );

/*! \fn     OSStatus osx_configure_input_audio_queue (
              cahal_device*                 in_device,
              cahal_recorder_info*          in_callback_info,
              AudioStreamBasicDescription*  io_asbd,
              AudioQueueRef*                out_audio_queue
            )
    \brief  Configures the platforms input (record) audio queue.
 
    \param  in_device The recording device.
    \param  in_callback_info  The callback that is called by the system when
                              a buffer of samples is ready for processing.
    \param  io_asbd The ASDB containing all the format and endocing information.
                    This value is somteimes updated by the queue when format
                    parameters are changed.
    \param  out_audio_queue The newly created platform-specific audio queue.
    \return noErr(0) if no error occurs or an appropriate error code.
 */
OSStatus
osx_configure_input_audio_queue (
                                 cahal_device*                 in_device,
                                 cahal_recorder_info*          in_callback_info,
                                 AudioStreamBasicDescription*  io_asbd,
                                 AudioQueueRef*                out_audio_queue
                                 );

/*! \fn     OSStatus osx_configure_input_audio_queue (
              cahal_device*                 in_device,
              cahal_recorder_info*          in_callback_info,
              AudioStreamBasicDescription*  io_asbd,
              AudioQueueRef*                out_audio_queue
            )
    \brief  Configures the platforms ouput (playback) audio queue.

    \param  in_device The playback device.
    \param  in_callback_info  The callback that is called by the system when
                              a buffer of samples needs to be filled.
    \param  in_asbd The ASDB containing all the format and endocing information.
    \param  out_audio_queue The newly created platform-specific audio queue.
    \return noErr(0) if no error occurs or an appropriate error code.
    */
OSStatus
osx_configure_output_audio_queue (
                                cahal_device*                 in_device,
                                cahal_playback_info*          in_callback_info,
                                AudioStreamBasicDescription*  in_asbd,
                                AudioQueueRef*                out_audio_queue
                                  );

/*! \fn     OSStatus osx_configure_input_audio_queue_buffer  (
              AudioStreamBasicDescription* in_asbd,
              AudioQueueRef                io_audio_queue
            )
    \brief  Allocates the necessary audio queue buffers and populates the queue
            with them.
 
    \param  in_asbd The ASBD containing the format and encoding information.
    \param  io_audio_queue  The audio queue that is populated with buffers.
 */
OSStatus
osx_configure_input_audio_queue_buffer  (
                                     AudioStreamBasicDescription* in_asbd,
                                     AudioQueueRef                io_audio_queue
                                         );
/*! \fn     OSStatus osx_configure_output_audio_queue_buffer  (
              AudioStreamBasicDescription*  in_asbd,
              cahal_playback_info*          in_playback,
              AudioQueueRef                 io_audio_queue
            )
    \brief  Allocates the necessary audio queue buffers and populates the queue
            with them. This function will call the playback callback to
            initially populate the buffer queue with data.

    \param  in_asbd The ASBD containing the format and encoding information.
    \param  in_playback The user data struct containing device specific info
                        required by the playback callback.
    \param  io_audio_queue  The audio queue that is populated with buffers.
    */
OSStatus
osx_configure_output_audio_queue_buffer  (
                                    AudioStreamBasicDescription*  in_asbd,
                                    cahal_playback_info*          in_playback,
                                    AudioQueueRef                 io_audio_queue
                                          );

#endif /* __OSX_CAHAL_DEVICE_H__ */

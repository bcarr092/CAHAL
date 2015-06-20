#ifndef __CAHAL_WRAPPER_H__
#define __CAHAL_WRAPPER_H__

#include <Python.h>

#include <cahal.h>

/*! \fn     void python_recorder_callback (
              cahal_device* in_recording_device,
              UCHAR*        in_data_buffer,
              UINT32        in_data_buffer_length,
              void*         in_user_data
            )
    \brief  This is the trampoline callback function that is called from the
            CAHAL library. It is responsible for converting the input
            parameters to Python objects and calling the python callback
            function.

    \param  in_recording_device The recording device being used to capture
            audio samples.
    \param  in_data_buffer  The data buffer containing the audio samples. Its
            length is in_data_buffer_length in bytes.
    \param  in_data_buffer_length The length (in bytes) of in_data_buffer.
    \param  in_user_data  A function pointer to the Python callback.
    \return True iff the callback was succesfully called and succesfully
            executed.
*/
CPC_BOOL
python_recorder_callback(
  cahal_device* in_recording_device,
  UCHAR*        in_data_buffer,
  UINT32        in_data_buffer_length,
  void*         in_user_data
);

/*! \fn     void python_playback_callback (
              cahal_device* in_playback_device,
              UCHAR*        out_data_buffer,
              UINT32*       io_data_buffer_length,
              void*         in_user_data
            )
    \brief  This is the trampoline callback function that is called from the
            CAHAL library. It is responsible for converting the input
            parameters to Python objects and calling the python callback
            function.

    \param  in_playback_defivce The playback device being used to play
                                audio samples.
    \param  in_data_buffer  The data buffer that is to populated with audio
                            samples. Its maximum capacity is
                            io_data_buffer_length in bytes.
    \param  io_data_buffer_length The capacity (in bytes) of in_data_buffer.
                                  This value is to be set in this function to
                                  the number of bytes placed in in_data_buffer.
    \param  in_user_data  A function pointer to the Python callback.
    \return True iff the callback was succesfully called and succesfully
            executed.
*/
CPC_BOOL
python_playback_callback(
  cahal_device* in_playback_device,
  UCHAR*        out_data_buffer,
  UINT32*       io_data_buffer_length,
  void*         in_user_data
);

/*! \fn     cahal_device* cahal_device_list_get (
              cahal_device** device_list,
              int index
            )
    \brief  Accessor function for a cahal device list. Note that no bounds
            checks are performed by this function.

    \param  in_device_list The array of cahal_device structs to access.
    \param  in_index  The index of the desired element in in_device_list.
    \return The element at in_device_list[ in_index ]
*/
cahal_device*
cahal_device_list_get(
  cahal_device**  in_device_list,
  int             in_index
);

/*! \fn     cahal_device_stream* cahal_device_stream_list_get (
              cahal_device_stream** stream_list,
              int index
            )
    \brief  Accessor function for a cahal device stream list. Note that no
            bounds checks are performed by this function.

    \param  in_stream_list The array of cahal_device_stream structs to access.
    \param  in_index  The index of the desired element in in_stream_list.
    \return The element at in_stream_list[ in_index ]
*/
cahal_device_stream*
cahal_device_stream_list_get(
  cahal_device_stream** in_stream_list,
  int                   in_index
);

/*! \fn     cahal_audio_format_description* cahal_audio_format_description_list_get (
              cahal_audio_format_description** description_list,
              int index
            )
    \brief  Accessor function for a cahal audio format description list. Note
            that no bounds checks are performed by this function.

    \param  in_description_list The array of cahal_audio_format_description
                                structs to access.
    \param  in_index  The index of the desired element in in_description_list.
    \return The element at in_description_list[ in_index ]
*/
cahal_audio_format_description*
cahal_audio_format_description_list_get(
  cahal_audio_format_description**  in_description_list,
  int                               in_index
);

/*! \fn     cahal_sample_rate_range* cahal_sample_rate_range_list_get (
              cahal_sample_rate_range**
              sample_rate_range_list,
              int index
            )
    \brief  Accessor function for a cahal sample rate range list. Note
            that no bounds checks are performed by this function.

    \param  in_sample_rate_range_list The array of cahal_sample_rate_range
                                      structs to access.
    \param  in_index  The index of the desired element in
                      in_sample_rate_range_list.
    \return The element at in_sample_rate_range[ in_index ]
*/
cahal_sample_rate_range*
cahal_sample_rate_range_list_get(
  cahal_sample_rate_range**   in_sample_rate_range_list,
  int                         in_index
);

/*! \fn     double* cahal_float_array_get (
              FLOAT64** array,
              int index
            )
    \brief  Wrapper to provide access to an array of doubles. Note that this
            function provides no bounds checking.

    \param  in_array  The array to be accessed.
    \param  in_index  The index of the element in in_array to be returned.
    \return The element at in_array[ in_index ]
*/
double*
cahal_float_array_get(
  FLOAT64** in_array,
  int       in_index
);

/*! \fn     void start_recording (
              cahal_device*         in_device,
              int                   in_format_id,
              int                   in_number_of_channels,
              double                in_sample_rate,
              int                   in_bit_depth,
              PyObject*             in_callback_function,
              int                   in_format_flags
            )
    \brief  Wrapper function that passes the trampoline callback
            python_recorder_callback as the intermediate callback to the
            cahal library. The actual Python callback is passed as an
            additional parameter to be used by the trampoline callback.

    \param  in_device The device to use in the recording. This object is passed
                      back by the cahal library in the callback.
    \param  in_format_id  The desired format (e.g. lpcm). This is a cahal
                          flag.
    \param  in_number_of_channels The number of channels requested in the
                                  recording.
    \param  in_sample_rate  The desired sample rate to use in the recording.
    \param  in_bit_depth  The quantization level, i.e. number of bits per
                          sample, to use in the recording.
    \param  in_callback_function  The Python callback function to be called
                                  by the trampoline callback function when
                                  buffers of audio data are made available.
    \param  in_format_flags The format flag bitmask indicating the cahal
                            flags to use in the recording.
    \return Returns the result of cahal_start_recording, which is a boolean.
*/
PyObject*
start_recording(
  cahal_device* in_device,
  int           in_format_id,
  int           in_number_of_channels,
  double        in_sample_rate,
  int           in_bit_depth,
  PyObject*     in_callback_function,
  int           in_format_flags
);

/*! \fn     void start_playback (
              cahal_device*         in_device,
              int                   in_format_id,
              int                   in_number_of_channels,
              double                in_sample_rate,
              int                   in_bit_depth,
              PyObject*             in_callback_function,
              int                   in_format_flags
            )
    \brief  Wrapper function that passes the trampoline callback
            python_playback_callback as the intermediate callback to the
            cahal library. The actual Python callback is passed as an
            additional parameter to be used by the trampoline callback.

    \param  in_device The device to use in the playback. This object is passed
                      back by the cahal library in the callback.
    \param  in_format_id  The desired format (e.g. lpcm). This is a cahal
                          flag.
    \param  in_number_of_channels The number of channels requested in the
                                   playback stream.
    \param  in_sample_rate  The desired sample rate to use in the playback.
    \param  in_bit_depth  The quantization level, i.e. number of bits per
                          sample, to use in the playback.
    \param  in_callback_function  The Python callback function to be called
                                  by the trampoline callback function when
                                  buffers of audio data are made available.
    \param  in_format_flags The format flag bitmask indicating the cahal
                            flags to use in the playback.
    \return Returns the result of cahal_start_playback, which is a boolean.
*/
PyObject*
start_playback(
               cahal_device* in_device,
               int           in_format_id,
               int           in_number_of_channels,
               double        in_sample_rate,
               int           in_bit_depth,
               PyObject*     in_callback_function,
               int           in_format_flags
               );

/*! \fn     void python_cahal_initialize( void )
    \brief  Wrapper for the cahal_initialize function to ensure the GIL is
            properly set up for threads to be iniitialized in external C
            calls.
 */
void
python_cahal_initialize( void );

#endif  /*  __CAHAL_WRAPPER_H__ */

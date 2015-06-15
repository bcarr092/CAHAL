#include "cahal_wrapper.h"

cahal_device*
cahal_device_list_get(
  cahal_device**  in_device_list,
  int             in_index
)
{
  if( NULL == in_device_list )
  {
    return( NULL );
  }
  else
  {
    return( in_device_list[in_index] );
  }
}

cahal_device_stream*
cahal_device_stream_list_get(
  cahal_device_stream** in_stream_list,
  int                   in_index
)
{
  if( NULL == in_stream_list )
  {
    return( NULL );
  }
  else
  {
    return( in_stream_list[in_index] );
  }
}

cahal_audio_format_description*
cahal_audio_format_description_list_get(
  cahal_audio_format_description**  in_description_list,
  int                               in_index
)
{
  if( NULL == in_description_list )
  {
    return( NULL );
  }
  else
  {
    return( in_description_list[in_index] );
  }
}

cahal_sample_rate_range*
cahal_sample_rate_range_list_get(
  cahal_sample_rate_range**   in_sample_rate_range_list,
  int                         in_index
)
{
  if( NULL == in_sample_rate_range_list )
  {
    return( NULL );
  }
  else
  {
    return( in_sample_rate_range_list[in_index] );
  }
}

double*
cahal_float_array_get(
  FLOAT64** in_array,
  int       in_index
)
{
  if( NULL == in_array )
  {
    return( NULL );
  }
  else
  {
    return( in_array[in_index] );
  }
}

PyObject*
start_recording(
  cahal_device* in_device,
  int           in_format_id,
  int           in_number_of_channels,
  double        in_sample_rate,
  int           in_bit_depth,
  PyObject*     in_callback_function,
  int           in_format_flags
)
{
  int result = cahal_start_recording(
    in_device,
    in_format_id,
    in_number_of_channels,
    in_sample_rate,
    in_bit_depth,
    python_recorder_callback,
    in_callback_function,
    in_format_flags
    );

  if( 1 == result )
  {
    Py_RETURN_TRUE;
  }
  else
  {
    Py_RETURN_FALSE;
  }
}

CPC_BOOL
python_recorder_callback(
  cahal_device* in_recording_device,
  UCHAR*        in_data_buffer,
  UINT32        in_data_buffer_length,
  void*         in_user_data
)
{
  CPC_BOOL return_value = CPC_FALSE;

  PyObject *python_callback = NULL;
  PyObject *result          = NULL;
  PyObject *argument_list   = NULL;

  python_callback = ( PyObject* )in_user_data;
  
  if( PyCallable_Check(( python_callback ) ) )
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Acquiring GIL." );
    
    PyGILState_STATE state = PyGILState_Ensure();
    
    CPC_LOG( CPC_LOG_LEVEL_DEBUG, "Acquired GIL: 0x%x.", state );
    
    argument_list =
      Py_BuildValue(
                    "(Os#n)",
                    in_recording_device,
                    in_data_buffer,
                    in_data_buffer_length,
                    in_data_buffer_length
                    );

    if( NULL != argument_list )
    {
      result = PyEval_CallObject( python_callback, argument_list );

      if( NULL == result )
      {
        PyErr_Print( );
      }
      else
      {
        return_value = CPC_TRUE;
      }
    }
    else
    {
      PyErr_Print( );
    }
    
    Py_XDECREF( argument_list );
    Py_XDECREF( result );
    
    CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Releasing GIL." );
    
    PyGILState_Release( state );
    
    CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Released GIL." );
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Callback is not callable." );
  }

  return( return_value );
}

PyObject*
start_playback(
  cahal_device* in_device,
  int           in_format_id,
  int           in_number_of_channels,
  double        in_sample_rate,
  int           in_bit_depth,
  PyObject*     in_callback_function,
  int           in_format_flags
)
{
  int result = cahal_start_playback(
    in_device,
    in_format_id,
    in_number_of_channels,
    in_sample_rate,
    in_bit_depth,
    python_playback_callback,
    in_callback_function,
    in_format_flags
    );

  if( 1 == result )
  {
    Py_RETURN_TRUE;
  }
  else
  {
    Py_RETURN_FALSE;
  }
}

CPC_BOOL
python_playback_callback(
  cahal_device* in_playback_device,
  UCHAR*        out_data_buffer,
  UINT32*       io_data_buffer_length,
  void*         in_user_data
)
{
  CPC_BOOL return_value = CPC_FALSE;

  PyObject *python_callback = NULL;
  PyObject *result          = NULL;
  PyObject *argument_list   = NULL;

  python_callback = ( PyObject* )in_user_data;
  
  argument_list =
  Py_BuildValue(
                "(On)",
                in_playback_device,
                *io_data_buffer_length
                );
  
  if( PyCallable_Check(( python_callback ) ) )
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Acquiring GIL." );
    
    PyGILState_STATE state = PyGILState_Ensure();
    
    CPC_LOG( CPC_LOG_LEVEL_DEBUG, "Acquired GIL: 0x%x.", state );

    if( NULL != argument_list )
    {
      result = PyEval_CallObject( python_callback, argument_list );

      if( NULL != result && PyString_Check( result ) )
      {
        Py_ssize_t length = PyString_Size( result );
        char* buffer = PyString_AsString( result );

        if( length <= *io_data_buffer_length )
        {
          *io_data_buffer_length = length;

          memcpy( out_data_buffer, buffer, length );

          return_value = CPC_TRUE;
        }
        else
        {
          CPC_ERROR (
                     "Buffer is too small (size=%d) for data (size=%zd)\n",
                     ( unsigned int )*io_data_buffer_length,
                     length
                     );
        }
      }
      else
      {
        PyErr_Print( );
      }
    }
    else
    {
      PyErr_Print( );
    }
    
    Py_XDECREF( argument_list );
    Py_XDECREF( result );
    
    CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Releasing GIL." );
    
    PyGILState_Release( state );
    
    CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Released GIL." );
  }
  else
  {
    CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Callback is not callable." );
  }

  return( return_value );
}

void
python_cahal_sleep  (
                     UINT32 in_sleep_duration
                     )
{
  PyThreadState *state = PyThreadState_Get();
  
  CPC_LOG( CPC_LOG_LEVEL_DEBUG, "Releasing GIL: 0x%x.", state->interp );
  
  Py_BEGIN_ALLOW_THREADS
  
  CPC_LOG( CPC_LOG_LEVEL_DEBUG, "Sleeping for 0x%x.", in_sleep_duration );
  
  cahal_sleep( in_sleep_duration );
  
  CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Done sleeping." );
  
  CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Reaquiring GIL." );
  
  Py_END_ALLOW_THREADS
  
  state = PyThreadState_Get();
  
  CPC_LOG( CPC_LOG_LEVEL_ERROR, "Reacquired GIL: 0x%x.", state->interp );
}

CPC_BOOL
python_cahal_stop_recording( void )
{
  CPC_BOOL return_value = CPC_FALSE;
  PyThreadState *state  = PyThreadState_Get();
  
  CPC_LOG( CPC_LOG_LEVEL_DEBUG, "Releasing GIL: 0x%x.", state->interp );
  
  Py_BEGIN_ALLOW_THREADS
  
  CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Stopping recording..." );
  
  return_value = cahal_stop_recording();
  
  CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Recording stopped." );
  
  CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Reaquiring GIL." );
  
  Py_END_ALLOW_THREADS
  
  state = PyThreadState_Get();
  
  CPC_LOG( CPC_LOG_LEVEL_DEBUG, "Reacquired GIL: 0x%x.", state->interp );
  
  return( return_value );
}

CPC_BOOL
python_cahal_stop_playback( void )
{
  CPC_BOOL return_value = CPC_FALSE;
  PyThreadState *state  = PyThreadState_Get();
  
  CPC_LOG( CPC_LOG_LEVEL_DEBUG, "Releasing GIL: 0x%x.", state->interp );
  
  Py_BEGIN_ALLOW_THREADS
  
  CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Stopping playback..." );
  
  return_value = cahal_stop_playback();
  
  CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Playback stopped." );
  
  CPC_LOG_STRING( CPC_LOG_LEVEL_DEBUG, "Reaquiring GIL." );
  
  Py_END_ALLOW_THREADS
  
  state = PyThreadState_Get();
  
  CPC_LOG( CPC_LOG_LEVEL_DEBUG, "Reacquired GIL: 0x%x.", state->interp );

  return( return_value );
}

void
python_cahal_initialize( void )
{
  Py_Initialize();
  PyEval_InitThreads();
  
  cahal_initialize();
}

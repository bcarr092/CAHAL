%module cahal_tests

%inline %{

#include <cahal.h>

cahal_device*
cahal_device_list_get (
                        cahal_device** device_list,
                        int index
                      )
{
  return( device_list[ index ] );
}

cahal_device_stream*
cahal_device_stream_list_get  (
                                cahal_device_stream** stream_list,
                                int index
                              )
{
  return( stream_list[ index ] );
}

cahal_audio_format_description*
cahal_audio_format_description_list_get (
                            cahal_audio_format_description** description_list,
                            int index
                                        )
{
  return( description_list[ index ] );
}

cahal_sample_rate_range*
cahal_sample_rate_range_list_get  (
                                    cahal_sample_rate_range**
                                      sample_rate_range_list,
                                    int index
)
{
  return( sample_rate_range_list[ index ] );
}

double* cahal_float_array_get( FLOAT64** array, int index )
{
  return( array[ index ] );
}

%}

%apply char       { CHAR  }
%apply char       { INT8  }
%apply short      { INT16 }
%apply int        { INT32 }
%apply long long  { INT64 }
%apply float      { FLOAT32 }
%apply double     { FLOAT64 }

%apply unsigned char      { UCHAR   }
%apply unsigned char      { UINT8   }
%apply unsigned short     { UINT16  }
%apply unsigned int       { UINT32  }
%apply unsigned long long { UINT64  }

%apply size_t { SIZE  }

%apply char *       { CHAR *  }
%apply char *       { INT8 *  }
%apply short *      { INT16 * }
%apply int *        { INT32 * }
%apply long long *  { INT64 * }
%apply float *      { FLOAT32 * }
%apply double *     { FLOAT64 * }

%apply unsigned char *      { UCHAR *   }
%apply unsigned char *      { UINT8 *   }
%apply unsigned short *     { UINT16 *  }
%apply unsigned int *       { UINT32 *  }
%apply unsigned long long * { UINT64 *  }

%apply size_t * { SIZE *  }

%include <cahal.h>
%include <cahal_audio_format_description.h>
%include <cahal_device.h>
%include <cahal_device_stream.h>

%include <types.h>
%include <error_codes.h>
%include <log_definitions.h>
%include <log_functions.h>

%include <cpointer.i>
%pointer_functions( double, doubleP )

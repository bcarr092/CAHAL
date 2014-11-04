%module cahal_tests

%inline %{

#include <cahal.h>

cahal_device* cahal_device_list_get( cahal_device** device_list, int index )
{
  return( device_list[ index ] );
}

%}

%apply char      { CHAR  }
%apply char      { INT8  }
%apply short     { INT16 }
%apply int       { INT32 }
%apply long long { INT64 }

%apply unsigned char      { UCHAR   }
%apply unsigned char      { UINT8   }
%apply unsigned short     { UINT16  }
%apply unsigned int       { UINT32  }
%apply unsigned long long { UINT64  }

%apply size_t { SIZE  }

%apply char *      { CHAR *  }
%apply char *      { INT8 *  }
%apply short *     { INT16 * }
%apply int *       { INT32 * }
%apply long long * { INT64 * }

%apply unsigned char *      { UCHAR *   }
%apply unsigned char *      { UINT8 *   }
%apply unsigned short *     { UINT16 *  }
%apply unsigned int *       { UINT32 *  }
%apply unsigned long long * { UINT64 *  }

%apply size_t * { SIZE *  }

%include <cahal.h>
%include <cahal_device.h>

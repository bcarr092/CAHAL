%module cahal_tests

%import <cpcommon.h>

%apply unsigned short { UINT16 }
%apply char * { CHAR * }

%{
#include <cahal.h>
%}

%include <cahal.h>

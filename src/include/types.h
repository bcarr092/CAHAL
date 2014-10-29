// This is the cross-platform and cross-architecture standard types file. This
// header is automatically included by CAHAL.h.

#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef MACOSX
  typedef int8_t    INT8;
  typedef int16_t   INT16;
  typedef int32_t   INT32;
  typedef int64_t   INT64;
  typedef uint8_t   UNIT8;
  typedef uint16_t  UINT16;
  typedef uint32_t  UINT32;
  typedef uint64_t  UINT64;

  typedef char          CHAR;
  typedef unsigned char UCHAR;
#endif

#endif /* _TYPES_H_ */
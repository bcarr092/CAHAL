// This is the only header that an external caller should have to include into
// their project.

#ifndef _CAHAL_H_
#define _CAHAL_H_

#include <cpcommon.h>

#include "cahal_device.h"

enum cahal_error_codes
{
  CAHAL_ERROR_NO_ERROR  = 0
};

/**
 *  Returns the version of the library in a short. The top 8 MSBs are the major
 *  version and the 8 LSBs are the minor version.
 *
 *  @return A short containing the major and minor version (8 MSBs are major,
 *          8 LSBs are minor)
 */
UINT16 get_version( void );

/**
 *  Returns a string containing the library name followed by the version of the
 *  library.
 *
 *  @return A signed character array containing a string representation of the
 *          library with version. The caller must free the version string.
 */
CHAR* get_version_string( void );

cahal_device**
get_device_list( void );

#endif /* _CAHAL_H_ */


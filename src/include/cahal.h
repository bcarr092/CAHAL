/*! \file   cahal.h
    \brief  This is the only header that an external library/binary should have 
            include. Provides a cross-platform abstraction layer to all the
            audio input/output hardware supported by the platform this library
            is run on.
 */
#ifndef _CAHAL_H_
#define _CAHAL_H_

#include <cpcommon.h>

#include "cahal_device.h"

/*! \fn     UINT16 get_version( void )
    \brief  Returns the version of the library in a SHORT. The top 8 MSBs are
            the major version and the 8 LSBs are the minor version.
 
    \return A short containing the major and minor version (8 MSBs are major,
            8 LSBs are minor)
 */
UINT16
get_version( void );

/*! \fn     CHAR* get_version_string( void )
    \brief  Returns a string containing the library name followed by the version
            of the library.
 
    \return A signed character array containing a string representation of the
            library with version. The caller must free the version string.
 */
CHAR*
get_version_string( void );

/*! \fn     cahal_device** get_device_list( void )
    \brief
 
    \return
 */
cahal_device**
get_device_list( void );

#endif /* _CAHAL_H_ */


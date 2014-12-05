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
#include "cahal_device_stream.h"
#include "cahal_audio_format_flags.h"
#include "cahal_audio_format_description.h"

enum cahal_states
{
  CAHAL_STATE_NOT_INITIALIZED  = 0,
  CAHAL_STATE_INITIALIZED,
  CAHAL_STATE_TERMINATED
};

typedef UINT32 cahal_state;

extern cahal_state g_cahal_state;

/*! \fn     UINT16 get_version( void )
    \brief  Returns the version of the library in a SHORT. The top 8 MSBs are
            the major version and the 8 LSBs are the minor version.
 
    \return A short containing the major and minor version (8 MSBs are major,
            8 LSBs are minor)
 */
UINT16
cahal_get_version( void );

/*! \fn     CHAR* get_version_string( void )
    \brief  Returns a string containing the library name followed by the version
            of the library.
 
    \return A signed character array containing a string representation of the
            library with version. The caller must free the version string.
 */
CHAR*
cahal_get_version_string( void );

/*! \fn     void cahal_initialize( void )
    \brief  Entry point for the library. This must be the first function that
            external libraries call. If not, the system will not be 
            appropriately initialized.
 */
void
cahal_initialize( void );

void
cahal_terminate( void );

void
cahal_sleep (
    UINT32 in_sleep_time
            );

#endif /* _CAHAL_H_ */


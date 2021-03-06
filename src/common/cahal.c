/*! \file   cahal.c
 
    \author Brent Carrara
 */
#include "cahal.h"

cahal_state g_cahal_state = CAHAL_STATE_NOT_INITIALIZED;

UINT16 cahal_get_version( void )
{
  UINT16 version = ( ( MAJOR_VERSION & 0x000000FF ) << 8 )
                    | ( MINOR_VERSION & 0x000000FF );

  return( version );
}

CHAR* cahal_get_version_string( void )
{
  CHAR *version = NULL;
  
  cpc_snprintf  (
                 &version,
                 strlen( LIBRARY_NAME ) + 8,
                 "%s v%.2d.%.2d",
                 LIBRARY_NAME,
                 MAJOR_VERSION,
                 MINOR_VERSION
                );
  
  return( version );
}

#include "cahal.h"

#define MAJOR_VERSION 1U
#define MINOR_VERSION 0U

#define LIBRARY_NAME  "cahal"

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


void
cahal_sleep (
    UINT32 in_sleep_time
            )
{
  sleep( in_sleep_time );
}

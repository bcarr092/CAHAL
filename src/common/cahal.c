#include "cahal.h"

#define MAJOR_VERSION 1U
#define MINOR_VERSION 0U

#define LIBRARY_NAME  "cahal"

UINT16 getVersion( void )
{
  UINT16 version = ( ( MAJOR_VERSION & 0x000000FF ) << 8 )
                    | ( MINOR_VERSION & 0x000000FF );

  return( version );
}

CHAR* getVersionString( void )
{
  CHAR *version = NULL;
  
  cpc_snprintf  (
                 &version,
                 strlen( LIBRARY_NAME ) + 8,
                 "%s  v%.2d.%.2d",
                 LIBRARY_NAME,
                 MAJOR_VERSION,
                 MINOR_VERSION
                );
  
  return( version );
}


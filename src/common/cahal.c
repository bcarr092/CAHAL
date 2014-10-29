#include <cahal.h>

#define MAJOR_VERSION 1U
#define MINOR_VERSION 0U

#define LIBRARY_NAME  "CAHAL"

UINT16 getVersion( void )
{
  UINT16 version = ( ( MAJOR_VERSION & 0x000000FF ) << 8 )
                    | ( MINOR_VERSION & 0x000000FF );

  return( version );
}

CHAR* getVersionString( void )
{
  CHAR* version =
    ( CHAR* ) malloc  ( sizeof( CHAR ) * ( strlen( LIBRARY_NAME ) + 8 ) );
  
  sprintf (
            version,
            "%s v%.2d.%.2d",
            LIBRARY_NAME,
            MAJOR_VERSION,
            MINOR_VERSION
          );
  
  return( version );
}
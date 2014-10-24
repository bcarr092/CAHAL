#include <cahal.h>

#define MAJOR_VERSION   1U
#define MINOR_VERSION   0U

#define LIBRARY_NAME    "CAHAL"

UINT16 getVersion( void )
{
    UINT16 version = (
                        ( MAJOR_VERSION & 0x000000FF ) << 8 )
                        | ( MINOR_VERSION & 0x000000FF );
    
    return( version );
}

CHAR* getVersionString( void )
{
    CHAR* version = ( CHAR* ) malloc( sizeof( UCHAR ) * 0x100 );
    
    sprintf( version, "%s v%d.%d", LIBRARY_NAME, MAJOR_VERSION, MINOR_VERSION );
    
    return( version );
}
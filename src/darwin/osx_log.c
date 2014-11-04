#include "darwin/osx_log.h"

/** This code is taken from "Learning Core Audio - pg. 82"
 */
void
  osx_print_code  (
                    CPC_LOG_LEVEL log_level,
                    OSStatus      code
                  )
{
  if( noErr != code )
  {
    char error_string[ 7 ] = { 0 };

    * ( UINT32 * ) ( error_string + 1 ) = CFSwapInt32HostToBig( code );
    if( isprint( error_string[ 1 ] ) && isprint( error_string[ 2 ] )
        && isprint( error_string[ 3 ] ) && isprint( error_string[ 4 ] ) )
    {
      error_string[ 0 ] = error_string[ 5 ] = '\'';
      error_string[ 6 ] = '\0';

      cpc_log( log_level, "Code: %s", error_string );
    }
  }
}


#include "darwin/osx_cahal_device.h"

OSStatus
osx_set_cahal_device_struct(
                            AudioObjectID  in_device_id,
                            cahal_device*  out_device
                            )
{
  OSStatus result = noErr;
  
  out_device->handle       = in_device_id;
  out_device->device_name  = NULL;
  
  result =
  osx_get_device_string_property (
                                  out_device->handle,
                                  kAudioObjectPropertyName,
                                  &out_device->device_name
                                  );
  
  if( ! result )
  {
    result =
    osx_get_device_string_property (
                                    out_device->handle,
                                    kAudioObjectPropertyModelName,
                                    &out_device->model
                                    );
    
    if( ! result )
    {
      result =
      osx_get_device_string_property (
                                      out_device->handle,
                                      kAudioObjectPropertyManufacturer,
                                      &out_device->manufacturer
                                      );
      
      if( ! result )
      {
        result =
        osx_get_device_string_property (
                                        out_device->handle,
                                        kAudioObjectPropertySerialNumber,
                                        &out_device->serial_number
                                        );
        
        if( ! result )
        {
          result =
          osx_get_device_string_property (
                                          out_device->handle,
                                          kAudioObjectPropertyFirmwareVersion,
                                          &out_device->version
                                          );
        }
      }
    }
  }

  return( result );
}

OSStatus
osx_get_device_string_property  (
                                 AudioObjectID               in_device_id,
                                 AudioObjectPropertySelector property,
                                 CHAR**                      out_device_property
                                 )
{
  CFStringRef device_property;
  
  UINT32 property_size = sizeof( CFStringRef );
  
  AudioObjectPropertyAddress property_address =
  {
    property,
    kAudioObjectPropertyScopeGlobal,
    kAudioObjectPropertyElementMaster
  };
  
  OSStatus result =
  AudioObjectGetPropertyData  (
                               in_device_id,
                               &property_address,
                               0,
                               NULL,
                               &property_size,
                               &device_property
                               );
  
  *out_device_property = NULL;
  
  if( result )
  {
    cpc_log (
             CPC_LOG_LEVEL_ERROR,
             "Error in AudioObjectGetPropertyDataSize: %d",
             result
             );
    
    darwin_print_code( CPC_LOG_LEVEL_ERROR, result );
  }
  else
  {
    *out_device_property =
    darwin_convert_cfstring_to_char_string( device_property );
    
    if( NULL != device_property )
    {
      CFRelease( device_property );
    }
    
    if( NULL != *out_device_property )
    {
      cpc_log(
              CPC_LOG_LEVEL_DEBUG,
              "Device property 0x%x for device 0x%x is %s",
              property,
              in_device_id,
              *out_device_property
              );
    }
  }
  
  return( noErr );

}

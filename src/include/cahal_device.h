/*! \file     cahal_device.h
    \brief    File responsible for defining and manipulating the cahal_device
              struct.
 */
#ifndef __CAHAL_DEVICE_H__
#define __CAHAL_DEVICE_H__

#include <cpcommon.h>

#include "cahal_audio_format_description.h"
#include "cahal_device_stream.h"

/*! \var    cahal_device_handle
 \brief  Type definition for device handles
 */
typedef UINT32 cahal_device_handle;

/*! \var    cahal_device_t
    \brief  Struct definition for devices
 */
typedef struct cahal_device_t
{
  /*! \var    handle
      \brief  OS-specific handle for the device
   */
  cahal_device_handle   handle;
  
  /*! \var    device_name
      \brief  Printable name for the device
   */
  CHAR*               	device_name;
  
  /*! \var    model
      \brief  Printable model name of the device
   */
  CHAR*                 model;
  
  /*! \var    manufacturer
      \brief  Printable manufacturer name of the device
   */
  CHAR*                 manufacturer;
  
  /*! \var    serial_number
      \brief  Printable serial number of the device
   */
  CHAR*                 serial_number;
  
  /*! \var    version
      \brief  Printable version string for the device
   */
  CHAR*                 version;
  
  /*! \var    device_uid
      \brief  Printable universal ID for the device
   */
  CHAR*                 device_uid;
  
  /*! \var    model_uid
      \brief  Printable universal ID for the model
   */
  CHAR*                 model_uid;
  
  /*! \var    preferred_sample_rate
      \brief  The nominal sample rate for the device. This is the preferred
              the sample rate for the device.
   */
  FLOAT64               preferred_sample_rate;
  
  /*! \var    supported_sample_rates
      \brief  Array of supported sample rates in the form of
              cahal_sample_rate_ranges. This is a null-terminated list.
              This is a complete list of the sample rate ranges supported by the
              device.
   */
  cahal_sample_rate_range** supported_sample_rates;
  
  /*! \var    device_streams
      \brief  Array of streams supported by the device. This is a null-
              terminated list.
   */
  cahal_device_stream** device_streams;
  
  
  /*! \var    preferred_number_of_channels
      \brief  The preferred number of channels as defined by the audio hardware.
   */
  UINT32                preferred_number_of_channels;
  
  /*! \var    is_alive
      \brief  1 iff the device is ready to receive input/output. 0 otherwise.
   */
  UINT32                is_alive;
  
  /*! \var    is_running
      \brief  1 iff another process is actively using the device. 0 otherwise.
   */
  UINT32                is_running;
  
} cahal_device;

/*! \fn     void print_cahal_device  (
              cahal_device* in_device
            )
    \brief  Prints in_device using the cpcommon logger at the LOG level.
 
    \param  in_device The device to print.
 */
void
print_cahal_device  (
                     cahal_device* in_device
                     );

/*! \fn     void print_cahal_device_list (
              cahal_device** in_device_list
            )
    \brief  Prints all the devices in in__device_list
 
    \param  in_device_list  A null-terminated list of cahal_device structs that
                            is logged at the CPC_LOG_LEVEL_INFO level.
 */
void
print_cahal_device_list (
                         cahal_device** in_device_list
                         );

/*! \fn     void free_cahal_device_list (
              cahal_device** in_device_list
            )
    \brief  Frees the list of cahal_devices. This function will free all
            devices in the list as well as all members of those devices.
 
    \param  in_device_list  The device list to free
 */
void
free_cahal_device_list (
                        cahal_device** in_device_list
                        );

#endif  /*  _CAHAL_DEVICE_H__ */

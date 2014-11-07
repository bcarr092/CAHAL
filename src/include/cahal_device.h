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

/*! \var    cahal_device
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
      \brief  Array of supported sample rates. This is a null-terminated list.
              This is a complete list of the sample rates supported by the
              device.
   */
  FLOAT64**             supported_sample_rates;
  
  /*! \var    device_streams
      \brief  Array of streams supported by the device. This is a null-
              terminated list.
   */
  cahal_device_stream** device_streams;
  
  
  /*! \var    number_of_channels
      \brief  The number of channels that are supported by the device.
   */
  UINT32                number_of_channels;
  
  /*! \var    is_alive
      \brief  If 1, the device is ready to receive input/output. 0 otherwise.
   */
  UINT32                is_alive;
  
  /*! \var    is_running
      \brief  If 1, another process is actively using the device. 0 otherwise.
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

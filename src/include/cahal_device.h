/*! \file     cahal_device.h
    \brief    File responsible for defining and manipulating the cahal_device
              struct.
 */
#ifndef __CAHAL_DEVICE_H__
#define __CAHAL_DEVICE_H__

#include <cpcommon.h>

#define CAHAL_DEVICE_OUTPUT_STREAM  0
#define CAHAL_DEVICE_INPUT_STREAM   1

/*! \var    cahal_device_handle
    \brief  Type definition for device handles
 */
typedef UINT32 cahal_device_handle;

/*! \var    cahal_device_stream_handle
    \brief  Type definition for device stream handles
 */
typedef UINT32 cahal_device_stream_handle;

/*! \var    cahal_device_stream_direction
    \brief  Type definition for device stream directions.
 */
typedef UINT32 cahal_device_stream_direction;

/*! \var    cahal_device_stream
    \brief  Struct definition for device streams
 */
typedef struct cahal_device_stream_t
{
  /*! \var    handle
      \brief  OS-specific handle for the stream
   */
  cahal_device_stream_handle      handle;
  
  /*! \var    direction
      \brief  Direction of the stream. Set to either CAHAL_DEVICE_OUTPUT_STREAM
              or CAHAL_DEVICE_INPUT_STREAM.
   */
  cahal_device_stream_direction   direction;
  
} cahal_device_stream;

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

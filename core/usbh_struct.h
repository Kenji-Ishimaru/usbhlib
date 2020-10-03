/*=======================================================================
// USB Host Library
//
// File:
//   usbh_struct.h
//
// Title:
//   USB host struct definition
//
// Created:
//    30 April 2013
//
//======================================================================*/
//
// Copyright (c) 2013, Kenji Ishimaru
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//  -Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
//  -Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Revision History
// $Date: 2015-03-08 07:29:43 +0900 (Sun, 08 Mar 2015) $
// $Rev: 103 $
                  
#ifndef __USBH_STRUCT_H__
#define __USBH_STRUCT_H__

#include "usbh_config.h"

/* descriptor type */
typedef enum {
  UDT_DEVICE = 1,
  UDT_CONFIGURATION = 2,
  UDT_STRING = 3,
  UDT_INTERFACE = 4,
  UDT_ENDPOINT = 5,
  UDT_DEVICE_QUALIFIER = 6,
  UDT_OTHER_SPEED_CONFIGURATION = 7,
  UDT_INTERFACE_POWER = 8,
  UDT_HUB = 0x29,
  UDT_HID = 0x23
} en_usb_descriptor_type;

typedef enum {
  USF_DEVICE_REMOTE_WAKEUP = 1,
  UDF_ENDPOINT_HALT = 0,
  TEST_MODE = 2
} en_std_feature_selctor;

typedef enum {
  UDRT_STANDARD = 0,
  UDRT_CLASS = 1,
  UDRT_VENDOR = 2,
  UDRT_RESERVED = 3
} en_usb_device_request_type;

typedef enum {
  UDRR_DEVICE = 0,
  UDRR_INTERFACE = 1,
  UDRR_ENDPOINT = 2,
  UDRR_OTHER = 3
} en_usb_device_request_recipient;

typedef enum {
  UCC_HID = 0x03,
  UCC_MASS_STRAGE = 0x08,
  UCC_HUB = 0x09
} en_usb_class_code;

typedef enum {
  USBH_OK = 0x00,
  USBH_TIMEOUT = 0x08,
  USBH_NAK = 0x10,
  USBH_STALL = 0x20,
  USBH_ERROR = 0xff
} en_usb_status;

typedef enum {
  UEPT_CONTROL = 0x00,
  UEPT_ISOCHRONOUS = 0x01,
  UEPT_BULK = 0x02,
  UEPT_INTERRUPT = 0x03
} en_usb_endpoint_type;

typedef struct {
  union {
    unsigned char PACKED[18];
    struct {
      unsigned char bmRequestType;
      unsigned char bRequest;
      unsigned char wValue[2];
      unsigned char wIndex[2];
      unsigned char wLength[2];
    } BYTE;
  };
} st_usb_setup_data;

/* device descriptor */
typedef struct {
  union {
    unsigned char PACKED[18];
    struct {
      unsigned char bLength;
      unsigned char bDescriptor;
      unsigned char bcdUSB[2];
      unsigned char bDeviceClass;
      unsigned char bDeviceSubClass;
      unsigned char bDeviceProtocol;
      unsigned char bMaxPacketSize0;
      unsigned char idVendor[2];
      unsigned char idProduct[2];
      unsigned char bcdDevice[2];
      unsigned char iManufacturer;
      unsigned char iProduct;
      unsigned char iSerialNumber;
      unsigned char bNumConfigurations;
    } BYTE;
  };
} st_device_descriptor;

/* configuration descriptor */
typedef struct {
  union {
    unsigned char PACKED[9];
    struct {
      unsigned char bLength;
      unsigned char bDescriptor;
      unsigned char wTotalLength[2];
      unsigned char bNumInterfaces;
      unsigned char bConfigurationValue;
      unsigned char iConfiguration;
      unsigned char bmAttributes;
      unsigned char bMaxPower;
    } BYTE;
  };
} st_configuration_descriptor;

/* HID descriptor */
typedef struct {
  union {
    unsigned char PACKED[9];
    struct {
      unsigned char bLength;
      unsigned char bDescriptorType;
      unsigned char bcdHID[2];
      unsigned char bCountryCode;
      unsigned char bNumDescriptors;
      unsigned char bDescriptorTypeR;
      unsigned char wDescriptorLength[2];
    } BYTE;
  };
} st_hid_descriptor;

/* endpoint descriptor */
typedef struct {
  union {
    unsigned char PACKED[7];
    struct {
      unsigned char bLength;
      unsigned char bDescriptor;
      unsigned char bEndpointAddress;
      unsigned char bmAttributes;
      unsigned char wMaxPacketSize[2];
      unsigned char bInterval;
    } BYTE;
  };
} st_endpoint_descriptor;

/* interface descriptor */
typedef struct {
  union {
    unsigned char PACKED[9];
    struct {
      unsigned char bLength;
      unsigned char bDescriptorType;
      unsigned char bInterfaceNumber;
      unsigned char bAlernateSetting;
      unsigned char bNumEndpoints;
      unsigned char bInterfaceClass;
      unsigned char bInterfaceSubClass;
      unsigned char bInterfaceProtocol;
      unsigned char iInterface;
    } BYTE;
  };
#ifdef USBH_USE_HID
  st_hid_descriptor hid;
#endif
  st_endpoint_descriptor endp[USBH_MAX_ENDP]; /* endpoint */
} st_interface_descriptor;

/* HUB descriptor */
typedef struct {
  union {
    unsigned char PACKED[9];  /* bDescLength will know the actual value */
    struct {
      unsigned char bDescLength;
      unsigned char bDescriptorType;
      unsigned char bNbrPorts;
      unsigned char wHubCharacteristics[2];
      unsigned char bPwrOn2PwrGood;
      unsigned char bHubContrCurrent;
      unsigned char DeviceRemovable;
      unsigned char PortPwrCtrlMask;
    } BYTE;
  };
} st_usb_hub_descriptor;

typedef struct {
  union {
    unsigned char PACKED[4];
    struct {
      union {
        unsigned char BYTE[2];
        struct {
          unsigned char LocalPowerSource: 1;
          unsigned char OverCurrent: 1;
          unsigned char            : 6;
          unsigned char            : 8;
        } BIT;
      } wHubStatus;
      union {
        unsigned char BYTE[2];
        struct {
          unsigned char LocalPowerStatusChange: 1;
          unsigned char OverCurrentChange: 1;
          unsigned char            : 6;
          unsigned char            : 8;
        } BIT;
      } wHubChange;
    } BYTE;
  };
} st_hub_status;

typedef struct {
  union {
    unsigned char PACKED[4];
    struct {
      union {
        unsigned short WORD;
        struct {
          unsigned short CurrentConnectStatus: 1;
          unsigned short PortEnabled : 1;
          unsigned short Suspend     : 1;
          unsigned short OverCurrent : 1;
          unsigned short Reset : 1;
          unsigned short       : 3;
          unsigned short PortPower: 1;
          unsigned short LowSpeedDeviceAttached: 1;
          unsigned short HighSpeedDeviceAttached: 1;
          unsigned short PortTestMode: 1;
          unsigned short PortIndicatorControl: 1;
          unsigned short            : 3;
        } BIT;
      } wPortStatus;
      union {
        unsigned short WORD;
        struct {
          unsigned short ConnectStatusChange: 1;
          unsigned short PortEnableDisableChange: 1;
          unsigned short SuspendChange: 1;
          unsigned short OverCurrentIndicatorChange: 1;
          unsigned short ResetChange: 1;
          unsigned short            : 11;
        } BIT;
      } wPortChange;
    } WORD;
  };
} st_hub_port_status;

/* USB device descriptors */
typedef struct {
  st_device_descriptor device;
  st_configuration_descriptor config;
  st_interface_descriptor interface[USBH_MAX_INTERFACE];
#ifdef USBH_USE_HUB
  st_usb_hub_descriptor hub;
#endif
} st_usb_descriptors;

/*  device information    */
typedef struct {
  char out_data_type;      /* DATA0/DATA1 toggle */
  char ea_in;              /* endpoint address */
  char ea_in_max_packet_size;
  char ea_out;             /* endpoint address */
  char ea_out_max_packet_size;
} st_usb_endpoint_info;

typedef struct {
  char available;          /* available = 1 */
  char speed;              /* low:0 or full:1 */
  char preamble_enable;    /* preamble enable */
  unsigned char address;   /* default 0 */

  unsigned char cur_interface;   /* current interface number */
  st_usb_endpoint_info interface[USBH_MAX_INTERFACE];
} st_usb_device_info;

/* host information */
typedef struct {
  unsigned char speed;      /* low:0 or full:1 */
  unsigned char next_adrs;  /* next device address */
} st_usb_host_info;


/* device definition */
typedef struct _usb_device st_usb_device;
struct _usb_device {
  st_usb_descriptors descs;  /* set descriptor information */
  st_usb_device_info info;   /* other data */
#ifdef USBH_USE_HUB
  st_usb_device* p_devices[USBH_MAX_HUB_PORT]; /* device pointer */
#endif
#ifdef USBH_USE_HID
#ifdef USBH_USE_REPORT_DESCRIPTOR
 unsigned char rep_buf[USBH_HID_REPORT_DESC_MAX_BUF];
#endif

#ifdef USBH_USE_MOUSE
#ifndef USBH_USE_MOUSE_BOOT_PROTOCOL
  unsigned char mouse_bt_idx;
  unsigned char mouse_x_idx;
  unsigned char mouse_y_idx;
  unsigned char mouse_z_idx;
#endif
#endif
#ifdef USBH_USE_KEYBOARD
#ifndef USBH_USE_KEYBOARD_BOOT_PROTOCOL
  unsigned char keyboard_m_idx;
  unsigned char keyboard_k_idx;
#endif
#endif
#ifdef USBH_USE_GAMEPAD
#ifdef USBH_USE_REPORT_DESCRIPTOR
  unsigned char gamepad_bt_idx;
  unsigned char gamepad_x_idx;
  unsigned char gamepad_y_idx;
#endif
#endif
#endif
};

/* device detect lookup */
typedef struct {
  st_usb_device *p_dev;
  char inum;
} st_usb_dev_lookup;


#endif

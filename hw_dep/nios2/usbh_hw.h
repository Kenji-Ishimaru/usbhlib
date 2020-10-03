/*=======================================================================
// USB Host Library
//
// File:
//   usbh_hw.h
//
// Title:
//   OpenCores USB hostslave hardware dependent header
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
// $Date: 2014-09-11 10:46:17 +0900 (Thu, 11 Sep 2014) $
// $Rev: 87 $

#ifndef __USB_HW_H__
#define __USB_HW_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "usbh_hw_reg.h"
#include "usbh_struct.h"
#include "usbh_misc.h"

// host int status
typedef enum {
  IS_TRANS_DONE = 1,
  IS_RESUME = 2,
  IS_CONNECTION_EVENT = 4,
  IS_SOF_SENT = 8
} en_int_status;

// rx status
typedef enum {
  RS_CRC_ERROR = 0x01,
  RS_BIT_STUFF_ERROR = 0x02,
  RS_OVERFLOW = 0x04,
  RS_TIME_OUT = 0x08,
  RS_NAK_RXED = 0x10,
  RS_STALL_RXED = 0x20,
  RS_ACK_RXED = 0x40,
  RS_DATA_SEQUENCE = 0x80
} en_rx_status;

/* hardware dependent initialization */
void ul_hw_init();
/* hardware setup after the device detection */
void ul_hw_device_setup(int conn_num,st_usb_device *dev,int adrs);
/* OpenCores usbhostslave API */
void ul_host_config(int conn_num);
void ul_host_bus_reset(int conn_num);
int ul_speed_configuration(int conn_num);
en_usb_status ul_control_setup_transaction(int conn_num,st_usb_device *dev,st_usb_setup_data *sd);
en_usb_status ul_control_in_transaction(int conn_num,st_usb_device *dev,int size, unsigned char *data);
en_usb_status ul_control_out_transaction(int conn_num,st_usb_device *dev,int size, unsigned char *data);
en_usb_status ul_control_ack_out_transaction(int conn_num,st_usb_device *dev);
en_usb_status ul_control_ack_in_transaction(int conn_num,st_usb_device *dev);
  /* interrupt transfer(one or more transactions) */
en_usb_status ul_interrupt_in_transfer(int conn_num,st_usb_device *dev,int *size,unsigned char *data);
en_usb_status ul_interrupt_out_transfer(int conn_num,st_usb_device *dev,int size,unsigned char *data);
  /* bulk transfer(one or more transactions) */
#ifdef USBH_USE_MSD
en_usb_status ul_bulk_out_transfer(int conn_num,st_usb_device *dev, int size, unsigned char* data);
en_usb_status ul_bulk_in_transfer(int conn_num,st_usb_device *dev, int size, unsigned char* data);
#endif
int wait_int_event(int conn_num,en_int_status s);
void full_speed_mode(int conn_num);
void hub_low_speed_mode(int conn_num);
#ifdef __cplusplus
}
#endif

#endif

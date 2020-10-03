/*=======================================================================
// USB Host Library
//
// File:
//   usbh_hub.h
//
// Title:
//   USB host hub header
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
// $Date: 2014-09-11 09:07:33 +0900 (Thu, 11 Sep 2014) $
// $Rev: 82 $
                  
#ifndef __USBH_HUB_H__
#define __USBH_HUB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "usbh_base.h"

/* USB connector struct */
typedef struct {
  st_usb_host_info host_info;
  st_usb_device *p_root;
} st_usb_connector;

typedef enum {
  HPF_PORT_CONNECTION = 0,
  HPF_PORT_ENABLE = 1,
  HPF_PORT_SUSPEND = 2,
  HPF_PORT_OVER_CURRENT = 3,
  HPF_PORT_RESET = 4,
  HPF_PORT_POWER = 8,
  HPF_PORT_LOW_SPEED = 9,
  HPF_C_PORT_CONNECTION = 16,
  HPF_C_PORT_ENABLE = 17,
  HPF_C_PORT_SUSPEND = 18,
  HPF_C_PORT_OVER_CURRENT = 19,
  HPF_C_PORT_RESET = 20,
  HPF_PORT_TEST = 21,
  HPF_PORT_INDICATOR = 22
} en_hub_class_port_feature_selctor;

/* HUB API */
en_usb_status um_get_hub_descriptor(int conn_num,st_usb_device *dev);
int um_get_hub_num_ports(st_usb_device *dev);
void um_show_hub_descriptor(st_usb_hub_descriptor *desc);

void um_set_port_feature(int conn_num,st_usb_device *dev,
                         en_hub_class_port_feature_selctor fs,int sel,int port);
void um_set_hub_feature(int conn_num,st_usb_device *dev,
                        en_hub_class_port_feature_selctor fs,int sel,int port);
void um_clear_port_feature(int conn_num,st_usb_device *dev,
                           en_hub_class_port_feature_selctor fs,int sel,int port);
  void um_get_port_status(int conn_num,st_usb_device *dev,int port,unsigned char *data);
en_usb_status um_hub_port_device_setup(int conn_num,st_usb_host_info *hi,st_usb_device *dev,int port);
void um_hub_port_device_delete(int conn_num,st_usb_host_info *hi,st_usb_device *dev,int port);
void um_show_port_status(st_usb_device *dev,int port,st_hub_port_status *data);
void um_hub_setup(int conn_num,st_usb_host_info *hi,st_usb_device *dev);
extern void um_add_hub_device(st_usb_device *dev, int port);

#ifdef __cplusplus
}
#endif

#endif

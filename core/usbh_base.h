/*=======================================================================
// USB Host Library
//
// File:
//   usbh_base.h
//
// Title:
//   USB host base header
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
// $Date: 2013-09-30 13:53:43 +0900 (Mon, 30 Sep 2013) $
// $Rev: 37 $

#ifndef __USBH_BASE_H__
#define __USBH_BASE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "usbh_config.h"
#include "usbh_struct.h"
#include "usbh_hw.h"
#include "usbh_timer.h"

/* external functions */
/* HW dependent */

/* misc */
void um_error(char*s);
unsigned char um_get_next_address(st_usb_host_info *hi);

/* hw independent API */
void um_device_setup(int conn_num,st_usb_host_info *hi,st_usb_device *dev);
int um_control_out_transfer(int conn_num,st_usb_device *dev,st_usb_setup_data *sd,unsigned char *data);
int um_control_in_transfer(int conn_num, st_usb_device *dev,st_usb_setup_data *sd,unsigned char *data);
/* standard device request */
void um_set_address(int conn_num,st_usb_device *dev,int adrs);
void um_set_configuration(int conn_num,st_usb_device *dev, int c);
void um_set_feature(int conn_num,st_usb_device *dev,en_std_feature_selctor fs);
en_usb_status um_clear_feature(int conn_num,st_usb_device *dev,en_std_feature_selctor fs,int data);
void um_get_std_descriptors(int conn_num,st_usb_device *dev);
en_usb_status um_get_std_descriptor(int conn_num,st_usb_device *dev,en_usb_descriptor_type kind);

int  um_get_config_num(st_usb_device *dev);
int  um_is_hub(st_usb_device *dev);
int  um_is_hid(st_usb_device *dev,int inum);
int  um_is_msd_bot(st_usb_device *dev);
int  um_is_hub_valid(st_usb_device *dev);
int  um_is_hid_valid(st_usb_device *dev,int inum);
int  um_is_msd_bot_valid(st_usb_device *dev);
void um_show_device_descriptor(st_device_descriptor *desc);
void um_show_configuration_descriptor(st_configuration_descriptor *desc);
void um_show_interface_descriptor(st_interface_descriptor *desc);
void um_show_endpoint_descriptor(st_endpoint_descriptor *desc);
void um_show_descriptor(st_usb_descriptors *descs,en_usb_descriptor_type kind);
void um_show_hid_descriptor(st_hid_descriptor *desc);


#ifdef __cplusplus
}
#endif

#endif

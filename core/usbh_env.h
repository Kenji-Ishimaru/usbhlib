/*=======================================================================
// USB Host Library
//
// File:
//   usbh_env.h
//
// Title:
//   USB host library header
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
// $Date: 2014-09-08 12:17:05 +0900 (Mon, 08 Sep 2014) $
// $Rev: 78 $

#ifndef __USBH_ENV_H__
#define __USBH_ENV_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "usbh_base.h"
#include "usbh_hid.h"
#include "usbh_hub.h"
#include "usbh_user_api.h"

typedef struct {
  st_usb_connector conn[USBH_NUM_OF_CONNS];
  st_usb_device usb_devices[USBH_NUM_OF_DEVICES];
  int num_of_devices;
#ifdef USBH_USE_HUB
  unsigned char hub_status;
#ifdef USBH_USE_HUB_CALLBACK
  void(*pf_hd_attached)(int conn_num, st_usb_device *dev, int port_num);
  void(*pf_hd_detached)(int conn_num, st_usb_device *dev, int port_num);
#endif
#endif
#ifdef USBH_USE_HID
#ifdef USBH_USE_MOUSE
  st_usb_dev_lookup mouse_lookups[USBH_NUM_OF_CONNS][USBH_LOOKUP_MAX_CNT];
  void(*pf_mouse)(int buttons, int x, int y, int z);
  void(*pf_mouse_raw)(int size, unsigned char *buf);
#endif
#ifdef USBH_USE_KEYBOARD
  st_usb_dev_lookup keyboard_lookups[USBH_NUM_OF_CONNS][USBH_LOOKUP_MAX_CNT];
  void(*pf_keyboard)(int key, int modifier);
  void(*pf_keyboard_raw)(int size, unsigned char *buf);
#endif
#ifdef USBH_USE_GAMEPAD
  st_usb_dev_lookup gamepad_lookups[USBH_NUM_OF_CONNS][USBH_LOOKUP_MAX_CNT];
  void(*pf_gamepad)(int buttons, int x, int y);
  void(*pf_gamepad_raw)(int size, unsigned char *buf);
#endif
#endif
#ifdef USBH_USE_MSD
  st_usb_dev_lookup msd_lookups[USBH_NUM_OF_CONNS][USBH_LOOKUP_MAX_CNT];
#endif
} st_usbh_env;

extern st_usbh_env usbh_env;

void um_init_device_info_struct(st_usb_device_info *info);
void um_init_usb_device_struct(st_usb_device *dev);
void um_init_lookups();
void uh_init();
#ifdef USBH_USE_HUB
void um_add_hub_device(st_usb_device *dev, int port);
void um_delete_hub_device(st_usb_device *dev, int port);
#endif

#ifdef __cplusplus
}
#endif

#endif

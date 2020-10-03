/*=======================================================================
// USB Host Library
//
// File:
//   usbh_user_api.h
//
// Title:
//   USB host library user API header
//
// Created:
//    30 April 2013
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
// $Date: 2014-09-08 12:18:19 +0900 (Mon, 08 Sep 2014) $
// $Rev: 80 $

#ifndef __USBH_USER_API_H__
#define __USBH_USER_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "usbh_base.h"
#include "usbh_hid.h"
#include "usbh_hub.h"

void um_check_hub_status(); /* update hub status */
void um_check_hub_status_core(int conn_num,st_usb_device *dev);
char um_get_ascii_code(unsigned int d, unsigned int m);

/* device lookup*/
void um_set_dev_lookup();
void um_set_dev_lookup_core(int conn_num,st_usb_device *dev);
#ifdef USBH_USE_HUB
void um_find_hub(int conn_num,st_usb_device *dev);
#ifdef USBH_USE_HUB_CALLBACK
void uh_hub_at_attach_func(void(*pf)(int conn_num, st_usb_device *dev, int port_num));
void uh_hub_dt_attach_func(void(*pf)(int conn_num, st_usb_device *dev, int port_num));
#endif
#endif

/*  User API */
#ifdef USBH_USE_HID
void um_check_hid_status(); /* update hid device values  */
/* Mouse API */
#ifdef USBH_USE_MOUSE
st_usb_device* uh_mouse_find(int conn_num,int ln);
int uh_mouse_available(int conn_num);
void uh_mouse_attach_func(void(*pf)(int buttons, int x, int y, int z));
void uh_mouse_attach_func_raw(void(*pf)(int size, unsigned char *buf));
#endif /*UM_USE_MOUSE*/

/* Keyboard API */
#ifdef USBH_USE_KEYBOARD
st_usb_device* uh_keyboard_find(int conn_num,int ln);
int uh_keyboard_available(int conn_num);
void uh_keyboard_attach_func(void(*pf)(int key, int modifier));
void uh_mouse_attach_func_raw(void(*pf)(int size, unsigned char *buf));
int uh_keyboard_getchar();
en_usb_status uh_keyboard_LED(unsigned char d);
#endif /*USBH_USE_KEYBOARD*/

/* Gamepad API*/
#ifdef USBH_USE_GAMEPAD
st_usb_device* uh_gamepad_find(int conn_num,int ln);
int uh_gamepad_available(int conn_num);
void uh_gamepad_attach_func(void(*pf)(int buttons, int x,int y));
void uh_mouse_attach_func_raw(void(*pf)(int size, unsigned char *buf));
#endif /*USBH_USE_GAMEPAD*/

#endif /*USBH_USE_HID*/

/* Mass-storage device API */
#ifdef USBH_USE_MSD
st_usb_device* uh_msd_find(unsigned char drv,unsigned char *cnn);
int uh_msd_available(unsigned char drv);
#endif /*USBH_USE_MSD*/

/* hub/hid status update */
void uh_update();

#ifdef __cplusplus
}
#endif

#endif

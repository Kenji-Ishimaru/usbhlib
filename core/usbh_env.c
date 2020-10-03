/*=======================================================================
// USB Host Library
//
// File:
//   usbh_env.c
//
// Title:
//   USB host library body
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
// $Date: 2014-09-08 12:16:31 +0900 (Mon, 08 Sep 2014) $
// $Rev: 77 $

#include "usbh_env.h"

/* usbh data instance */
st_usbh_env usbh_env;

void um_init_usb_device_struct(st_usb_device *dev) {
#ifdef USBH_USE_HUB
  int i;
  for (i=0;i<USBH_MAX_HUB_PORT;i++)
    dev->p_devices[i] = 0;
#endif
  um_init_device_info_struct(&dev->info);
}
void um_init_device_info_struct(st_usb_device_info *info) {
  int i;
  info->available = 0;
  info->speed = 0;
  info->preamble_enable = 0;
  info->address = 0;
  info->cur_interface = 0;
  for (i=0;i<USBH_MAX_INTERFACE;i++) {
    info->interface[i].out_data_type = 0;
    info->interface[i].ea_in = 0;
    info->interface[i].ea_in_max_packet_size = 0;
    info->interface[i].ea_out = 0;
    info->interface[i].ea_out_max_packet_size = 0;
  }
}

void um_init_lookups(){
  int i,conn_num;
  for (conn_num = USBH_START_CONN; conn_num < USBH_NUM_OF_CONNS; conn_num++) {
    for(i=0;i<USBH_LOOKUP_MAX_CNT;i++) {
#ifdef USBH_USE_HID
#ifdef USBH_USE_MOUSE
    usbh_env.mouse_lookups[conn_num][i].p_dev = NULL;
    usbh_env.mouse_lookups[conn_num][i].inum = 0;
#endif
#ifdef USBH_USE_KEYBOARD
    usbh_env.keyboard_lookups[conn_num][i].p_dev = NULL;
    usbh_env.keyboard_lookups[conn_num][i].inum = 0;
#endif
#ifdef USBH_USE_GAMEPAD
    usbh_env.gamepad_lookups[conn_num][i].p_dev = NULL;
    usbh_env.gamepad_lookups[conn_num][i].inum = 0;
#endif
#endif
#ifdef USBH_USE_MSD
    usbh_env.msd_lookups[conn_num][i].p_dev = NULL;
    usbh_env.msd_lookups[conn_num][i].inum = 0;
#endif
    }
  }
}

void uh_init() {
  int i,conn_num;
  int status;
  /* hardware-dependent initialization */
  ul_hw_init();
  /* usbh_env initialization */
  um_init_lookups();
#ifdef USBH_USE_HUB
  usbh_env.hub_status = 0;
#ifdef USBH_USE_HUB_CALLBACK
  usbh_env.pf_hd_attached = NULL;
  usbh_env.pf_hd_detached = NULL;
#endif
#endif
#ifdef USBH_USE_HID
#ifdef USBH_USE_MOUSE
  usbh_env.pf_mouse = NULL;
  usbh_env.pf_mouse_raw = NULL;
#endif
#ifdef USBH_USE_KEYBOARD
  usbh_env.pf_keyboard = NULL;
  usbh_env.pf_keyboard_raw = NULL;
#endif
#ifdef USBH_USE_GAMEPAD
  usbh_env.pf_gamepad = NULL;
  usbh_env.pf_gamepad_raw = NULL;
#endif
#endif
  usbh_env.num_of_devices = 0;
  for (i=0;i < USBH_NUM_OF_DEVICES;i++)
    um_init_usb_device_struct(&usbh_env.usb_devices[i]);
  /* USB connector loop */
  for (conn_num = USBH_START_CONN; conn_num < USBH_NUM_OF_CONNS; conn_num++) {
    usbh_env.conn[conn_num].host_info.next_adrs =  USBH_DEV_INITIAL_ADDRESS;
    usbh_env.conn[conn_num].p_root = NULL;      
    status = ul_speed_configuration(conn_num);  /* speed detection */
    if (status == USBH_ERROR) {
      INFOPRINT("\nconnector %d has no device\n",conn_num);
      continue; /* skip this connector */
    }
    usbh_env.conn[conn_num].host_info.speed = status;
    /* connector root device setup */   
    if (usbh_env.num_of_devices > USBH_NUM_OF_DEVICES)
      um_error("not enough usb device struct.");
    usbh_env.conn[conn_num].p_root =
      &usbh_env.usb_devices[usbh_env.num_of_devices++];
    um_get_std_descriptors(conn_num,usbh_env.conn[conn_num].p_root);

    if (um_is_hub(usbh_env.conn[conn_num].p_root)) {
      INFOPRINT("\nroot device is hub\n");
#ifdef USBH_USE_HUB
      /* show string descriptor */
      um_get_std_descriptor(conn_num,usbh_env.conn[conn_num].p_root,UDT_STRING);

      um_hub_setup(conn_num,
                     &usbh_env.conn[conn_num].host_info,
                     usbh_env.conn[conn_num].p_root);
#else
      um_error("HUD detected,please define USBH_USE_HUB");
#endif
    } else
    if (um_is_msd_bot(usbh_env.conn[conn_num].p_root)) {
      INFOPRINT("root device is msd\n");
#ifdef USBH_USE_MSD
      um_device_setup(conn_num,
                         &usbh_env.conn[conn_num].host_info,
                         usbh_env.conn[conn_num].p_root);
#else
      um_error("MSD detected,please define USBH_USE_MSD");
#endif
    } else
    if (um_is_hid(usbh_env.conn[conn_num].p_root,0)) { /* check interface0 */
      INFOPRINT("\nroot device is hid\n");
      um_device_setup(conn_num,
                        &usbh_env.conn[conn_num].host_info,
                        usbh_env.conn[conn_num].p_root);
    } else {
       um_error("the device is not supported.");
    }
  }  /* for (conn_num = USBH_START_CONN; conn_num < USBH_START_CONN+USBH_NUM_OF_CONNS; conn_num++) */
  INFOPRINT("\nUSB address and configuration set finished\n");
  um_set_dev_lookup();
}

#ifdef USBH_USE_HUB
void um_add_hub_device(st_usb_device *dev, int port) {
  int i;
  if (usbh_env.num_of_devices > USBH_NUM_OF_DEVICES)
    um_error("\nnot enough usb device struct.\n");
  for (i=0;i<USBH_NUM_OF_DEVICES;i++) {
    if (!usbh_env.usb_devices[i].info.available) {
      dev->p_devices[port] = &usbh_env.usb_devices[i];
      usbh_env.num_of_devices++;
      break;
    }
  }
}

void um_delete_hub_device(st_usb_device *dev, int port) {
  if (usbh_env.num_of_devices> 0) usbh_env.num_of_devices--;
  dev->p_devices[port] = NULL;
}
#endif

//=======================================================================
// USB Host Library
//
// File:
//   usbh_config.h
//
// Title:
//   USB host library configuration sample.
//     mouse only
// Created:
//    30 April 2013
//======================================================================
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
// $Date: 2014-09-17 16:00:36 +0900 (Wed, 17 Sep 2014) $
// $Rev: 97 $
                  
#ifndef __USBH_CONFIG_H__
#define __USBH_CONFIG_H__
#include "usbh_misc.h"
/* USB Env */
#define USBH_DEV_INITIAL_ADDRESS 1
#define USBH_START_CONN 0
#define USBH_NUM_OF_CONNS 1
#define USBH_NUM_OF_DEVICES 8
#define USBH_MAX_INTERFACE 4
#define USBH_MAX_ENDP 4
#define USBH_MAX_HUB_PORT 7

/* USB Device Class Select */
#define USBH_USE_HUB
#define USBH_USE_HID
//#define USBH_USE_MSD

/* Device Lookup Table Size */
#define USBH_LOOKUP_MAX_CNT 3

/* USB HUB */
#define USBH_HUB_REPORT_MAX_BUF 2

/* USB HID Select */
#define USBH_USE_MOUSE
//#define USBH_USE_KEYBOARD
//#define USBH_USE_GAMEPAD

/* USB HID Gamepad Configuration */
#define USBH_USE_REPORT_DESCRIPTOR
#define USBH_HID_REPORT_DESC_MAX_BUF 138
#define USBH_HID_REPORT_MAX_BUF 50
#define USBH_HID_SUPPORT_PS3_GC

/* USB HID Mouse Configuration */
#define USBH_USE_MOUSE_BOOT_PROTOCOL
#ifndef USBH_USE_MOUSE_BOOT_PROTOCOL
  #define USBH_USE_MOUSE_BTN_IDX 0
  #define USBH_USE_MOUSE_X_IDX 1
  #define USBH_USE_MOUSE_Y_IDX 2
  #define USBH_USE_MOUSE_Z_IDX 3
#endif

/* USB HID Keyboard Configuration */
#define USBH_USE_KEYBOARD_BOOT_PROTOCOL
#ifndef USBH_USE_KEYBOARD_BOOT_PROTOCOL
  #define USBH_USE_KEYBOARD_M_IDX 0
  #define USBH_USE_KEYBOARD_K_IDX 2
#endif

#define USBH_USE_GAMEPAD_BTN_IDX 6
#define USBH_USE_GAMEPAD_X_IDX 3
#define USBH_USE_GAMEPAD_Y_IDX 4

/* USB HID LANGUAGE */
#define USBH_HID_LANG_JAPANESE

/* Error, NAK trial count */
#define USBH_INTSTATUS_LOOP_CNT 50000
#define USBH_TRIAL_LOOP_CNT 500
#define USBH_BOT_TRIAL_LOOP_CNT 10

#ifndef NULL
#define NULL 0
#endif

/* Debug Print */
#ifdef DEBUG_PRINT
#include <stdio.h>
#define DBGPRINT   MPRINT
#else
#define DBGPRINT   1 ? (void)0 : (void)MPRINT
#endif

#define INFO_PRINT
#ifdef INFO_PRINT
#include <stdio.h>
#define INFOPRINT   MPRINT
#else
#define INFOPRINT   1 ? (void)0 : (void)MPRINT
#endif

#define ERROR_PRINT
#ifdef ERROR_PRINT
#include <stdio.h>
#define ERRORPRINT   MPRINT
#else
#define ERRORPRINT   1 ? (void)0 : (void)MPRINT
#endif

#define TIMEOUT_PRINT
#ifdef TIMEOUT_PRINT
#include <stdio.h>
#define TMPRINT   MPRINT
#else
#define TMPRINT   1 ? (void)0 : (void)MPRINT
#endif

#endif

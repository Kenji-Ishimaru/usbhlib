/*=======================================================================
// USB Host Library
//
// File:
//   usbh_hid.h
//
// Title:
//   USB host hid header
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
// $Date: 2013-10-28 19:19:43 +0900 (Mon, 28 Oct 2013) $
// $Rev: 55 $

#ifndef __USBH_HID_H__
#define __USBH_HID_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "usbh_base.h"

typedef enum {
  RDBT_MAIN = 0,
  RDBT_GLOBAL = 1,
  RDBT_LOCAL = 2,
  RDBT_RESERVED = 3
} en_report_descriptor_btype;

typedef enum {
  RDBT_MAIN_INPUT = 0x8,
  RDBT_MAIN_OUTPUT = 0x9,
  RDBT_MAIN_FEATURE = 0xb,
  RDBT_MAIN_COLLECTION = 0xa,
  RDBT_MAIN_END_COLLECTION = 0xc
} en_report_descriptor_main_items;

typedef enum {
  RDBT_GLOBAL_USAGE_PAGE = 0x0,
  RDBT_GLOBAL_LOGICAL_MIN = 0x1,
  RDBT_GLOBAL_LOGICAL_MAX = 0x2,
  RDBT_GLOBAL_PHYSICAL_MIN = 0x3,
  RDBT_GLOBAL_PHYSICAL_MAX = 0x4,
  RDBT_GLOBAL_UNIT_EXP = 0x5,
  RDBT_GLOBAL_UNIT = 0x6,
  RDBT_GLOBAL_REPORT_SIZE = 0x7,
  RDBT_GLOBAL_REPORT_ID = 0x8,
  RDBT_GLOBAL_REPORT_COUNT = 0x9,
  RDBT_GLOBAL_PUSH = 0xa,
  RDBT_GLOBAL_POP = 0xb
} en_report_descriptor_global_items;

typedef enum {
  RDBT_LOCAL_USAGE = 0x0,
  RDBT_LOCAL_USAGE_MIN = 0x1,
  RDBT_LOCAL_USAGE_MAX = 0x2,
  RDBT_LOCAL_DESIGNATOR_INDEX = 0x3,
  RDBT_LOCAL_DESIGNATOR_MIN = 0x4,
  RDBT_LOCAL_DESIGNATOR_MAX = 0x5,
  RDBT_LOCAL_STRING_INDEX = 0x7,
  RDBT_LOCAL_STRING_MIN = 0x8,
  RDBT_LOCAL_STRING_MAX = 0x9,
  RDBT_LOCAL_DELIMITER = 0xa
} en_report_descriptor_local_items;

typedef enum {
  RDBT_USAGE_PAGES_UNDEFINED = 0x00,
  RDBT_USAGE_PAGES_GENERIC_DESKTOP_CONTROLS = 0x01,
  RDBT_USAGE_PAGES_SIMULATION_CONTROLS = 0x02,
  RDBT_USAGE_PAGES_VR_CONTROLS = 0x03,
  RDBT_USAGE_PAGES_SPORTS_CONTROLS = 0x04,
  RDBT_USAGE_PAGES_GAME_CONTROLS = 0x05,
  RDBT_USAGE_PAGES_GENERIC_DEVICE_CONTROLS = 0x06,
  RDBT_USAGE_PAGES_KEYBOARD = 0x07,
  RDBT_USAGE_PAGES_LEDS = 0x08,
  RDBT_USAGE_PAGES_BUTTON = 0x09,
  RDBT_USAGE_PAGES_ORDINAL = 0x0a,
  RDBT_USAGE_PAGES_TELEPHONY = 0x0b,
  RDBT_USAGE_PAGES_CONSUMER = 0x0c,
  RDBT_USAGE_PAGES_DIGITIZER = 0x0d,
  RDBT_USAGE_PAGES_PID_PAGE = 0x0f,
  RDBT_USAGE_PAGES_UNICODE = 0x10,
  RDBT_USAGE_PAGES_ALPHANUMERIC_DISPLAY = 0x14,
  RDBT_USAGE_PAGES_MEDICAL_INSTRUMENTS = 0x40,
  RDBT_USAGE_PAGES_MONITOR_PAGES = 0x80,
  RDBT_USAGE_PAGES_POWER_PAGES = 0x84,
  RDBT_USAGE_PAGES_BAR_CODE_SCANNER_PAGE = 0x8c,
  RDBT_USAGE_PAGES_SCALE_PAGE = 0x8d,
  RDBT_USAGE_PAGES_MSR_DEVICES = 0x8e,
  RDBT_USAGE_PAGES_RESERVED_POINT_OF_SALE_PAGES = 0x8f,
  RDBT_USAGE_PAGES_CAMERA_CONTROL_PAGE = 0x90,
  RDBT_USAGE_PAGES_ARCADE_PAGE = 0x91
} en_report_descriptor_usage_pages;

typedef enum { /* used in Usage Page (Generic Desktop) */
  RDBT_GDP_UNDEFINED = 0x00,
  RDBT_GDP_POINTER = 0x01,
  RDBT_GDP_MOUSE = 0x02,
  RDBT_GDP_JOYSTICK = 0x04,
  RDBT_GDP_GAMEPAD = 0x05,
  RDBT_GDP_KEYBOARD = 0x06,
  RDBT_GDP_X = 0x30,
  RDBT_GDP_Y = 0x31,
  RDBT_GDP_Z = 0x32,
  RDBT_GDP_WHEEL = 0x38,
  RDBT_GDP_HAT_SWITCH = 0x39,
  RDBT_GDP_START = 0x3d,
  RDBT_GDP_SELECT = 0x3e
} en_report_descriptor_generic_desktop_page;

typedef enum { /* used in Usage Page (Game Controls) */
  RDBT_GCP_UNDEFINED = 0x00,
  RDBT_GCP_3DGAME_CONTROLLER = 0x01,
  RDBT_GCP_PINBALL_DEVICE = 0x02,
  RDBT_GCP_GUN_DEVICE_DEVICE = 0x03,
  RDBT_GCP_TURN_RIGHT_LEFT = 0x21,
  RDBT_GCP_PITCH_FORWARD_BACKWARD = 0x22,
  RDBT_GCP_ROLL_RIGHT_LEFT = 0x23,
  RDBT_GCP_MOVE_RIGHT_LEFT = 0x24,
  RDBT_GCP_MOVE_FORWARD_BACKWARD = 0x25,
  RDBT_GCP_MOVE_UP_DOWN = 0x26,
  RDBT_GCP_NEW_GAME = 0x2d
} en_report_descriptor_game_controls_page;

typedef enum { /* used in Usage Page (Button) */
  RDBT_BP_NO_BUTTON_PRESSED = 0x00,
  RDBT_BP_BUTTON1 = 0x01,
  RDBT_BP_BUTTON2 = 0x02,
  RDBT_BP_BUTTON3 = 0x03,
  RDBT_BP_BUTTON4 = 0x04
} en_report_descriptor_button_page;

typedef enum { /* used in Usage Page (Keyboard) */
  RDBT_KP_RESERVED = 0x00,
  RDBT_KP_a = 0x04,
  RDBT_KP_b,
  RDBT_KP_c,
  RDBT_KP_d,
  RDBT_KP_e,
  RDBT_KP_f,
  RDBT_KP_g,
  RDBT_KP_h,
  RDBT_KP_i,
  RDBT_KP_j,
  RDBT_KP_k,
  RDBT_KP_l,
  RDBT_KP_m,
  RDBT_KP_n,
  RDBT_KP_o,
  RDBT_KP_p,
  RDBT_KP_q,
  RDBT_KP_r,
  RDBT_KP_s,
  RDBT_KP_t,
  RDBT_KP_u,
  RDBT_KP_v,
  RDBT_KP_w,
  RDBT_KP_x,
  RDBT_KP_y,
  RDBT_KP_z,
  RDBT_KP_1, /* ! */
  RDBT_KP_2, /* @ */
  RDBT_KP_3, /* # */
  RDBT_KP_4, /* $ */
  RDBT_KP_5, /* % */
  RDBT_KP_6, /* ^ */
  RDBT_KP_7, /* & */
  RDBT_KP_8, /* * */
  RDBT_KP_9, /* ( */
  RDBT_KP_0, /* ) */
  RDBT_KP_RETURN,
  RDBT_KP_ESCAPE,
  RDBT_KP_DELETE,
  RDBT_KP_TAB,
  RDBT_KP_SPACEBAR,
  RDBT_KP_MIN, /* - and _ */
  RDBT_KP_EQ, /* = and + */
  RDBT_KP_BRL, /* [ and { */
  RDBT_KP_BRR, /* ] and } */
  RDBT_KP_BS, /* \ and | */
  RDBT_KP_BA, /* ` and ~ */
  RDBT_KP_SEMIC, /* ; and : */
  RDBT_KP_SQ, /* ' and " */
  RDBT_KP_GRAVE_ACCENT, /* ` and ~  */
  RDBT_KP_COMMA, /* , and < */
  RDBT_KP_DOT, /* . and > */
  RDBT_KP_SL, /* / and ? */
  RDBT_KP_CAPS_LOCK,
  RDBT_KP_F1,
  RDBT_KP_F2,
  RDBT_KP_F3,
  RDBT_KP_F4,
  RDBT_KP_F5,
  RDBT_KP_F6,
  RDBT_KP_F7,
  RDBT_KP_F8,
  RDBT_KP_F9,
  RDBT_KP_F10,
  RDBT_KP_F11,
  RDBT_KP_F12,
  RDBT_KP_PrintScreen,
  RDBT_KP_ScrollLock,
  RDBT_KP_Pause,
  RDBT_KP_Insert,
  RDBT_KP_Home,
  RDBT_KP_PageUp,
  RDBT_KP_Delete,
  RDBT_KP_End,
  RDBT_KP_PageDown,
  RDBT_KP_RightArrow,
  RDBT_KP_LeftArrow,
  RDBT_KP_DownArrow,
  RDBT_KP_UpArrow,
  RDBT_KP_KP_NumLock_Clear,
  RDBT_KP_KP_SL, /* / */
  RDBT_KP_KP_AST,  /* * */
  RDBT_KP_KP_MNUS, /* - */
  RDBT_KP_KP_PLUS, /* + */
  RDBT_KP_KP_ENTER,
  RDBT_KP_KP_1, /* End*/
  RDBT_KP_KP_2, /* Down Arrow */
  RDBT_KP_KP_3, /* PageDn */
  RDBT_KP_KP_4, /* Left Arrow */
  RDBT_KP_KP_5,
  RDBT_KP_KP_6, /* Right Arrow */
  RDBT_KP_KP_7, /* Home */
  RDBT_KP_KP_8, /* Up Arrow */
  RDBT_KP_KP_9, /* PageUp */
  RDBT_KP_KP_0, /* Insert */
  RDBT_KP_KP_DOT, /* . and Delete*/
  RDBT_KP_NU_BS, /* \ and | */  /* = 0x64 */
  RDBT_KP_AP, /* \ and | */  /* = 0x65 */
  RDBT_KP_LeftControl = 0xe0,
  RDBT_KP_LeftShift,
  RDBT_KP_LeftAlt,
  RDBT_KP_LeftGUI,
  RDBT_KP_RightControl,
  RDBT_KP_RightShift,
  RDBT_KP_RightAlt,
  RDBT_KP_RightGUI
} en_report_descriptor_keyboard_page;

/* report descriptor */
typedef union {
  unsigned char BYTE;
  struct { 
    unsigned char bSize : 2;
    unsigned char bType : 2;
    unsigned char bTag : 4;
 } BIT;
} un_report_descriptor_prefix;

typedef union {
  unsigned int LONG;
  unsigned char BYTE[4];
} un_btol;

#ifdef USBH_USE_HID
#ifdef USBH_USE_REPORT_DESCRIPTOR
en_usb_status um_get_report_descriptor(int conn_num,st_usb_device *dev);
char um_detect_byte_pos(st_usb_device *dev,
                       en_report_descriptor_generic_desktop_page usage,  /* mouse, pointer, keyboard,etc */
                       en_report_descriptor_usage_pages usage_pages);
void  um_show_report_descriptor(st_usb_device *dev);
void  um_analyze_report_descriptor(st_usb_device *dev);

en_report_descriptor_generic_desktop_page um_hid_type_query(st_usb_device *dev);

#ifdef USBH_HID_SUPPORT_PS3_GC
int um_is_ps3_gc(st_usb_device *dev);
en_usb_status um_ps3_enable(int conn_num,st_usb_device *dev);
en_usb_status um_set_report_ps3_enable(int conn_num,st_usb_device *dev);
/*en_usb_status um_set_report_ps3_led1_on(int conn_num,st_usb_device *dev);*/
#endif /* #ifdef USBH_HID_SUPPORT_PS3_GC*/
#endif /* #ifdef USBH_USE_REPORT_DESCRIPTOR*/
#endif /* #ifdef USBH_USE_HID */

en_usb_status um_set_report(int conn_num,st_usb_device *dev,unsigned char *data);
void um_set_protocol(int conn_num,st_usb_device *dev,int protocol);
void um_get_protocol(int conn_num,st_usb_device *dev,unsigned char *data);

#ifdef __cplusplus
}
#endif

#endif

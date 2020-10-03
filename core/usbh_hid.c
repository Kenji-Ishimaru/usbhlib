/*=======================================================================
// USB Host Library
//
// File:
//   usbh_hid.c
//
// Title:
//   USB host hid body
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
// $Date: 2013-10-29 11:08:15 +0900 (Tue, 29 Oct 2013) $
// $Rev: 58 $

#include "usbh_hid.h"
en_usb_status um_set_report(int conn_num,st_usb_device *dev,unsigned char *data) {
  st_usb_setup_data sd;
  sd.BYTE.bmRequestType = 0x21;
  sd.BYTE.bRequest      = 9; 
  sd.BYTE.wValue[0]     = 0;    /* Report ID */
  sd.BYTE.wValue[1]     = 0x02; /* Report Type */
  sd.BYTE.wIndex[0]     = 0;    /* Interface */
  sd.BYTE.wIndex[1]     = 0;
  sd.BYTE.wLength[0]    = 1;
  sd.BYTE.wLength[1]    = 0;
  return um_control_out_transfer(conn_num,dev,&sd,data);
}

#ifdef USBH_USE_HID
#ifdef USBH_USE_REPORT_DESCRIPTOR
#ifdef USBH_USE_GAMEPAD
#ifdef USBH_HID_SUPPORT_PS3_GC
int um_is_ps3_gc(st_usb_device *dev){
  if ((dev->descs.device.BYTE.idVendor[1]== 0x05)&&
      (dev->descs.device.BYTE.idVendor[0]== 0x4c)&&
      (dev->descs.device.BYTE.idProduct[1]== 0x02)&&
      (dev->descs.device.BYTE.idProduct[0]== 0x68)) return 1;
  else return 0;
}
en_usb_status um_ps3_enable(int conn_num,st_usb_device *dev) {
  dev->gamepad_x_idx = 0; /* index2,Right:0x20,Left:0x80 */
  dev->gamepad_y_idx = 1; /* index2,Up: 0x10, Down: 0x40 */
  dev->gamepad_bt_idx = 0x3 | 0x40;  /* index3, from bit4 */
  INFOPRINT("PS3 X,Y,Buttons setting\n");
  return um_set_report_ps3_enable(conn_num,dev);
}

en_usb_status um_set_report_ps3_enable(int conn_num,st_usb_device *dev) {
  st_usb_setup_data sd;
  unsigned char data[] = {0x42,0x0c,0x00,0x00};
  sd.BYTE.bmRequestType = 0x21;
  sd.BYTE.bRequest      = 9; 
  sd.BYTE.wValue[0]     = 0xf4;  /* Report ID */
  sd.BYTE.wValue[1]     = 0x03;  /* Report Type */
  sd.BYTE.wIndex[0]     = 0;     /* Interface */
  sd.BYTE.wIndex[1]     = 0;
  sd.BYTE.wLength[0]    = 4;
  sd.BYTE.wLength[1]    = 0;
  return um_control_out_transfer(conn_num,dev,&sd,data);
}
/*
en_usb_status um_set_report_ps3_led1_on(int conn_num,st_usb_device *dev) {
  st_usb_setup_data sd;
  unsigned char data[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x02, 0xff, 0x27, 0x10, 0x00, 0x32, 0xff,
                          0x27, 0x10, 0x00, 0x32, 0xff, 0x27, 0x10, 0x00,
                          0x32, 0xff, 0x27, 0x10, 0x00, 0x32, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                         };
  sd.BYTE.bmRequestType = 0x21;
  sd.BYTE.bRequest      = 9; 
  sd.BYTE.wValue[0]     = 1;
  sd.BYTE.wValue[1]     = 0x02;
  sd.BYTE.wIndex[0]     = 0;
  sd.BYTE.wIndex[1]     = 0;
  sd.BYTE.wLength[0]    = 0x30;
  sd.BYTE.wLength[1]    = 0;
  return um_control_out_transfer(conn_num,dev,&sd,data);
}
*/
#endif /* #ifdef USBH_HID_SUPPORT_PS3_GC*/
#endif /* #ifdef USBH_USE_GAMEPAD */

en_usb_status um_get_report_descriptor(int conn_num,st_usb_device *dev) {
  st_usb_setup_data sd;
  sd.BYTE.bmRequestType = 0x81; /* class descriptor */
  sd.BYTE.bRequest     = 6; 
  sd.BYTE.wValue[0] = 0;
  sd.BYTE.wValue[1] = 0x22; /* Input */
  sd.BYTE.wIndex[0] = 0;  /* interface number */
  sd.BYTE.wIndex[1] = 0;
  if (dev->descs.interface[0].hid.BYTE.wDescriptorLength[0]>USBH_HID_REPORT_DESC_MAX_BUF) {
    INFOPRINT("not enough buffer for getting HID report descriptor.\n USBH_HID_REPORT_DESC_MAX_BUF should greater than %d", dev->descs.interface[0].hid.BYTE.wDescriptorLength[0]);
    um_error("\n");
  }
  sd.BYTE.wLength[0] = dev->descs.interface[0].hid.BYTE.wDescriptorLength[0];  /* descriptor length */
  sd.BYTE.wLength[1] = 0;
  return um_control_in_transfer(conn_num,dev,&sd,dev->rep_buf);
}

/* Item parser P.15 */
en_report_descriptor_generic_desktop_page um_hid_type_query(st_usb_device *dev) {
  en_report_descriptor_generic_desktop_page result;
  result = RDBT_GDP_GAMEPAD;
  if (dev->descs.interface[0].BYTE.bInterfaceSubClass ==1) {
    /* boot interface subclass */
    if (dev->descs.interface[0].BYTE.bInterfaceProtocol == 1)
      result = RDBT_GDP_KEYBOARD;
    else
    if (dev->descs.interface[0].BYTE.bInterfaceProtocol == 2)
      result = RDBT_GDP_MOUSE;
  }
  return result;
}

char um_detect_byte_pos(st_usb_device *dev,
                         /* mouse, pointer, keyboard,etc */
                         en_report_descriptor_generic_desktop_page usage,
                         /* generic desktop, buttons,keycodes, etc. */
                         en_report_descriptor_usage_pages usage_pages) {
/*
// manage current byte position, which is increased by "input"
// return byte position
// main item tag: input
// current byte position <= 8
// current bit position
// current report size
// current report count
*/
  int i,j,usage_ptr;
  un_report_descriptor_prefix rdp;
  un_btol bl;
  int current_report_size = 0;
  int current_report_count = 0;
  int current_bit_pos = 0;
  int current_byte_pos = -1;
  int usage_min = 65535;
  int usage_max = -1;
  char result = -1;
  int current_bit_pos_in_byte = 0;
  en_report_descriptor_usage_pages current_usage_pages = RDBT_USAGE_PAGES_UNDEFINED;
  en_report_descriptor_generic_desktop_page usages[16];
  i = 0;
  usage_ptr = 0;
  for (j=0;j<16;j++) usages[j] = RDBT_GDP_UNDEFINED;
  while (i<dev->descs.interface[0].hid.BYTE.wDescriptorLength[0]) {
    rdp.BYTE = dev->rep_buf[i++];    /* prefix */
    for (j=0;j<4;j++)bl.BYTE[j] = 0; /* data clear */
    for (j=0;j<rdp.BIT.bSize;j++)    /* data */
      bl.BYTE[j] = dev->rep_buf[i++];
    if (rdp.BIT.bSize == 3)
      bl.BYTE[3] = dev->rep_buf[i++];
    if (rdp.BYTE == 0xfe) {
      INFOPRINT("\nlong item is not supported.\n");
    } else {
      /* short item */
      switch (rdp.BIT.bType) {
      case RDBT_MAIN:  /* P.28 */
        if (rdp.BIT.bTag == RDBT_MAIN_INPUT) {
          DBGPRINT("INPUT detected %x %d %d\n", usages[0],current_report_size,current_report_count);
          /* update byte position */
          for (j=0;j<current_report_count;j++) {
            current_bit_pos += current_report_size;
            current_bit_pos_in_byte = ((current_bit_pos-1)% 8)/*+1*/;
            current_byte_pos = current_bit_pos/8;
            DBGPRINT("report_cnt %d %d %d %d\n",j,current_byte_pos,current_bit_pos,current_bit_pos_in_byte);
            if ((current_usage_pages == usage_pages)&& (usages[j] == usage)) {
              DBGPRINT("HIT usage = %x byte_pos = %d\n",usage,current_byte_pos);
              if (current_bit_pos_in_byte ==7) result = current_byte_pos-1;
              else {
                result = current_byte_pos;
                result |= current_bit_pos_in_byte << 4;
              }
            }
          }
          for (j=0;j<16;j++) usages[j] = RDBT_GDP_UNDEFINED;
          usage_ptr = 0;
          usage_min = 65535;
          usage_max = -1;
        }
        if (rdp.BIT.bTag == RDBT_MAIN_COLLECTION) {
          usage_ptr = 0;
         }
         break;
      case RDBT_GLOBAL:
        if (rdp.BIT.bTag == RDBT_GLOBAL_USAGE_PAGE) {
          current_usage_pages = bl.LONG;
        }
        if (rdp.BIT.bTag == RDBT_GLOBAL_REPORT_SIZE) {
          current_report_size = bl.LONG;
        }
        if (rdp.BIT.bTag == RDBT_GLOBAL_REPORT_COUNT) {
          current_report_count = bl.LONG;
        }
        if (rdp.BIT.bTag == RDBT_GLOBAL_REPORT_ID) {
          current_bit_pos += 8;
        }
        break;
      case RDBT_LOCAL:
        if (rdp.BIT.bTag == RDBT_LOCAL_USAGE_MIN) {
          usage_min = bl.LONG;
          if (usage_min < usage_max) {
            for (j=0;j<=usage_max-usage_min;j++) usages[j] = usage_min++;
            usage_ptr = usage_max-usage_min;
            if (usage_ptr>=16) um_error("not enough usage buffer");
          }
        }
        if (rdp.BIT.bTag == RDBT_LOCAL_USAGE_MAX) {
          usage_max = bl.LONG;
          if (usage_min < usage_max) {
            for (j=0;j<=usage_max-usage_min;j++) usages[j] = usage_min++;
            usage_ptr = usage_max-usage_min;
            if (usage_ptr>=16) um_error("not enough usage buffer");
          }
        }
        if (rdp.BIT.bTag == RDBT_LOCAL_USAGE) {
          usages[usage_ptr++] = bl.LONG;
          if (usage_ptr>=16) um_error("not enough usage buffer");
        }
        break;
      }
    }
  }  /* while */

  return result;
}


void  um_show_report_descriptor(st_usb_device *dev) {
  int i,j;
  un_report_descriptor_prefix rdp;
  un_btol bl;
  i = 0;
  while (i<dev->descs.interface[0].hid.BYTE.wDescriptorLength[0]) {
    rdp.BYTE = dev->rep_buf[i++]; /* prefix */
    for (j=0;j<4;j++)bl.BYTE[j] = 0;   /* data clear */
    for (j=0;j<rdp.BIT.bSize;j++)      /* data */
      bl.BYTE[j] = dev->rep_buf[i++];
    if (rdp.BIT.bSize == 3) bl.BYTE[3] = dev->rep_buf[i++];

    if (rdp.BYTE == 0xfe) {
      INFOPRINT("long item is not supported.\n");
    } else {
      /* short item */
      switch (rdp.BIT.bType) {
      case RDBT_MAIN:  /* P.28 */
        if (rdp.BIT.bTag == RDBT_MAIN_INPUT) DBGPRINT("MAIN Input %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_MAIN_OUTPUT) DBGPRINT("MAIN Output %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_MAIN_FEATURE) DBGPRINT("MAIN Feature %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_MAIN_COLLECTION) DBGPRINT("MAIN Collection %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_MAIN_END_COLLECTION) DBGPRINT("MAIN End Collection %x %08x\n",rdp.BYTE,bl.LONG);
      break;
      case RDBT_GLOBAL:
        if (rdp.BIT.bTag == RDBT_GLOBAL_USAGE_PAGE) DBGPRINT("Global Usage Page %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_GLOBAL_LOGICAL_MIN) DBGPRINT("Global Logical Min %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_GLOBAL_LOGICAL_MAX) DBGPRINT("Global Logical Max %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_GLOBAL_PHYSICAL_MIN) DBGPRINT("Global Physical Min %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_GLOBAL_PHYSICAL_MAX) DBGPRINT("Global Physical Max %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_GLOBAL_UNIT_EXP) DBGPRINT("Global Unit Exponent %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_GLOBAL_UNIT) DBGPRINT("Global Unit %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_GLOBAL_REPORT_SIZE) DBGPRINT("Global Report Size %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_GLOBAL_REPORT_ID) DBGPRINT("Global Report ID %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_GLOBAL_REPORT_COUNT) DBGPRINT("Global Report Count %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_GLOBAL_PUSH) DBGPRINT("Global Push %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_GLOBAL_POP) DBGPRINT("Global Pop %x %08x\n",rdp.BYTE,bl.LONG);
      break;
      case RDBT_LOCAL:
        if (rdp.BIT.bTag == RDBT_LOCAL_USAGE) DBGPRINT("Local Usage %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_LOCAL_USAGE_MIN) DBGPRINT("Local Usage Min %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_LOCAL_USAGE_MAX) DBGPRINT("Local Usage Max %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_LOCAL_DESIGNATOR_INDEX) DBGPRINT("Local Designator Index %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_LOCAL_DESIGNATOR_MIN) DBGPRINT("Local Designator Min %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_LOCAL_DESIGNATOR_MAX) DBGPRINT("Local Designator Max %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_LOCAL_STRING_INDEX) DBGPRINT("Local String Index %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_LOCAL_STRING_MIN) DBGPRINT("Local String Min %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_LOCAL_STRING_MAX) DBGPRINT("Local String Max %x %08x\n",rdp.BYTE,bl.LONG);
        if (rdp.BIT.bTag == RDBT_LOCAL_DELIMITER) DBGPRINT("Local Delimiter %x %08x\n",rdp.BYTE,bl.LONG);
      break;
      case RDBT_RESERVED:
        DBGPRINT("Reserved Type detected\n");
      break;
      }
    }
  }  /* while */
}
void  um_analyze_report_descriptor(st_usb_device *dev){
#ifdef USBH_USE_GAMEPAD
  /* x index */
  dev->gamepad_x_idx = um_detect_byte_pos(dev,RDBT_GDP_X,RDBT_USAGE_PAGES_GENERIC_DESKTOP_CONTROLS);
  /* y index */
  dev->gamepad_y_idx = um_detect_byte_pos(dev,RDBT_GDP_Y,RDBT_USAGE_PAGES_GENERIC_DESKTOP_CONTROLS);
  /* button index */
  dev->gamepad_bt_idx = um_detect_byte_pos(dev,RDBT_GDP_POINTER,RDBT_USAGE_PAGES_BUTTON);/* detect primary/trigger button */
  INFOPRINT("HID report descriptor analysis X,Y,Buttons = %d %d %d\n",dev->gamepad_x_idx,dev->gamepad_y_idx,dev->gamepad_bt_idx&0xf);
#endif
}
#endif /*#ifdef USBH_USE_REPORT_DESCRIPTOR*/
#endif /* ifdef USBH_USE_HID */

void um_set_protocol(int conn_num,st_usb_device *dev,int protocol) {
  st_usb_setup_data sd;
  sd.BYTE.bmRequestType = 0x21;
  sd.BYTE.bRequest     = 0xb;  /* SET_PROTOCOL */
  sd.BYTE.wValue[0] = protocol;
  sd.BYTE.wValue[1] = 0;
  sd.BYTE.wIndex[0] = dev->info.cur_interface;
  sd.BYTE.wIndex[1] = 0;
  sd.BYTE.wLength[0] = 0;
  sd.BYTE.wLength[1] = 0;
  um_control_out_transfer(conn_num,dev,&sd,0);
}

void um_get_protocol(int conn_num,st_usb_device *dev,unsigned char *data) {
  st_usb_setup_data sd;
  sd.BYTE.bmRequestType = 0xa1;
  sd.BYTE.bRequest     = 3;  /* GET_PROTOCOL */
  sd.BYTE.wValue[0] = 0;
  sd.BYTE.wValue[1] = 0;
  sd.BYTE.wIndex[0] = dev->info.cur_interface;
  sd.BYTE.wIndex[1] = 0;
  sd.BYTE.wLength[0] = 1;
  sd.BYTE.wLength[1] = 0;
  um_control_in_transfer(conn_num,dev,&sd,data);
}


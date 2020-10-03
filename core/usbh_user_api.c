//=======================================================================
// USB Host Library
//
// File:
//   usbh_user_api.c
//
// Title:
//   USB host library user API body
//
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
// $Date: 2014-09-11 10:46:17 +0900 (Thu, 11 Sep 2014) $
// $Rev: 87 $

#include "usbh_env.h"

#ifdef USBH_USE_HUB
void um_check_hub_status() {
  int conn_num;
  /* hub detect */
  for (conn_num=USBH_START_CONN;conn_num<USBH_NUM_OF_CONNS;conn_num++) {
    if (um_is_hub_valid(usbh_env.conn[conn_num].p_root)) {
      um_check_hub_status_core(conn_num,usbh_env.conn[conn_num].p_root);
    }
  }
}

void um_check_hub_status_core(int conn_num,st_usb_device *dev) {
  int i,num,size;
  unsigned char data[USBH_HUB_REPORT_MAX_BUF];
  en_usb_status  status;
  st_hub_port_status ps;
  /* hub */
  status = ul_interrupt_in_transfer(conn_num,dev,&size,data);
  if (status != USBH_NAK) {
    usbh_env.hub_status = data[0];
    INFOPRINT("\nHUB status changed = %x, size = %d\n",data[0],size);
    for (i=1;i<=um_get_hub_num_ports(dev);i++) {
      num = 1 << i;
      if (data[0] & num) {
        um_get_port_status(conn_num,dev,i,ps.PACKED);
        um_show_port_status(dev,i,&ps);
        if(ps.WORD.wPortStatus.BIT.CurrentConnectStatus) {
          if (dev->p_devices[i] == NULL) {
            um_add_hub_device(dev,i);
            status = um_hub_port_device_setup(conn_num,&usbh_env.conn[conn_num].host_info,dev,i);
            if (status == USBH_ERROR) um_delete_hub_device(dev,i);
            else {
              INFOPRINT("hub port %d device detected\n",i);
#ifdef USBH_USE_HUB_CALLBACK
         if(usbh_env.pf_hd_attached != NULL)
           (*usbh_env.pf_hd_attached)(conn_num,dev,i);

#endif
	    }
          } else {
            if(!ps.WORD.wPortStatus.BIT.PortEnabled) {
              status = um_hub_port_device_setup(conn_num,&usbh_env.conn[conn_num].host_info,dev,i);
              if (status == USBH_ERROR) um_delete_hub_device(dev,i);
              else {
                INFOPRINT("hub port %d device port disabled, trying to enable\n",i);
#ifdef USBH_USE_HUB_CALLBACK
                if(usbh_env.pf_hd_attached != NULL)
                 (*usbh_env.pf_hd_attached)(conn_num,dev,i);
#endif
              }
            }
          }
        } else {
          if (dev->p_devices[i] != NULL) {
            /* device was detached */
            um_hub_port_device_delete(conn_num,&usbh_env.conn[conn_num].host_info,dev,i);
#ifdef USBH_USE_HUB_CALLBACK
            if(usbh_env.pf_hd_detached != NULL)
              (*usbh_env.pf_hd_detached)(conn_num,dev,i);
#endif
            um_delete_hub_device(dev,i);
            INFOPRINT("hub port %d device detached %d\n",i,usbh_env.num_of_devices);
          }
        }
      }
    }
    /* update device lookup table */
    um_set_dev_lookup();
  }
  for (i=1;i<=um_get_hub_num_ports(dev);i++) {
    if (um_is_hub_valid(dev->p_devices[i])) /* down stream port hub */
      um_check_hub_status_core(conn_num,dev->p_devices[i]);
  }
}

#ifdef USBH_USE_HUB_CALLBACK
void uh_hub_at_attach_func(void(*pf)(int conn_num, st_usb_device *dev,int port_num)) {
  usbh_env.pf_hd_attached = pf;
}
void uh_hub_dt_attach_func(void(*pf)(int conn_num, st_usb_device *dev,int port_num)) {
  usbh_env.pf_hd_detached = pf;
}
#endif
#endif /*USBH_USE_HUB*/

#ifdef USBH_USE_HID
void um_check_hid_status() {
  int i,j,size;
#ifdef USBH_USE_KEYBOARD
  char c;
#endif
  st_usb_device * dev;
  unsigned char buf[USBH_HID_REPORT_MAX_BUF];
  en_usb_status  status;
  for (i=USBH_START_CONN;i<USBH_NUM_OF_CONNS;i++) {
    /* mouse */
#ifdef USBH_USE_MOUSE
    for (j=0;j<USBH_LOOKUP_MAX_CNT;j++) {
      dev =  uh_mouse_find(i,j);
      if (dev != NULL) {
        status = ul_interrupt_in_transfer(i, dev,&size, buf);
        if (status != USBH_NAK)
#ifdef USBH_USE_MOUSE_BOOT_PROTOCOL
         if(usbh_env.pf_mouse != NULL)
           (*usbh_env.pf_mouse)(buf[0], buf[1], buf[2], buf[3]);
#else
#ifdef USBH_USE_REPORT_DESCRIPTOR
        /* rep*/
         if(usbh_env.pf_mouse != NULL)
           (*usbh_env.pf_mouse)(buf[0], buf[1], buf[2], buf[3]);
#else
         if(usbh_env.pf_mouse != NULL)
           (*usbh_env.pf_mouse)(buf[USBH_USE_MOUSE_BTN_IDX],
                                buf[USBH_USE_MOUSE_X_IDX],
                                buf[USBH_USE_MOUSE_Y_IDX],
                                buf[USBH_USE_MOUSE_Z_IDX]);
#endif /*#ifdef USBH_USE_REPORT_DESCRIPTOR*/
#endif /*#ifdef USBH_USE_MOUSE_BOOT_PROTOCOL */
      }
    }
#endif /*USBH_USE_MOUSE*/
    /* gamepad */
#ifdef USBH_USE_GAMEPAD
    for (j=0;j<USBH_LOOKUP_MAX_CNT;j++) {
      dev =  uh_gamepad_find(i,j);
      if (dev != NULL) {
        status = ul_interrupt_in_transfer(i, dev,&size, buf);
        if (status != USBH_NAK) {
          if (size > USBH_HID_REPORT_MAX_BUF) {
            INFOPRINT("not enough buffer for gamepad data.\n USBH_HID_REPORT_MAX_BUF should greater than %d", size);
            um_error("\n");
          }
#ifdef USBH_USE_REPORT_DESCRIPTOR
#ifdef USBH_HID_SUPPORT_PS3_GC
          if (um_is_ps3_gc(dev)) {
            /* modify the result */
            /* set Generic Desktop Controls X to index0 */
            buf[0] = 0x80;
            if (buf[2] & 0x80) buf[0] = 0x00;
            if (buf[2] & 0x20) buf[0] = 0xff;
            /* set Generic Desktop Controls Y to index0 */
            buf[1] = 0x80;
            if (buf[2] & 0x10) buf[1] = 0x00;
            if (buf[2] & 0x40) buf[1] = 0xff;
          }
#endif /* #ifdef USBH_HID_SUPPORT_PS3_GC */
          if(usbh_env.pf_gamepad != NULL)
            (*usbh_env.pf_gamepad)(((dev->gamepad_bt_idx>>4)==0) ?
                                    buf[dev->gamepad_bt_idx&0xf]:
                                    buf[dev->gamepad_bt_idx&0xf]>> (dev->gamepad_bt_idx>>4),
                                    buf[dev->gamepad_x_idx],
                                    buf[dev->gamepad_y_idx]);
#else
          if(usbh_env.pf_gamepad != NULL)
            (*usbh_env.pf_gamepad)(buf[USBH_USE_GAMEPAD_BTN_IDX],
                                   buf[USBH_USE_GAMEPAD_X_IDX],
                                   buf[USBH_USE_GAMEPAD_Y_IDX]);
#endif /*#ifdef USBH_USE_REPORT_DESCRIPTOR*/
        }
      }
    }
#endif /*USBH_USE_GAMEPAD*/
    /* keyboard */
#ifdef USBH_USE_KEYBOARD
    for (j=0;j<USBH_LOOKUP_MAX_CNT;j++) {
      dev =  uh_keyboard_find(i,j);
      if (dev != NULL) {
        status = ul_interrupt_in_transfer(i, dev,&size, buf);
        if (status != USBH_NAK) {
#ifdef USBH_USE_KEYBOARD_BOOT_PROTOCOL
          c = um_get_ascii_code(buf[2],buf[0]);
          if ((c != 0)|(buf[0] !=0))
            if(usbh_env.pf_keyboard != NULL)
             (*usbh_env.pf_keyboard)(c, buf[0]);
#else
#ifdef USBH_USE_REPORT_DESCRIPTOR
#else
            c = um_get_ascii_code(buf[USBH_USE_KEYBOARD_K_IDX],
                                buf[USBH_USE_KEYBOARD_M_IDX]);
          if ((c != 0)|(buf[USBH_USE_KEYBOARD_M_IDX] !=0))
          if(usbh_env.pf_keyboard != NULL)
              (*usbh_env.pf_keyboard)(c, buf[USBH_USE_KEYBOARD_M_IDX]);
#endif /*#ifdef USBH_USE_REPORT_DESCRIPTOR */
#endif /*#ifdef USBH_USE_KEYBOARD_BOOT_PROTOCOL*/
        }
      }
    }
#endif /*USBH_USE_KEYBOARD*/
  }
}

#endif /*USBH_USE_HID*/


void um_set_dev_lookup() {
  int conn_num,i;

  um_init_lookups();
  /* USB connector loop */
  for (conn_num = USBH_START_CONN; conn_num < USBH_NUM_OF_CONNS; conn_num++) {
    if (usbh_env.conn[conn_num].p_root->info.available) {
#ifdef USBH_USE_HUB
      if (um_is_hub(usbh_env.conn[conn_num].p_root)) {
        for (i=1;i<=um_get_hub_num_ports(usbh_env.conn[conn_num].p_root);i++) {
          if (um_is_hub(usbh_env.conn[conn_num].p_root->p_devices[i])) {
            um_find_hub(conn_num,usbh_env.conn[conn_num].p_root->p_devices[i]);
          } else {
            um_set_dev_lookup_core(conn_num,usbh_env.conn[conn_num].p_root->p_devices[i]);
          }
        }
      } else
#endif
      um_set_dev_lookup_core(conn_num,usbh_env.conn[conn_num].p_root);
    }
  }
}

void um_set_dev_lookup_core(int conn_num,st_usb_device *dev) {
  int i,j,f;
  for (i=0;i<dev->descs.config.BYTE.bNumInterfaces;i++) {
#ifdef USBH_USE_HID
#ifdef USBH_USE_MOUSE
    if (um_is_hid_valid(dev,i)&&
        dev->descs.interface[i].BYTE.bInterfaceProtocol==2) {
      f = 0;
      for(j=0;j<USBH_LOOKUP_MAX_CNT;j++) {
        if (usbh_env.mouse_lookups[conn_num][j].p_dev == NULL) {
            usbh_env.mouse_lookups[conn_num][j].p_dev = dev;
            usbh_env.mouse_lookups[conn_num][j].inum = i;
          f = 1;
          break;
        }
      }
      if (f==0) um_error("not enough space for mouse lookup");
    }
#endif
#ifdef USBH_USE_KEYBOARD
    if (um_is_hid_valid(dev,i)&&
        dev->descs.interface[i].BYTE.bInterfaceProtocol==1) {
      f = 0;
      for(j=0;j<USBH_LOOKUP_MAX_CNT;j++) {
        if (usbh_env.keyboard_lookups[conn_num][j].p_dev == NULL) {
          usbh_env.keyboard_lookups[conn_num][j].p_dev = dev;
          usbh_env.keyboard_lookups[conn_num][j].inum = i;
          f = 1;
          break;
        }
      }
      if (f==0) um_error("not enough space for keyboard lookup");
    }
#endif
#ifdef USBH_USE_GAMEPAD
    if (um_is_hid_valid(dev,i)&&
        dev->descs.interface[i].BYTE.bInterfaceProtocol==0) {
      f = 0;
      for(j=0;j<USBH_LOOKUP_MAX_CNT;j++) {
        if (usbh_env.gamepad_lookups[conn_num][j].p_dev == NULL) {
          usbh_env.gamepad_lookups[conn_num][j].p_dev = dev;
          usbh_env.gamepad_lookups[conn_num][j].inum = i;
          f = 1;
          break;
        }
      }
      if (f==0) um_error("not enough space for gamepad lookup");
    }
#endif
#endif
#ifdef USBH_USE_MSD
    if (um_is_msd_bot_valid(dev)) {
      f = 0;
      for(j=0;j<USBH_LOOKUP_MAX_CNT;j++) {
        if (usbh_env.msd_lookups[conn_num][j].p_dev == NULL) {
          usbh_env.msd_lookups[conn_num][j].p_dev = dev;
          usbh_env.msd_lookups[conn_num][j].inum = i;
          f = 1;
          break;
        }
      }
      if (f==0) um_error("not enough space for msd lookup");
    }
#endif
  }
}

#ifdef USBH_USE_HUB
void um_find_hub(int conn_num,st_usb_device *dev) {
  int i;
  for (i=1;i<=um_get_hub_num_ports(dev);i++) {
    if (um_is_hub(dev->p_devices[i])) {
      um_find_hub(conn_num,dev);
    } else {
      um_set_dev_lookup_core(conn_num,dev->p_devices[i]);
    }
  }
}
#endif

/* Mouse API */
#ifdef USBH_USE_MOUSE
st_usb_device* uh_mouse_find(int conn_num,int ln) {
  if (usbh_env.mouse_lookups[conn_num][ln].p_dev != NULL) {
    usbh_env.mouse_lookups[conn_num][ln].p_dev->info.cur_interface =
      usbh_env.mouse_lookups[conn_num][ln].inum;
    return usbh_env.mouse_lookups[conn_num][ln].p_dev;
  } else
    return NULL;
}

int uh_mouse_available(int conn_num) {
  int i;
  for (i=0;i<USBH_LOOKUP_MAX_CNT;i++) {
    if (uh_mouse_find(conn_num,i) != NULL) return 1;
  }
  return 0;
}
void uh_mouse_attach_func(void(*pf)(int buttons, int x, int y, int z)) {
  usbh_env.pf_mouse = pf;
}
void uh_mouse_attach_func_raw(void(*pf)(int size, unsigned char *buf)) {
  usbh_env.pf_mouse_raw = pf;
}
#endif /*#ifdef USBH_USE_MOUSE*/

/* Keyboard API */
#ifdef USBH_USE_KEYBOARD
st_usb_device* uh_keyboard_find(int conn_num,int ln) {
  if (usbh_env.keyboard_lookups[conn_num][ln].p_dev != NULL) {
      usbh_env.keyboard_lookups[conn_num][ln].p_dev->info.cur_interface =
      usbh_env.keyboard_lookups[conn_num][ln].inum;
    return usbh_env.keyboard_lookups[conn_num][ln].p_dev;
  } else
    return NULL;
}

int uh_keyboard_available(int conn_num) {
  int i;
  for (i=0;i<USBH_LOOKUP_MAX_CNT;i++) {
    if (uh_keyboard_find(conn_num,i) != NULL) return 1;
  }
  return 0;
}
void uh_keyboard_attach_func(void(*pf)(int key, int modifier)) {
  usbh_env.pf_keyboard = pf;
}
void uh_keyboard_attach_func_raw(void(*pf)(int size, unsigned char *buf)) {
  usbh_env.pf_keyboard_raw = pf;
}
int uh_keyboard_getchar() {
  int i,j,size;
  char c;
  st_usb_device * dev;
  unsigned char buf[8];
  en_usb_status  status;
  for (i=USBH_START_CONN;i<USBH_NUM_OF_CONNS;i++) {
    for (j=0;j<USBH_LOOKUP_MAX_CNT;j++) {
      dev =  uh_keyboard_find(i,j);
      if (dev != NULL) {
        status = ul_interrupt_in_transfer(1, dev,&size, buf);
        if (status != USBH_NAK) {
          c = um_get_ascii_code(buf[2],buf[0]);
          if (c != 0) return c;
        }
      }
    }
  }
  return 0;
}

en_usb_status uh_keyboard_LED(unsigned char d) {
  int i,j;
  st_usb_device * dev;
  for (i=USBH_START_CONN;i<USBH_NUM_OF_CONNS;i++) {
    for (j=0;j<USBH_LOOKUP_MAX_CNT;j++) {
      dev =  uh_keyboard_find(i,j);
      if (dev != NULL) {
        um_set_report(i,dev,&d);
        return USBH_OK;
      }
    }
  }
  return USBH_ERROR;
}

#define UK_LEFT_CTRL    0x1
#define UK_LEFT_SHIFT   0x2
#define UK_LEFT_ALT     0x4
#define UK_LEFT_GUI     0x8
#define UK_RIGHT_CTRL   0x10
#define UK_RIGHT_SHIFT  0x20
#define UK_RIGHT_ALT    0x40
#define UK_RIGHT_GUI    0x80

char um_get_ascii_code(unsigned int d, unsigned int m) {
/*
0 LEFT CTRL
1 LEFT SHIFT
2 LEFT ALT
3 LEFT GUI
4 RIGHT CTRL
5 RIGHT SHIFT
6 RIGHT ALT
7 RIGHT GUI
*/
  static char led = 0;
  static char keypad_numlock = 0;
  char c;
  char sk,skc;
  sk = 0;
  if (m & UK_LEFT_SHIFT) sk = 1;  /* left shift key */
  if (m & UK_RIGHT_SHIFT) sk = 1; /* right shift key */
  skc = sk ^ ((led & UK_LEFT_SHIFT)>> 1);
  if (d == RDBT_KP_KP_NumLock_Clear) {
    if (keypad_numlock) keypad_numlock = 0;
    else keypad_numlock = 1;
  }
  switch (d) {
  case RDBT_KP_RESERVED: c = 0;
    break;
  case RDBT_KP_1:
    if (sk) c = '!'; else c = '1';
    break;
  case RDBT_KP_2:
#ifdef USBH_HID_LANG_JAPANESE
    if (sk) c = '"'; else c = '2';
#else
    if (sk) c = '@'; else c = '2';
#endif
    break;
  case RDBT_KP_3:
    if (sk) c = '#'; else c = '3';
    break;
  case RDBT_KP_4:
    if (sk) c = '$'; else c = '4';
    break;
  case RDBT_KP_5:
    if (sk) c = '%'; else c = '5';
    break;
  case RDBT_KP_6:
#ifdef USBH_HID_LANG_JAPANESE
    if (sk) c = '&'; else c = '6';
#else
    if (sk) c = '^'; else c = '6';
#endif
    break;
  case RDBT_KP_7:
#ifdef USBH_HID_LANG_JAPANESE
        if (sk) c = '\''; else c = '7';
#else
        if (sk) c = '&'; else c = '7';
#endif
        break;
  case RDBT_KP_8:
#ifdef USBH_HID_LANG_JAPANESE
      if (sk) c = '('; else c = '8';
#else
      if (sk) c = '*'; else c = '8';
#endif
      break;
  case RDBT_KP_9:
#ifdef USBH_HID_LANG_JAPANESE
    if (sk) c = ')'; else c = '9';
#else
    if (sk) c = '('; else c = '9';
#endif
    break;
  case RDBT_KP_0:
#ifdef USBH_HID_LANG_JAPANESE
        if (sk) c = '*'; else c = '0';
#else
        if (sk) c = ')'; else c = '0';
#endif
        break;
  case RDBT_KP_RETURN:
    c = 0x0a;
    break;
  case RDBT_KP_ESCAPE:
    c = 0x1a;
    break;
  case RDBT_KP_DELETE:
    c = 0x7f;
    break;
  case RDBT_KP_TAB:
    c = 0x09;
    break;
  case RDBT_KP_SPACEBAR:
    c = ' ';
    break;
  case RDBT_KP_MIN:
#ifdef USBH_HID_LANG_JAPANESE
    if (sk) c = '='; else c = '-';
#else
    if (sk) c = '_'; else c = '-';
#endif
    break;
  case RDBT_KP_EQ:
#ifdef USBH_HID_LANG_JAPANESE
    if (sk) c = '~'; else c = '^';
#else
    if (sk) c = '+'; else c = '=';
#endif
    break;
  case RDBT_KP_BRL:
#ifdef USBH_HID_LANG_JAPANESE
    if (sk) c = '`'; else c = '@';
#else
    if (sk) c = '{'; else c = '[';
#endif
    break;
  case RDBT_KP_BRR:
#ifdef USBH_HID_LANG_JAPANESE
    if (sk) c = '{'; else c = '[';
#else
    if (sk) c = '}'; else c = ']';
#endif
    break;
  case RDBT_KP_BS:
   if (sk) c = '|'; else c = '\\';
   break;
  case RDBT_KP_BA:
#ifdef USBH_HID_LANG_JAPANESE
    if (sk) c = '}'; else c = ']';
#else
    if (sk) c = '~'; else c = '`';
#endif
    break;
  case RDBT_KP_SEMIC:
#ifdef USBH_HID_LANG_JAPANESE
    if (sk) c = '+'; else c = ';';
#else
    if (sk) c = ':'; else c = ';';
#endif
    break;
  case RDBT_KP_SQ:
#ifdef USBH_HID_LANG_JAPANESE
    if (sk) c = '*'; else c = ':';
#else
    if (sk) c = '"'; else c = '\'';
#endif
    break;
  case RDBT_KP_GRAVE_ACCENT:
    if (sk) c = '~'; else c = '`';
    break;
  case RDBT_KP_COMMA:
    if (sk) c = '<'; else c = ',';
    break;
  case RDBT_KP_DOT:
    if (sk) c = '>'; else c = '.';
    break;
  case RDBT_KP_SL:
    if (sk) c = '?'; else c = '/';
    break;
  /* LED */
  case RDBT_KP_KP_NumLock_Clear:
    led = led ^ 0x01;
    uh_keyboard_LED(led);
    c = 0;
    break;
  case RDBT_KP_CAPS_LOCK:
#ifdef USBH_HID_LANG_JAPANESE
    if (sk) {
      led = led ^ 0x02;
      uh_keyboard_LED(led);
    }
#else
    led = led ^ 0x02;
    uh_keyboard_LED(led);
#endif
    c = 0;
    break;
  case RDBT_KP_ScrollLock:
    led = led ^ 0x04;
    uh_keyboard_LED(led);
    c = 0;
    break;
  case RDBT_KP_AP:
    led = led ^ 0x08;
    uh_keyboard_LED(led);
    c = 0;
    break;
  case RDBT_KP_F1:
  case RDBT_KP_F2:
  case RDBT_KP_F3:
  case RDBT_KP_F4:
  case RDBT_KP_F5:
  case RDBT_KP_F6:
  case RDBT_KP_F7:
  case RDBT_KP_F8:
  case RDBT_KP_F9:
  case RDBT_KP_F10:
  case RDBT_KP_F11:
  case RDBT_KP_F12:
  case RDBT_KP_PrintScreen:
  case RDBT_KP_Pause:
  case RDBT_KP_Insert:
  case RDBT_KP_Home:
  case RDBT_KP_PageUp:
  case RDBT_KP_Delete:
  case RDBT_KP_End:
  case RDBT_KP_PageDown:
  case RDBT_KP_RightArrow:
  case RDBT_KP_LeftArrow:
  case RDBT_KP_DownArrow:
  case RDBT_KP_UpArrow:
  case RDBT_KP_KP_SL:
    c = '/';
    break;
  case RDBT_KP_KP_AST:
    c = '*';
    break;
  case RDBT_KP_KP_MNUS:
    c = '-';
    break;
  case RDBT_KP_KP_PLUS:
    c = '+';
    break;
  case RDBT_KP_KP_ENTER:
    c = 0x0d;
    break;
  case RDBT_KP_KP_1:
    if (keypad_numlock) c = '1'; else c = '1';
    break;
  case RDBT_KP_KP_2:
    if (keypad_numlock) c = '2'; else c = '2';
    break;
  case RDBT_KP_KP_3:
    if (keypad_numlock) c = '3'; else c = '3';
    break;
  case RDBT_KP_KP_4:
    if (keypad_numlock) c = '4'; else c = '4';
    break;
  case RDBT_KP_KP_5:
    if (keypad_numlock) c = '5'; else c = '5';
    break;
  case RDBT_KP_KP_6:
    if (keypad_numlock) c = '6'; else c = '6';
    break;
  case RDBT_KP_KP_7:
    if (keypad_numlock) c = '7'; else c = '7';
    break;
  case RDBT_KP_KP_8:
    if (keypad_numlock) c = '8'; else c = '8';
    break;
  case RDBT_KP_KP_9:
    if (keypad_numlock) c = '9'; else c = '9';
    break;
  case RDBT_KP_KP_0:
    if (keypad_numlock) c = '0'; else c = '0';
    break;
  case RDBT_KP_KP_DOT:
    if (keypad_numlock) c = '.'; else c = 0x7f; /* Del */
    break;
  case RDBT_KP_NU_BS:
    c = 0x08;
    break;
  case RDBT_KP_LeftControl:
  case RDBT_KP_LeftShift:
  case RDBT_KP_LeftAlt:
  case RDBT_KP_LeftGUI:
  case RDBT_KP_RightControl:
  case RDBT_KP_RightShift:
  case RDBT_KP_RightAlt:
  case RDBT_KP_RightGUI:
    c = 0;
    break;
  default:
    // a-z
    if (skc) c = (d-RDBT_KP_a) + 0x41;
    else  c = (d-RDBT_KP_a) + 0x61;
    break;
  }
  return c;
}

#endif /*#ifdef USBH_USE_KEYBOARD*/
/* Gamepad API*/
#ifdef USBH_USE_GAMEPAD
st_usb_device* uh_gamepad_find(int conn_num,int ln) {
  if (usbh_env.gamepad_lookups[conn_num][ln].p_dev != NULL) {
    usbh_env.gamepad_lookups[conn_num][ln].p_dev->info.cur_interface =
      usbh_env.gamepad_lookups[conn_num][ln].inum;
    return usbh_env.gamepad_lookups[conn_num][ln].p_dev;
  } else
    return NULL;
}

int uh_gamepad_available(int conn_num) {
  int i;
  for (i=0;i<USBH_LOOKUP_MAX_CNT;i++) {
    if (uh_gamepad_find(conn_num,i) != NULL) return 1;
  }
  return 0;
}
void uh_gamepad_attach_func(void(*pf)(int buttons, int x, int y)) {
  usbh_env.pf_gamepad = pf;
}
void uh_gamepad_attach_func_raw(void(*pf)(int size, unsigned char *buf)) {
  usbh_env.pf_gamepad_raw = pf;
}
#endif /*#ifdef USBH_USE_GAMEPAD*/

/* Mass-storage device API */
#ifdef USBH_USE_MSD
st_usb_device* uh_msd_find(unsigned char drv,unsigned char *cnn) {
  int i,conn_num,n=0;
  for (conn_num=USBH_START_CONN;conn_num<USBH_NUM_OF_CONNS;conn_num++) {
    *cnn = conn_num;
    if (um_is_msd_bot_valid(usbh_env.conn[conn_num].p_root)) {
      if (n == drv) return usbh_env.conn[conn_num].p_root;
      n++;
    } else if (um_is_hub(usbh_env.conn[conn_num].p_root)) {
      for (i=1;i<=um_get_hub_num_ports(usbh_env.conn[conn_num].p_root);i++) {
        if (um_is_msd_bot_valid(usbh_env.conn[conn_num].p_root->p_devices[i])) {
          if (n == drv) return usbh_env.conn[conn_num].p_root->p_devices[i];
          n++;
        }
      }
    }
  }
  return NULL;
}
int uh_msd_available(unsigned char drv) {
  int i,conn_num,n=0;
  for (conn_num=USBH_START_CONN;conn_num<USBH_NUM_OF_CONNS;conn_num++) {
    if (um_is_msd_bot_valid(usbh_env.conn[conn_num].p_root)) {
      if (n == drv) return 1;
      n++;
    } else if (um_is_hub(usbh_env.conn[conn_num].p_root)) {
      for (i=1;i<=um_get_hub_num_ports(usbh_env.conn[conn_num].p_root);i++) {
        if (um_is_msd_bot_valid(usbh_env.conn[conn_num].p_root->p_devices[i])) {
          if (n == drv) return 1;
          n++;
        }
      }
    }
  }
  return 0;
}
#endif /*#ifdef USBH_USE_MSD*/

void uh_update() {
#ifdef USBH_USE_HUB
  um_check_hub_status(); /* hub status update */
#endif
#ifdef USBH_USE_HID
  um_check_hid_status(); /* interrupt transfer if hid exists */
#endif
}

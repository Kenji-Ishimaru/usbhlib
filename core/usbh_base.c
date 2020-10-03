/*=======================================================================
// USB Host Library
//
// File:
//   usbh_base.c
//
// Title:
//   USB host base body
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
// $Date: 2015-03-08 07:29:43 +0900 (Sun, 08 Mar 2015) $
// $Rev: 103 $
                  
#include "usbh_base.h"
#ifdef USBH_USE_HID
#include "usbh_hid.h"
#endif

/******************************************
 misc API
 *****************************************/

void um_error(char *s) {
  ERRORPRINT(s);
  while(1);
}
unsigned char um_get_next_address(st_usb_host_info *hi) {
  unsigned char na;
  na = hi->next_adrs;
  if (hi->next_adrs >= 127) hi->next_adrs = 1; 
  else hi->next_adrs++;

  return na;
}

/******************************************
 HW independent API
 *****************************************/
void um_device_setup(int conn_num,st_usb_host_info *hi,st_usb_device *dev) {
  int i,j,adrs;
#ifdef USBH_USE_HID
  unsigned char pt;
#endif
  INFOPRINT("device setup ... \n");

  um_get_std_descriptors(conn_num,dev);
  /* show string descriptor */
  um_get_std_descriptor(conn_num,dev,UDT_STRING);

  for ( j = 0 ; j < dev->descs.config.BYTE.bNumInterfaces;j++) {
    for ( i = 0 ; i < dev->descs.interface[j].BYTE.bNumEndpoints;i++) {
#ifdef USBH_USE_MSD
      if (um_is_msd_bot(dev)) /* Bulk-Endpoint only */
        if ((dev->descs.interface[j].endp[i].BYTE.bmAttributes & 0x03) != UEPT_BULK) continue;
#endif
      if (dev->descs.interface[j].endp[i].BYTE.bEndpointAddress & 0x80) {
        /* IN Endpoint */
        dev->info.interface[j].ea_in = dev->descs.interface[j].endp[i].BYTE.bEndpointAddress & 0xf;
        dev->info.interface[j].ea_in_max_packet_size = dev->descs.interface[j].endp[i].BYTE.wMaxPacketSize[0];
      } else {
        /* OUT Endpoint */
        dev->info.interface[j].ea_out = dev->descs.interface[j].endp[i].BYTE.bEndpointAddress & 0xf;
        dev->info.interface[j].ea_out_max_packet_size = dev->descs.interface[j].endp[i].BYTE.wMaxPacketSize[0];
      }
#ifdef USBH_USE_MSD
      /* Use the first pair of Bulk-in and Bulk-out Endpoints only */
      if (um_is_msd_bot(dev))
        if ((dev->info.interface[j].ea_in != 0)&&(dev->info.interface[j].ea_out != 0)) break;
#endif
    }
  }
  /* device address */
  adrs = um_get_next_address(hi) ;
  /* hardware-dependent setup if needed */
  ul_hw_device_setup(conn_num,dev,adrs);
  /* configure the device */
  um_set_address(conn_num,dev,adrs);
  ul_timer(2);
  um_set_configuration(conn_num,dev,um_get_config_num(dev));
  dev->info.available = 1;
  INFOPRINT("device setup address: %d\n",adrs);
#ifdef USBH_USE_HID
  for ( i = 0 ; i < dev->descs.config.BYTE.bNumInterfaces;i++) {
    if (um_is_hid(dev,i)) {
      /* keyboard */
      if (dev->descs.interface[i].BYTE.bInterfaceProtocol == 1) {
        dev->info.cur_interface = i;
        um_get_protocol(conn_num,dev,&pt);
#ifdef USBH_USE_KEYBOARD_BOOT_PROTOCOL
        if (pt == 1) /* report protocol */
          um_set_protocol(conn_num,dev,0);
        INFOPRINT("boot protocol keyboard\n");
#else
        if (pt == 0) /* boot protocol */
          um_set_protocol(conn_num,dev,1);
        INFOPRINT("report protocol keyboard\n");
#endif
      }
      /* mouse */
      if (dev->descs.interface[i].BYTE.bInterfaceProtocol == 2) {
        dev->info.cur_interface = i;
        um_get_protocol(conn_num,dev,&pt);
#ifdef USBH_USE_MOUSE_BOOT_PROTOCOL
        if (pt == 1) /* report protocol */
          um_set_protocol(conn_num,dev,0);
        INFOPRINT("\nboot protocol mouse\n");
#else
        if (pt == 0) /* boot protocol */
          um_set_protocol(conn_num,dev,1);
        INFOPRINT("\nreport protocol mouse\n");
#endif
      }
    }
    /* gamepad*/
    if (dev->descs.interface[0].BYTE.bInterfaceProtocol == 0) {
#ifdef USBH_USE_REPORT_DESCRIPTOR
#ifdef USBH_USE_GAMEPAD
#ifdef USBH_HID_SUPPORT_PS3_GC
      if (um_is_ps3_gc(dev))
        um_ps3_enable(conn_num,dev);
      else
#endif /* #ifdef USBH_HID_SUPPORT_PS3_GC */
#endif /* #ifdef USBH_USE_GAMEPAD */
      {
        um_get_report_descriptor(conn_num,dev);
        /*um_show_report_descriptor(dev);*/
        um_analyze_report_descriptor(dev);
      }
#endif /* #ifdef USBH_USE_REPORT_DESCRIPTOR */
    }
  }
#endif /* #ifdef USBH_USE_HID*/
}

int um_control_out_transfer(int conn_num,
                              st_usb_device *dev,
                              st_usb_setup_data *sd,
                              unsigned char *data) {
  en_usb_status status;
  status = ul_control_setup_transaction(conn_num,dev,sd);
  if (status != USBH_OK) return status;
  if (sd->BYTE.wLength[0] != 0) {
    status = ul_control_out_transaction(conn_num,dev,sd->BYTE.wLength[0],data);
    if (status != USBH_OK) return status;
  }
  status = ul_control_ack_in_transaction(conn_num,dev);
  if (status != USBH_OK) return status;
  else return USBH_OK;
}

int um_control_in_transfer(int conn_num,
                             st_usb_device *dev,
                             st_usb_setup_data *sd,
                             unsigned char *data) {
  en_usb_status status;
  status = ul_control_setup_transaction(conn_num,dev,sd);
  if (status != USBH_OK) return status;
  status = ul_control_in_transaction(conn_num,dev,sd->BYTE.wLength[0],data);
  if (status != USBH_OK) return status;
  status = ul_control_ack_out_transaction(conn_num,dev);
  if (status != USBH_OK) return status;
  else return USBH_OK;
}

void um_set_address(int conn_num,st_usb_device *dev,int adrs) {
  st_usb_setup_data sd;
  sd.BYTE.bmRequestType = 0;
  sd.BYTE.bRequest     = 5;
  sd.BYTE.wValue[0] = adrs;  /* new address */
  sd.BYTE.wValue[1] = 0;
  sd.BYTE.wIndex[0] = 0;
  sd.BYTE.wIndex[1] = 0;
  sd.BYTE.wLength[0] = 0;
  sd.BYTE.wLength[1] = 0;
  um_control_out_transfer(conn_num,dev,&sd,0);  /* out data size = 0 */
  dev->info.address = adrs;  /* update address after control_out_transfer */
}
void um_set_configuration(int conn_num,st_usb_device *dev,int c) {
  st_usb_setup_data sd;
  sd.BYTE.bmRequestType = 0;
  sd.BYTE.bRequest     = 9;
  sd.BYTE.wValue[0] = c;   /* configuration */
  sd.BYTE.wValue[1] = 0;
  sd.BYTE.wIndex[0] = 0;
  sd.BYTE.wIndex[1] = 0;
  sd.BYTE.wLength[0] = 0;
  sd.BYTE.wLength[1] = 0;
  um_control_out_transfer(conn_num,dev,&sd,0);  /* out data size = 0 */
}
void um_set_feature(int conn_num,st_usb_device *dev,en_std_feature_selctor fs) {
  st_usb_setup_data sd;
  sd.BYTE.bmRequestType = 0;
  sd.BYTE.bRequest     = 3;  /* set feature */
  sd.BYTE.wValue[0] = fs;    /* feature selector */
  sd.BYTE.wValue[1] = 0;
  sd.BYTE.wIndex[0] = 0;
  sd.BYTE.wIndex[1] = 0;
  sd.BYTE.wLength[0] = 0;
  sd.BYTE.wLength[1] = 0;
  um_control_out_transfer(conn_num,dev,&sd,0);  /* out data size = 0 */
}


en_usb_status um_clear_feature(int conn_num,st_usb_device *dev,en_std_feature_selctor fs,int data) {
  st_usb_setup_data sd;
  if (fs == UDF_ENDPOINT_HALT)
    sd.BYTE.bmRequestType = 2;
  else
    sd.BYTE.bmRequestType = 0;
  sd.BYTE.bRequest     = 1; /* clear feature */
  sd.BYTE.wValue[0] = fs;   /* feature selector */
  sd.BYTE.wValue[1] = 0;
  sd.BYTE.wIndex[0] = data;
  sd.BYTE.wIndex[1] = 0;
  sd.BYTE.wLength[0] = 0;
  sd.BYTE.wLength[1] = 0;
  return um_control_out_transfer(conn_num,dev,&sd,0);  /* out data size = 0 */
}

void um_get_std_descriptors(int conn_num,st_usb_device *dev) {
  int i,j;
  ul_timer(100);
  DBGPRINT("\ngetting device descriptor ...\n");
  um_get_std_descriptor(conn_num,dev,UDT_DEVICE);
  um_show_device_descriptor(&dev->descs.device);
  DBGPRINT("\ngetting configuration descriptor ...\n");
  um_get_std_descriptor(conn_num,dev,UDT_CONFIGURATION);
  um_show_configuration_descriptor(&dev->descs.config);
  um_get_std_descriptor(conn_num,dev,UDT_INTERFACE);
  for (i=0;i<dev->descs.config.BYTE.bNumInterfaces;i++) {
    DBGPRINT("\ngetting interface descriptor %d...\n",i);
    um_show_interface_descriptor(&dev->descs.interface[i]);
#ifdef USBH_USE_HID
    if (dev->descs.interface[i].BYTE.bInterfaceClass == UCC_HID) {
      DBGPRINT("\ngetting hid descriptor %d...\n",i);
      um_get_std_descriptor(conn_num,dev,UDT_HID);
      um_show_hid_descriptor(&dev->descs.interface[i].hid);
    }
#endif
    DBGPRINT("\ngetting endpoint descriptor %d...\n",i);
    um_get_std_descriptor(conn_num,dev,UDT_ENDPOINT);
    for (j=0;j<dev->descs.interface[i].BYTE.bNumEndpoints;j++)
      um_show_endpoint_descriptor(&dev->descs.interface[i].endp[j]);
  }
}

int um_get_config_num(st_usb_device *dev) {
  return dev->descs.config.BYTE.bConfigurationValue;
}

int um_is_hub(st_usb_device *dev) {
  if (/*(dev->info.available) &&*/
     (dev->descs.device.BYTE.bDeviceClass == UCC_HUB))
    return 1;
  else
    return 0;
}

int um_is_hid(st_usb_device *dev,int inum) {
  if (/*(dev->info.available) &&*/
      (dev->descs.interface[inum].BYTE.bInterfaceClass == UCC_HID))
    return 1;
  else
    return 0;
}

int um_is_msd_bot(st_usb_device *dev) {
  if (/*(dev->info.available) &&*/
      (dev->descs.interface[0].BYTE.bInterfaceClass == UCC_MASS_STRAGE)&&
      (dev->descs.interface[0].BYTE.bInterfaceSubClass == 0x06)&&  /* SCSI transport command set */
      (dev->descs.interface[0].BYTE.bInterfaceProtocol == 0x50))
    return 1;
  else
    return 0;
}

int um_is_hub_valid(st_usb_device *dev) {
 if ((dev != NULL) && (dev->info.available) && um_is_hub(dev))
    return 1;
  else
    return 0;
}

int um_is_hid_valid(st_usb_device *dev,int inum) {
  if ((dev != NULL) && (dev->info.available) && um_is_hid(dev,inum))
    return 1;
  else
    return 0;
}

int um_is_msd_bot_valid(st_usb_device *dev) {
  if ((dev != NULL) && (dev->info.available) && um_is_msd_bot(dev))
    return 1;
  else
    return 0;
}
en_usb_status um_get_std_descriptor(int conn_num,st_usb_device *dev,en_usb_descriptor_type kind) {
  int i,j,k,size;
  st_usb_setup_data sd;
  en_usb_status status = USBH_OK;
#ifdef USBH_USE_HID
  unsigned char buf[9+USBH_MAX_INTERFACE*(9+9+USBH_MAX_ENDP*7)];
#else
  unsigned char buf[9+USBH_MAX_INTERFACE*(9+USBH_MAX_ENDP*7)];
#endif
  sd.BYTE.bmRequestType = 0x80;
  sd.BYTE.bRequest     = 6;
  sd.BYTE.wValue[0] = 0;      /* Descriptor Index */
  sd.BYTE.wValue[1] = kind;   /* Descriptor Type */
  sd.BYTE.wIndex[0] = 0;
  sd.BYTE.wIndex[1] = 0;
  sd.BYTE.wLength[0] = 0;
  sd.BYTE.wLength[1] = 0;
  if (kind == UDT_DEVICE) {
    size = 18;
    sd.BYTE.wLength[0] = size;
    status = um_control_in_transfer(conn_num,dev,&sd,dev->descs.device.PACKED);
  } else if (kind == UDT_STRING) {
    /* get Langage ID */
    sd.BYTE.wLength[0] = 4;
    /* iManufacturer string size */
    sd.BYTE.wValue[0] = 0;
    status = um_control_in_transfer(conn_num,dev,&sd,buf);
    sd.BYTE.wIndex[0]  = buf[2];  /* Language ID */
    sd.BYTE.wIndex[1]  = buf[3];
    if (dev->descs.device.BYTE.iManufacturer != 0) {
      sd.BYTE.wValue[0] = dev->descs.device.BYTE.iManufacturer;
      status = um_control_in_transfer(conn_num,dev,&sd,buf);
      sd.BYTE.wLength[0] = buf[0]; /* actual size */
      status = um_control_in_transfer(conn_num,dev,&sd,buf);
      INFOPRINT("iManufacturer: ");
      for (i=2;i<buf[0];i+=2) INFOPRINT("%c",buf[i]);  // UNICODE characters
      INFOPRINT("\n");
    } else
      INFOPRINT("iManufacturer: none\n");
    if (dev->descs.device.BYTE.iProduct != 0) {
      sd.BYTE.wLength[0] = 4;
      sd.BYTE.wValue[0] = dev->descs.device.BYTE.iProduct;
      status = um_control_in_transfer(conn_num,dev,&sd,buf);
      sd.BYTE.wLength[0] = buf[0]; /* actual size */
      status = um_control_in_transfer(conn_num,dev,&sd,buf);
      INFOPRINT("iProduct: ");
      for (i=2;i<buf[0];i+=2) INFOPRINT("%c",buf[i]);  // UNICODE characters
      INFOPRINT("\n");
    } else
      INFOPRINT("iProduct: none\n");
  } else if (kind ==  UDT_CONFIGURATION) {
    size = 9;
    sd.BYTE.wLength[0] = size;
    status = um_control_in_transfer(conn_num,dev,&sd,dev->descs.config.PACKED);
  } else if (kind ==  UDT_INTERFACE) {
    for (j=0;j<dev->descs.config.BYTE.bNumInterfaces;j++) {
      /* get interface descriptor only */
#ifdef USBH_USE_HID
      if (j==0) size = 9;
      else if (dev->descs.interface[j-1].BYTE.bInterfaceClass == UCC_HID)
        size = 9+j*(9+9+7*dev->descs.interface[j-1].BYTE.bNumEndpoints);
      else
          size = 9+j*(9+7*dev->descs.interface[j-1].BYTE.bNumEndpoints);
#else
      size = 9+j*(9+7*dev->descs.interface[j].BYTE.bNumEndpoints);
#endif
      sd.BYTE.wValue[1] = UDT_CONFIGURATION;   /* Descriptor Type */
      sd.BYTE.wLength[0] = size+9; /* configuration + interface */
      status = um_control_in_transfer(conn_num,dev,&sd,buf);
      for (i=0;i<9;i++) dev->descs.interface[j].PACKED[i] = buf[size+i];
      if (dev->descs.interface[j].BYTE.bInterfaceClass == UCC_HID) {
#ifdef USBH_USE_HID
        size = 9+9+j*(9+9+7*dev->descs.interface[j-1].BYTE.bNumEndpoints);
        sd.BYTE.wLength[0] = size+9;
        status = um_control_in_transfer(conn_num,dev,&sd,buf);
        /* get hid descriptor */
        for (i=0;i<9;i++) {
          dev->descs.interface[j].hid.PACKED[i] = buf[size+i];
        }
#else
        um_error("HID detected,please define USBH_USE_HID");
#endif
      }
    }
  } else if (kind ==  UDT_ENDPOINT) {
    /* config data is already available */
    for (k=0;k<dev->descs.config.BYTE.bNumInterfaces;k++) {
      if (dev->descs.interface[k].BYTE.bInterfaceClass == UCC_HID)
        size = 9+9+9+k*(9+9+7*dev->descs.interface[k-1].BYTE.bNumEndpoints);
      else
        size = 9+9+k*(9+7*dev->descs.interface[k-1].BYTE.bNumEndpoints);
      /* get_descriptor by config */
      sd.BYTE.wValue[1] = UDT_CONFIGURATION;   /* Descriptor Type */
      sd.BYTE.wLength[0] = size+7*dev->descs.interface[k].BYTE.bNumEndpoints;
      status = um_control_in_transfer(conn_num,dev,&sd,buf);
      for (j=0;j<dev->descs.interface[k].BYTE.bNumEndpoints;j++) {
        for (i=0;i<7;i++) {
          dev->descs.interface[k].endp[j].PACKED[i] = buf[size+j*7+i];
        }
      }
    }
  } else if (kind ==  UDT_HID) {
    /* already available, nothing to do */
    ;
  }
  return status;
}

void um_show_device_descriptor(st_device_descriptor *desc) {
  int i;
  for (i = 0; i<desc->BYTE.bLength; i++) {
    if (i == 0)  DBGPRINT("\nIN data %d = bLength            %x\n", i, desc->PACKED[i]);
    if (i == 1)  DBGPRINT("IN data %d = bDescriptor        %x\n", i, desc->PACKED[i]);
    if (i == 2)  DBGPRINT("IN data %d = bcdUSB0            %x\n", i, desc->PACKED[i]);
    if (i == 3)  DBGPRINT("IN data %d = bcdUSB1            %x\n", i, desc->PACKED[i]);
    if (i == 4)  DBGPRINT("IN data %d = bDeviceClass       %x\n", i, desc->PACKED[i]);
    if (i == 5)  DBGPRINT("IN data %d = bDeviceSubClass    %x\n", i, desc->PACKED[i]);
    if (i == 6)  DBGPRINT("IN data %d = bDeviceProtocol    %x\n", i, desc->PACKED[i]);
    if (i == 7)  DBGPRINT("IN data %d = bMaxPacketSize0    %x\n", i, desc->PACKED[i]);
    if (i == 8)  DBGPRINT("IN data %d = idVendor0          %x\n", i, desc->PACKED[i]);
    if (i == 9)  DBGPRINT("IN data %d = idVendor1          %x\n", i, desc->PACKED[i]);
    if (i == 10) DBGPRINT("IN data %d = idProduct0         %x\n", i, desc->PACKED[i]);
    if (i == 11) DBGPRINT("IN data %d = idProduct1         %x\n", i, desc->PACKED[i]);
    if (i == 12) DBGPRINT("IN data %d = bcdDevice0         %x\n", i, desc->PACKED[i]);
    if (i == 13) DBGPRINT("IN data %d = bcdDevice1         %x\n", i, desc->PACKED[i]);
    if (i == 14) DBGPRINT("IN data %d = iManufacturer      %x\n", i, desc->PACKED[i]);
    if (i == 15) DBGPRINT("IN data %d = iProduct           %x\n", i, desc->PACKED[i]);
    if (i == 16) DBGPRINT("IN data %d = iSerialNumber      %x\n", i, desc->PACKED[i]);
    if (i == 17) DBGPRINT("IN data %d = bNumConfigurations %x\n", i, desc->PACKED[i]);
  }
}

void um_show_configuration_descriptor(st_configuration_descriptor *desc) {
  int i;
  for (i = 0; i<desc->BYTE.bLength; i++) {
    if (i == 0) DBGPRINT("IN data %d = bLength             %x\r\n", i, desc->PACKED[i]);
    if (i == 1) DBGPRINT("IN data %d = bDescriptor         %x\r\n", i, desc->PACKED[i]);
    if (i == 2) DBGPRINT("IN data %d = wTotalLength0       %x\r\n", i, desc->PACKED[i]);
    if (i == 3) DBGPRINT("IN data %d = wTotalLength1       %x\r\n", i, desc->PACKED[i]);
    if (i == 4) DBGPRINT("IN data %d = bNumInterface       %x\r\n", i, desc->PACKED[i]);
    if (i == 5) DBGPRINT("IN data %d = bConfigurationValue %x\r\n", i, desc->PACKED[i]);
    if (i == 6) DBGPRINT("IN data %d = iConfiguration      %x\r\n", i, desc->PACKED[i]);
    if (i == 7) DBGPRINT("IN data %d = bmAttributes        %x\r\n", i, desc->PACKED[i]);
    if (i == 8) DBGPRINT("IN data %d = bMaxPower           %x\r\n", i, desc->PACKED[i]);
    if (i == 9) DBGPRINT("IN data %d =                     %x\r\n", i, desc->PACKED[i]);
  }
}

void um_show_interface_descriptor(st_interface_descriptor *desc) {
  int i;
 
  for (i = 0; i<desc->BYTE.bLength; i++) {
    if (i == 0) DBGPRINT("IN data %d = bLength             %x\r\n", i, desc->PACKED[i]);
    if (i == 1) DBGPRINT("IN data %d = bDescriptorType     %x\r\n", i, desc->PACKED[i]);
    if (i == 2) DBGPRINT("IN data %d = bInterfaceNumber    %x\r\n", i, desc->PACKED[i]);
    if (i == 3) DBGPRINT("IN data %d = bAttributeSetting   %x\r\n", i, desc->PACKED[i]);
    if (i == 4) DBGPRINT("IN data %d = bNumEndpoints       %x\r\n", i, desc->PACKED[i]);
    if (i == 5) DBGPRINT("IN data %d = bInterfaceClass     %x\r\n", i, desc->PACKED[i]);
    if (i == 6) DBGPRINT("IN data %d = bInterfaceSubClass  %x\r\n", i, desc->PACKED[i]);
    if (i == 7) DBGPRINT("IN data %d = bInterfaceProtocol  %x\r\n", i, desc->PACKED[i]);
    if (i == 8) DBGPRINT("IN data %d = iInterface          %x\r\n", i, desc->PACKED[i]);
  }
}

void um_show_endpoint_descriptor(st_endpoint_descriptor *desc) {
  int i;
  if (desc->BYTE.bLength == 0) DBGPRINT("Endpoint descriptor is empty\r\n");
  for (i = 0; i<desc->BYTE.bLength; i++) {
    if (i == 0)  DBGPRINT("IN data %d = bLength             %x\r\n", i, desc->PACKED[i]);
    if (i == 1)  DBGPRINT("IN data %d = bDescriptorType     %x\r\n", i, desc->PACKED[i]);
    if (i == 2)  DBGPRINT("IN data %d = bEndpointAddress    %x\r\n", i, desc->PACKED[i]);
    if (i == 3)  DBGPRINT("IN data %d = bmAttributes        %x\r\n", i, desc->PACKED[i]);
    if (i == 4)  DBGPRINT("IN data %d = wMaxPacketSize0     %x\r\n", i, desc->PACKED[i]);
    if (i == 5)  DBGPRINT("IN data %d = wMaxPacketSize1     %x\r\n", i, desc->PACKED[i]);
    if (i == 6)  DBGPRINT("IN data %d = bInterval           %x\r\n", i, desc->PACKED[i]);
  }
}
void um_show_descriptor(st_usb_descriptors *descs,en_usb_descriptor_type kind) {
  int i;
  if (kind == UDT_DEVICE) um_show_device_descriptor(&descs->device);
  if (kind == UDT_CONFIGURATION) um_show_configuration_descriptor(&descs->config);
  if (kind == UDT_INTERFACE) um_show_interface_descriptor(&descs->interface[0]);
  if (kind == UDT_ENDPOINT)
  for (i = 0;i<descs->interface[0].BYTE.bNumEndpoints;i++) um_show_endpoint_descriptor(&descs->interface[0].endp[i]);
}

void um_show_hid_descriptor(st_hid_descriptor *desc) {
  int i;
  for (i = 0; i<desc->BYTE.bLength; i++) {
    if (i == 0)  DBGPRINT("IN data %d = bLength             %x\r\n", i, desc->PACKED[i]);
    if (i == 1)  DBGPRINT("IN data %d = bDescriptorType     %x\r\n", i, desc->PACKED[i]);
    if (i == 2)  DBGPRINT("IN data %d = bcdHID[0]           %x\r\n", i, desc->PACKED[i]);
    if (i == 3)  DBGPRINT("IN data %d = bcdHID[1]           %x\r\n", i, desc->PACKED[i]);
    if (i == 4)  DBGPRINT("IN data %d = bCountryCode        %x\r\n", i, desc->PACKED[i]);
    if (i == 5)  DBGPRINT("IN data %d = bNumDescriptors     %x\r\n", i, desc->PACKED[i]);
    if (i == 6)  DBGPRINT("IN data %d = bDescriptorType     %x\r\n", i, desc->PACKED[i]);
    if (i == 7)  DBGPRINT("IN data %d = wDescriptorLength[0]%x\r\n", i, desc->PACKED[i]);
    if (i == 8)  DBGPRINT("IN data %d = wDescriptorLength[1]%x\r\n", i, desc->PACKED[i]);
  }
}


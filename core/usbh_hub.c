/*=======================================================================
// USB Host Library
//
// File:
//   usbh_hub.h
//
// Title:
//   USB host hub body
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
// $Date: 2014-09-08 12:17:32 +0900 (Mon, 08 Sep 2014) $
// $Rev: 79 $
                  
#include "usbh_hub.h"
#ifdef USBH_USE_HUB

en_usb_status um_get_hub_descriptor(int conn_num,st_usb_device *dev) {
  st_usb_setup_data sd;
  en_usb_status status = USBH_OK;
  sd.BYTE.bmRequestType = 0xa0;
  sd.BYTE.bRequest     = 6;
  sd.BYTE.wValue[0] = 0;       /* Descriptor Index */
  sd.BYTE.wValue[1] = UDT_HUB; /* request configuration descriptor */
  sd.BYTE.wIndex[0] = 0;
  sd.BYTE.wIndex[1] = 0;
  sd.BYTE.wLength[0] = 8;      /* try size = 8 */
  sd.BYTE.wLength[1] = 0;
  /* try size =8,then check actual  size */
  status = um_control_in_transfer(conn_num,dev,&sd,dev->descs.hub.PACKED);
  if (dev->descs.hub.BYTE.bDescLength != 8) {
    sd.BYTE.wLength[0] = dev->descs.hub.BYTE.bDescLength;
    status = um_control_in_transfer(conn_num,dev,&sd,dev->descs.hub.PACKED);
  }
  return status;
}

void um_show_hub_descriptor(st_usb_hub_descriptor *desc) {
  int i;
  for (i = 0; i<desc->BYTE.bDescLength; i++) {
    if (i == 0) DBGPRINT("IN data %d = bDescLength          %x\r\n", i, desc->PACKED[i]);
    if (i == 1) DBGPRINT("IN data %d = bDescriptorType      %x\r\n", i, desc->PACKED[i]);
    if (i == 2) DBGPRINT("IN data %d = bNbrPorts            %x\r\n", i, desc->PACKED[i]);
    if (i == 3) DBGPRINT("IN data %d = wHubCharacteristics0 %x\r\n", i, desc->PACKED[i]);
    if (i == 4) DBGPRINT("IN data %d = wHubCharacteristics1 %x\r\n", i, desc->PACKED[i]);
    if (i == 5) DBGPRINT("IN data %d = bPwrOn2PwrGood       %x\r\n", i, desc->PACKED[i]);
    if (i == 6) DBGPRINT("IN data %d = bHubContrCurrent     %x\r\n", i, desc->PACKED[i]);
    if (i == 7) DBGPRINT("IN data %d = DeviceRemovable      %x\r\n", i, desc->PACKED[i]);
    if (i >= 8) DBGPRINT("IN data %d = PortPwrCtrlMask      %x\r\n", i, desc->PACKED[i]);
  }
}
void um_set_port_feature(int conn_num,st_usb_device *dev,
                      en_hub_class_port_feature_selctor fs,int sel,int port) {
  st_usb_setup_data sd;
  sd.BYTE.bmRequestType = 0x23;
  sd.BYTE.bRequest     = 3;
  sd.BYTE.wValue[0] = fs;
  sd.BYTE.wValue[1] = 0;
  sd.BYTE.wIndex[0] = port;
  sd.BYTE.wIndex[1] = sel;
  sd.BYTE.wLength[0] = 0;
  sd.BYTE.wLength[1] = 0;
  um_control_out_transfer(conn_num,dev,&sd,0);  /* out data size = 0*/
}

void um_set_hub_feature(int conn_num,st_usb_device *dev,
                     en_hub_class_port_feature_selctor fs,int sel,int port) {
  st_usb_setup_data sd;
  sd.BYTE.bmRequestType = 0x20;
  sd.BYTE.bRequest     = 3;
  sd.BYTE.wValue[0] = fs;
  sd.BYTE.wValue[1] = 0;
  sd.BYTE.wIndex[0] = port;
  sd.BYTE.wIndex[1] = sel;
  sd.BYTE.wLength[0] = 0;
  sd.BYTE.wLength[1] = 0;
  um_control_out_transfer(conn_num,dev,&sd,0);  /* out data size = 0 */
}
void um_clear_port_feature(int conn_num,st_usb_device *dev,
                        en_hub_class_port_feature_selctor fs,int sel,int port) {
  st_usb_setup_data sd;
  sd.BYTE.bmRequestType = 0x23;
  sd.BYTE.bRequest     = 1;
  sd.BYTE.wValue[0] = fs;
  sd.BYTE.wValue[1] = 0;
  sd.BYTE.wIndex[0] = port;
  sd.BYTE.wIndex[1] = sel;
  sd.BYTE.wLength[0] = 0;
  sd.BYTE.wLength[1] = 0;
  um_control_out_transfer(conn_num,dev,&sd,0);  /* out data size = 0 */
}
void um_get_port_status(int conn_num,st_usb_device *dev,int port,unsigned char *data) {
  st_usb_setup_data sd;
  sd.BYTE.bmRequestType = 0xa3;
  sd.BYTE.bRequest     = 0;
  sd.BYTE.wValue[0] = 0;
  sd.BYTE.wValue[1] = 0;
  sd.BYTE.wIndex[0] = port;
  sd.BYTE.wIndex[1] = 0;
  sd.BYTE.wLength[0] = 4;
  sd.BYTE.wLength[1] = 0;
  um_control_in_transfer(conn_num,dev,&sd,data);
}
en_usb_status um_hub_port_device_setup(int conn_num,st_usb_host_info *hi,st_usb_device *dev,int port) {
  st_hub_port_status ps;
  int i;
  um_clear_port_feature(conn_num,dev,HPF_C_PORT_CONNECTION,0,port);
  um_get_port_status(conn_num,dev,port,ps.PACKED);
  um_show_port_status(dev,port,&ps);
  dev->p_devices[port]->info.address = 0;
  if(!ps.WORD.wPortStatus.BIT.CurrentConnectStatus) {
    INFOPRINT("waiting for CurrentConnectStaus\n");
    for (i=0;i<USBH_TRIAL_LOOP_CNT;i++) {
      ul_timer(20);
      um_get_port_status(conn_num,dev,port,ps.PACKED);
      if(ps.WORD.wPortStatus.BIT.CurrentConnectStatus) break;
    }
  }
  if(!ps.WORD.wPortStatus.BIT.CurrentConnectStatus) {
    um_error("HUB CurrentConnectStatus failed");
    while(1);
  }
  if (ps.WORD.wPortStatus.BIT.LowSpeedDeviceAttached) {
    dev->p_devices[port]->info.speed = 0;
    dev->p_devices[port]->info.preamble_enable = 1;
    INFOPRINT("Low speed device is attached on port %d\n",port);
  } else {
    dev->p_devices[port]->info.speed = 1;
    dev->p_devices[port]->info.preamble_enable = 0;
    INFOPRINT("Full speed device is attached on port %d\n",port);
  }
  um_set_port_feature(conn_num,dev,HPF_PORT_RESET,0,port);
  ul_timer(20);  /* wait 20ms */
  um_get_port_status(conn_num,dev,port,ps.PACKED);
  if(!(ps.WORD.wPortStatus.BIT.CurrentConnectStatus&
       ps.WORD.wPortStatus.BIT.PortEnabled)) {
    /* the downstream port device missing after the port reset */
    INFOPRINT("HUB CurrentConnectStatus/PortEnabled failed,try again...\n");
    goto fail;
  }
  um_clear_port_feature(conn_num,dev,HPF_C_PORT_RESET,0,port);
  um_get_port_status(conn_num,dev,port,ps.PACKED);
  um_clear_port_feature(conn_num,dev,HPF_C_PORT_CONNECTION,0,port);
  um_show_port_status(dev,port,&ps);
  /* check hub */
  um_get_std_descriptors(conn_num,dev->p_devices[port]);
  um_show_descriptor(&dev->p_devices[port]->descs,UDT_DEVICE);
  if (um_is_hub(dev->p_devices[port])) {
    /* show string descriptor */
    um_get_std_descriptor(conn_num,dev->p_devices[port],UDT_STRING);
    um_hub_setup(conn_num,hi,dev->p_devices[port]);
  } else {
    um_device_setup(conn_num,hi,dev->p_devices[port]);
  }
  ul_timer(100);
  um_show_port_status(dev,port,&ps);
  return USBH_OK;
 fail:
  return USBH_ERROR;
}

int um_get_hub_num_ports(st_usb_device *dev) {
  return dev->descs.hub.BYTE.bNbrPorts;
}

void um_hub_port_device_delete(int conn_num,st_usb_host_info *hi,st_usb_device *dev,int port) {
  st_hub_port_status ps;
  um_get_port_status(conn_num,dev,port,ps.PACKED);
  um_show_port_status(dev,port,&ps);
  um_clear_port_feature(conn_num,dev,HPF_C_PORT_CONNECTION,0,port);
  um_clear_port_feature(conn_num,dev,HPF_C_PORT_ENABLE,0,port);

  dev->p_devices[port]->info.available = 0;
  dev->p_devices[port]->info.address = 0;
}

void um_show_port_status(st_usb_device *dev,int port,st_hub_port_status *data){  
  DBGPRINT("Port %d status: %04x %04x\r\n",port,data->WORD.wPortStatus.WORD,
                                          data->WORD.wPortChange.WORD);
  DBGPRINT("  CurrentConnectStatus: %x\r\n",data->WORD.wPortStatus.BIT.CurrentConnectStatus);
  DBGPRINT("  PortEnabled:          %x\r\n",data->WORD.wPortStatus.BIT.PortEnabled);
  DBGPRINT("  Suspend:              %x\r\n",data->WORD.wPortStatus.BIT.Suspend);
  DBGPRINT("  OverCurrent:          %x\r\n",data->WORD.wPortStatus.BIT.OverCurrent);
  DBGPRINT("  Reset:                %x\r\n",data->WORD.wPortStatus.BIT.Reset);
  DBGPRINT("  PortPower:            %x\r\n",data->WORD.wPortStatus.BIT.PortPower);
  DBGPRINT("  LowSpeedDeviceAttached: %x\r\n",data->WORD.wPortStatus.BIT.LowSpeedDeviceAttached);
  DBGPRINT("  HighSpeedDeviceAttached:%x\r\n",data->WORD.wPortStatus.BIT.HighSpeedDeviceAttached);
  DBGPRINT("  PortTestMode:           %x\r\n",data->WORD.wPortStatus.BIT.PortTestMode);
  DBGPRINT("  PortIndicatorControl:   %x\r\n",data->WORD.wPortStatus.BIT.PortIndicatorControl);
 
  DBGPRINT("  ConnectStatusChange:        %x\r\n",data->WORD.wPortChange.BIT.ConnectStatusChange);
  DBGPRINT("  PortEnableDisableChange:    %x\r\n",data->WORD.wPortChange.BIT.PortEnableDisableChange);
  DBGPRINT("  SuspendChange:              %x\r\n",data->WORD.wPortChange.BIT.SuspendChange);
  DBGPRINT("  OverCurrentIndicatorChange: %x\r\n",data->WORD.wPortChange.BIT.OverCurrentIndicatorChange);
  DBGPRINT("  ResetChange:                %x\r\n",data->WORD.wPortChange.BIT.ResetChange);
}

void um_hub_setup(int conn_num,st_usb_host_info *hi,st_usb_device *dev) {
  int i,j,adrs;
  st_hub_port_status ps;
  /* address for this hub */
  adrs = um_get_next_address(hi) ;
  /* configure the device */
  um_set_address(conn_num,dev,adrs);
  ul_timer(2);
  um_set_configuration(conn_num,dev,um_get_config_num(dev));
  dev->info.available = 1;
  if (um_is_hub(dev)) {
    DBGPRINT("the primary device is HUB\n");
    um_get_hub_descriptor(conn_num,dev);
    for ( j = 0 ; j < dev->descs.config.BYTE.bNumInterfaces;j++) {
      for ( i = 0 ; i < dev->descs.interface[j].BYTE.bNumEndpoints;i++) {
        if (dev->descs.interface[j].endp[i].BYTE.bEndpointAddress & 0x80) {
          dev->info.interface[j].ea_in =
            dev->descs.interface[j].endp[i].BYTE.bEndpointAddress & 0xf;
          dev->info.interface[j].ea_in_max_packet_size =
            dev->descs.interface[j].endp[i].BYTE.wMaxPacketSize[0];
        } else {
          dev->info.interface[j].ea_out =
            dev->descs.interface[j].endp[i].BYTE.bEndpointAddress & 0xf;
          dev->info.interface[j].ea_out_max_packet_size =
            dev->descs.interface[j].endp[i].BYTE.wMaxPacketSize[0];
        }
      }
    }
    /* hub port power on */
    for (i=1;i<=um_get_hub_num_ports(dev);i++)
      um_set_port_feature(conn_num,dev,HPF_PORT_POWER,0,i);
    /* clear port connection feature */
    for (i=1;i<=um_get_hub_num_ports(dev);i++)
      um_clear_port_feature(conn_num,dev,HPF_C_PORT_CONNECTION,0,i);
    /* get port status */
    for (i=1;i<=um_get_hub_num_ports(dev);i++) {
      um_get_port_status(conn_num,dev,i,ps.PACKED);
      if(ps.WORD.wPortStatus.BIT.CurrentConnectStatus) {
        INFOPRINT("\nhub port %d device available\n",i);
        um_add_hub_device(dev,i);
        /* setup device under the port */
        um_hub_port_device_setup(conn_num,hi,dev,i);
      }
    }
  }
}

#endif /*#ifdef USBH_USE_HUB*/

/*=======================================================================
// USB Host Library
//
// File:
//   usbh_ms_bot.c
//
// Title:
//   USB host mass storage body
//     Bulk only transfer
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
// $Date: 2015-03-08 07:29:43 +0900 (Sun, 08 Mar 2015) $
// $Rev: 103 $

#include "usbh_msd_bot.h"
#include "usbh_env.h"
#ifdef USBH_USE_MSD

un_command_block_wrapper cbw;
un_command_status_wrapper csw;

un_scsi_test_unit_ready scsi_tur;
un_scsi_read scsi_read;
un_scsi_write scsi_write;
un_scsi_request_sense scsi_request_sense;
un_scsi_inquiry scsi_inquiry;
un_scsi_read_capacity scsi_read_capacity;
un_scsi_mode_sense scsi_mode_sense;
unsigned char active_lun = 0;
unsigned char max_lun = 0;

void um_disk_env_init(int conn_num,st_usb_device *dev) {
  int i;
  /* cbw setup */
  cbw.KIND.dCBWSignatire = 0x43425355;
  cbw.KIND.dCBWTag = 0xdeadbeef;
  cbw.KIND.dCBWDataTransferLength = 0;/* no data transfer */
  cbw.KIND.bmCBWFlags = 0;
  cbw.KIND.bCBWLUN = 0;
  cbw.KIND.bCBWCBLength = 0;
  for (i=0;i<16;i++) cbw.KIND.CBWCB[i] = 0;
  /* SCSI command setup */
  for (i=0;i<6;i++)
    scsi_tur.PACKED[i] = 0;
  scsi_tur.KIND.OP = 0x00;
  scsi_tur.KIND.LUN = 0;
  for (i=0;i<10;i++)
    scsi_read.PACKED[i] = 0;
  scsi_read.KIND.OP = 0x28;
  scsi_read.KIND.LUN = 0;
  
  for (i=0;i<10;i++)
    scsi_write.PACKED[i] = 0;
  scsi_write.KIND.OP = 0x2a;
  scsi_write.KIND.LUN = 0;
  
  for (i=0;i<6;i++)
     scsi_request_sense.PACKED[i] = 0;
  scsi_request_sense.KIND.OP = 0x03;
  scsi_request_sense.KIND.LUN = 0;
  scsi_request_sense.KIND.AllocationLength = 18; /* usually 18 */
  for (i=0;i<6;i++)
          scsi_inquiry.PACKED[i] = 0;
  scsi_inquiry.KIND.OP = 0x12;
  scsi_inquiry.KIND.AllocationLength = 36; /* usually 36 */

  for (i=0;i<10;i++)
    scsi_read_capacity.PACKED[i] = 0;
  scsi_read_capacity.KIND.OP = 0x25;

  for (i=0;i<10;i++)
    scsi_mode_sense.PACKED[i] = 0;
  scsi_mode_sense.KIND.OP = 0x1a;
  scsi_mode_sense.KIND.AllocationLength[0] = 13; /* usually 252 */
  max_lun = um_bot_get_max_lun(conn_num,dev);
}

en_usb_status um_bot_set_command_request_sense(int conn_num,st_usb_device *dev,unsigned char lun) {
  int i;
  en_usb_status status;
  cbw.KIND.bmCBWFlags = 0x80;  /* direction: Data-In */
  cbw.KIND.bCBWLUN = lun;
  cbw.KIND.bCBWCBLength = 0xc; /* 6 */
  cbw.KIND.dCBWDataTransferLength = 18; /* 18 */
  scsi_request_sense.KIND.LUN = (lun & 0x7) << 5;
  for (i=0;i<0xc;i++)
    cbw.KIND.CBWCB[i] = 0;

  for (i=0;i<6;i++)
    cbw.KIND.CBWCB[i] = scsi_request_sense.PACKED[i];
        
  status = ul_bulk_out_transfer(conn_num,dev,31/*size*/,cbw.PACKED);
  return status;
};

en_usb_status um_bot_set_command_test_unit_ready(int conn_num,st_usb_device *dev,int lun) {

  int i;
  en_usb_status status;
  cbw.KIND.bmCBWFlags = 0x00;  /* direction: Data-Out */
  cbw.KIND.bCBWCBLength = 6;
  cbw.KIND.bCBWLUN = lun;
  cbw.KIND.dCBWDataTransferLength = 0;
  scsi_tur.KIND.LUN = (lun & 0x7) << 5;
  for (i=0;i<6;i++)
    cbw.KIND.CBWCB[i] = scsi_tur.PACKED[i];
  DBGPRINT("\ncbw contents\n"); 
  for (i=0;i<31;i++) DBGPRINT("%d %x\n",i,cbw.PACKED[i]);
  DBGPRINT("\nset_command_test_unit_ready LUN %d\n",lun);
  status = ul_bulk_out_transfer(conn_num,dev,31/*size*/,cbw.PACKED);
  DBGPRINT("\nset_command_test_unit_ready status %x\n",status);
  return status;
}


en_usb_status um_bot_set_command_inquiry(int conn_num,st_usb_device *dev,int lun) {
  int i;
  en_usb_status status;
  cbw.KIND.bmCBWFlags = 0x80;  /* direction: Data-In */
  cbw.KIND.bCBWCBLength = 6;
  cbw.KIND.dCBWDataTransferLength = 36; /* usually 36 */
  cbw.KIND.bCBWLUN = lun;
  scsi_tur.KIND.LUN = (lun & 0x7) << 5;
  for (i=0;i<6;i++)
    cbw.KIND.CBWCB[i] = scsi_inquiry.PACKED[i];
  DBGPRINT("\ncbw contents\n");
  for (i=0;i<31;i++) DBGPRINT("%d %x\n",i,cbw.PACKED[i]);
  DBGPRINT("\nset_command_inquiry LUN %d\n",lun);
  status = ul_bulk_out_transfer(conn_num,dev,31/*size*/,cbw.PACKED);
  DBGPRINT("\nset_command_inquiry status %x\n",status);
  return status;
}

en_usb_status um_bot_set_command_read_capacity(int conn_num,st_usb_device *dev,int lun) {
  int i;
  en_usb_status status;
  cbw.KIND.bmCBWFlags = 0x80;  /* direction: Data-In */
  cbw.KIND.bCBWCBLength = 10;
  cbw.KIND.dCBWDataTransferLength = 8;
  cbw.KIND.bCBWLUN = lun;
  scsi_tur.KIND.LUN = (lun & 0x7) << 5;
  for (i=0;i<10;i++)
    cbw.KIND.CBWCB[i] = scsi_read_capacity.PACKED[i];
  DBGPRINT("\ncbw contents\n");
  for (i=0;i<31;i++) INFOPRINT("%d %x\n",i,cbw.PACKED[i]);
  DBGPRINT("\nset_command_inquiry LUN %d\n",lun);
  status = ul_bulk_out_transfer(conn_num,dev,31/*size*/,cbw.PACKED);
  INFOPRINT("\nset_command_read_capacity status %x\n",status);
  return status;
}

void um_bot_set_command_read(int conn_num,st_usb_device *dev,
                                      unsigned char lun,int sector, int size) {
  int i;
  DBGPRINT("sector %x\n",sector  );
  /* cbw setup */
  cbw.KIND.dCBWDataTransferLength = size*512;
  cbw.KIND.bmCBWFlags = 0x80;  /* direction: Data-In */
  cbw.KIND.bCBWCBLength = 10;
  cbw.KIND.bCBWLUN = lun;
    scsi_read.KIND.LUN = (lun & 0x7) << 5;
  for (i=0;i<4;i++) /* byte swap */
    scsi_read.KIND.LogicalBlockAddress[3-i] = (sector >> (i*8)) & 0xff;
  for (i=0;i<2;i++) /* byte swap */
    scsi_read.KIND.TransferLength[1-i] = (size >> (i*8)) & 0xff;
  for (i=0;i<10;i++) {
    cbw.KIND.CBWCB[i] = scsi_read.PACKED[i];
    DBGPRINT("cbr contents %d %x\n",i, scsi_read.PACKED[i]  );
  }
  ul_bulk_out_transfer(conn_num,dev,31/*size*/,cbw.PACKED);
}

void um_bot_set_command_write(int conn_num,st_usb_device *dev,
                            unsigned char lun,unsigned int sector, unsigned int size) {
  int i;
  /* cbw setup */
  cbw.KIND.dCBWDataTransferLength = size*512;
  cbw.KIND.bmCBWFlags = 0x00;  /* direction: Data-Out */
  cbw.KIND.bCBWCBLength = 10;
  cbw.KIND.bCBWLUN = lun;
  scsi_write.KIND.LUN = (lun & 0x7) << 5;
  for (i=0;i<4;i++) /* byte swap */
    scsi_write.KIND.LogicalBlockAddress[3-i] = (sector >> (i*8)) & 0xff;
  for (i=0;i<2;i++) /* byte swap */
    scsi_write.KIND.TransferLength[1-i] = (size >> (i*8)) & 0xff;
  for (i=0;i<10;i++) {
    cbw.KIND.CBWCB[i] = scsi_write.PACKED[i];
    DBGPRINT("cbw contents %d %x\n",i, scsi_write.PACKED[i]  ); 
  }

  ul_bulk_out_transfer(conn_num,dev,31/*size*/,cbw.PACKED);
}

en_scsi_status um_bot_get_status(int conn_num,st_usb_device *dev) {
  en_scsi_status ss;
  en_usb_status us;
  int i;
  DBGPRINT("get_status in\n");
  for (i=0;i<12;i++) csw.PACKED[i] = 0;
  us = ul_bulk_in_transfer(conn_num,dev,13/*size*/,csw.PACKED);
  for (i=0;i<13;i++) DBGPRINT("GET_STAT %d %x\n",i,csw.PACKED[i]);
  DBGPRINT("get_status out %x\n",us);
  if (us != USBH_OK) return us;
  ss = csw.PACKED[12];
  return ss;
}

void um_bot_scsi_write_data(int conn_num,st_usb_device *dev,unsigned char *buf, int size) {
  ul_bulk_out_transfer(conn_num,dev,size*512,buf);
}

void um_bot_scsi_read_data(int conn_num,st_usb_device *dev,unsigned char *buf, int size) {
  ul_bulk_in_transfer(conn_num,dev,size*512,buf);
}

static st_usb_device * um_get_ms_dev(unsigned char drv,unsigned char *cnn) {
  st_usb_device *dev;
  dev = uh_msd_find(drv,cnn);
  if (dev != NULL) return dev;
  um_error("\n no msd found \n");
  return 0;
}

int uh_disk_initialize(unsigned char drv) {
  int i;
  int cl;
  unsigned char buf[16];
  en_scsi_status ss;
  en_usb_status us;
  unsigned char conn_num;
  st_usb_device *dev;

  dev = um_get_ms_dev(drv,&conn_num);
 
  DBGPRINT("\nUSB_disk_initialize called\n");

  um_disk_env_init(conn_num,dev);
  for (cl=0;cl<=max_lun;cl++) {
    for(i=0;i<USBH_BOT_TRIAL_LOOP_CNT;i++) {
      um_bot_set_command_test_unit_ready(conn_num,dev,cl);
      ss = um_bot_get_status(conn_num,dev);
      DBGPRINT("get_status set_command_test_unit_ready %x \n",ss);
      if (ss == CSW_STATUS_PASSED) {
        active_lun = cl;
        return 0;
      }
      if (ss == CSW_STATUS_BULK_STALL)
        um_clear_feature(conn_num,dev,UDF_ENDPOINT_HALT,
                         dev->info.interface[dev->info.cur_interface].ea_in);
      if (ss == CSW_STATUS_PHASE_ERROR) um_bot_reset_recovery(conn_num,dev);
      if (ss == CSW_STATUS_FAILED) {
        DBGPRINT("set_command_request_sense\n");
        um_bot_set_command_request_sense(conn_num,dev,cl);
        us = ul_bulk_in_transfer(conn_num,dev,18,buf);
        if (us != USBH_OK) {
          um_bot_reset_recovery(conn_num,dev);
        } else {
          ss = um_bot_get_status(conn_num,dev);
          if (ss == CSW_STATUS_BULK_STALL)
            um_clear_feature(conn_num,dev,UDF_ENDPOINT_HALT,
                             dev->info.interface[dev->info.cur_interface].ea_in);
          if (ss == CSW_STATUS_PHASE_ERROR) um_bot_reset_recovery(conn_num,dev);
        }
      }
      ul_timer(1);
    }
  }
  um_error("test_unit_ready status error, MSD not ready\n");
  return 1;
}

int uh_disk_status(unsigned char drv) {
  unsigned char conn_num;
  st_usb_device *dev;
   
  dev = um_get_ms_dev(drv,&conn_num);

  DBGPRINT("\nUSB_disk_status called\n");
  return 0;
}

int uh_disk_read (
  unsigned char drv,
  unsigned char *buff,   /* Data buffer to store read data */
  unsigned int sector,   /* Sector address (LBA) */
  unsigned char count    /* Number of sectors to read (1..128) */
)
{
  int i;
  unsigned char conn_num;
  st_usb_device *dev;
  en_scsi_status status;

  dev = um_get_ms_dev(drv,&conn_num);
  DBGPRINT("USB_disk_read called, sector %d, count %d\n",sector,count);
  /* SCSI read call */
  um_bot_set_command_read(conn_num,dev,active_lun,sector,count);
  um_bot_scsi_read_data(conn_num,dev,buff,count);
  for (i=0;i<USBH_BOT_TRIAL_LOOP_CNT;i++) {
    status = um_bot_get_status(conn_num,dev);
    if (status == CSW_STATUS_PASSED) break;
    if (status == CSW_STATUS_BULK_STALL)
      um_clear_feature(conn_num,dev,UDF_ENDPOINT_HALT,
                                    dev->info.interface[dev->info.cur_interface].ea_in);
    if (status == CSW_STATUS_PHASE_ERROR) {
       um_bot_reset_recovery(conn_num,dev);
       um_bot_set_command_read(conn_num,dev,drv,sector,count);
       um_bot_scsi_read_data(conn_num,dev,buff,count);
    }
  }
  return status;
}

int uh_disk_write (
  unsigned char drv,
  const unsigned char *buff,    /* Data to be written */
  unsigned int sector,          /* Sector address (LBA) */
  unsigned char count           /* Number of sectors to write (1..128) */
)
{
  int i;
  unsigned char conn_num;
  st_usb_device *dev;
  en_scsi_status status;

  dev = um_get_ms_dev(drv,&conn_num);
  DBGPRINT("USB_disk_write called\n");
  /* SCSI write call */
  um_bot_set_command_write(conn_num,dev,active_lun,sector,count);
  um_bot_scsi_write_data(conn_num,dev,(unsigned char*)buff,count);
  for (i=0;i<USBH_BOT_TRIAL_LOOP_CNT;i++) {
    status = um_bot_get_status(conn_num,dev);
    if (status == CSW_STATUS_PASSED) break;
    if (status == CSW_STATUS_BULK_STALL)
      um_clear_feature(conn_num,dev,UDF_ENDPOINT_HALT,
                       dev->info.interface[dev->info.cur_interface].ea_in);
    if (status == CSW_STATUS_PHASE_ERROR) {
       um_bot_reset_recovery(conn_num,dev);
       um_bot_set_command_write(conn_num,dev,drv,sector,count);
       um_bot_scsi_write_data(conn_num,dev,(unsigned char*)buff,count);
    }
  }
  return 0;
}

int uh_disk_ioctl (
  unsigned char drv,
  unsigned char cmd,    /* Control code */
  void *buff            /* Buffer to send/receive control data */
)
{
  DBGPRINT("\nUSB_disk_ioctl called\n");
  return 0;
}

en_usb_status um_bot_mass_strage_reset(int conn_num,st_usb_device *dev) {
  st_usb_setup_data sd;
  en_usb_status s;
  sd.BYTE.bmRequestType = 0x21;
  sd.BYTE.bRequest     = 0xff;
  sd.BYTE.wValue[0] = 0;
  sd.BYTE.wValue[1] = 0;
  sd.BYTE.wIndex[0] = 0;  /* interface number */
  sd.BYTE.wIndex[1] = 0;
  sd.BYTE.wLength[0] = 0;
  sd.BYTE.wLength[1] = 0;
  s = um_control_out_transfer(conn_num,dev,&sd,0);  /* out data size = 0 */
  /*dev->info.out_data_type = 0;  // The device shall preserve the value of its bulk data toggle bits */
  return s;
}
unsigned char um_bot_get_max_lun(int conn_num,st_usb_device *dev){
  st_usb_setup_data sd;
  unsigned char buf;
  en_usb_status s;
  sd.BYTE.bmRequestType = 0xa1;
  sd.BYTE.bRequest     = 0xfe;
  sd.BYTE.wValue[0] = 0;
  sd.BYTE.wValue[1] = 0;
  sd.BYTE.wIndex[0] = 0;
  sd.BYTE.wIndex[1] = 0;
  sd.BYTE.wLength[0] = 1;
  sd.BYTE.wLength[1] = 0;
  s = um_control_in_transfer(conn_num,dev,&sd,&buf);
  if (s == USBH_STALL) {
    um_get_std_descriptor(conn_num,dev,UDT_DEVICE); /* Send SETUP transaction */
    return 0;
  }
  else return buf;
}

void um_bot_reset_recovery(int conn_num,st_usb_device *dev) {
  en_usb_status s;
  DBGPRINT("\nMSD Reset Recovery...\n");
  s = um_bot_mass_strage_reset(conn_num,dev);
  DBGPRINT("\n  mass_strage_reset %d\n",s);
  s = um_clear_feature(conn_num,dev,UDF_ENDPOINT_HALT,
                       dev->info.interface[dev->info.cur_interface].ea_in);
  DBGPRINT("\n  clear_feature ENDP IN %d %d\n",
                       dev->info.interface[dev->info.cur_interface].ea_in,s);
  s = um_clear_feature(conn_num,dev,UDF_ENDPOINT_HALT,
                       dev->info.interface[dev->info.cur_interface].ea_out);
  dev->info.interface[dev->info.cur_interface].out_data_type = 0;
  DBGPRINT("\n  clear_feature ENDP OUT%d %d\n",
                       dev->info.interface[dev->info.cur_interface].ea_out,s);
  DBGPRINT("\nMSD Reset Recovery done\n");

}

#endif /*#ifdef USBH_USE_MSD*/

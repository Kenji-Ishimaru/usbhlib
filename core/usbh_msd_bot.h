/*=======================================================================
// USB Host Library
//
// File:
//   usbh_ms_bot.h
//
// Title:
//   USB host mass storage header
//     Bulk only transfer
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

#ifndef __USBH_MSD_BOT__
#define __USBH_MSD_BOT__

#include "usbh_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/* command block wrapper */
typedef union {
  unsigned char PACKED[31];
  struct {
      unsigned int dCBWSignatire;
      unsigned int dCBWTag;
      unsigned int dCBWDataTransferLength;
      unsigned char bmCBWFlags;
      unsigned char bCBWLUN;
      unsigned char bCBWCBLength;
      unsigned char CBWCB[16];
  } KIND;
} un_command_block_wrapper;

/* command status wrapper */
typedef union {
  unsigned char PACKED[6];
  struct {
    unsigned int dCSWSignatire;
    unsigned int dCSWTag;
    unsigned int dCSWDataResidue;
    unsigned char bCSWStatus;
  } KIND;
} un_command_status_wrapper;

/* command status value*/
typedef enum {
  CSW_STATUS_PASSED = 0x00,
  CSW_STATUS_FAILED = 0x01,
  CSW_STATUS_PHASE_ERROR = 0x02,
  CSW_STATUS_BULK_STALL = 0x20
} en_scsi_status;

/* SCSI command*/
typedef enum {
  SCSI_INQUIRY = 0x12,
  SCSI_READ_CAPACITY = 0x25,
  SCSI_READ = 0x28,
  SCSI_WRITE = 0x2a,
  SCSI_REQUEST_SENSE = 0x03,
  SCSI_MODE_SENSE = 0x5a,
  SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL = 0x1e,
  SCSI_TEST_UNIT_READY = 0x00,
  SCSI_VERIFY = 0x2f
} en_scsi_command;

/* INQUIRY */
typedef union {
  unsigned char PACKED[6];
  struct {
    unsigned char OP;
    unsigned char LUN;
    unsigned char PageCode;
    unsigned char Reserved;
    unsigned char AllocationLength;
    unsigned char Control;
  } KIND;
} un_scsi_inquiry;

/* READ CAPACITY */
typedef union {
  unsigned char PACKED[10];
  struct {
    unsigned char OP;
    unsigned char LUN;
    unsigned char LogicalBlockAddress[4];
    unsigned char Reserved6;
    unsigned char Reserved7;
    unsigned char PMI;
    unsigned char Control;
  } KIND;
} un_scsi_read_capacity;
/* READ(10) */
typedef union {
  unsigned char PACKED[10];
  struct {
    unsigned char OP;
    unsigned char LUN;
    unsigned char LogicalBlockAddress[4];
    unsigned char Reserved6;
    unsigned char TransferLength[2];
    unsigned char Control;
  } KIND;
} un_scsi_read;

/* WRITE(10) */
typedef union {
  unsigned char PACKED[10];
  struct {
    unsigned char OP;
    unsigned char LUN;
    unsigned char LogicalBlockAddress[4];
    unsigned char Reserved6;
    unsigned char TransferLength[2];
    unsigned char Control;
  } KIND;
} un_scsi_write;

/* REQUEST SENSE */
typedef union {
  unsigned char PACKED[6];
  struct {
    unsigned char OP;
    unsigned char LUN;
    unsigned char Reserved2;
    unsigned char Reserved3;
    unsigned char AllocationLength;
    unsigned char Control;
  } KIND;
} un_scsi_request_sense;

/* MODE SENSE(10) */
typedef union {
  unsigned char PACKED[10];
  struct {
    unsigned char OP;
    unsigned char LUN;
    unsigned char PC;
    unsigned char Reserved3;
    unsigned char Reserved4;
    unsigned char Reserved5;
    unsigned char Reserved6;
    unsigned char AllocationLength[2];
  } KIND;
} un_scsi_mode_sense;

/* PREVENT ALLOW MEDIUM REMOVAL */
typedef union {
  unsigned char PACKED[6];
  struct {
    unsigned char OP;
    unsigned char LUN;
    unsigned char Reserved2;
    unsigned char Reserved3;
    unsigned char PRV;
    unsigned char Control;
  } KIND;
} un_scsi_prevent_allow_medium_removal;

/* TEST UNIT READY */
typedef union {
  unsigned char PACKED[6];
  struct {
    unsigned char OP;
    unsigned char LUN;
    unsigned char Reserved2;
    unsigned char Reserved3;
    unsigned char Reserved4;
    unsigned char Control;
  } KIND;
} un_scsi_test_unit_ready;

/* VERIFY */
typedef union {
  unsigned char PACKED[10];
  struct {
    unsigned char OP;
    unsigned char LUN;
    unsigned char LogicalBlockAddress[4];
    unsigned char Reserved6;
    unsigned char VerificationLength[2];
    unsigned char Control;
  } KIND;
} un_scsi_verify;

/* START/STOP UNIT */
/*
typedef union {
  unsigned char PACKED[6];
  struct {
    unsigned char OP;
    unsigned char LUN;
    unsigned char Reserved2;
    unsigned char Reserved3;
    unsigned char LoEStart;
    unsigned char Control;
  } KIND;
} un_scsi_start_stop_unit;
*/
en_usb_status um_bot_mass_strage_reset(int conn_num,st_usb_device *di);
unsigned char um_bot_get_max_lun(int conn_num,st_usb_device *di);


void um_disk_env_init(int conn_num,st_usb_device *dev);
en_usb_status um_bot_set_command_test_unit_ready(int conn_num,st_usb_device *dev,int lun);
en_usb_status um_bot_set_command_inquiry(int conn_num,st_usb_device *dev,int lun);
en_usb_status um_bot_set_command_read_capacity(int conn_num,st_usb_device *dev,int lun);

en_usb_status um_bot_set_command_request_sense(int conn_num,st_usb_device *dev,unsigned char lun);
void um_bot_set_command_read(int conn_num,st_usb_device *dev,
                             unsigned char lun,int sector, int size);
void um_bot_set_command_write(int conn_num,st_usb_device *dev,
                           unsigned char lun,unsigned int sector, unsigned int size);
en_scsi_status um_get_status(int conn_num,st_usb_device *dev);
void um_bot_scsi_write_data(int conn_num,st_usb_device *dev,
                            unsigned char *buf, int size);
void um_bot_scsi_read_data(int conn_num,st_usb_device *dev,
                           unsigned char *buf, int size);
void um_bot_reset_recovery(int conn_num,st_usb_device *dev);

/* disk function */
int uh_disk_initialize(unsigned char drv);
int uh_disk_status(unsigned char drv);
int uh_disk_read (
  unsigned char drv,
  unsigned char *buff,
  unsigned int sector,
  unsigned char count
);

int uh_disk_write (
  unsigned char drv,
  const unsigned char *buff,
  unsigned int sector,
  unsigned char count
);

int uh_disk_ioctl (
  unsigned char drv,
  unsigned char cmd,
  void *buff
);

#ifdef __cplusplus
}
#endif

#endif

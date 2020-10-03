/*=======================================================================
// USB Host Library
//
// File:
//   usb_hw_reg.c
//
// Title:
//   OpenCores USB host slave register definition
//   for microblaze
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
// $Date: 2014-09-08 12:21:32 +0900 (Mon, 08 Sep 2014) $
// $Rev: 81 $
                  
#ifndef USB_HW_REG_H
#define USB_HW_REG_H

#include <system.h>

// USB port select macro
#define USB_CONTROL_REG(n)            (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x00))
#define USB_TX_TRANS_TYPE_REG(n)      (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x01))
#define USB_TX_LINE_CONTROL_REG(n)    (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x02))
#define USB_TX_SOF_ENABLE_REG(n)      (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x03))
#define USB_TX_ADDR_REG(n)            (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x04))
#define USB_TX_ENDP_REG(n)            (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x05))
#define USB_FRAME_NUM_MSB_REG(n)      (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x06))
#define USB_FRAME_NUM_LSB_REG(n)      (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x07))
#define USB_INTERRUPT_STATUS_REG(n)   (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x08))
#define USB_INTERRUPT_MASK_REG(n)     (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x09))
#define USB_RX_STATUS_REG(n)          (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x0a))
#define USB_RX_PID_REG(n)             (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x0b))
#define USB_RX_ADDR_REG(n)            (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x0c))
#define USB_RX_ENDP_REG(n)            (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x0d))
#define USB_RX_CONNECT_STATE_REG(n)   (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x0e))
#define USB_SOF_TIMER_MSB_REG(n)      (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x0f))

#define USB_RX_FIFO_DATA(n)           (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x20))
#define USB_RX_FIFO_DATA_COUNT_MSB(n) (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x22))
#define USB_RX_FIFO_DATA_COUNT_LSB(n) (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x23))
#define USB_RX_FIFO_CONTROL_REG(n)    (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x24))

#define USB_TX_FIFO_DATA(n)           (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x30))
#define USB_TX_FIFO_CONTROL_REG(n)    (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0x34))
#define USB_HOST_SLAVE_CONTROL_REG(n) (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0xe0))
#define USB_HOST_SLAVE_VERSION_REG(n) (*(volatile unsigned char *)(FM_USB_WRAPPER_0_BASE+0xe1))


#endif


/*=======================================================================
// USB Host Library
//
// File:
//   usbh_timer.c
//
// Title:
//   Timer function for MicroBlaze
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
// $Date: 2014-09-11 13:22:41 +0900 (Thu, 11 Sep 2014) $
// $Rev: 91 $

#include <stdio.h>
#include "usbh_timer.h"
#include "usbh_hw_reg.h"
#include "../src/platform.h"
/******************************************
 CPU dependent API
 *****************************************/

extern volatile int fit_status;

void ul_timer (int m) {
  int i;
  for (i=0;i<m;i++) {
    fit_status = 0;
    while (!fit_status) {
      USB_HOST_SLAVE_VERSION_REG(0) = 1;  // dummy
    }
  }
}

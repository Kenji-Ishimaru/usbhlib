/*=======================================================================
// USB Host Library
//
// File:
//   usbh_hw.h
//
// Title:
//   OpenCores USB hostslave hardware dependent code
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
// $Date: 2014-09-11 13:20:34 +0900 (Thu, 11 Sep 2014) $
// $Rev: 90 $

#include "usbh_timer.h"
#include "usbh_hw.h"
#include "usbh_config.h"

void ul_hw_init() {
  /* hard ware dependent initialization */
  int  conn_num;
  /* USB connector loop */
  for (conn_num = USBH_START_CONN; conn_num < USBH_NUM_OF_CONNS; conn_num++) {
    ul_host_config(conn_num);      /* OpenCores USB host configuration */
	ul_host_bus_reset(conn_num);   /* bus reset */
  }
}

void ul_hw_device_setup(int conn_num,st_usb_device *dev,int adrs) {
  /* this function is called from um_device_setup */
  MPRINT("ul_hw_device_setup called\n");
}

void ul_host_config(int conn_num) {
  INFOPRINT("\nUSB Configuration connector %d...\n",conn_num);
   USB_HOST_SLAVE_CONTROL_REG(conn_num) = 2;   /* hardware reset*/
   ul_timer(10);
   USB_HOST_SLAVE_CONTROL_REG(conn_num) = 1;   /* host mode */
}

void ul_host_bus_reset(int conn_num) {
  /* bus reset */
  USB_TX_LINE_CONTROL_REG(conn_num) = 0x4;  /* Dp/Dn = 0 */
  ul_timer(100);
  USB_TX_LINE_CONTROL_REG(conn_num) = 0;   /* end */
  ul_timer(100);
}

int wait_int_event(int conn_num,en_int_status s) {
  unsigned int d;
  int num_of_tries = 0;
  while (1) {
    d = USB_INTERRUPT_STATUS_REG(conn_num);
    switch (s) {
    case IS_TRANS_DONE:
      if (d&IS_TRANS_DONE) {
        USB_INTERRUPT_STATUS_REG(conn_num) = 0xf;  // clear status
        return USBH_OK;
      }
      break;
    case IS_RESUME:
      if (d&IS_RESUME) {
        USB_INTERRUPT_STATUS_REG(conn_num) = 0xf;
        return USBH_OK;
      }
      break;
    case IS_CONNECTION_EVENT:
      if (d&IS_CONNECTION_EVENT) {
        USB_INTERRUPT_STATUS_REG(conn_num) = 0xf;
        return USBH_OK;
      }
      break;
    case IS_SOF_SENT:
      if (d&IS_SOF_SENT) {
        USB_INTERRUPT_STATUS_REG(conn_num) = 0xf;
        return USBH_OK;
      }
      break;
    }
    num_of_tries++;
    if (num_of_tries >= USBH_INTSTATUS_LOOP_CNT) break;
  }
  return USBH_ERROR;
}

int ul_speed_configuration(int conn_num) {
    unsigned int d;
    int status;
    status = wait_int_event(conn_num,IS_CONNECTION_EVENT);
    if (status == USBH_ERROR) return USBH_ERROR;

    d = USB_RX_CONNECT_STATE_REG(conn_num);
    if (d == 1) {
        INFOPRINT("\nconn:%d LOW_SPEED_CONNECT\n",conn_num);
        USB_TX_SOF_ENABLE_REG(conn_num) = 1;
        USB_TX_LINE_CONTROL_REG(conn_num) = 0x00;   // LOW SPEED
    } else if (d == 2) {
        INFOPRINT("\nconn:%d FULL_SPEED_CONNECT\n",conn_num);
        USB_TX_SOF_ENABLE_REG(conn_num) = 1;
        USB_TX_LINE_CONTROL_REG(conn_num) = 0x18;   // FULL SPEED
    } else INFOPRINT("\nconn:%d DISCONNECT\n",conn_num);

    return (d==1) ? 0 : 1;
}

void full_speed_mode(int conn_num) {
  USB_TX_LINE_CONTROL_REG(conn_num) = 0x18;
}

void hub_low_speed_mode(int conn_num) {
  USB_TX_LINE_CONTROL_REG(conn_num) = 0x8;
}

en_usb_status ul_control_setup_transaction(int conn_num,st_usb_device *dev,st_usb_setup_data *sd){
  int i;
  unsigned int status;
  int num_of_tries;
  USB_TX_ADDR_REG(conn_num) = dev->info.address;
  USB_TX_ENDP_REG(conn_num) = 0;
  USB_TX_TRANS_TYPE_REG(conn_num) = 0;  // SETUP TRANS
  // start transaction
  status = 0;
  num_of_tries =  0;
  DBGPRINT("CONTROL_SETUP %d\n", dev->info.address);
  while (status != RS_ACK_RXED) {
    // clear TX FIFO
    USB_TX_FIFO_CONTROL_REG(conn_num) = 1; 
    for (i = 0; i<8; i++) USB_TX_FIFO_DATA(conn_num) = sd->PACKED[i]; // set data
    if(dev->info.preamble_enable) {
      hub_low_speed_mode(conn_num);
      USB_CONTROL_REG(conn_num) = 7;  // with SOF
    } else
      USB_CONTROL_REG(conn_num) = 3;
    // wait int
    status = wait_int_event(conn_num,IS_TRANS_DONE);
    if (status == USBH_ERROR) {
      TMPRINT("\ncontrol setup transaction timeout %x\n",dev->info.address);
      return status;
    }
    DBGPRINT("control setup trans done\r\n");
    status = USB_RX_CONNECT_STATE_REG(conn_num);
    DBGPRINT("\nRX CONNECT STATUS %x %d\n", status,dev->info.address);
    status = USB_RX_STATUS_REG(conn_num);
    DBGPRINT("\nRX STATUS %x\n", status);
    if (status != RS_ACK_RXED) {
      ul_timer(1);
      num_of_tries++;
    }
    if ( num_of_tries >= USBH_TRIAL_LOOP_CNT) break;
  }
  if(dev->info.preamble_enable) full_speed_mode(conn_num);
  if (num_of_tries >= USBH_TRIAL_LOOP_CNT) {
    TMPRINT("\ncontrol setup transaction timeout %x\n",dev->info.address);
    return status;
  } else return USBH_OK;
}

en_usb_status ul_control_in_transaction(int conn_num,st_usb_device *dev,int size, unsigned char *data) {
  int i,cur_size,fnum = 0;
  en_rx_status status =  RS_ACK_RXED;
  int num_of_tries;
  USB_TX_ADDR_REG(conn_num) = dev->info.address;
  USB_TX_ENDP_REG(conn_num) = 0;
  USB_TX_TRANS_TYPE_REG(conn_num) = 1;  // IN TRANS
  cur_size = 0;
  num_of_tries =  0;
  DBGPRINT("CONTROL_IN %d %d \n", dev->info.address,size);
  while (cur_size < size) {
    if(dev->info.preamble_enable) {
      hub_low_speed_mode(conn_num);
      USB_CONTROL_REG(conn_num) = 5;
    } else
      USB_CONTROL_REG(conn_num) = 1;
    // wait int
    status = wait_int_event(conn_num,IS_TRANS_DONE);
    if (status == USBH_ERROR) {
      TMPRINT("\ncontrol in transaction timeout\n");
      return USBH_ERROR;
    }
    status = USB_RX_STATUS_REG(conn_num);
    DBGPRINT("\nCONTROL_IN stat %x\n", status);
    // get IN data
    fnum = USB_RX_FIFO_DATA_COUNT_LSB(conn_num);
    for (i = 0; i<fnum; i++) {
      *(data+cur_size+i) = USB_RX_FIFO_DATA(conn_num);
      DBGPRINT("\nCONTROL_IN data %d = %x\n", i, *(data+i));
    }
    cur_size += fnum;
    if (status == RS_NAK_RXED) {
      ul_timer(1); // wait 1ms
      num_of_tries++;
      if (num_of_tries >= USBH_TRIAL_LOOP_CNT) break;
    }
  }
  if(dev->info.preamble_enable) full_speed_mode(conn_num);
  DBGPRINT("\ndone status = %x %d %d\n",status,cur_size,fnum);
  if (num_of_tries >= USBH_TRIAL_LOOP_CNT) {
    TMPRINT("\ncontrol in transaction timeout\n");
    return status;
  } else return USBH_OK;
}

en_usb_status ul_control_out_transaction(int conn_num,st_usb_device *dev,int size, unsigned char *data) {
  int i,data_type,cur_size,ts;
  unsigned int status;
  int num_of_tries;
  DBGPRINT("\nControl out a:%x size %d\n",dev->info.address,size);
  USB_TX_ADDR_REG(conn_num) = dev->info.address;
  USB_TX_ENDP_REG(conn_num) = 0;

  // start transaction
  cur_size = 0;
  num_of_tries =  0;
  data_type = 1; // start from DATA1
  while (cur_size < size) {
    USB_TX_TRANS_TYPE_REG(conn_num) = (data_type) ? 3 : 2;
    status = 0;
    ts = ((size - cur_size) > 8) ?
          8 : (size - cur_size);  // check endpoint size 8
    // divied into endpoint buffer size
    while (status != RS_ACK_RXED) { 
      for (i = 0; i<ts; i++) {
        USB_TX_FIFO_DATA(conn_num) = *(data+cur_size+i); // set data again
        DBGPRINT("\nControl out dt %x\n",*(data+cur_size+i));
      }
      if (dev->info.preamble_enable) {
        hub_low_speed_mode(conn_num);
        USB_CONTROL_REG(conn_num) = 5;
      } else
        USB_CONTROL_REG(conn_num) = 1;
      // wait int
      status = wait_int_event(conn_num,IS_TRANS_DONE);
      if (status == USBH_ERROR) {
        TMPRINT("\ncontrol out transaction timeout\n");
        return USBH_ERROR;
      }
      status = USB_RX_STATUS_REG(conn_num);
      DBGPRINT("\nControl out stat %x\n",status);
      if (status != RS_ACK_RXED) {
        ul_timer(1);  // wait 1ms
        num_of_tries++;
      } else num_of_tries = 0;
      if ( num_of_tries >= USBH_TRIAL_LOOP_CNT) break;
    }
    if(data_type ==0) data_type = 1;
    else data_type = 0;
    cur_size += ts;
        if (dev->info.preamble_enable) full_speed_mode(conn_num);
    if (num_of_tries >= USBH_TRIAL_LOOP_CNT) {
      TMPRINT("\nControl out transaction timeout %x\n",status);
      return status;
    }
  }
  return USBH_OK;
}

en_usb_status ul_control_ack_out_transaction(int conn_num,st_usb_device *dev){
  unsigned int status;
  int num_of_tries;
  USB_TX_ADDR_REG(conn_num) = dev->info.address;
  USB_TX_ENDP_REG(conn_num) = 0;
  USB_TX_TRANS_TYPE_REG(conn_num) = 3;  // OUT TRANS DATA1 only used
  status = 0;
  num_of_tries =  0;
  while (status != RS_ACK_RXED) {
    if (dev->info.preamble_enable) {
      hub_low_speed_mode(conn_num);
      USB_CONTROL_REG(conn_num) = 5;
    } else
      USB_CONTROL_REG(conn_num) = 1;
    // wait int
    status = wait_int_event(conn_num,IS_TRANS_DONE);
    if (status == USBH_ERROR) {
      TMPRINT("\ncontrol ack out transaction timeout\n");
      return USBH_ERROR;
    }
    status = USB_RX_STATUS_REG(conn_num);
    DBGPRINT("\nACK OUT trans status %x\n",status);
    if (status != RS_ACK_RXED) {
      ul_timer(1);
      num_of_tries++;
    }
    if ( num_of_tries >= USBH_TRIAL_LOOP_CNT) break;
  }
  if (dev->info.preamble_enable) full_speed_mode(conn_num);
  if (num_of_tries >= USBH_TRIAL_LOOP_CNT) {
    TMPRINT("\nack out transaction timeout\n");
    return status;
  } else return USBH_OK;
}

en_usb_status ul_control_ack_in_transaction(int conn_num,st_usb_device *dev) {
  unsigned int status;
  int num_of_tries;
  USB_TX_ADDR_REG(conn_num) = dev->info.address;
  USB_TX_ENDP_REG(conn_num) = 0;
  USB_TX_TRANS_TYPE_REG(conn_num) = 1;  // IN TRANS
  status = 0;
  num_of_tries =  0;
  while (!(status == RS_DATA_SEQUENCE)) { 
    if (dev->info.preamble_enable) {
      hub_low_speed_mode(conn_num);
      USB_CONTROL_REG(conn_num) = 5;
    } else
      USB_CONTROL_REG(conn_num) = 1;
    // wait int
    status = wait_int_event(conn_num,IS_TRANS_DONE);
    if (status == USBH_ERROR) {
      TMPRINT("\ncontrol ack in transaction timeout\n");
      return status;
    }
    status = USB_RX_STATUS_REG(conn_num);
    if (status != RS_DATA_SEQUENCE) {
      ul_timer(1);
      num_of_tries++;
        }
    if ( num_of_tries >= USBH_TRIAL_LOOP_CNT) break;
  }
  if (dev->info.preamble_enable) full_speed_mode(conn_num);
  DBGPRINT("\nACK IN trans done %x\n",status);
  if (num_of_tries >= USBH_TRIAL_LOOP_CNT) {
    TMPRINT("\nack in transaction timeout %x\n",status);
    return status;
  } else return USBH_OK;
}

// interrupt transfer(one or more transactions)
en_usb_status ul_interrupt_in_transfer(int conn_num,
                             st_usb_device *dev,
                             int *size,
                             unsigned char *data) {
  int i, fnum = 0;
  en_rx_status status;

  USB_TX_ADDR_REG(conn_num) = dev->info.address;
  USB_TX_ENDP_REG(conn_num) =
         dev->info.interface[dev->info.cur_interface].ea_in;
  USB_TX_TRANS_TYPE_REG(conn_num) = 1;  // IN TRANS

  USB_RX_FIFO_CONTROL_REG(conn_num) = 1;  // clear RX FIFO
  DBGPRINT("\nInterrupt in transfer %d %d %d\n",dev->info.address,
    dev->info.interface[dev->info.cur_interface].ea_in,dev->info.preamble_enable);
  if (dev->info.preamble_enable) {
    hub_low_speed_mode(conn_num);
    USB_CONTROL_REG(conn_num) = 5;
  } else
    USB_CONTROL_REG(conn_num) = 1;
  // wait int
  status = wait_int_event(conn_num,IS_TRANS_DONE);
  if (status == USBH_ERROR) {
    TMPRINT("\ninterrupt in transfer timeout\n");
    return USBH_ERROR;
  }
  status = USB_RX_STATUS_REG(conn_num);
  // get IN data
  fnum = USB_RX_FIFO_DATA_COUNT_LSB(conn_num);
  for (i = 0; i<fnum; i++) {
    *(data+i) = USB_RX_FIFO_DATA(conn_num);
  }
  *size = fnum;
  if (dev->info.preamble_enable) full_speed_mode(conn_num);
  return status;
}

en_usb_status ul_interrupt_out_transfer(int conn_num,
                              st_usb_device *dev,
                              int size,
                              unsigned char* data) {
  int i,cur_size,ts;
  unsigned int status;
  int num_of_tries;
  DBGPRINT("\nInterrupt out a:%x e:%x size %d\n",dev->info.address,
    dev->info.interface[dev->info.cur_interface].ea_out,size);
  USB_TX_ADDR_REG(conn_num) = dev->info.address;
  USB_TX_ENDP_REG(conn_num) = dev->info.interface[dev->info.cur_interface].ea_out;

  // start transaction
  cur_size = 0;
  num_of_tries =  0;
  while (cur_size < size) {
    USB_TX_TRANS_TYPE_REG(conn_num) =
      (dev->info.interface[dev->info.cur_interface].out_data_type) ? 3 : 2;
    status = 0;
    ts = ((size - cur_size) > dev->info.interface[dev->info.cur_interface].ea_out_max_packet_size) ?
          dev->info.interface[dev->info.cur_interface].ea_out_max_packet_size : (size - cur_size);  // check endpoint size 8,16,32,64
    // divied into endpoint buffer size
    while (status != RS_ACK_RXED) { 
      for (i = 0; i<ts; i++) {
        USB_TX_FIFO_DATA(conn_num) = *(data+cur_size+i); // set data again
      }
      USB_CONTROL_REG(conn_num) = 1;
      // wait int
      status = wait_int_event(conn_num,IS_TRANS_DONE);
      if (status == USBH_ERROR) {
        TMPRINT("\nInterrupt out transfer timeout\n");
        return USBH_ERROR;
      }
      status = USB_RX_STATUS_REG(conn_num);
      if (status != RS_ACK_RXED) {
        ul_timer(1);  // wait 1ms
        num_of_tries++;
      } else num_of_tries = 0;
      if ( num_of_tries >= USBH_TRIAL_LOOP_CNT) break;
    }
    if(dev->info.interface[dev->info.cur_interface].out_data_type ==0)
      dev->info.interface[dev->info.cur_interface].out_data_type = 1;
    else dev->info.interface[dev->info.cur_interface].out_data_type = 0;
    cur_size += ts;
    if (num_of_tries >= USBH_TRIAL_LOOP_CNT) {
      TMPRINT("\nInterrupt out transfer timeout\n");
      return status;
    }
  }
  return USBH_OK;
}

#ifdef USBH_USE_MSD
// bulk transfer(one or more transactions)
en_usb_status ul_bulk_out_transfer(int conn_num,st_usb_device *dev, int size, unsigned char* data) {
  int i,cur_size,ts;
  unsigned int status;
  int num_of_tries;
  DBGPRINT("\nBulk out a:%x e:%x size %d dt %d\n",
             dev->info.address,
             dev->info.interface[dev->info.cur_interface].ea_out,size,
             dev->info.interface[dev->info.cur_interface].out_data_type);
  USB_TX_ADDR_REG(conn_num) = dev->info.address;
  USB_TX_ENDP_REG(conn_num) =
             dev->info.interface[dev->info.cur_interface].ea_out;

  // start transaction
  cur_size = 0;
  num_of_tries =  0;
  while (cur_size < size) {
    USB_TX_TRANS_TYPE_REG(conn_num) =
      (dev->info.interface[dev->info.cur_interface].out_data_type) ? 3 : 2;

    status = 0;
    ts = ((size - cur_size) > dev->info.interface[dev->info.cur_interface].ea_out_max_packet_size) ?
          dev->info.interface[dev->info.cur_interface].ea_out_max_packet_size : (size - cur_size);  // check endpoint size 8,16,32,64
    DBGPRINT("blkout type %d\n",USB_TX_TRANS_TYPE_REG(conn_num));
    // divied into endpoint buffer size
    while (status != RS_ACK_RXED) { 
      for (i = 0; i<ts; i++) {
        USB_TX_FIFO_DATA(conn_num) = *(data+cur_size+i); // set data again
        DBGPRINT("BOD: %d %x\n",i,*(data+i));
      }
      USB_CONTROL_REG(conn_num) = 1;
      // wait int
      status = wait_int_event(conn_num,IS_TRANS_DONE);
      if (status == USBH_ERROR) {
        TMPRINT("\nbulk out transfer timeout\n");
        return USBH_ERROR;
      }
      status = USB_RX_STATUS_REG(conn_num);
      DBGPRINT("\nBULK_OUT RX STATUS %x %d\n", status,
        dev->info.interface[dev->info.cur_interface].out_data_type);
      if (status == RS_STALL_RXED) {
        DBGPRINT("\nBULK_OUT RX STALL %x %d\n", status,
          dev->info.interface[dev->info.cur_interface].out_data_type);
        return status;
      } else if (status != RS_ACK_RXED) {
        ul_timer(1);  // wait
        num_of_tries++;
      } else num_of_tries = 0;
      if ( num_of_tries >= USBH_TRIAL_LOOP_CNT) break;
    }
    if(dev->info.interface[dev->info.cur_interface].out_data_type ==0)
      dev->info.interface[dev->info.cur_interface].out_data_type = 1;
    else dev->info.interface[dev->info.cur_interface].out_data_type = 0;
    cur_size += ts;
    if (num_of_tries >= USBH_TRIAL_LOOP_CNT) {
      TMPRINT("\nbulk out transfer timeout\n");
      return status;
    }
  }
  return USBH_OK;
}

en_usb_status ul_bulk_in_transfer(int conn_num,st_usb_device *dev, int size, unsigned char* data){
  int i,cur_size,fnum = 0;
  en_rx_status status = RS_ACK_RXED;
  int num_of_tries;

  USB_TX_ADDR_REG(conn_num) = dev->info.address;
  USB_TX_ENDP_REG(conn_num) =
    dev->info.interface[dev->info.cur_interface].ea_in;
  USB_TX_TRANS_TYPE_REG(conn_num) = 1;  // IN TRANS
  DBGPRINT("\nbulk in trans %x %x %d\n",dev->info.address,
           dev->info.interface[dev->info.cur_interface].ea_in,size);
  cur_size = 0;
  num_of_tries =  0;
  while (cur_size < size) {
    USB_CONTROL_REG(conn_num) = 1;
    // wait int
    status = wait_int_event(conn_num,IS_TRANS_DONE);
    if (status == USBH_ERROR) {
      TMPRINT("\nbulk in transfer timeout\n");
      return USBH_ERROR;
    }
    status = USB_RX_STATUS_REG(conn_num);
    DBGPRINT("\nBULK_IN RX STATUS %x \n", status);
    // get IN data
    fnum = USB_RX_FIFO_DATA_COUNT_LSB(conn_num);
    for (i = 0; i<fnum; i++) {
      *(data+cur_size+i) = USB_RX_FIFO_DATA(conn_num);
      DBGPRINT("\nBULK_IN data %d = %x\n", i, *(data+i));
    }
    cur_size += fnum;
        if (status == RS_STALL_RXED) {
          TMPRINT("\nbulk in transfer stalled %d\n",cur_size);
          return status;
    } else if (status == RS_NAK_RXED) {
      if (num_of_tries >= USBH_TRIAL_LOOP_CNT) break; 
      ul_timer(16); // wait 1ms
      num_of_tries++;
    } else num_of_tries = 0;
         
  }
  DBGPRINT("\ndone status = %x %d %d %d\n",status,cur_size,fnum,num_of_tries);
  if (num_of_tries >= USBH_TRIAL_LOOP_CNT) {
    TMPRINT("\nbulk in transfer timeout %d data available\n",cur_size);
    return status;
  } else return USBH_OK;
}

#endif



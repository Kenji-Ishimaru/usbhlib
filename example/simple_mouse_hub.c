/*
 * Simple Mouse Example with device attach/detach callback.
 *
 *
 */

#include <stdio.h>
#include "usbh_hw_dep_include.h"
#include "usbh_env.h"

// mouse callback
void on_mouse(int buttons, int x, int y, int z) {
  MPRINT("mouse changed %x %x %x %x\r\n",buttons,x,y,z);
}
#ifdef USBH_USE_HUB
#ifdef USBH_USE_HUB_CALLBACK
void on_dev_attached(int conn_num, st_usb_device *dev,int port_num) {
  MPRINT("***Device attach callback function\n");
  MPRINT("***Device is attached on HUB port %d\n",port_num);
  MPRINT("***  Vendor  ID: %02x%02x\n",
		  dev->p_devices[port_num]->descs.device.BYTE.idVendor[1],
		  dev->p_devices[port_num]->descs.device.BYTE.idVendor[0]);
  MPRINT("***  Profuct ID: %02x%02x\n",
		  dev->p_devices[port_num]->descs.device.BYTE.idProduct[1],
		  dev->p_devices[port_num]->descs.device.BYTE.idProduct[0]);
}
void on_dev_detached(int conn_num, st_usb_device *dev,int port_num) {
  MPRINT("***Device detach callback function\n");
  MPRINT("***Device is detached from HUB port %d\n",port_num);
  MPRINT("***  Vendor  ID: %02x%02x\n",
		  dev->p_devices[port_num]->descs.device.BYTE.idVendor[1],
		  dev->p_devices[port_num]->descs.device.BYTE.idVendor[0]);
  MPRINT("***  Profuct ID: %02x%02x\n",
		  dev->p_devices[port_num]->descs.device.BYTE.idProduct[1],
		  dev->p_devices[port_num]->descs.device.BYTE.idProduct[0]);
}
#endif
#endif

int main()
{
  MPRINT("Simple Mouse Example\n");
  uh_init();

  uh_mouse_attach_func(on_mouse);
#ifdef USBH_USE_HUB
#ifdef USBH_USE_HUB_CALLBACK
  uh_hub_at_attach_func(on_dev_attached);
  uh_hub_dt_attach_func(on_dev_detached);
#endif
#endif

  while(1) {
    uh_update();
    ul_timer(1);
  }
  return 0;
}

/*
 * Simple Mouse Example.
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

int main()
{
  MPRINT("Simple Mouse Example\n");
  uh_init();

  uh_mouse_attach_func(on_mouse);

  while(1) {
    uh_update();
    ul_timer(1);
  }
  return 0;
}

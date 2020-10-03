/*
 * Simple Keyboard Example.
 *
 *
 */

#include <stdio.h>
#include "usbh_hw_dep_include.h"
#include "usbh_env.h"

// keyboard call back
void on_keyboard(int key, int modifier) {
  if (key != 0) MPRINT("key changed %c %x\r\n",key,modifier);
}

int main()
{
  MPRINT("Simple Keyboard Example\n");
  uh_init();

  uh_keyboard_attach_func(on_keyboard);

  while(1) {
    uh_update();
    ul_timer(1);
  }
  return 0;
}


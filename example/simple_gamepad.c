/*
 * Simple Gamepad Example.
 *
 *
 */

#include <stdio.h>
#include "usbh_hw_dep_include.h"
#include "usbh_env.h"

// gamepad callback
void on_gamepad(int buttons, int x, int y) {
  static int initial_flag = 1;
  static int pb = 0, px = 0, py = 0;
  if (!initial_flag) {
    if ((pb != buttons)||(px != x)||(py!=y))
    MPRINT("gamepad changed %x %x %x\r\n",buttons,x,y);
  }
  initial_flag =0;
  pb = buttons; px = x; py = y;
}

int main()
{
  MPRINT("Simple Gamepad Example\n");
  uh_init();

  uh_gamepad_attach_func(on_gamepad);
  while(1) {
    uh_update();
    ul_timer(1);
  }
  return 0;
}


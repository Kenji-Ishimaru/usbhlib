/*
 * USB Host example
 *   HID: mouse, keyboard, gamepad
 *   MSC Bulk-only transfer with FatFs
 */

#include <stdio.h>
#include "usbh_hw_dep_include.h"
#include "usbh_env.h"
#include "ff.h"

FATFS Fatfs;    /* File system object */
FIL Fil;        /* File object */
BYTE Buff[128]; /* File read buffer */

/* File object */
void die (      /* Stop with dying message */
  FRESULT rc    /* FatFs return value */
)
{
  switch(rc) {
  case FR_OK:   /* 0 */
    break;
  case FR_DISK_ERR:     /* 1 */
    MPRINT("Failed with FR_DISK_ERR.\n");
    break;
  case FR_INT_ERR:      /* 2 */
    MPRINT("Failed with FR_INT_ERR.\n");
    break;
  case FR_NOT_READY:    /* 3 */
    MPRINT("Failed with FR_NOT_READY.\n");
    break;
  case FR_NO_FILE:      /* 4 */
          MPRINT("Failed with FR_NO_FILE.\n");
          break;
  case FR_NO_PATH:      /* 5 */
          MPRINT("Failed with FR_NO_PATH.\n");
          break;
  case FR_INVALID_NAME: /* 6 */
          MPRINT("Failed with FR_INVALID_NAME.\n");
          break;
  case FR_DENIED:       /* 7 */
          MPRINT("Failed with FR_DENIED.\n");
          break;
  case FR_EXIST:        /* 8 */
          MPRINT("Failed with FR_EXIST.\n");
          break;
  case FR_INVALID_OBJECT:/* 9 */
          MPRINT("Failed with FR_INVALID_OBJECT.\n");
          break;
  case FR_WRITE_PROTECTED:/* 10 */
          MPRINT("Failed with FR_WRITE_PROTECTED.\n");
          break;
  case FR_INVALID_DRIVE:  /* 11 */
          MPRINT("Failed with FR_INVALID_DRIVE.\n");
          break;
  case FR_NOT_ENABLED:    /* 12 */
          MPRINT("Failed with FR_NOT_ENABLED.\n");
          break;
  case FR_NO_FILESYSTEM:  /* 13 */
          MPRINT("Failed with FR_NO_FILESYSTEM.\n");
          break;
  case FR_MKFS_ABORTED:   /* 14 */
          MPRINT("Failed with FR_MKFS_ABORTED.\n");
          break;
  case FR_TIMEOUT:        /* 15 */
          MPRINT("Failed with FR_TIMEOUT.\n");
          break;
  case FR_LOCKED:         /* 16 */
          MPRINT("Failed with FR_LOCKED.\n");
          break;
  case FR_NOT_ENOUGH_CORE:/* 17 */
          MPRINT("Failed with FR_NOT_ENOUGH_CORE.\n");
          break;
  case FR_TOO_MANY_OPEN_FILES:/* 18 */
          MPRINT("Failed with FR_TOO_MANY_OPEN_FILES.\n");
          break;
  case FR_INVALID_PARAMETER:  /* 19 */
          MPRINT("Failed with FR_INVALID_PARAMETER.\n");
          break;
  }
}

void dir() {
  FRESULT rc;  /* Result code */
  DIR dir;     /* Directory object */
  FILINFO fno; /* File information object */
  UINT bw;

  //File system test
  f_mount(0, &Fatfs);
  MPRINT("\nOpen root directory.\n");
  rc = f_opendir(&dir, "");
  if (rc) die(rc);

  MPRINT("\nDirectory listing...\n");
  for (;;) {
    rc = f_readdir(&dir, &fno);      /* Read a directory item */
    if (rc || !fno.fname[0]) break;  /* Error or end of dir */
    if (fno.fattrib & AM_DIR)
      MPRINT("   <dir>  %s\n", fno.fname);
    else
      MPRINT("%8lu  %s\n", fno.fsize, fno.fname);
  }
  if (rc) die(rc);
}

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
// keyboard call back
void on_keyboard(int key, int modifier) {
  if (key != 0) MPRINT("key changed %c %x\r\n",key,modifier);
}
// mouse callback
void on_mouse(int buttons, int x, int y, int z) {
  MPRINT("mouse changed %x %x %x %x\r\n",buttons,x,y,z);
}

int main()
{
  MPRINT("usb host example\n");
  uh_init();

  uh_gamepad_attach_func(on_gamepad);
  uh_keyboard_attach_func(on_keyboard);
  uh_mouse_attach_func(on_mouse);
  if (uh_msd_available(0))
    dir();  // directory listing
  else MPRINT("MSD not found.");
  while(1) {
    uh_update();
    ul_timer(1);
  }
  return 0;
}


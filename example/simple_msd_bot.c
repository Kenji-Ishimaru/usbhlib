/*
 * Simple Mass Storage Device Example.
 *   with FatFs
 *
 */

#include <stdio.h>
#include "usbh_hw_dep_include.h"
#include "usbh_env.h"
#include "ff.h"

FATFS Fatfs;            /* File system object */
FIL Fil;                /* File object */
BYTE Buff[128];         /* File read buffer */

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
  for (;;) ;
}

int main()
{
  FRESULT rc;     /* Result code */
  DIR dir;        /* Directory object */
  FILINFO fno;    /* File information object */
  UINT bw, br;
  int i;

  uh_init();

  f_mount(0, &Fatfs);  /* Register volume work area (never fails) */
  MPRINT("\nOpen an existing file (message.txt).\n");
  rc = f_open(&Fil, "MESSAGE.TXT", FA_READ);
  if (rc) die(rc);

  MPRINT("\nType the file content.\n");
  for (;;) {
    rc = f_read(&Fil, Buff, sizeof Buff, &br);/* Read a chunk of file */
    if (rc || !br) break;             /* Error or end of file */
    for (i = 0; i < br; i++)          /* Type the data */
      putchar(Buff[i]);
    }
    if (rc) die(rc);

    MPRINT("\nClose the file.\n");
    rc = f_close(&Fil);
    if (rc) die(rc);

    MPRINT("\nCreate a new file (hello.txt).\n");
    rc = f_open(&Fil, "HELLO.TXT", FA_WRITE | FA_CREATE_ALWAYS);
    if (rc) die(rc);

    MPRINT("\nWrite a text data. (Hello world!)\n");
    rc = f_write(&Fil, "Hello world!\r\n", 14, &bw);
    if (rc) die(rc);
    MPRINT("%u bytes written.\n", bw);

    MPRINT("\nClose the file.\n");
    rc = f_close(&Fil);
    if (rc) die(rc);


    MPRINT("\nOpen root directory.\n");
    rc = f_opendir(&dir, "");
    if (rc) die(rc);

    MPRINT("\nDirectory listing...\n");
    for (;;) {
      rc = f_readdir(&dir, &fno);       /* Read a directory item */
      if (rc || !fno.fname[0]) break;   /* Error or end of dir */
      if (fno.fattrib & AM_DIR)
        MPRINT("   <dir>  %s\n", fno.fname);
      else
        MPRINT("%8lu  %s\n", fno.fsize, fno.fname);
    }
    if (rc) die(rc);
    MPRINT("\nTest completed.\n");

    while(1) {
      uh_update();
    }

    return 0;
}


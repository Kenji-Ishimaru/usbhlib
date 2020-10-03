/*
 * Simple Mass Storage Device Example.
 *   with Petit FatFs
 *
 */

#include <stdio.h>
#include "usbh_hw_dep_include.h"
#include "usbh_env.h"
#include "pff.h"


/* File object */
void die (              /* Stop with dying message */
        FRESULT rc      /* FatFs return value */
)
{
  switch(rc) {
  case FR_OK:
          break;
  case FR_DISK_ERR:     /* 1 */
          MPRINT("Failed with FR_DISK_ERR.\n");
          break;
  case FR_NOT_READY:    /* 2 */
          MPRINT("Failed with FR_NOT_READY.\n");
          break;
  case FR_NO_FILE:      /* 3 */
          MPRINT("Failed with FR_NO_FILE.\n");
          break;
  case FR_NO_PATH:       /* 4 */
          MPRINT("Failed with FR_NO_PATH.\n");
          break;
  case FR_NOT_OPENED:    /* 5 */
          MPRINT("Failed with FR_NOT_OPENED.\n");
          break;
  case FR_NOT_ENABLED:   /* 6 */
          MPRINT("Failed with FR_NOT_ENABLED.\n");
          break;
  case FR_NO_FILESYSTEM: /* 7 */
          MPRINT("Failed with FR_NO_FILESYSTEM.\n");
          break;

  }

  for (;;) ;
}


int main()
{
  FATFS fatfs;     /* File system object */
  DIR dir;         /* Directory object */
  FILINFO fno;     /* File information object */
  WORD bw, br, i;
  BYTE buff[64];
  FRESULT rc;

  uh_init();

  MPRINT("\nMount a volume.\n");
  rc = pf_mount(&fatfs);
  if (rc) die(rc);

  MPRINT("\nOpen a test file (message.txt).\n");
  rc = pf_open("MESSAGE.TXT");
  if (rc) die(rc);

  MPRINT("\nType the file content.\n");
  for (;;) {
    rc = pf_read(buff, sizeof(buff), &br);  /* Read a chunk of file */
    if (rc || !br) break;                   /* Error or end of file */
    for (i = 0; i < br; i++)                /* Type the data */
      putchar(buff[i]);
  }
  if (rc) die(rc);

#if _USE_WRITE
  MPRINT("\nOpen a file to write (write.txt).\n");
  rc = pf_open("WRITE.TXT");
  if (rc) die(rc);

  MPRINT("\nWrite a text data. (Hello world!)\n");
  for (;;) {
    rc = pf_write("Hello world!\r\n", 14, &bw);
    if (rc || !bw) break;
  }
  if (rc) die(rc);
  MPRINT("\nTerminate the file write process.\n");
  rc = pf_write(0, 0, &bw);
  if (rc) die(rc);
#endif

#if _USE_DIR
  MPRINT("\nOpen root directory.\n");
  rc = pf_opendir(&dir, "");
  if (rc) die(rc);

  MPRINT("\nDirectory listing...\n");
  for (;;) {
    rc = pf_readdir(&dir, &fno);    /* Read a directory item */
    if (rc || !fno.fname[0]) break; /* Error or end of dir */
    if (fno.fattrib & AM_DIR)
      MPRINT("   <dir>  %s\n", fno.fname);
    else
      MPRINT("%8lu  %s\n", fno.fsize, fno.fname);
  }
  if (rc) die(rc);
#endif

   MPRINT("\nTest completed.\n");
   while(1) {
     uh_update();
   }

   return 0;
}


#ifndef CSE_486_MPEG_H
#define CSE_486_MPEG_H

extern "C"{ 
#define INDEXON
#include "mpeg.h"

#include <libgen.h>
#include <string.h>
#include <strings.h>
#include <malloc.h>


typedef struct mpegframe {
  struct xbgr *xbgrbuffer; /* row-col XBGR buffer of frame image */
  char codetype; /* 'b', 'i', or 'p' */
  short int *dctbuffer; /* dctcoeffs, if they want them */
  int *frmotion, *fdmotion, *brmotion, *bdmotion; /* buffer of motion
                                                         vectors; */
  short int *crbuffer;
  int width, height;
  char *mbtypebuffer; /* buffer of macroblock types ('b', 'i', or 'p') */
  int imbcount;
  int frameno;
} mpegframe;


typedef struct mpeginfo {
  ImageDesc img;
  char *fname, *iname;
} mpeginfo;

typedef struct xbgr {
  char x;
  char b;
  char g;
  char r;
} xbgr;


void mpegClose(mpeginfo *);
int mpegGotoFrame(mpeginfo *mi, int frame_num);
void mpegDestroyFrame(mpegframe *);
mpegframe *mpegGetNextFrame(mpeginfo *mi);
mpeginfo *mpegOpen(char *filename);
int WriteSGIImage(xbgr* buf, int width, int ht, char *imfilename);
mpegframe* ReadSGIImage(char *imfilename);
}

#endif

/* ----------------------------- MNI Header -----------------------------------
@NAME       : mpeg.h
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: Types and function prototypes needed for applications to
              use the Berkely MPEG decoding engine via the MNI front
              end.
@METHOD     : 
@GLOBALS    : Types defined:
                 ImageDesc  - structure giving height, width, etc.
                 DitherEnum - the different dither types supported by
                              the decoding engine
@CALLS      : 
@CREATED    : Greg Ward, 94/6/16.
@MODIFIED   : Greg Ward, 94/9/12 (based on John Cristy's fixes): made
              more amenable to use with other libraries that also
	      happen to define TRUE, FALSE, [Bb]oolean, and added
	      PROTO macro
---------------------------------------------------------------------------- */

#ifndef __MPEG_H
#define __MPEG_H

#include <stdio.h>

/* An attempt at a portable and integrable boolean type... */

#if (!defined(TRUE) || !defined(FALSE))
# define TRUE 1
# define FALSE 0
#endif

#if (!defined (BOOLEAN_TYPE_EXISTS))
/*typedef unsigned int Boolean;*/
#ifndef __GL_GL_H__
typedef long Boolean;
#endif
#endif

typedef struct
{
   short red, green, blue;
} ColormapEntry;

typedef struct
{
   int	Height;			/* in pixels */
   int  Width;
   int  Depth;			/* image depth (bits) */
   int  PixelSize;              /* bits actually stored per pixel */
   int  Size;			/* bytes for whole image */
   int  BitmapPad;              /* "quantum" of a scanline -- each scanline */
				/* starts on an even interval of this */
                                /* many bits */
   int  PictureRate;		/* required number of frames/sec [?] */
   int  BitRate;		/* ??? */
   
   int  ColormapSize;
   ColormapEntry *Colormap;	/* an array of ColormapSize entries */
} ImageDesc;

typedef enum 
{
   HYBRID_DITHER,
   HYBRID2_DITHER,
   FS4_DITHER,
   FS2_DITHER,
   FS2FAST_DITHER,
   Twox2_DITHER,
   GRAY_DITHER,
   FULL_COLOR_DITHER,
   NO_DITHER,
   ORDERED_DITHER,
   MONO_DITHER,
   MONO_THRESHOLD,
   ORDERED2_DITHER,
   MBORDERED_DITHER
} DitherEnum;


typedef enum
{
   MPEG_DITHER,
   MPEG_QUIET,
   MPEG_LUM_RANGE,
   MPEG_CR_RANGE,
   MPEG_CB_RANGE,
   MPEG_CMAP_INDEX
} MPEGOptionEnum;

/* Kludge so we can compile under ANSI or K&R */

#undef PROTO
#if __STDC__
#define PROTO(formal_parameters) formal_parameters
#else
#define const
#define PROTO(formal_parameters) ()
#endif

/* Function prototypes (all are defined in wrapper.c) */

Boolean OpenMPEG PROTO((FILE *MPEGfile, ImageDesc *ImgInfo));
void    CloseMPEG PROTO((void));
Boolean RewindMPEG PROTO((FILE *MPEGfile, ImageDesc *Image));
void    SetMPEGOption PROTO((MPEGOptionEnum Option, int value));
Boolean GetMPEGFrame PROTO((char *Frame));

/* Following added by crandall */

#ifndef INDEXON
int SeekFrame(int frameno);
void SeekFrameNext(int frameno);
#else
int SeekFrame(int frameno,char *indexfile);
void SeekFrameNext(int frameno,char *indexfile);
#endif

void DisplayFrame(char *fname,int begframe,char *);

void PlayMpegFromTo(char *fname,int begframe,int endframe,char *);

void GetFrameBuffer(char *fname,int frame,char **bufptr,int *numrows,int *numcols);

void SeekWrapper(char *fname,int begframe,int endframe,int dispflag,char **pixelsret,int *numrows,int *numcols,char *);

void OpenMpegForStats(char *fname,ImageDesc *);

unsigned char * GetCurRgbBuffer();
unsigned char * GetCurCrBuffer();
unsigned char * GetCurCbBuffer();

int GetDCTBuffer(short int **buffer,int *rows, int *cols,int *codetype,char **, int *,int *,int **, int **, int **, int **);

int GetDCTBufferC(short int **lumbuffer,short int **crbuffer,int *rows, int *cols,int *codetype,char **, int *,int *,int **, int **, int **, int **);

#ifndef MPEGFUNDOTH
typedef struct {
   int y; 
   int cb;
   int cr;
} dc_coeff;
#endif

dc_coeff *ReturnReorderedDCTStruct(int flag);

/* These are types used in mbtypes param to the GetDCTBuffer functions
 * 
 * Note that this buffer used to return 0=non i frame, nonzero = i frame, which won't
 * work anymore. 
 */

/* These are defined as the characters (ifbtz) that boonlock expects.
 * 0 must be SKIPPED.
 */

#define MBTYPE_I              'i'
#define MBTYPE_MOTION_FORW    'f'
#define MBTYPE_MOTION_BACK    'b'
#define MBTYPE_MOTION_BOTH    't'
#define MBTYPE_MOTION_ZERO    'z'
#define MBTYPE_SKIPPED        0



#endif   /* __MPEG_H */

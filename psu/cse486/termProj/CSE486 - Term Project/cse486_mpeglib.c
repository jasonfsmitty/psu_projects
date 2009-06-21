#include "cse486_mpeglib.h"

/* mybasename(char *str)
 *
 * preconditions: none
 *
 * returns: assuming that str is a filename with a path it strips off
 *          all but the file name and returns the filename
 */

char* mybasename( char *str )
{
  int len;
  char *work, *hold = NULL;
  char *ptr;
  len = strlen(str);
  if (len >0) {
    work = (char *) malloc(len*sizeof(char));
    hold = (char *) malloc(len*sizeof(char));
    strcpy(work, str);
    ptr = strtok(work, "/");
    while(ptr){
      strcpy(hold, ptr);
      ptr = strtok(NULL, "/");
    }
  }  
  return hold;

}

/* mpegOpen()
 *
 * preconditions: filename must be a valid, accessible MPEG-1 video file
 *                and the current directory must be writable
 *
 * returns: a pointer which must be passed to future mpeg calls
 */

mpeginfo *mpegOpen(char *filename)
{
  mpeginfo *mi;

  mi=(mpeginfo *) malloc(sizeof(mpeginfo));
  
  mi->fname=(char *) malloc(strlen(filename)+2);
  mi->iname=(char *) malloc(strlen(filename)+10);
  strcpy(mi->fname,filename);
  strcpy(mi->iname,mybasename(filename));
  strcat(mi->iname,".index");

  OpenMpegForStats(mi->fname,&mi->img);

  return(mi);
}

__eprintf() {}


/* mpegClose()
 *
 * Closes the MPEG library
 *
 * preconditions: mi is a ptr to an mpeginfo returned by mpegOpen()
 */
void mpegClose(mpeginfo *mi)
{
  CloseMPEG();
}


/* mpegGotoFrame()
 *
 * Seeks to frame number frame_num (0-based frame numbers)
 *
 * preconditions: mi is a ptr to an mpeginfo returned by mpegOpen()
 *                
 * returns: -1 on failure (probably because the movie is shorter than
 *                         the frame_num supplied)
 */
int mpegGotoFrame(mpeginfo *mi, int frame_num)
{
  return(SeekFrame(frame_num, mi->iname));
}

/* mpegDestroyFrame()
 *
 * preconditions: mf is a frame returned by mpegGetNextFrame();
 *
 * This function dealloc's space associated with the frame 
 */
void mpegDestroyFrame(mpegframe *mf)
{
  free(mf->dctbuffer);
  free(mf->crbuffer);
  free(mf->mbtypebuffer);
  free(mf->frmotion);
  free(mf->fdmotion);
  free(mf->brmotion);
  free(mf->bdmotion);
  free(mf->xbgrbuffer);
  free(mf);
}

/* mpegGetNextFrame()
 *
 * preconditions: mi is a ptr to an mpeginfo returned by mpegOpen
 *
 * returns: a ptr to an mpegframe structure
 */

mpegframe *mpegGetNextFrame(mpeginfo *mi)
{
  mpegframe *mf;
  int *frmotion,*fdmotion,*brmotion,*bdmotion; /* motion vectors for frame*/
  int imbcount; /* # of i-coded mblocks in current frame */
  short int *dctbuffer, *crbuffer; /* dct coeffs for each block of cur frame */
  int codetype; /* coding type of cur frame */
  int frameno; /* cur frame number */
  char *mbtypebuffer; /* coding type of each mb in cur frame */
  int rows,cols; /* # of rows & cols of blocks in the movie */
  int pixels;

  mf=(mpegframe *) malloc(sizeof(mpegframe));


/* 
 * Call GetDCTBufferC(), which returns dct coeffs for lum and chrom blocks
 * (and a lot of other information). After this call, dctbuffer contains the 
 * lum coeffs, as a 3-dimensional array (macroblock rows on the x axis, cols 
 * on the y axis, and 4 lum blocks on the z axis. crbuffer is arranged in 
 * the same manner, but with the 2 chrominance dct coeffs on the z axis. 
 */

  if((GetDCTBufferC(&dctbuffer,&crbuffer,&rows,&cols,&codetype,&mbtypebuffer,&imbcount,&frameno,&frmotion,&fdmotion,&brmotion,&bdmotion)))
    {
      mf->height=rows*8;
      mf->width=cols*8;
      mf->codetype=codetype;
      mf->imbcount=imbcount;
      mf->frameno=frameno;

      pixels=rows*cols*64;

      mf->xbgrbuffer=(xbgr *) malloc(pixels*sizeof(xbgr));
      bcopy(GetCurRgbBuffer(),mf->xbgrbuffer,pixels*sizeof(xbgr));

      mf->dctbuffer=(short int *) malloc(pixels*sizeof(short int));
      bcopy(dctbuffer,mf->dctbuffer,pixels*sizeof(short int));

      mf->crbuffer=(short int *) malloc(pixels/2*sizeof(short int));
      bcopy(crbuffer,mf->crbuffer,pixels/2*sizeof(short int));

      mf->mbtypebuffer=(char *) malloc(pixels/256);
      bcopy(mbtypebuffer,mf->mbtypebuffer,pixels/256*sizeof(char));

      mf->frmotion=(int *) malloc(pixels/256);
      bcopy(frmotion,mf->frmotion,pixels/256);
      mf->fdmotion=(int *) malloc(pixels/256);
      bcopy(fdmotion,mf->fdmotion,pixels/256);
      mf->brmotion=(int *) malloc(pixels/256);
      bcopy(brmotion,mf->brmotion,pixels/256);
      mf->bdmotion=(int *) malloc(pixels/256);
      bcopy(bdmotion,mf->bdmotion,pixels/256);
      
      return(mf);
    }

  return(NULL);
}


/*
BEGIN_FUNCTION_DESC
WriteSGIImage(xbgr* buf, int width, int ht, char *imfilename)
Writes a XBGR format frame buffer to a file as an SGI RGB image.
END_FUNCTION_DESC
*/
int WriteSGIImage(xbgr* buf, int width, int ht, char *imfilename) 
{
  unsigned char bval, zero=0;
  unsigned short sval;
  int  i, j, k;
  unsigned long lval, p, row;
  FILE *imfile;
  char imagename[80];
  
  imfile = fopen(imfilename, "wb");
  if (imfile == NULL) {
    fprintf(stderr,"WriteSGIImage(): unable to open %s for writing\n",imfilename);
    return (-1);
  }
  sval = 474; /* magic number */
  fwrite(&sval, 2, 1, imfile);
  bval = 0; /* not rle, verbatim  */
  fwrite(&bval, 1, 1, imfile);
  bval = 1; /* bytes per component */
  fwrite(&bval, 1, 1, imfile);
  sval = 3; /* dimension = 3 - x,y,z*/
  fwrite(&sval, 2, 1, imfile);
  
  sval = width;
  fwrite(&sval, 2, 1, imfile);
  sval = ht;
  fwrite(&sval, 2, 1, imfile);
  sval = 3; /* 3 channels: z = r,g,b */
  fwrite(&sval, 2, 1, imfile);
  
  lval = 0; /* min pixel value */
  fwrite(&lval, 4, 1, imfile);
  lval = 255; /* max pixel value */
  fwrite(&lval, 4, 1, imfile);
  lval = 0; /* dummy */
  fwrite(&lval, 4, 1, imfile);
  
  strcpy(imagename, "Generated by WriteSGIImage() - output of video indexing");
  fwrite(imagename, 80, 1, imfile);
  
  lval = 0; /* color map */
  fwrite(&lval, 4, 1, imfile);
  
  bval = 0;
  for (i=0; i < 404; i+=1)
    fwrite(&bval, 1, 1, imfile); /* pad header to 512 */
  
  for (k=3; k > 0; k--) {   /* entire R image, then entire G, then B image */
    for (i=ht-1; i >= 0; i--) { /* lower left coordinate system */
      lval = i*width;
      for (j=lval; j < (lval+width); j++) {
	switch(k) {
	case 3:
		bval = buf[j].r;
	 	break;
	case 2:
		bval = buf[j].g;
	 	break;
	case 1:
		bval = buf[j].b;
	 	break;
	}
	fwrite(&bval, 1, 1,imfile);
      }
    }
  }
  fclose(imfile);
  return 1;
}


mpegframe* ReadSGIImage(char *imfilename) 
{
  unsigned char bval, zero=0;
  unsigned short sval;
  int  i, j, k, nbands;
  unsigned long lval, p, row;
  FILE *imfile;
  char imagename[80];
  int width, ht;
  mpegframe *mf;
  
  imfile = fopen(imfilename, "rb");
  if (imfile == NULL) {
    fprintf(stderr,"ReadSGIImage(): unable to open %s for reading\n",imfilename);
    return (struct mpegframe*)0;
  }
  /* magic number */
  fread(&sval, 2, 1, imfile);
  if (sval != 474) {
    fprintf(stderr,"ReadSGIImage(): %s is not an SGI .rgb image\n",imfilename);
    return (struct mpegframe*)0;
  }
  
  fread(&bval, 1, 1, imfile);
  if (bval != 0 ) /* not rle, verbatim  */ {
    fprintf(stderr,"ReadSGIImage(): %s is in RLE format, currently inimplemented.\n",
	    imfilename);
    return (struct mpegframe*)0;
  }
  
  /* bytes per component */
  fread(&bval, 1, 1, imfile);
  if ( bval != 1) {
    fprintf(stderr,"ReadSGIImage(): %s uses more than 1 byte per band."
	    " Cannot fit into XBGR buffer.\n",imfilename);
    return (struct mpegframe*)0;
  }
  
  /* dimension = 3 - x,y,z*/
  fread(&sval, 2, 1, imfile);
  if (sval != 3) {
    fprintf(stderr,"ReadSGIImage(): %s has more than 3 dimensions (x,y,z)."
	    " Cannot represent as 2-D image.\n",imfilename);
    return (struct mpegframe*)0 ;
  }
  
  
  fread(&sval, 2, 1, imfile);
  width = sval;
  fread(&sval, 2, 1, imfile);
  ht = sval;
  /* 3 channels: z = r,g,b */
  fread(&sval, 2, 1, imfile);
  nbands = sval;
  if (nbands != 3) {
    fprintf(stderr,"ReadSGIImage(): %s has other than 3 bands. Cannot fit"
	    " into XBGR buffer.\n",imfilename);
    return (struct mpegframe*)0;
  }
  
  /* min pixel value */
  fread(&lval, 4, 1, imfile);
  /* max pixel value */
  fread(&lval, 4, 1, imfile);
  /* dummy */
  fread(&lval, 4, 1, imfile);
  
  fread(imagename, 80, 1, imfile);
    
    /* color map */
    fread(&lval, 4, 1, imfile);
  if (lval != 0) {
    fprintf(stderr,"ReadSGIImage(): %s uses a color map, currently unimplemented"
	    ".\n",imfilename);
    return (struct mpegframe*)0;
  }
  
  
  for (i=0; i < 404; i+=1)
    fread(&bval, 1, 1, imfile); /* pad header to 512 */

  mf = (struct mpegframe*) malloc(sizeof(struct mpegframe));
  mf->dctbuffer =mf->crbuffer = 0;
  mf->frmotion = mf->fdmotion = mf->brmotion = mf->bdmotion = 0;
  mf->mbtypebuffer = 0;
  mf->imbcount = 0;
  mf->codetype = 'x';
  mf->frameno = -1;
  mf->width = width;
  mf->height = ht;

  mf->xbgrbuffer = (struct xbgr*) malloc (width*ht*sizeof(struct xbgr));

  for (k=3; k > 0; k--) {   /* entire R image, then entire G, then B image */
    for (i=ht-1; i >= 0; i--) { /* lower left coordinate system */
      lval = i*width;
      for (j=0; j < width; j++) {
	p =lval+j;
	fread(&bval, 1, 1,imfile);
	switch(k){
	case 3:
	  mf->xbgrbuffer[p].r = bval;
	  break;
	case 2:
	  mf->xbgrbuffer[p].g = bval;
	  break;
	case 1:
	  mf->xbgrbuffer[p].b = bval;
	  mf->xbgrbuffer[p].x = 0;
	  break;
	}
      }
    }
  }
  fclose(imfile);
  return mf;
}

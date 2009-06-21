/*
 * Copyright (c) 1993-1997, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED 
 * Permission to use, copy, modify, and distribute this software for 
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that 
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission. 
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * US Government Users Restricted Rights 
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(R) is a registered trademark of Silicon Graphics, Inc.
 */

/* This program modified from image.c from the redbook examples for
 * cse/ee 486 course by David Crandall and Sameer Antani.
 */

/*  image.c
 */
#if defined( _WIN32 )
#	include "glut.h"
#else
#	include <GL/glut.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include "cse486_mpeglib.h"

  mpeginfo *mi;

void init(void)
{    
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_FLAT);
   glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

void display(void)
{
  mpegframe *mf;
  int i,t;

  while(1) {
    /*   glClear(GL_COLOR_BUFFER_BIT);*/

   if(!(mf= mpegGetNextFrame(mi)))
     printf("reached end of file\n");
   
   for(i=0;i<mf->width*mf->height;i++)
     {
       mf->xbgrbuffer[i].x=mf->xbgrbuffer[i].r;
       t=mf->xbgrbuffer[i].g;
       mf->xbgrbuffer[i].g=mf->xbgrbuffer[i].b;
       mf->xbgrbuffer[i].b=t;
       }
   
   glDrawPixels(mf->width, mf->height, GL_RGBA, GL_UNSIGNED_BYTE, 
                mf->xbgrbuffer);
   mpegDestroyFrame(mf);
   glFlush();
  }
}

int main(int argc, char** argv)
{

  mi=mpegOpen(argv[1]);

  mpegGotoFrame(mi,76);

   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA); 
   glutInitWindowSize(320, 240);
   glutInitWindowPosition(100, 100);
   glutCreateWindow(argv[0]);
   init();
   glutDisplayFunc(display);
   glutMainLoop();
   return 0; 
}

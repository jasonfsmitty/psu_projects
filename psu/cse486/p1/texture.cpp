/******************************************************************************
	texture.cpp

	Texture loading functions.

 ******************************************************************************/
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <GL\gl.h>
#include <GL\glu.h>
#include <GL\glaux.h>


#include "texture.h"


AUX_RGBImageRec *LoadBMP(char *Filename)                // Loads A Bitmap Image
{
        FILE *File=NULL;                                // File Handle

        if (!Filename)                                  // Make Sure A Filename Was Given
        {
                return NULL;                            // If Not Return NULL
        }

        File=fopen(Filename,"r");                       // Check To See If The File Exists

        if (File)                                       // Does The File Exist?
        {
                fclose(File);                           // Close The Handle
                return auxDIBImageLoad(Filename);       // Load The Bitmap And Return A Pointer
        }
        return NULL;                                    // If Load Failed Return NULL
}


bool LoadTexture( char* textureFile, GLuint &textureID )                                    // Load Bitmaps And Convert To Textures
{
        bool Status=false;  // Status Indicator

        AUX_RGBImageRec *TextureImage[1];               // Create Storage Space For The Texture

        memset(TextureImage,0,sizeof(void *)*1);        // Set The Pointer To NULL

        // Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
        if (TextureImage[0]=LoadBMP( textureFile ))
        {
                Status=true;                            // Set The Status To TRUE

				glEnable( GL_TEXTURE_2D );
                glGenTextures(1, &textureID );          // Create One Texture

				if( textureID==0 ){
					MessageBox(NULL, "Error getting texture ID", "Dislay Error", 0 );
				}

                // Create Linear Filtered Texture
                glBindTexture(GL_TEXTURE_2D, textureID );
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
                glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
        }
        if (TextureImage[0])                            // If Texture Exists
        {
                if (TextureImage[0]->data)              // If Texture Image Exists
                {
                        free(TextureImage[0]->data);    // Free The Texture Image Memory
                }

                free(TextureImage[0]);                  // Free The Image Structure
        }

        return Status;                                  // Return The Status
}

void SetTexture( GLuint tID )
{
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, tID );
}

void DeleteTexture( GLuint tID )
{
	glDeleteTextures( 1, &tID );
}

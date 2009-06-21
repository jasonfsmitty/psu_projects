/************************************************************************************
	texture.h

	Functions to load/set textures.
 ************************************************************************************/


#ifndef TEXTURE_H
#define TEXTURE_H


/*---  Load a BMP from a file.  ---*/
AUX_RGBImageRec *LoadBMP(char *Filename);

/*	Completely loads and initializes a texture from a BMP file.
	Returns: true->success,  false->failure
	textureID is the ID for the Texture Object that the
	  texture was loaded into.*/
bool	LoadTexture( char* textureFile, GLuint &textureID );

/*---  Sets the texture object refered to by textureID to the current texture ---*/
void	SetTexture( GLuint tID );

/*---  Delete the texture object  ---*/
void	DeleteTexture( GLuint tID );

#endif /* defined(TEXTURE_H) */
/*--------------------------------------------------------------------+

	Jason F Smith

	CSE 486 - Project 1

	cse486p1.cpp - main windows implementation file.

	Some functions taken/modified from NeHe (these functions 
	are commented as such).  If no source specified, written 
	by Jason F Smith (JFS)
 +--------------------------------------------------------------------*/

#include "cse486p1.h"		// main header file


/*---------------------------------------------------------------------+
	LoadImage()
	Loads a raw image into memory pointed to by control.origImage.
 +---------------------------------------------------------------------*/
bool LoadImage( void )
{
	ifstream in;

	if( control.origImage ){
		delete control.origImage;
		control.origImage=NULL;
	}

	in.open( control.imagePath, ios::binary );

	if( in.fail() ){
		MessageBox( NULL, "Error opening the input file.", "Load Error", MB_OK|MB_ICONEXCLAMATION );
		in.close();
		return false;
	}

	control.origImage = new unsigned char [control.imageHeight*control.imageWidth];

	if( control.origImage==NULL ){
		MessageBox( NULL, "Could not allocate memory to load image.", "Load Error", MB_OK|MB_ICONEXCLAMATION );
		in.close();
		return false;
	}

	in.read( control.origImage, control.imageHeight * control.imageWidth );
	in.close();

	ProcessData();

	return true;
}

/*-------------------------------------------------------------------+
	A generic function to save the image data to a file.
 +-------------------------------------------------------------------*/

bool SaveImage( char * file, unsigned char* image, int length )
{
	ofstream out;

	if( file==NULL || image==NULL ){
		return false;
	}

	out.open( file, ios::binary );

	if( out.fail() ){
		MessageBox( NULL, "Could not open save file.", "Save Error", MB_OK|MB_ICONEXCLAMATION );
		return false;
	}

	out.write( image, length );
	out.close();
	return true;
}


/*-------------------------------------------------------------------+
	A function to set up the window and display the appropriate
	image which depends on type
 +-------------------------------------------------------------------*/
void DisplayWindow( int type )
{
	unsigned char* data;
	char *title;

	if( glWindowCreated ){
		KillGLWindow();
	}
	control.currentType=0;

	if( control.origImage==NULL ){
		MessageBox(dialogHWND, "Please load a file first.", "Display Error", MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	switch( type ){
		case ORIGINAL_IMAGE:
			title = "Original Image";
			data = InitOriginal();
			break;

		case MEDIAL_IMAGE:
			data = InitMedial();
			title = "Medial Axis Image";
			break;

		case OVERLAY_IMAGE:
			data = InitOverlay();
			title = "Medial Axis Overlay";
			break;

		case PATH_IMAGE:
			data = InitPath();
			title = "Robot Path Image";
			break;

	}; // switch

	if( control.currentImage!=NULL ){
		delete control.currentImage;
		control.currentImage = NULL;
	}

	control.currentType = type;
	control.currentImage = data;

	if( data==NULL ){
		MessageBox( NULL, "Could not display the image.", "Display Error", MB_OK|MB_ICONEXCLAMATION );
		return;
	}


	if( !CreateGLWindow( title, control.imageWidth, control.imageWidth, 16, false ) ){
		MessageBox( NULL, "Could not create the display window.", "Display Error", MB_OK|MB_ICONEXCLAMATION );
		KillGLWindow();
	}


	// use the data pointer to set the current texture
	glEnable( GL_TEXTURE_2D );

	// set up the texture so the image can be seen
	glDeleteTextures( 1, &texName );

	glGenTextures( 1, &texName );
	glBindTexture( GL_TEXTURE_2D, texName );
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

	glTexImage2D(	GL_TEXTURE_2D,// target
					0, // level 
					3, // internal format 
					control.imageWidth, // width 
					control.imageHeight, // height 
					0, // border 
					GL_RGB, // format 
					GL_UNSIGNED_BYTE, // type 
					data // texels 
				);

}


/*------------------------------------------------------------------+
	Initialize the original image.
 +------------------------------------------------------------------*/
unsigned char* InitOriginal( void )
{
	unsigned char* data;

	if( control.origImage == NULL ){
		return NULL;
	}

	data = new unsigned char [control.imageHeight*control.imageWidth*3];

	if( data==NULL ) {
		MessageBox( NULL, "Could not allocate enough memory\nfor the original image.", "InitMedial() Error", MB_OK|MB_ICONEXCLAMATION );
		return NULL;
	}

	for( int i=0; i<control.imageHeight*control.imageWidth; i++ ){
		data[i*3] = control.origImage[i];
		data[i*3+1] = control.origImage[i];
		data[i*3+2] = control.origImage[i];
	}

	return data;
}


/*------------------------------------------------------------------+
	Initialize the medial axis image;
 +------------------------------------------------------------------*/
unsigned char* InitMedial( void )
{
	unsigned char c;
	unsigned char* data;

	if( control.origImage==NULL || control.medialImage==NULL ){
		return NULL;
	}

	data = new unsigned char [control.imageHeight*control.imageWidth*3];

	if( data==NULL ){
		MessageBox( NULL, "Could not allocate enough memory\nfor the medial axis image.", "InitMedial() Error", MB_OK|MB_ICONEXCLAMATION );
		return NULL;
	}

	for( int i=0; i<control.imageHeight*control.imageWidth; i++ ){
		c = ( control.medialImage[i]==0 ? 255 : 0 );
		data[i*3] = c;
		data[i*3+1] = c;
		data[i*3+2] = c;
	}

	return data;
}



/*------------------------------------------------------------------+
	Initialize the overlay image.
 +------------------------------------------------------------------*/
unsigned char* InitOverlay( void )
{
	unsigned char* data;
	unsigned char c;

	if( control.overlayImage==NULL ){
		return NULL;
	}

	data = new unsigned char [control.imageHeight*control.imageWidth*3];

	if( data==NULL ){
		MessageBox( NULL, "Error allocating memory for the overlay image.", "InitOverlay() Error", MB_OK|MB_ICONEXCLAMATION );
		return NULL;
	}

	for( int i=0; i<control.imageHeight*control.imageWidth; i++ ){
		c = ( control.overlayImage[i] );
		
		data[i*3] = ( c==128 ? 255 : c );
		data[i*3+1] = ( c==128 ? 0 : c );
		data[i*3+2] = ( c==128 ? 0 : c );
	}

	return data;
}



/*------------------------------------------------------------------+
	Initialize the path image.
 +------------------------------------------------------------------*/
unsigned char* InitPath( void )
{
	unsigned char* data;

	if( control.pathImage==NULL ){
		return NULL;
	}

	data = InitOverlay();

	if( data==NULL ){
		MessageBox( NULL, "Could not allocate memory to display the path.", "InitPath() Error", MB_OK|MB_ICONEXCLAMATION );
		return NULL;
	}

	for( int i=0; i<control.imageHeight*control.imageWidth; i++ ){
		if( control.pathImage[i] == 255 ){
			data[i*3] =		0;
			data[i*3+1] =	255;
			data[i*3+2] =	0;
		} else if( control.pathImage[i] != 0 ){
			data[i*3]	=	0;
			data[i*3+1]	=	0;
			data[i*3+2] =	255;
		}
	}

	return data;
}


/*-----------------------------------------------------------------+
	ProcessData
	Take the newly opened original image and calculate
	the medial axis and all other data (overlay image, etc).
 +-----------------------------------------------------------------*/
void ProcessData( void )
{
	int i;

	if( control.medialImage ){
		delete control.medialImage;
		control.medialImage = NULL;
	}
	if( control.overlayImage ){
		delete control.overlayImage;
		control.overlayImage = NULL;
	}
	if( control.pathImage ){
		delete control.pathImage;
		control.pathImage = NULL;
	}

	// First calculate the medial axis
	backgroundPixel=0;
	control.medialImage = new unsigned char [control.imageHeight*control.imageWidth];
	CalculateMedial( control.origImage, control.medialImage, control.imageWidth, control.imageHeight );

	// Calculate the overlay image
	control.overlayImage = new unsigned char[control.imageHeight*control.imageWidth];
	for( i=0; i<control.imageHeight*control.imageWidth; i++ ){
		control.overlayImage[i] = ( control.medialImage[i]!=0 ? 128 : control.origImage[i] );
	} // for


	// calculate the robot path from the medial image data
	// this is calculated by ProcessRobot()

}

/*-----------------------------------------------------------------+
	Take the information on the robot and initialize everything
 +-----------------------------------------------------------------*/
void ProcessRobot( void )
{
	long i;
	unsigned char min;

	if( control.origImage==NULL || control.medialImage==NULL || control.overlayImage==NULL ){
		MessageBox( NULL, "Other images are not initialized correctly for ProcessRobot().", "ProcessRobot() Error", MB_OK|MB_ICONEXCLAMATION );
		return;
	}

	if( control.currentImage ){
		delete control.currentImage;
		control.currentImage = NULL;
	}	
	
	if( glWindowCreated ){
		KillGLWindow();
	}

	// disable the size text box
	//SendDlgItemMessage( dialogHWND, IDC_ROBOTSIZE, WM_ENABLE, (WPARAM)0, (LPARAM)0 );
	//SendDlgItemMessage( dialogHWND, IDC_STARTX, WM_ENABLE, (WPARAM)0, (LPARAM)0 );
	//SendDlgItemMessage( dialogHWND, IDC_STARTY, WM_ENABLE, (WPARAM)0, (LPARAM)0 );
	//SendDlgItemMessage( dialogHWND, IDC_ENDX, WM_ENABLE, (WPARAM)0, (LPARAM)0 );
	//SendDlgItemMessage( dialogHWND, IDC_ENDY, WM_ENABLE, (WPARAM)0, (LPARAM)0 );


	// Calculate the medial axis
	// this is incase the user re-processes the image, we have
	//  to make sure we haven't thrown away any neccessary data due to
	//  size restrictions from the previous image processing
	CalculateMedial( control.origImage, control.medialImage, control.imageWidth, control.imageHeight );


	// eliminate all of the paths which are not large enough for the robot
	//   (leave the original image the same)
	min = (unsigned char)( ((float)control.robotSize)/2.0f + 0.5f );
	for( i=0; i<control.imageHeight*control.imageWidth; i++ ){
		if( control.medialImage[i] < min ){
			control.medialImage[i] = 0;
		}
	} // for

	// re-calculate the overlay image
	for( i=0; i<control.imageHeight*control.imageWidth; i++ ){
		control.overlayImage[i] = ( control.medialImage[i]!=0 ? 128 : control.origImage[i] );
	} // for

	// calculate the robot path image
	if( CalcRobotPath() ){
		MessageBox( dialogHWND, "Found a path for the robot!!", "Robot Path Status", MB_OK|MB_ICONINFORMATION );
		DisplayWindow( PATH_IMAGE );
	} else {
		MessageBox( dialogHWND, "Could not find a path for the robot.", "Robot Path Status", MB_OK|MB_ICONINFORMATION );
		if( control.pathImage ){
			delete control.pathImage;
			control.pathImage=NULL;
		}
	}

}

/*-----------------------------------------------------------------+
	Quick and dirty function to convert a string into an integer
 +-----------------------------------------------------------------*/
int StringToInt( char *str )
{
	int total = 0;
	int i=0;
	for( i=0; i<128 && str[i]!=NULL && str[i]!='\n'; i++ ){
		if( str[i]>='0' || str[i]<='9' ){
			total = total*10 + (str[i]-'0');
		}
	}
	return total;
}

/*-------------------------------------------------------------------+
	Another quick and dirty function.  This one converts an
	integer to a char string
 +-------------------------------------------------------------------*/
char* IntToString( int value )
{
	static char temp[64];
	char *ptr;
	temp[63] = '\0';
	ptr = temp+63;

	do{
		ptr--;
		*ptr = (char)(value%10)+'0';
		value = value/10;
	} while( value>0 );

	return ptr;
}

/*-------------------------------------------------------------------+
	Initializs any data/objects which are connected to the window.
	This function is called on every valid CreateGLWindow call.
 +-------------------------------------------------------------------*/
bool InitGL( void )
{
	glClearColor( 0.0f, 1.0f, 0.0f, 1.0f );
	glShadeModel( GL_FLAT );

	glEnable( GL_TEXTURE_2D );

	return true;
}

/*-------------------------------------------------------+
	Delete/Release anything which is related to the
	current window (example - textures, DirectInput, etc).
	This is called from within the KillGLWindow function.
 +-------------------------------------------------------*/
void KillGL( void )
{
	glDeleteTextures( 1, &texName );
	if( control.currentImage!=NULL ){
		delete control.currentImage;
		control.currentImage = NULL;
	}

	if( control.currentImage ){
		delete control.currentImage;
		control.currentImage=NULL;
	}

	glWindowCreated=false;
}

/*----------------------------------------------------------------+
	Create an OpenGL window.

	Modified from NeHe.
 +----------------------------------------------------------------*/
bool CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=false;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return false;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen


	// Added by JFS
	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	// Also added by JFS
	glWindowCreated=true;

	return true;									// Success
}


/*----------------------------------------------------------------+
	Resize the window and set up the viewport.
 +----------------------------------------------------------------*/
GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}


	// Set up a 2D window for the image.
	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0.0, (double)width-1.0, (double)height-1.0, 0.0 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	screenWidth = width;
	screenHeight = height;
}


/*-----------------------------------------------------------------+
	All cleanup and restoration of allocated memory, etc, happens
	here.
 +-----------------------------------------------------------------*/
void CleanUp( void )
{
	if( glWindowCreated ){
		KillGLWindow();	// Destroy the GL window (if created)
	}

	// Delete any allocated memory
	if( control.origImage ){
		delete control.origImage;
		control.origImage=NULL;
	}

	if( control.medialImage ){
		delete control.medialImage;
		control.medialImage = NULL;
	}

	if( control.overlayImage ){
		delete control.overlayImage;
		control.overlayImage = NULL;
	}

	if( control.pathImage ){
		delete control.pathImage;
		control.pathImage=NULL;
	}

	if( control.currentImage ){
		delete control.currentImage;
		control.currentImage = NULL;
	}

	glDeleteTextures( 1, &texName );
}


/*----------------------------------------------------------------+
	Draw the image into the GL window.

	It seems easiest to draw a single image in OpenGL by
	setting it as a texture and drawing it parallel to the
	viewing plane, so that's what I do here.  Yeah I know I could
	write my own blitting function, but this works great if the
	user alters the window size (a blurry magnification/minification).
 +----------------------------------------------------------------*/
void DrawGLScene( void )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable( GL_TEXTURE_2D );

	glBindTexture( GL_TEXTURE_2D, texName );

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

	// Just draw a quad the size of the screen
	glBegin( GL_QUADS );
		glTexCoord2f( 0.0f, 0.0f );		glVertex2i( 0, 0 );
		glTexCoord2f( 0.0f, 1.0f );		glVertex2i( 0, screenHeight-1 );
		glTexCoord2f( 1.0f, 1.0f );		glVertex2i( screenWidth-1, screenHeight-1 );
		glTexCoord2f( 1.0f, 0.0f );		glVertex2i( screenWidth-1, 0 );
	glEnd();
}


/*-----------------------------------------------------------------+
	Properly kill an OpenGL window

	Modified from NeHe.
 +-----------------------------------------------------------------*/
GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{

	// Added by JFS
	if( glWindowCreated==false )
		return;

	// Also by JFS
	glWindowCreated=false;

	// Again by JFS
	KillGL();

	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}


/*-----------------------------------------------------------------------+
	Dialog Process Message Handler

	This function handles all of the messaging for the control panel
	dialog box which is the default window for the GUI.
 +-----------------------------------------------------------------------*/
int CALLBACK DialogProc(HWND h,UINT m,WPARAM w,LPARAM l)
{

	static OPENFILENAME ofn;
	static OPENFILENAME sfn;
	static char path[MAX_PATH];
	char file[MAX_PATH]="";
	char temp[128]="";

	switch (m) {

		case WM_COMMAND:
			switch (LOWORD(w)) {
				case IDEXIT:
					// quit everything
					active=false;
					DestroyWindow( h );
					break;

				case IDC_OPENDIALOG:
					// Open a file using the open dialog box
					ofn.lpstrFile=file;
					if( GetOpenFileName( &ofn ) ){
						memcpy(path, file, ofn.nFileOffset);
						path[ofn.nFileOffset-1]=0;
						SendDlgItemMessage( dialogHWND, IDC_EDIT_FILENAME, WM_SETTEXT, (WPARAM)0, (LPARAM)file );
					}
					break;

				case IDC_LOADBUTTON:
					// load an image into the program

					// check for valid parameters and load
					// the file

					GetDlgItemText( dialogHWND, IDC_EDIT_FILENAME, control.imagePath, MAX_PATH );

					if( strlen( control.imagePath)==0 ){
						MessageBox( dialogHWND, "Please enter a filename.", "Load Error", MB_OK|MB_ICONEXCLAMATION );
					} else {
						// Check all of the other image data to make sure that everything is set
						//  before loading the image
						GetDlgItemText( dialogHWND, IDC_WIDTH, temp, 128 );
						if( strlen( temp )>0 ){
							control.imageWidth = StringToInt( temp );
							GetDlgItemText( dialogHWND, IDC_HEIGHT, temp, 128 );
							if( strlen( temp )>0 ) {
								control.imageHeight = StringToInt( temp );
								GetDlgItemText( dialogHWND, IDC_FOREGROUND, temp, 128 );
								if( strlen(temp)>0 ){
									control.imageForeground = StringToInt( temp );
									
									if( glWindowCreated ){
										KillGLWindow();
									}

									if( LoadImage() ){
										SendDlgItemMessage( dialogHWND, IDC_ROBOTSIZE, WM_ENABLE, (WPARAM)1, (LPARAM)0 );
										SendDlgItemMessage( dialogHWND, IDC_STARTX, WM_ENABLE, (WPARAM)1, (LPARAM)0 );
										SendDlgItemMessage( dialogHWND, IDC_STARTY, WM_ENABLE, (WPARAM)1, (LPARAM)0 );
										SendDlgItemMessage( dialogHWND, IDC_ENDX, WM_ENABLE, (WPARAM)1, (LPARAM)0 );
										SendDlgItemMessage( dialogHWND, IDC_ENDY, WM_ENABLE, (WPARAM)1, (LPARAM)0 );

										MessageBox( dialogHWND, "Image loaded.", "Load Image", MB_OK|MB_ICONINFORMATION );
									} else {
										MessageBox( dialogHWND, "Error loading image.", "Load Error", MB_OK|MB_ICONEXCLAMATION );
									}

								} else {
									MessageBox( dialogHWND, "Please enter a valid value for the foreground pixel.", "Load Error", MB_ICONEXCLAMATION );
								}
							} else {
								MessageBox( dialogHWND, "Please enter a valid height for the image.", "Load Error", MB_ICONEXCLAMATION );
							} // if-else
						} else {
							MessageBox( dialogHWND, "Please enter a valid width for the image.", "Load Error", MB_ICONEXCLAMATION );
						}// if-else

					}// if else

					break;

				case IDC_DISPLAYORIGINAL:
					DisplayWindow( ORIGINAL_IMAGE );
					break;

				case IDC_DISPLAYMEDIAL:
					DisplayWindow( MEDIAL_IMAGE );
					break;

				case IDC_DISPLAYMEDIALOVERLAY:
					DisplayWindow( OVERLAY_IMAGE );
					break;

				case IDC_DISPLAYPATH:
					if( control.pathImage ){
						DisplayWindow( PATH_IMAGE );
					} else {
						MessageBox( dialogHWND, "You must process the robot data first.", "RobotPath Error", MB_OK|MB_ICONEXCLAMATION );
					}
					break;

				case IDC_DISPLAYSAVE:
					if( !glWindowCreated ){
						MessageBox( dialogHWND, "Please display an image first.", "Save Error", MB_OK|MB_ICONEXCLAMATION );
					} else {
						// get the save file
						ofn.lpstrFile=file;
						if( GetSaveFileName( &ofn ) ){
							memcpy(path, file, ofn.nFileOffset);
							path[ofn.nFileOffset-1]=0;
							if( SaveImage( file, control.currentImage, control.imageHeight*control.imageWidth*3 ) ){
								MessageBox( dialogHWND, "Image saved.", "Save Complete", MB_OK|MB_ICONINFORMATION );
							}else{
								MessageBox( dialogHWND, "Error saving the image.", "Save Error", MB_OK|MB_ICONEXCLAMATION );
							}// if saveimage - else
						} // if getopenfilename
					} // if - else window created
					break;

				case IDC_DISPLAYCLOSE:
					if( glWindowCreated ){
						KillGLWindow();
					}
					if( control.currentImage ){
						delete control.currentImage;
						control.currentImage = NULL;
					}

					break;
					
				case IDC_PROCESS_ROBOT:
					GetDlgItemText( dialogHWND, IDC_ROBOTSIZE, temp, 128 );
					if( strlen( temp )<=0 ){
						MessageBox( dialogHWND, "Please enter the robot size.", "Process Error", MB_OK|MB_ICONEXCLAMATION );
					} else {
						if( (control.robotSize = StringToInt( temp ))>256 ){
							MessageBox( NULL, "The robot size is too big.", "Process Error", MB_OK|MB_ICONEXCLAMATION );
							return 0;
						}
						GetDlgItemText( dialogHWND, IDC_STARTX, temp, 128 );
						if( strlen(temp)<=0 ){
							MessageBox( dialogHWND, "Please enter the start X.", "Process Error", MB_OK|MB_ICONEXCLAMATION );
						} else {
							if( (control.startX = StringToInt( temp ))>256 ){
								MessageBox( NULL, "The start X is invalid.", "Process Error", MB_OK|MB_ICONEXCLAMATION );
								return 0;
							}
							GetDlgItemText( dialogHWND, IDC_STARTY, temp, 128 );
							if( strlen(temp)<=0 ){
								MessageBox( dialogHWND, "Please enter the start Y.", "Process Error", MB_OK|MB_ICONEXCLAMATION );
							} else {
								control.startY = StringToInt( temp );
								GetDlgItemText( dialogHWND, IDC_ENDX, temp, 128 );
								if( strlen(temp)<=0 ){
									MessageBox( dialogHWND, "Please enter the end X.", "Process Error", MB_OK|MB_ICONEXCLAMATION );
								} else {
									control.endX = StringToInt( temp );
									GetDlgItemText( dialogHWND, IDC_ENDY, temp, 128 );
									if( strlen(temp)<=0 ){
										MessageBox( dialogHWND, "Please enter the end Y.", "Process Error", MB_OK|MB_ICONEXCLAMATION );
									} else {
										control.endY = StringToInt( temp );
										// everything is a-ok
										ProcessRobot();

									}// if endy else
								} // if endx else
							} // if starty else
						} // if startx else
					} // if robot size else
					break;

				case IDC_RESET:
					MessageBox( NULL, "Sorry, that feature is not implemented yet.", "Error", 0 );
					break;

			}
			break;

		case WM_INITDIALOG:
			dialogHWND=h;
			GetCurrentDirectory(MAX_PATH,path);
			memset(&ofn,0,sizeof(ofn));
			ofn.lStructSize=sizeof(ofn);
			ofn.hwndOwner=h;
			ofn.hInstance=dInstance;
			ofn.nMaxFile=MAX_PATH;
			ofn.lpstrInitialDir=path;
			ofn.Flags=OFN_HIDEREADONLY|OFN_EXPLORER;
			ofn.lpstrFilter = "RAW image files (*.raw)\0*.raw\0All files (*.*)\0*.*\0\0";

			// Set the default values
			SendDlgItemMessage( dialogHWND, IDC_FOREGROUND, WM_SETTEXT, (WPARAM)0, (LPARAM)"255" );
			SendDlgItemMessage( dialogHWND, IDC_WIDTH, WM_SETTEXT, (WPARAM)0, (LPARAM)"256" );
			SendDlgItemMessage( dialogHWND, IDC_HEIGHT, WM_SETTEXT, (WPARAM)0, (LPARAM)"256" );

			SendDlgItemMessage( dialogHWND, IDC_FOREGROUND, WM_ENABLE, (WPARAM)0, (LPARAM)0 );
		

			SendDlgItemMessage( dialogHWND, IDC_ROBOTSIZE, WM_ENABLE, (WPARAM)0, (LPARAM)0 );
			SendDlgItemMessage( dialogHWND, IDC_STARTX, WM_ENABLE, (WPARAM)0, (LPARAM)0 );
			SendDlgItemMessage( dialogHWND, IDC_STARTY, WM_ENABLE, (WPARAM)0, (LPARAM)0 );
			SendDlgItemMessage( dialogHWND, IDC_ENDX, WM_ENABLE, (WPARAM)0, (LPARAM)0 );
			SendDlgItemMessage( dialogHWND, IDC_ENDY, WM_ENABLE, (WPARAM)0, (LPARAM)0 );


			return 1;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 1;
	}

	return 0;
}


/*---------------------------------------------------------------------+
	Default Windows messaging loop for the OpenGL window

	Modified from:  NeHe
 +---------------------------------------------------------------------*/
LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{

	switch( uMsg )									// Check For Windows Messages
	{
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if( !HIWORD(wParam) ){					// Check Minimization State
				active=TRUE;						// Program Is Active
			} else {
				//active=FALSE;						// Program Is No Longer Active
			}

			return 0;								// Return To The Message Loop
		}

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
					return 0;						// Prevent From Happening
			}
			break;									// Exit
		}

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			KillGLWindow();
			if( control.currentImage ){
				delete control.currentImage;
				control.currentImage = NULL;
			}
			return 0;								// Jump Back
		}

		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			keys[wParam] = TRUE;					// If So, Mark It As TRUE
			if( keys[VK_ESCAPE] && glWindowCreated ){
				KillGLWindow();
			}
			return 0;								// Jump Back
		}

		case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = FALSE;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}

		case WM_RBUTTONDOWN: // process right mouse button
			// this will be the endX, endY values for the robot path
			SendDlgItemMessage( dialogHWND, IDC_ENDX, WM_SETTEXT, (WPARAM)0, (LPARAM)IntToString(LOWORD(lParam)) );
			SendDlgItemMessage( dialogHWND, IDC_ENDY, WM_SETTEXT, (WPARAM)0, (LPARAM)IntToString(HIWORD(lParam)) );
			return 0;

		case WM_LBUTTONDOWN:
			// Starting coordinate for the robot.
			// note that if the window is resized, the coordinates will 
			//  not match up correctly.
			SendDlgItemMessage( dialogHWND, IDC_STARTX, WM_SETTEXT, (WPARAM)0, (LPARAM)IntToString(LOWORD(lParam)) );
			SendDlgItemMessage( dialogHWND, IDC_STARTY, WM_SETTEXT, (WPARAM)0, (LPARAM)IntToString(HIWORD(lParam)) );
			return 0;

		case WM_SIZE:								// Resize The OpenGL Window
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}

	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);

} // WinProc


/*--------------------------------------------------------------------------+
	WinMain is the entry point for all Windows applications.

	Originally modified from NeHe, but the only remaining untouched
	code is in the if{} portion inside of while(!done).  The rest was
	deleted/modified/replaced by JFS.
 +--------------------------------------------------------------------------*/
int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{

	MSG msg;
	bool done=false;

	// Set things up

	if (!CreateDialog(hInstance,MAKEINTRESOURCE(IDD_MAIN_DIALOG),NULL,&DialogProc)) {
		MessageBox( NULL, "Could not create the dialog window.", "Error", 0 );
		return 0;
	}

	ShowWindow(dialogHWND,SW_SHOW);						// Show The Window
	//SetForegroundWindow(dialogHWND);						// Slightly Higher Priority
	//SetFocus(dialogHWND);									// Sets Keyboard Focus To The Window
	

	// run through the message loop
	while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=true;							// If So done=TRUE
			} 
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		} else {										// If There Are No Messages
			if( !active ){
				done=true;
			} else if( glWindowCreated ){
				DrawGLScene();
				SwapBuffers(hDC);
			}

		} // if(peekmessage)-else
	} // while(!done)


	// cleanup
	CleanUp();

		
	return (msg.wParam);
}
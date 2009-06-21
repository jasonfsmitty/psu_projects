/*****************************************************************
	Jason F Smith    4588
	Mike Schrieber   5259
	CSE 411 Proj 3
*******************************************************************/

extern int fs_init ( );
extern int fs_shutdown ( );
extern int fs_create ( char *fname ) ;
extern int fs_delete ( char *fname) ; 
extern int fs_open ( char *fname , char *mode ) ;
extern int fs_close ( int fhandle ) ;
extern int fs_read ( int fhandle , char *buf , int nbytes );
extern int fs_write ( int fhandle , char *buf , int nbytes );
extern int fs_seek ( int fhandle , int offset ) ;



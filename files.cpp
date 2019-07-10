///////////////////////////////////////////////////////////////////////////////
// Files.cpp
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008, Joe Riedel
// All rights reserved.
//
// Redistribution and use in source and binary forms, 
// with or without modification, are permitted provided 
// that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice, 
// this list of conditions and the following disclaimer. 
//
// Redistributions in binary form must reproduce the above copyright notice, 
// this list of conditions and the following disclaimer in the documentation and/or 
// other materials provided with the distribution. 
//
// Neither the name of the <ORGANIZATION> nor the names of its contributors may be 
// used to endorse or promote products derived from this software without specific 
// prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
//
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
// OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "os.h"
#include "files.h"

#define Con_LPrintf(...)
#define Log_Msg(...)

#define MAX_FS_BASE_LENGTH	1024
static char c_basedir[MAX_FS_BASE_LENGTH+1];
static char c_moddir[MAX_FS_BASE_LENGTH+1];

#define PAK_ID		'CLST'
#define PAK_MAGIC	293946
#define BAD_PAK_FILE_NUM	-1

typedef struct
{
	unsigned int id;
	unsigned int magic;
	
	unsigned int dir_ofs;
	unsigned int dir_comp_size;	
	unsigned int dir_size;
	
} PakHeader_t;

#define PAK_HEADER_SIZE	20

#define PAKENTRY_NAME_LEN	255
typedef struct
{
	char name[PAKENTRY_NAME_LEN+1]; // 255 bytes
	TimeDate_t timedate; // 10-bytes
	unsigned int ofs;
	unsigned int comp_size;
	unsigned int size;
	Byte shadowed;
	Byte padd;
} PakEntry_t; // total size = 88 bytes

typedef struct
{
	char name[256];
	PakHeader_t header;
	
	unsigned int num_files;
	PakEntry_t* dir;
	
	FILE* fp;
	bool base;
} PakFile_t;

typedef struct MakePakEntry_s
{
	struct MakePakEntry_s* n;
	
	PakEntry_t e;
	
} MakePakEntry_t;

static int c_numpaks=0;
static int c_numfails=0;
static PakFile_t c_paks[MAX_PAK_FILES];
static PakFile_t c_makepak;
static MakePakEntry_t* c_make_files;
static bool c_mod_valid=FALSE;

static bool OpenPak( const char* filename, PakFile_t* pk );
static bool ClosePak( PakFile_t* pk );
static bool ReadPakHeader( PakFile_t* pk);
static bool DecompressPakDir( PakFile_t* pk);
static int FindFileInPak( PakFile_t* pk, const char* filename );
static bool FileExistsInOpenPaks( const char* filename, int* loc );
static bool LoadFileFromOpenPaks( const char* filename, void** data, LongWord* length, bool handle );
static bool LoadFileFromPak( PakFile_t* pk, int num, void** data, LongWord* length );
static bool LoadFileHandleFromPak( PakFile_t* pk, int num, void*** data );
static bool MakePak( const char* filename, PakFile_t* pk );
static bool FinishMakePak( PakFile_t* pk );
static bool WriteFileToPak( const char* filename, const char* entry_name, PakFile_t* pk );
static bool SortPakFiles( PakEntry_t* dir, int num_files );
static bool FreeMakePakFiles( void );
static int ANSICALL SortPakEntry( const void* p1, const void* p2 );
static void ShadowPakFiles( PakFile_t* pk );
static bool GetFileModTimeInOpenPaks( const char* filename, TimeDate_t* td );

static bool FS_OpenDiskSearch( FS_FileSearch_t* fs_search, bool base );
static bool FS_CloseDiskSearch( FS_FileSearch_t* fs_search );
static bool FS_GetNextFileFromDiskSearch( FS_FileSearch_t* fs_search, FS_FileSearchData_t* fs_data );
static bool FS_OpenPakSearch( FS_FileSearch_t* fs_search, bool base );
static bool FS_ClosePakSearch( FS_FileSearch_t* fs_search );
static bool FS_GetNextFileFromPakSearch( FS_FileSearch_t* fs_search, FS_FileSearchData_t* fs_data );

void FS_FileFromPath( const char* path, char* out )
{
	if( !path || !path[0] || !out )
		return;
	
	LongWord pos = 0;
	LongWord ofs = 0;
	
	for(;;)
	{
		if( path[ofs] == ':' || path[ofs] == '/' || path[ofs] == '\\' )
		{
			pos = ofs;
		}
		
		if( path[ofs] == 0 )
			break;
		
		ofs++;
	}
	
	strcpy( out, &path[pos+1] );
}

#define COPY_CHUNK_SIZE		(64*1024)

bool FS_CopyFileToDirectory( const char* filename, const char* directory )
{
	char outfile[1024];
	char strfile[1024];
	
	if( !filename || !filename[0] || !directory )
		return TRUE;
		
	FS_FileFromPath( filename, strfile );
	
	sprintf( outfile, "%s/%s", directory, strfile );
	FS_MakeBurgerPath( outfile, outfile );
	
	CreateDirectoryPath2( outfile );
	
	FILE* fp = FS_fopen( outfile, "wb" );
	if( !fp )
		return TRUE;
	
	LongWord size;
	void* infile = FS_LoadFile( filename, &size );
	if( !infile )
	{
		FS_fclose( fp );
		return TRUE;
	}
	
	bool failed = FS_fwrite( fp, infile, size );
	FS_fclose( fp );
	
	OS_DeallocAPointer( infile );
	
	return failed;
}

LibRef_t* FS_LoadLibLoc( const char* filename, int loc )
{
	char buff[1024];
	
	FS_GetFullPath( filename, buff, loc );
	FS_MakeBurgerPath( buff, buff );
	
	LibRef_t* lib = LibRefInit( buff );
		
	return lib;
}

bool FS_QuickWriteFile( const char* filename, int loc, void* data, LongWord length )
{
	FILE* fp = FS_fopen_loc( filename, "w", loc );
	if( !fp )
		return TRUE;
		
	bool f = FS_fwrite( fp, data, length );
	FS_fclose( fp );
	
	return f;
}

bool FS_QuickReadFile( const char* filename, int loc, void* data, LongWord length )
{
	FILE* fp = FS_fopen_loc( filename, "r", loc );
	if( !fp )
		return TRUE;
		
	bool f = FS_fread( fp, data, length );
	FS_fclose( fp );
	
	return f;
}

FILE* FS_fopen( const char* filename, const char* mode )
{
	char buff[1024];
	
	FS_MakeBurgerPath( filename, buff );
	return OpenAFile( buff, mode );
}

FILE* FS_fopen_loc( const char* filename, const char* mode, int loc )
{
	if( loc != FS_FILELOC_MOD && loc != FS_FILELOC_BASE )
		return 0;
	if( loc == FS_FILELOC_MOD && !c_mod_valid )
		return 0;
		
	char buff[1024];
	
	if( loc == FS_FILELOC_BASE )
		strcpy( buff, c_basedir );
	else
		strcpy( buff, c_moddir );
		
	strcat( buff, filename );
	FS_MakeBurgerPath( buff, buff );
	
	return OpenAFile( buff, mode );	
}

bool FS_GetFileModTimeLoc( const char* filename, TimeDate_t* td, int loc, int flags )
{
	if( flags&_fs_flag_disk )
	{
		char fullpath[1024];
		FS_GetFullPath( filename, fullpath, loc );
		
		if( GetFileModTime( fullpath, td ) == FALSE )
			return FALSE;
	}
	if( flags&_fs_flag_paks )
	{
		if( GetFileModTimeInOpenPaks( filename, td ) == FALSE )
			return FALSE;
	}
			
	return TRUE;
}

bool FS_GetFileModTime( const char* filename, TimeDate_t* td, int flags )
{
	int loc = FS_GetFileLoc( filename, flags );
	if( loc == FS_FILELOC_BAD )
		return TRUE;
	
	return FS_GetFileModTimeLoc( filename, td, loc, flags );
}

int FS_GetFileLoc( const char* filename, int flags )
{
	if( flags&_fs_flag_disk )
	{
		char fullpath[512];
		
		if( c_mod_valid )
		{
			FS_GetFullPath( filename, fullpath, FS_FILELOC_MOD );
			FS_MakeBurgerPath( fullpath, fullpath );
			
			if(DoesFileExist( fullpath ))
				return FS_FILELOC_MOD;
		}
		
		FS_GetFullPath( filename, fullpath, FS_FILELOC_BASE );
		FS_MakeBurgerPath( fullpath, fullpath );
		
		if(DoesFileExist( fullpath ))
			return FS_FILELOC_BASE;
	}
	if( flags&_fs_flag_paks )
	{
		int loc;
		char fullpath[512];
		strcpy( fullpath, filename );
		FS_MakeBurgerPath( fullpath, fullpath );
		
		if( FileExistsInOpenPaks( fullpath, &loc ) )
			return loc;
	}
	
	return FS_FILELOC_BAD;
}

bool FS_OpenPaks(void)
{
	int i;
	char buff[256];
	
	if( c_mod_valid )
	{
		for(i = MAX_PAK_FILES-1; i >= 0 ; i--)
		{
			sprintf(buff, "/pak%i.cluster", i);
			FS_OpenPakFile( buff, FS_FILELOC_MOD );
		}
	}
		
	for(i = MAX_PAK_FILES-1; i >= 0 ; i--)
	{
		sprintf(buff, "/pak%i.cluster", i);
		FS_OpenPakFile( buff, FS_FILELOC_BASE );
	}
	
	return FALSE;
}

LongWord FS_ftell( FILE* fp )
{
	return ftell( fp );
}

bool FS_fread( FILE* fp, void* data, LongWord length )
{
	return fread( data, 1, length, fp ) != length;
}

bool FS_fread_float( FILE* fp, float* data )
{
	unsigned long l;
	
	if( FS_fread_ulong( fp, &l ) )
		return TRUE;
		
	*data = ((float*)&l)[0];
	return FALSE;
}

bool FS_fread_double( FILE* fp, double* data )
{
	double d;
	
	if( FS_fread( fp, &d, sizeof(double) ) )
		return TRUE;
	
	*data = LoadIntelDouble( &d );
	return FALSE;
}

bool FS_fread_uchar( FILE* fp, unsigned char* data )
{
	return FS_fread( fp, data, sizeof(unsigned char) );
}

bool FS_fread_schar( FILE* fp, char* data )
{
	return FS_fread( fp, data, sizeof(char) );
}

bool FS_fread_ulong( FILE* fp, LongWord* data )
{
	LongWord swap;
	if( FS_fread( fp, &swap, sizeof(LongWord) ) )
		return TRUE;
		
	*data = LoadIntelLong( swap );
	return FALSE;
}

bool FS_fread_slong( FILE* fp, int* data )
{
	int swap;
	if( FS_fread( fp, &swap, sizeof(int) ) )
		return TRUE;
		
	*data = LoadIntelLong( swap );
	return FALSE;
}

bool FS_fread_ushort( FILE* fp, unsigned short* data )
{
	unsigned short swap;
	if( FS_fread( fp, &swap, sizeof(unsigned short) ) )
		return TRUE;
		
	*data = LoadIntelUShort( swap );
	return FALSE;
}

bool FS_fread_sshort( FILE* fp, short* data )
{
	short swap;
	if( FS_fread( fp, &swap, sizeof(short) ) )
		return TRUE;
		
	*data = LoadIntelShort( swap );
	return FALSE;
}

bool FS_fread_string( FILE* fp, char** string, LongWord* length )
{
	LongWord l2;
	
	if( FS_fread_ulong( fp, &l2 ) )
		return TRUE;
		
	*string = (char*)OS_AllocAPointer( l2+1 );
	
	if( l2 > 0 )
	{
		if( FS_fread( fp, (*string), l2 ) )
			return TRUE;
	}
			
	if( length )
		*length = l2;
		
	(*string)[l2] = 0;
	
	return FALSE;
}

bool FS_fwrite( FILE* fp, const void* data, LongWord length )
{
	return fwrite( data, 1, length, fp ) != length;
}

bool FS_fwrite_float( FILE* fp, float data )
{
	unsigned long l = ((unsigned long*)&data)[0];
	return FS_fwrite_ulong( fp, l );
}

bool FS_fwrite_double( FILE* fp, double data )
{
	double s = LoadIntelDouble( &data );
	return FS_fwrite( fp, &s, sizeof(double) );
}

bool FS_fwrite_uchar( FILE* fp, unsigned char data )
{
	return FS_fwrite( fp, &data, sizeof(unsigned char) );
}

bool FS_fwrite_schar( FILE* fp, char data )
{
	return FS_fwrite( fp, &data, sizeof(char) );
}

bool FS_fwrite_ulong( FILE* fp, LongWord data )
{
	LongWord swap = LoadIntelLong( data );
	return FS_fwrite( fp, &swap, sizeof(LongWord) );
}

bool FS_fwrite_slong( FILE* fp, int data )
{
	int swap = LoadIntelLong( data );
	return FS_fwrite( fp, &swap, sizeof(int) );
}

bool FS_fwrite_ushort( FILE* fp, unsigned short data )
{
	unsigned short swap = LoadIntelUShort( data );
	return FS_fwrite( fp, &swap, sizeof(unsigned short) );
}

bool FS_fwrite_sshort( FILE* fp, short data )
{
	short swap = LoadIntelShort( data );
	return FS_fwrite( fp, &swap, sizeof(short) );
}

bool FS_fwrite_string( FILE* fp, const char* string )
{
	LongWord len = strlen( string );
	
	if( FS_fwrite_ulong( fp, len ) )
		return TRUE;
	
	return FS_fwrite( fp, string, len );
}

bool FS_fseek( FILE* fp, int ofs, int relative )
{
	return fseek( fp, ofs, relative ) != 0;
}

void FS_fclose( FILE* fp )
{
	fclose( fp );
}

bool FS_ImageExists( const char* filename, char* outfile, int flags)
{
	char temp[1024];
	
	if( outfile != 0 && outfile != filename )
		outfile[0] = 0;
		
	strcpy( temp, filename );
	strcat( temp, ".jpg" );
	
	if( FS_FileExists( temp, flags ) )
	{
		if( outfile ) strcpy( outfile, temp );
		return TRUE;
	}
	
	strcpy( temp, filename );
	strcat( temp, ".tga" );
	
	if( FS_FileExists( temp, flags ) )
	{
		if( outfile ) strcpy( outfile, temp );
		return TRUE;
	}
		
	strcpy( temp, filename );
	strcat( temp, ".bmp" );
	
	if( FS_FileExists( temp, flags ) )
	{
		if( outfile ) strcpy( outfile, temp );
		return TRUE;
	}
	
	strcpy( temp, filename );
	strcat( temp, ".gif" );
	
	if( FS_FileExists( temp, flags ) )
	{
		if( outfile ) strcpy( outfile, temp );
		return TRUE;
	}
	
	return FALSE;
}

void FS_AppendResolutionToFilename( const char* infile, char* outfile )
{
	char temp[1024];
	
	sprintf( temp, "%s_%dx%d", infile, ScreenWidth, ScreenHeight );
	strcpy( outfile, temp );
}

static bool FileExistsInOpenPaks( const char* filename, int* loc )
{
	int i, num;
	
	for(i = 0; i < c_numpaks; i++)
	{
		OS_ASSERT( c_paks[i].fp );
		num = FindFileInPak( &c_paks[i], filename );
		if( num > BAD_PAK_FILE_NUM )
		{
			if( loc )
				*loc = (c_paks[i].base)?FS_FILELOC_BASE:FS_FILELOC_MOD;
				
			return TRUE;
		}
	}
	
	return FALSE;
}

static bool GetFileModTimeInOpenPaks( const char* filename, TimeDate_t* td )
{
	int i, num;
	
	for(i = 0; i < c_numpaks; i++)
	{
		OS_ASSERT( c_paks[i].fp );
		
		num = FindFileInPak( &c_paks[i], filename );
		if( num > BAD_PAK_FILE_NUM )
		{
			*td = c_paks[i].dir[num].timedate;
			return FALSE;
		}
	}
	
	return TRUE;
}

static bool LoadFileFromOpenPaks( const char* filename, void** data, LongWord* length, bool handle )
{
	int i, num;
	
	for(i = 0; i < c_numpaks; i++)
	{
		OS_ASSERT( c_paks[i].fp );
		num = FindFileInPak( &c_paks[i], filename );
		if( num > BAD_PAK_FILE_NUM )
		{
			if( handle )
			{
				if( !LoadFileHandleFromPak( &c_paks[i], num, (void***)data ) )
				{
					//Con_DPrintf("loaded '%s' out of pak '%s'\n", filename, c_paks[i].name );
					return FALSE;
				}
			}
			else
			{
				if( !LoadFileFromPak( &c_paks[i], num, data, length ) )
				{
					//Con_DPrintf("loaded '%s' out of pak '%s'\n", filename, c_paks[i].name );
					return FALSE;
				}
			}
		}
	}
	
	return TRUE;
}

static bool LoadFileHandleFromPak( PakFile_t* pk, int num, void*** out_data )
{
	OS_ASSERT( num > BAD_PAK_FILE_NUM );
	
	PakEntry_t* file = &pk->dir[num];
	
	OS_ASSERT( pk->fp );
	
	FS_fseek( pk->fp, file->ofs, SEEK_SET );
	
	void* comp_data = OS_AllocAPointer( file->comp_size );
	if( FS_fread( pk->fp, comp_data, file->comp_size ) )
	{
		OS_DeallocAPointer( comp_data );
		return TRUE;
	}
	
	//
	// decompress.
	//
	void** h_data = AllocAHandle( file->size );
	if( !h_data )
	{
		OS_DeallocAPointer( comp_data );
		return TRUE;
	}
	
	void* data = LockAHandle( h_data );
	
	DInflate( (Byte*)data, (Byte*)comp_data, file->size, file->comp_size );
	DInflate( 0, 0, 0, 0 );
	
	OS_DeallocAPointer( comp_data );
	
	UnlockAHandle( h_data );
	
	*out_data = h_data;
	
	return FALSE;
}

static bool LoadFileFromPak( PakFile_t* pk, int num, void** out_data, LongWord* out_length )
{
	OS_ASSERT( num > BAD_PAK_FILE_NUM );
	
	PakEntry_t* file = &pk->dir[num];
	
	OS_ASSERT( pk->fp );
	
	FS_fseek( pk->fp, file->ofs, SEEK_SET );
	
	void* comp_data = OS_AllocAPointer( file->comp_size );
	if( FS_fread( pk->fp, comp_data, file->comp_size ) )
	{
		OS_DeallocAPointer( comp_data );
		return TRUE;
	}
	
	//
	// decompress.
	//
	void* data = OS_AllocAPointer( file->size );
	DInflate( (Byte*)data, (Byte*)comp_data, file->size, file->comp_size );
	DInflate( 0, 0, 0, 0 );
	
	OS_DeallocAPointer( comp_data );
	
	*out_data = data;
	if( out_length )
		*out_length = file->size;
	
	return FALSE;
}

static int FindFileInPak( PakFile_t* pk, const char* filename )
{	
	int start, end, mid;
	
	start = 0;
	end = pk->num_files;
	
binary_search_pak:

	if( end < start+4 )
	{
		//
		// we're close enough, just linear from here.
		//
		int i;
		for(i = start; i < end; i++)
		{
			OS_ASSERT( i < pk->num_files );
			if( !stricmp( filename, pk->dir[i].name ) )
				return i;
		}
		
		return BAD_PAK_FILE_NUM;
	}
	
	mid = (end-start)>>1;
	mid += start;
	
	OS_ASSERT( mid < pk->num_files );
	
	int ofs = stricmp( pk->dir[mid].name, filename );
	
	if( ofs == 0 )
	{
		return mid;
	}
	if( ofs > 0 )
	{
		end = mid;
		goto binary_search_pak;
	}
	
	start = mid;
	goto binary_search_pak;
	
	return BAD_PAK_FILE_NUM;
}

bool FS_OpenPakFile( const char* file, int loc )
{
	if( c_numpaks >= MAX_PAK_FILES )
	{
		return TRUE;
	}
	
	PakFile_t* pk = &c_paks[c_numpaks];
	char fullpath[1024];
	
	FS_GetFullPath( file, fullpath, loc );
	
	if( OpenPak( fullpath, pk ) )
		return TRUE;
	
	ShadowPakFiles( pk );
	
	pk->base = (loc==FS_FILELOC_BASE)?TRUE:FALSE;
	
	c_numpaks++;
	return FALSE;
}

static void ShadowPakFiles( PakFile_t* pk )
{
	int i, k;
	PakFile_t* testpk;
	PakEntry_t* e;
	
	if(c_numpaks == 0 )
		return;
		
	for(i = 0; i < pk->num_files; i++)
	{
		e = &pk->dir[i];
		e->shadowed = FALSE;
		
		for(k = 0; k < c_numpaks; k++)
		{
			testpk = &c_paks[k];
			if( FindFileInPak( testpk, e->name ) != BAD_PAK_FILE_NUM )
			{
				e->shadowed = TRUE;
				break;
			}
		}
	}
}

static bool OpenPak( const char* filename, PakFile_t* pk )
{
	memset( pk, 0, sizeof(PakFile_t) );
	
	pk->fp = FS_fopen( filename, "rb" );
	if( !pk->fp )
		return TRUE;
		
	if( ReadPakHeader( pk ) )
	{
		ClosePak( pk );
		return TRUE;
	}
	
	if( DecompressPakDir( pk ) )
	{
		ClosePak( pk );
		return TRUE;
	}
	
	strcpy( pk->name, filename );
	
	return FALSE;
}

static bool ClosePak( PakFile_t* pk )
{
	if( pk->dir )
		OS_DeallocAPointer( pk->dir );
	
	if( pk->fp )
		fclose(pk->fp);
		
	return FALSE;
}

static bool ReadPakHeader( PakFile_t* pk )
{
	OS_ASSERT( pk->fp );
	
	FS_fseek( pk->fp, 0, SEEK_SET );
	if( FS_fread( pk->fp, &pk->header, sizeof(PakHeader_t) ) )
	{
		return TRUE;
	}
	
	pk->header.id = LoadIntelLong( pk->header.id );
	pk->header.magic = LoadIntelLong( pk->header.magic );
	pk->header.dir_ofs = LoadIntelLong( pk->header.dir_ofs );
	pk->header.dir_size = LoadIntelLong( pk->header.dir_size );
	
	if( pk->header.id != PAK_ID || pk->header.magic != PAK_MAGIC )
		return TRUE;
		
	return FALSE;
}

static bool DecompressPakDir( PakFile_t* pk )
{
	//
	// seek to the dir.
	//
	void* comp_dir = OS_AllocAPointer( pk->header.dir_comp_size );
	
	FS_fseek( pk->fp, pk->header.dir_ofs, SEEK_SET );
	if( FS_fread( pk->fp, comp_dir, pk->header.dir_comp_size ) )
	{
		return TRUE;
	}
	
	//
	// allocate the real directory.
	//
	pk->dir = (PakEntry_t*)OS_AllocAPointer( pk->header.dir_size );
	pk->num_files = pk->header.dir_size/sizeof(PakEntry_t);
	
	//
	// decompress.
	//
	DInflate( (Byte*)pk->dir, (Byte*)comp_dir, pk->header.dir_size, pk->header.dir_comp_size );
	
	//
	// shutdown.
	//
	DInflate( 0, 0, 0, 0 );
	
	OS_DeallocAPointer( comp_dir );
	
	//
	// endian swap.
	//
	int i;
	for(i = 0; i < pk->num_files; i++)
	{
		pk->dir[i].ofs = LoadIntelLong( pk->dir[i].ofs );
		pk->dir[i].comp_size = LoadIntelLong( pk->dir[i].comp_size );
		pk->dir[i].size = LoadIntelLong( pk->dir[i].size );
	}
	
	return FALSE;
}

bool FS_ExpandPakToDir( const char* pak, const char* dir )
{
	char pakpath[1024];
	char filepath[1024];
	PakFile_t pk;
	int i, expand_count;
	int numerrors;
	void* data;
	LongWord length;
	FILE* fp;
	bool f;
	
	strcpy( pakpath, "9:");
	strcat( pakpath, pak );
	
	if( OpenPak( pakpath, &pk ) )
	{
		Con_LPrintf("FS_ExpandPakToDir: unable to open '%s'\n", pak );
		return TRUE;
	}
	
	Con_LPrintf("expanding: %s, %d file(s)\n", pak, pk.num_files);
	
	expand_count = 0;
	numerrors = 0;
	for(i = 0; i < pk.num_files; i++)
	{
		if( LoadFileFromPak( &pk, i, &data, &length ) )
		{
			Con_LPrintf("ERROR: FS_ExpandPakToDir: failed to expand '%s'\n", pk.dir[i].name);
			numerrors++;
			continue;
		}
		
		if( data == 0 || length == 0 )
		{
			Con_LPrintf("ERROR: FS_ExpandPakToDir: zero length file '%s'\n", pk.dir[i].name );
			numerrors++;
			continue;
		}
		
		strcpy( filepath, "9:" );
		strcat( filepath, dir );
		strcat( filepath, pk.dir[i].name );
		
		CreateDirectoryPath2( filepath );
		fp = FS_fopen( filepath, "wb" );
		if( !fp )
		{
			Con_LPrintf("ERROR: FS_ExpandPakToDir: unable to open output file '%s'\n", filepath );
			OS_DeallocAPointer( data );
			numerrors++;
			continue;
		}
		
		f = FS_fwrite( fp, data, length );
		OS_DeallocAPointer( data );
		FS_fclose( fp );
		
		if( f )
		{
			Con_LPrintf("ERROR: FS_ExpandPakToDir: failed to write '%s'\n", filepath );
			numerrors++;
			continue;
		}
		
		Con_LPrintf("...expanded '%s', %d byte(s)\n", pk.dir[i].name, length );
		expand_count++;
	}
	
	ClosePak( &pk );
	Con_LPrintf("successfuly expanded %i files, %i errors\n", expand_count, numerrors);
	
	return FALSE;
}

bool FS_CreatePakFromDir( const char* dir, const char* pkfile )
{
	//
	// creates the pak file from the requested directory.
	// NOTE: this does not add current pak files.
	// this directory must be located inside the base directory.
	//
	c_numfails=0;
	
	FS_FileSearch_t search;
	FS_FileSearchData_t file;
	
	if( FS_OpenSearch( dir, 0, &search, _fs_osf_disk|_fs_osf_recurse ) )
	{
		Con_LPrintf("the directory '%s' cannot be found!\n", dir);
		return TRUE;
	}
	
	//
	// start the pak.
	//
	if( FS_BeginPakFile( pkfile ) )
	{
		FS_CloseSearch( &search );
		return TRUE;
	}
	
	int len = strlen( dir );
	char name[256];
	
	while( FS_GetNextFile( &search, &file ) == FALSE )
	{
		strcpy( name, &file.name[len+1] ); // skip the base directory.
		FS_AddFileToPak( file.name, name );
	}
	
	FS_CloseSearch( &search );
	
	FS_EndPakFile();
	
	return FALSE;
}

static bool WriteFileToPak( const char* filename, const char* entry_name, PakFile_t* pk )
{
	if( strlen( filename ) > PAKENTRY_NAME_LEN )
	{
		Con_LPrintf("WriteFileToPak: '%s' filename exceeds the filename size limit of %d characters.\n", filename, PAKENTRY_NAME_LEN );
		return TRUE;
	}
	
	//
	// first load up this sucker.
	//
	LongWord len;
	void* data = FS_LoadFile( filename, &len, _fs_flag_disk );
	
	if( !data || !len )
	{
		Con_LPrintf("WriteFileToPak: unable to load '%s'\n", filename );
		return TRUE;
	}
	
	TimeDate_t td;
	if( FS_GetFileModTime( filename, &td, _fs_flag_disk ) )
	{
		OS_DeallocAPointer( data );
		Con_LPrintf("WriteFileToPak: FS_GetFileModTime: failed!\n" );
		return TRUE;
	}
		
	//
	// make a new entry for it.
	//
	MakePakEntry_t* e = (MakePakEntry_t*)OS_AllocAPointerClear( sizeof(MakePakEntry_t) );
	
	e->n = c_make_files;
	c_make_files = e;
	
	//
	// make the name lower case.
	//
	int i;
	for(i = 0; filename[i] != 0; i++)
	{
		e->e.name[i] = (char)tolower( entry_name[i] );
	}
	
	e->e.ofs = ftell( pk->fp );
	e->e.size = len;
	e->e.timedate = td;
	
	//
	// compress this file.
	//
	void** h_comp_data = EncodeInflate( (Byte*)data, len );
	
	OS_DeallocAPointer( data );
	
	if( !h_comp_data )
	{
		Con_LPrintf( "WriteFileToPak: unable to compress file '%s'!\n", filename );
		return TRUE;
	}
	
	void* comp_data = LockAHandle( h_comp_data );
	len = GetAHandleSize( h_comp_data );
	
	e->e.comp_size = len;
	
	//
	// write the entry data.
	//
	if( FS_fwrite( pk->fp, comp_data, len ) )
	{
		Con_LPrintf("WriteFileToPak: failed to write '%s'!\n", filename );
		UnlockAHandle( h_comp_data );
		DeallocAHandle( h_comp_data );
	}
	
	UnlockAHandle( h_comp_data );
	DeallocAHandle( h_comp_data );
	
	Con_LPrintf("%.0f%% (%d/%d) %s\n", 100.0f-((float)e->e.comp_size)/((float)e->e.size)*100.0f, e->e.size, e->e.comp_size, filename );
	
	pk->num_files++;
	
	return FALSE;
}


static bool SortPakFiles( PakEntry_t* dir, int num_files )
{
	qsort( dir, num_files, sizeof(PakEntry_t), SortPakEntry );
	return FALSE;
}

static int ANSICALL SortPakEntry( const void* p1, const void* p2 )
{
	PakEntry_t* e1 = (PakEntry_t*)p1;
	PakEntry_t* e2 = (PakEntry_t*)p2;
	
	return stricmp( e1->name, e2->name );
}

static bool FreeMakePakFiles( void )
{
	if( !c_make_files )
		return FALSE;
		
	MakePakEntry_t* n, *e;
	
	for( e = c_make_files; e; e = n )
	{
		n = e->n;
		OS_DeallocAPointer( e );
	}
	
	c_make_files = 0;
	return FALSE;
}

static bool MakePak( const char* filename, PakFile_t* pk )
{
	//
	// open the requested file.
	//
	FILE* fp = FS_fopen( filename, "wb" );
	if( !fp )
	{
		Con_LPrintf("MakePak: unable to open '%s'\n", filename );
		return TRUE;
	}
	
	memset( pk, 0, sizeof(PakFile_t) );
			
	pk->fp = fp;
	
	PakHeader_t ph;
	if( FS_fwrite( fp, &ph, sizeof(PakHeader_t) ) )
	{
		Con_LPrintf("MakePak: unable to write header for '%s'\n", filename );
		return TRUE;
	}
	
	return FALSE;
}

static bool FinishMakePak( PakFile_t* pk )
{
	if( pk->num_files == 0 )
	{
		FS_fclose( pk->fp );
		pk->fp = 0;
		
		FreeMakePakFiles();
		
		Con_LPrintf("FinishMakePak: pak has no files in it. pak creation aborted!\n");
		return FALSE;
	}
	
	//
	// get directory offset.
	//
	pk->header.dir_ofs = LoadIntelLong(ftell(pk->fp));
	
	//
	// allocate a contiguous block of memory for the directory.
	//
	int i;
	PakEntry_t* dir;
	MakePakEntry_t* e;
	
	//
	// copy the directory.
	//
	dir = (PakEntry_t*)OS_AllocAPointer( sizeof(PakEntry_t)*pk->num_files );
	i = 0;
	for( e = c_make_files; e; e = e->n, i++ )
	{
		FastMemCpy( dir[i].name, e->e.name, PAKENTRY_NAME_LEN+1 );
		
		dir[i].ofs = LoadIntelLong( e->e.ofs );
		dir[i].comp_size = LoadIntelLong( e->e.comp_size );
		dir[i].size = LoadIntelLong( e->e.size );
		dir[i].shadowed = 0;
		dir[i].padd = 0;
	}
	
	FreeMakePakFiles();
	SortPakFiles( dir, pk->num_files );
	
	pk->header.dir_size = LoadIntelLong(sizeof(PakEntry_t)*pk->num_files);
	
	//
	// compress the directory.
	//
	void** h_comp_dir = EncodeInflate( (Byte*)dir, sizeof(PakEntry_t)*pk->num_files );
	
	OS_DeallocAPointer( dir );
	
	if( !h_comp_dir )
	{
		Con_LPrintf("FinishMakePak: EncodeInflate failed compressing directory!\n");
		
		fclose(pk->fp);
		
		return TRUE;
	}
	
	LongWord size = GetAHandleSize( h_comp_dir );
	void* comp_dir = LockAHandle( h_comp_dir );
	
	//
	// write the directory.
	//
	if( FS_fwrite( pk->fp, comp_dir, size ) )
	{
		Con_LPrintf("FinishMakePak: failed to write directory!\n");
		fclose(pk->fp);
		
		UnlockAHandle( h_comp_dir );
		DeallocAHandle( h_comp_dir );
	
		return TRUE;
	}
	
	UnlockAHandle( h_comp_dir );
	DeallocAHandle( h_comp_dir );
	
	
	//
	// backup and write the header.
	//
	fseek( pk->fp, 0, SEEK_SET );
	
	pk->header.id = LoadIntelLong( PAK_ID );
	pk->header.magic = LoadIntelLong( PAK_MAGIC );
	pk->header.dir_comp_size = LoadIntelLong( size );
	
	if( FS_fwrite( pk->fp, &pk->header, sizeof(PakHeader_t) ) )
	{
		Con_LPrintf("FinishMakePak: failed to write header!\n");
		fclose(pk->fp);
		return TRUE;
	}
	
	fclose(pk->fp);
	pk->fp = 0;
	
	Con_LPrintf("successfully wrote %d file(s) to cluster, %d error(s)\n", pk->num_files, c_numfails);
	
	return FALSE;
}

bool FS_BeginPakFile( const char* file )
{
	c_make_files = 0;
	return MakePak( file, &c_makepak );
}

bool FS_EndPakFile()
{
	return FinishMakePak( &c_makepak );
}

bool FS_AddFileToPak( const char* file, const char* entry_name )
{
	if(WriteFileToPak( file, entry_name, &c_makepak ))
	{
		c_numfails++;
		return TRUE;
	}
	return FALSE;
}

StreamHandle_t* FS_GetFileStream( const char* filename, int flags )
{
	void** handle;
	handle = FS_LoadFileHandle( filename, flags );
	if(!handle)
		return 0;
			
	StreamHandle_t* h = (StreamHandle_t*)OS_AllocAPointer(sizeof(StreamHandle_t));
	if(!h)
	{
		DeallocAHandle( handle );
		return 0;
	}

	StreamHandleInitGet( h, handle );
	return h;
}

void FS_GetFullPath(const char* filename, char* fullname, int loc)
{
	fullname[0] = 0;
	
	if( loc == FS_FILELOC_BASE )
	{
		strcpy(fullname, c_basedir);
	}
	else
	if( loc == FS_FILELOC_MOD )
	{
		strcpy(fullname, c_moddir );
	}
	
	strcat(fullname, filename);
}

void FS_MakeBurgerPath( const char* filename, char* outname )
{
	/*
	just changes '/' && '\' to ':'
	*/
	int i;
	int len = strlen( filename );
	
	for(i = 0; i < len; i++)
	{
		if( filename[i] == '/' || filename[i] == '\\' )
			outname[i] = ':';
		else
			outname[i] = filename[i];
	}
	
	outname[i] = 0;
}

bool FS_FileExists( const char* filename, int flags )
{
	OS_ASSERT( flags&(_fs_flag_disk|_fs_flag_paks) );
	
	return FS_GetFileLoc(filename,flags) != FS_FILELOC_BAD;
}

bool FS_OpenSearch( const char* dir, const char* ext, FS_FileSearch_t* fs_search, int flags )
{
	/* always on disk for now */
	OS_ASSERT( fs_search );
	
	if( flags&_fs_osf_fullpath )
		flags &= ~_fs_osf_paks;
		
	if( ext && ext[0] && strlen(ext) <= FS_MAX_SEARCH_EXT_LENGTH )
		strcpy( fs_search->ext, ext );
	else
		fs_search->ext[0] = 0;
	
	if( dir && dir[0] && strlen(dir) <= FS_MAX_EXT_PATH_LENGTH )
	{
		if( dir[0] != ':' && (flags&_fs_osf_fullpath) == 0 )
		{
			sprintf( fs_search->dir, ":%s", dir );
		}
		else
		{
			strcpy( fs_search->dir, dir );
		}
	}
	else
	{
		fs_search->dir[0] = 0;
	}
			
	fs_search->open = FALSE;
	fs_search->flags = flags;
	
	if( (flags&(_fs_osf_paks|_fs_osf_disk)) == 0 )
		return TRUE;
	
	if( (flags&_fs_osf_paks) && !(flags&_fs_osf_disk) )
	{
		return FS_OpenPakSearch( fs_search, c_mod_valid == FALSE );
	}
		
	//
	// do mod(disk,paks), then base(disk,paks).
	//
	
	if( FS_OpenDiskSearch( fs_search, c_mod_valid == FALSE ) ) // didn't find it on disk.
	{
		//
		// failed to open.
		//
		if( flags&_fs_osf_paks ) // we can search paks?
			return FS_OpenPakSearch(fs_search, c_mod_valid == FALSE );
			
		return TRUE;
	}
	
	return FALSE;
}

bool FS_GetNextFile( FS_FileSearch_t* fs_search, FS_FileSearchData_t* fs_data )
{
	OS_ASSERT( fs_search->open );
	
	if( fs_search->on_disk )
	{
do_disk_search:
		bool f = FS_GetNextFileFromDiskSearch( fs_search, fs_data );
		if( f )
		{
			if( fs_search->recursed_search )
				return TRUE;
				
			if( (fs_search->flags&_fs_osf_paks) )
			{
				//
				// we are going to search paks next, close this search.
				//
				FS_CloseSearch( fs_search );
				
				if( FS_OpenPakSearch( fs_search, fs_search->base ) )
				{
					if( fs_search->base == FALSE ) // switch to disk search of the base.
					{
						if( FS_OpenDiskSearch( fs_search, TRUE ) )
						{
							if( FS_OpenPakSearch( fs_search, TRUE ) ) // switch to pak search of the base
							{
								return TRUE; // we are done.
							}
							else
							{
								goto do_pak_search;
							}
						}
						else
						{
							goto do_disk_search;
						}
					}
					else
					{
						return TRUE;
					}
				}
								
				//
				// get the next file.
				//
				goto do_pak_search;
			}
			else
			{
				FS_CloseSearch( fs_search );
				
				//
				// not searching paks.
				//
				if( fs_search->base == FALSE )
				{
					//
					// try to search disk base.
					//
					if( FS_OpenDiskSearch( fs_search, TRUE ) == FALSE )
					{
						goto do_disk_search;
					}
				}
			}
		}
		
		return f;
	}
	else
	{	
do_pak_search:
		//
		// we're doin' paks.
		//
		if( FS_GetNextFileFromPakSearch( fs_search, fs_data ) == TRUE )
		{
			//
			// this died. do we switch to the base folder?
			//
			if( fs_search->base == FALSE && (fs_search->flags&_fs_osf_disk) )
			{
				FS_CloseSearch( fs_search );
				if( FS_OpenDiskSearch( fs_search, TRUE ) == FALSE )
				{
					goto do_disk_search;
				}
			}
		}
		else
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

bool FS_CloseSearch( FS_FileSearch_t* fs_search )
{
	FS_FileSearch_t* next, *head;
	
	OS_ASSERT( fs_search );
	
	if( fs_search->open == FALSE ) /* already closed */
		return FALSE;
		
	head = fs_search;
	for( ; fs_search; fs_search = next )
	{
		next = fs_search->recursed_search;
		
		if( fs_search->on_disk )
			FS_CloseDiskSearch( fs_search );
		else
			FS_ClosePakSearch( fs_search );
			
		if( fs_search != head ) /* make safe w/ autos */
			OS_DeallocAPointer( fs_search );
	}
	
	/* mark as closed */
	head->open = FALSE;
	
	return FALSE;
}

static bool FS_OpenPakSearch(FS_FileSearch_t* fs_search, bool base )
{
	OS_ASSERT( fs_search->open == FALSE );
	
	if( c_numpaks < 1 )
		return TRUE;
		
	fs_search->on_disk = FALSE;
	fs_search->open = TRUE;
	fs_search->recursed_search = 0;
	fs_search->base = base;
	fs_search->pak_num = 0;
	fs_search->pak_file_num = 0;
	
	return FALSE;
}

static bool FS_ClosePakSearch( FS_FileSearch_t* fs_search )
{
	fs_search->open = FALSE;
	return FALSE;
}

static bool FS_GetNextFileFromPakSearch( FS_FileSearch_t* fs_search, FS_FileSearchData_t* fs_data )
{
	int i, k;
	PakFile_t* pk;
	PakEntry_t* file;
	bool matched_dir;
	
	int dir_len=0;
	
	if( fs_search->dir[0] )
		dir_len = strlen( fs_search->dir );
		
	for(i = fs_search->pak_num; i < c_numpaks; i++)
	{
		pk = &c_paks[i];
		OS_ASSERT( pk->fp );
		
		for(k = fs_search->pak_file_num; k < pk->num_files; k++)
		{
			matched_dir = FALSE;
			
			file = &pk->dir[k];
			
			if( file->shadowed )
			{
				int b = 0;
				continue;
			}
							
			//
			// if this file is shadowed on the disk, and we're doing a disk search as well,
			// discard it.
			//
			if( fs_search->flags&_fs_osf_disk )
			{
				int loc = FS_GetFileLoc( file->name, _fs_osf_disk );
				if( loc != FS_FILELOC_BAD )
				{
					if( fs_search->base && loc == FS_FILELOC_BASE )
						continue;
					if( !fs_search->base && loc == FS_FILELOC_MOD )
						continue;
				}
			}
			
			//
			// do these suckers lay in the same directory.
			//
			if( dir_len > 0 )
			{
				int len = strlen(file->name);
				if( len <= dir_len )
					continue;
					
				int j;
				
				for(j = 0; j < dir_len; j++)
				{
					if( tolower( file->name[j] ) != tolower( fs_search->dir[j] ) )
						break;
				}
				
				if( j < dir_len )
					continue;
			}
			
			//
			// the directories match. do the extensions?
			//
			if( fs_search->ext[0] )
			{
				char* ext = StrGetFileExtension( file->name );
				if( ext == 0 || ext == file->name )
					continue;
					
				if( stricmp( ext, fs_search->ext ) )
					continue;
			}
			
			fs_data->hidden = fs_data->system = fs_data->locked = FALSE;
			CHKSTRLEN( file->name, FS_MAX_FILENAME_LENGTH, "file-system filename" );
			strcpy( fs_data->name, file->name );
			
			break;
		}
		
		//
		// did we find one?
		//
		if( k < pk->num_files )
		{
			fs_search->pak_file_num = k+1;
			break;
		}
		
		fs_search->pak_file_num = 0;
	}
	
	fs_search->pak_num = i;
	
	return fs_search->pak_num >= c_numpaks;
}

static bool FS_OpenDiskSearch( FS_FileSearch_t* fs_search, bool base )
{
	char fullpath[1024];
	
	fs_search->base = base;
	
	if( fs_search->flags&_fs_osf_fullpath )
	{
		strcpy( fullpath, fs_search->dir );
	}
	else
	{
		if( base || c_mod_valid == FALSE )
		{
			strcpy(fullpath, c_basedir);
			strcat( fullpath, fs_search->dir );
		}
		else
		{
			strcpy(fullpath, c_moddir);
			strcat(fullpath, fs_search->dir);
		}
	}
			
	fs_search->on_disk = TRUE;
	
	if( OpenADirectory(&fs_search->dir_search, fullpath) )
	{
		//if( base == FALSE )
		//	return FS_OpenDiskSearch( fs_search, TRUE );
			
		return TRUE;
	}
	
	fs_search->open = TRUE;
	fs_search->recursed_search = 0;
	return FALSE;
}

static bool FS_CloseDiskSearch( FS_FileSearch_t* fs_search )
{
	OS_ASSERT( fs_search->on_disk );
	if( fs_search->open )
		CloseADirectory( &fs_search->dir_search );
	fs_search->open = FALSE;
	return FALSE;
}

static bool FS_GetNextFileFromDiskSearch( FS_FileSearch_t* fs_search, FS_FileSearchData_t* fs_data )
{
	OS_ASSERT( fs_search );
	OS_ASSERT( fs_data );
	OS_ASSERT( fs_search->on_disk );
	
	if( fs_search->recursed_search )
	{
		/* defer */
		if( FS_GetNextFileFromDiskSearch( fs_search->recursed_search, fs_data ) )
		{
			/* close the recursed search. */
			FS_CloseDiskSearch( fs_search->recursed_search );
			OS_DeallocAPointer( fs_search->recursed_search );
			fs_search->recursed_search = 0;
		}
		else
			return FALSE;	/* got it */
	}
	
retry_getnextfilefromdisksearch:

	if( GetADirectoryEntry( &fs_search->dir_search ) )
		return TRUE;
	
	/* a directory */
	if( fs_search->dir_search.Dir )
	{
		if( fs_search->flags&_fs_osf_recurse ) /* go down the directory tree? */
		{
			FS_FileSearch_t* new_search;
			char ext_path[FS_MAX_EXT_PATH_LENGTH+1];
			
			new_search = (FS_FileSearch_t*)OS_AllocAPointer( sizeof(FS_FileSearch_t) );
			if( !new_search )
				return TRUE; /* error */
			
			/* setup another search */
			strcpy( ext_path, fs_search->dir );
			strcat( ext_path, ":");
			strcat( ext_path, fs_search->dir_search.Name );
			
			if( FS_OpenSearch( ext_path, fs_search->ext, new_search, _fs_osf_disk|_fs_osf_recurse ) )
			{
				OS_DeallocAPointer( new_search );
				//return FS_GetNextFileFromDiskSearch( fs_search, fs_data ); /* can't open it, just go on */
				goto retry_getnextfilefromdisksearch;
			}
			
			fs_search->recursed_search = new_search;
			/* defer */
			return FS_GetNextFileFromDiskSearch( fs_search, fs_data );
		}
		else
		if( fs_search->flags&_fs_osf_nodirectories )
		{
			//return FS_GetNextFileFromDiskSearch( fs_search, fs_data );
			goto retry_getnextfilefromdisksearch;
		}
	}
	
	if( !fs_search->dir_search.Dir )
	{
		if(fs_search->ext[0])
		{
			char* file_ext = StrGetFileExtension( fs_search->dir_search.Name );
			if( !file_ext || stricmp( fs_search->ext, file_ext ) )
				goto retry_getnextfilefromdisksearch;
		}
	}
	
	fs_data->hidden = fs_search->dir_search.Hidden;
	fs_data->system = fs_search->dir_search.System;
	fs_data->locked = fs_search->dir_search.Locked;
	
	if( (FastStrLen(fs_search->dir)+FastStrLen(fs_search->dir_search.Name)) > FS_MAX_FILENAME_LENGTH )
	{
		OS_BreakMsg("FS_MAX_FILENAME_LENGTH! while searching disk\n");
	}
	
	strcpy( fs_data->name, fs_search->dir );
	if( fs_data->name[0] )
		strcat(fs_data->name, ":");
	strcat( fs_data->name, fs_search->dir_search.Name );
	
	//
	// this may be shadowed by the non-base directory.
	//
	if( fs_search->base && c_mod_valid )
	{
		char fullpath[1024];
		
		FS_GetFullPath( fs_data->name, fullpath, FS_FILELOC_MOD );
		FS_MakeBurgerPath( fullpath, fullpath );
		
		if(DoesFileExist( fullpath ))
		{
			goto retry_getnextfilefromdisksearch;
		}
	}
		
	
	return FALSE;
}

bool FS_Init(void)
{
	memset(c_basedir, 0, sizeof(c_basedir));
	c_numpaks = 0;
	c_make_files = 0;
	
	c_mod_valid = FALSE;
	
	return FALSE;
}

bool FS_Shutdown(void)
{
	//
	// free/close all open paks.
	//
	int i;
	for(i = 0; i < c_numpaks; i++)
		ClosePak(&c_paks[i]);
	
	c_numpaks = 0;
	c_make_files = 0;
	
	return FALSE;
}

bool FS_SetBase(const char* sysbase)
{
	if(strlen(sysbase) > MAX_FS_BASE_LENGTH)
		return FALSE;
	strcpy(c_basedir, sysbase);
	return FALSE;
}

bool FS_SetMod(const char* sysbase)
{
	if( strlen(sysbase) > MAX_FS_BASE_LENGTH )
		return FALSE;
		
	strcpy(c_moddir, sysbase);
	c_mod_valid = TRUE;
	return FALSE;
}

void** FS_LoadFileHandle(const char* filename, int flags)
{
	OS_ASSERT( flags&(_fs_flag_disk|_fs_flag_paks) );
	
	CHKSTRLEN( filename, FS_MAX_FILENAME_LENGTH, "file-system filename" );
	
	if( flags &_fs_flag_disk )
	{
		char fullpath[1024];
		
		if( c_mod_valid )
		{
			FS_GetFullPath(filename, fullpath, FS_FILELOC_MOD);
			FS_MakeBurgerPath( fullpath, fullpath );
			void** data = LoadAFileHandle(fullpath);
			
			if( data )
			{	
				Log_Msg("FS_LoadFileHandle: path '%s'\n", fullpath );
				return data;
			}
		}
		
		FS_GetFullPath(filename, fullpath, FS_FILELOC_BASE);
		FS_MakeBurgerPath( fullpath, fullpath );
		void** data = LoadAFileHandle(fullpath);
		
		if( data )
		{
			Log_Msg("FS_LoadFileHandle: path '%s'\n", fullpath );
			return data;
		}
	}
	if( flags&_fs_flag_paks )
	{
		char fullpath[1024];
		strcpy( fullpath, filename );
		FS_MakeBurgerPath(fullpath, fullpath);
		
		void* data;
		if( LoadFileFromOpenPaks( fullpath, &data, 0, TRUE ) == FALSE )
		{
			Log_Msg("FS_LoadFileHandle: pakload: path '%s'\n", fullpath );
			return (void**)data;
		}
	}
	
	Log_Msg("FS_LoadFileHandle: failed to load '%s'\n", filename );
	
	return 0;
}

void* FS_LoadFile(const char* filename, LongWord* Length, int flags)
{
	OS_ASSERT( flags&(_fs_flag_disk|_fs_flag_paks) );
	
	CHKSTRLEN( filename, FS_MAX_FILENAME_LENGTH, "file-system filename" );
	
	if( flags&_fs_flag_disk )
	{
		char fullpath[1024];
		
		if( c_mod_valid )
		{
			FS_GetFullPath(filename, fullpath, FS_FILELOC_MOD);
			FS_MakeBurgerPath(fullpath, fullpath);
			void* data = LoadAFile(fullpath, Length);
			
			if( data )
			{
				Log_Msg("FS_LoadFile: path '%s'\n", fullpath );
				return data;
			}
		}
		
		
		FS_GetFullPath(filename, fullpath, FS_FILELOC_BASE);
		FS_MakeBurgerPath(fullpath, fullpath);
		void* data = LoadAFile(fullpath, Length);
		
		if( data )
		{
			Log_Msg("FS_LoadFileHandle: path '%s'\n", fullpath );
			return data;
		}
	}
	
	if( flags&_fs_flag_paks )
	{
		char fullpath[1024];
		strcpy( fullpath, filename );
		FS_MakeBurgerPath(fullpath, fullpath);
			
		void* data;
		if(LoadFileFromOpenPaks( fullpath, &data, Length, FALSE ) == FALSE )
		{
			Log_Msg("FS_LoadFileHandle: pakload: path '%s'\n", fullpath );
			return data;
		}
	}
	
	Log_Msg("FS_LoadFileHandle: failed to load '%s'\n", filename );
	
	return 0;
}

Image_t* FS_LoadJPG(const char* filename, int flags)
{
	void* data;
	Image_t* image;
	LongWord len;
	
	return 0;

	data = FS_LoadFile(filename, &len, flags);
	if(!data)
		return 0;
		
	image = (Image_t*)OS_AllocAPointer(sizeof(Image_t));
	if(ImageParseJPG(image, (unsigned char*)data, len))
	{
		OS_DeallocAPointer(image);
		image = 0;
	}
	
	OS_DeallocAPointer( data );
	return image;
}

Image_t* FS_LoadTGA(const char* filename, int flags)
{
	void* data;
	Image_t* image;

	data = FS_LoadFile(filename, 0, flags);
	if(!data)
		return 0;

	image = (Image_t*)OS_AllocAPointer(sizeof(Image_t));
	if(ImageParseTGA(image, (unsigned char*)data))
	{
		OS_DeallocAPointer(image);
		image = 0;
	}

	OS_DeallocAPointer( data );
	return image;
}

Image_t* FS_LoadBMP(const char* filename, int flags)
{
	void* data;
	Image_t* image;

	data = FS_LoadFile(filename, 0, flags);
	if(!data)
		return 0;

	image = (Image_t*)OS_AllocAPointer(sizeof(Image_t));
	if(ImageParseBMP(image, (unsigned char*)data))
	{
		OS_DeallocAPointer(image);
		image = 0;
	}

	OS_DeallocAPointer( data );
	return image;
}

Image_t* FS_LoadGIF(const char* filename, int flags)
{
	void* data;
	Image_t* image;

	data = FS_LoadFile(filename, 0, flags);
	if(!data)
		return 0;

	image = (Image_t*)OS_AllocAPointer(sizeof(Image_t));
	if(ImageParseGIF(image, (unsigned char*)data))
	{
		OS_DeallocAPointer(image);
		image = 0;
	}

	OS_DeallocAPointer( data );
	return image;
}

Image_t* FS_LoadImageAuto( const char* filename, int flags )
{
	char buff[1024];
	if( FS_ImageExists( filename, buff, flags ) )
	{
		return FS_LoadImage( buff, flags );
	}
	
	return 0;
}

Image_t* FS_LoadImage(const char* filename, int flags)
{
	void* data;
	Image_t* image;
	LongWord len;

	char* ext = StrGetFileExtension( filename );
	if( ext == 0 || ext == filename || ext[0] == 0 )
		return 0;

	data = FS_LoadFile(filename, &len, flags);
	if(!data)
		return 0;

	image = (Image_t*)OS_AllocAPointer(sizeof(Image_t));
	
	if( !stricmp( ext, "tga" ) )
	{
		if(ImageParseTGA(image, (unsigned char*)data))
		{
			OS_DeallocAPointer( image );
			image = 0;
		}
	}
	else
	if( !stricmp( ext, "jpg" ) )
	{
		if(ImageParseJPG(image, (unsigned char*)data, len))
		{
			OS_DeallocAPointer( image );
			image = 0;
		}
	}
	else
	if( !stricmp( ext, "bmp" ) )
	{
		if(ImageParseBMP(image, (unsigned char*)data))
		{
			OS_DeallocAPointer( image );
			image = 0;
		}
	}
	else
	if( !stricmp( ext, "gif" ) )
	{
		if( ImageParseGIF( image, (unsigned char*)data) )
		{
			OS_DeallocAPointer( image );
			image = 0;
		}
	}
	
	OS_DeallocAPointer( data );
	return image;
}
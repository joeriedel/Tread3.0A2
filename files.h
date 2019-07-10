///////////////////////////////////////////////////////////////////////////////
// Files.h
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

#ifndef __FILES_H__
#define __FILES_H__

#ifndef __OS_H__
#include "os.h"
#endif

#define MAX_PAK_FILES	64

// $sb "files.h block1"

#define FS_MAX_FILENAME_LENGTH		255

#define FS_MAX_SEARCH_EXT_LENGTH	32
#define FS_MAX_EXT_PATH_LENGTH		1024
#define FS_FILELOC_BAD				-1
#define FS_FILELOC_BASE				0
#define FS_FILELOC_MOD				1
#define FS_FILELOC_ALL				2

typedef struct FS_FileSearch_s {
	DirectorySearch_t dir_search;
	char dir[FS_MAX_EXT_PATH_LENGTH+1];
	char ext[FS_MAX_SEARCH_EXT_LENGTH+1];
	Byte on_disk;
	Byte open;
	struct FS_FileSearch_s* recursed_search;
	Byte base;
	Byte Padding[3];
	int flags;
	int pak_num;
	int pak_file_num;
} FS_FileSearch_t;

typedef struct FS_FileSearchData_t {
	char name[FS_MAX_FILENAME_LENGTH+1];
	Byte hidden;
	Byte system;
	Byte locked;
	Byte Padding;
} FS_FileSearchData_t;

enum
{
	_fs_flag_paks=0x1,
	_fs_flag_disk=0x2,
	_fs_flag_all=0x3
};

enum
{
	_fs_osf_recurse = 0x00000001,			// Go down directories.
	_fs_osf_disk = 0x00000002,				// search the disk.
	_fs_osf_paks = 0x00000004,				// search the registered pak files.
	_fs_osf_nodirectories = 0x00000008,		// Don't return directories (don't combine with _osf_recurse)
	_fs_osf_fullpath = 0x00000010
};

// $eb

// Call to set the base working dir from which all file lookups
// will be relative too.
extern bool FS_SetBase(const char* sysbase);
extern bool FS_SetMod(const char* sysbase);
extern bool FS_Init(void);
extern bool FS_Shutdown(void);
extern bool FS_OpenPaks(void);
extern void FS_GetFullPath(const char* filename, char* fullname, int loc);
extern void FS_MakeBurgerPath( const char* filename, char* outname );

extern int FS_GetFileLoc( const char* filename, int flags=_fs_flag_all );
extern bool FS_GetFileModTime( const char* filename, TimeDate_t* td, int flags=_fs_flag_all );
extern bool FS_GetFileModTimeLoc( const char* filename, TimeDate_t* td, int loc, int flags=_fs_flag_all );
extern bool FS_FileExists( const char* filename, int flags=_fs_flag_all );
extern void* FS_LoadFile(const char* filename, LongWord* filesize, int flags=_fs_flag_all);
extern void** FS_LoadFileHandle(const char* filename, int flags=_fs_flag_all);
extern Image_t* FS_LoadTGA(const char* filename, int flags=_fs_flag_all);
extern Image_t* FS_LoadBMP(const char* filename, int flags=_fs_flag_all);
extern Image_t* FS_LoadJPG(const char* filename, int flags=_fs_flag_all);
extern Image_t* FS_LoadGIF(const char* filename, int flags=_fs_flag_all);
extern Image_t* FS_LoadImage(const char* filename, int flags=_fs_flag_all);
extern Image_t* FS_LoadImageAuto( const char* filename, int flags=_fs_flag_all);
extern bool FS_ImageExists( const char* filename, char* outfile=0, int flags=_fs_flag_all);
extern LibRef_t* FS_LoadLibLoc( const char* filename, int loc );
extern bool FS_CopyFileToDirectory( const char* filename, const char* directory );
extern void FS_FileFromPath( const char* path, char* out );

//
// the FS_fopen read/write all work from the harddrive,
// not pak files.
//
extern FILE* FS_fopen_loc( const char* filename, const char* mode, int loc );
extern FILE* FS_fopen( const char* filename, const char* mode );
extern void FS_fclose( FILE* fp );

extern bool FS_QuickWriteFile( const char* filename, int loc, void* data, LongWord length );
extern bool FS_QuickReadFile( const char* filename, int loc, void* data, LongWord length );

//
// the functions automatically handle endianess.
//
extern bool FS_fwrite( FILE* fp, const void* data, LongWord length );
extern bool FS_fwrite_float( FILE* fp, float data );
extern bool FS_fwrite_double( FILE* fp, double data );
extern bool FS_fwrite_ulong( FILE* fp, LongWord data );
extern bool FS_fwrite_slong( FILE* fp, int data );
extern bool FS_fwrite_ushort( FILE* fp, unsigned short data );
extern bool FS_fwrite_sshort( FILE* fp, short data );
extern bool FS_fwrite_uchar( FILE* fp, unsigned char data );
extern bool FS_fwrite_schar( FILE* fp, char data );
extern bool FS_fwrite_string( FILE* fp, const char* string );

extern bool FS_fread( FILE* fp, void* data, LongWord length );
extern bool FS_fread_float( FILE* fp, float* data );
extern bool FS_fread_double( FILE* fp, double* data );
extern bool FS_fread_ulong( FILE* fp, LongWord* data );
extern bool FS_fread_slong( FILE* fp, int* data );
extern bool FS_fread_ushort( FILE* fp, unsigned short* data );
extern bool FS_fread_sshort( FILE* fp, short* data );
extern bool FS_fread_uchar( FILE* fp, unsigned char* data );
extern bool FS_fread_schar( FILE* fp, char* data );
extern bool FS_fread_string( FILE* fp, char** string, LongWord* length = 0 /* optional */  );

extern bool FS_fseek( FILE* fp, int ofs, int relative );
extern LongWord FS_ftell( FILE* fp );

// Call StreamHandleDelete() on the returned stream.
extern StreamHandle_t* FS_GetFileStream( const char* filename, int flags=_fs_flag_all );

extern bool FS_OpenSearch( const char* dir, const char* ext, FS_FileSearch_t* fs_search, int flags );
extern bool FS_GetNextFile( FS_FileSearch_t* fs_search, FS_FileSearchData_t* fs_data );
extern bool FS_CloseSearch( FS_FileSearch_t* fs_search );

//
// Abducted PAK creation and loading.
//
extern bool FS_OpenPakFile( const char* file, int loc );
extern bool FS_BeginPakFile( const char* file );
extern bool FS_EndPakFile();
extern bool FS_AddFileToPak( const char* file, const char* entry_name );
extern bool FS_CreatePakFromDir( const char* dir, const char* pkfile );
extern bool FS_ExpandPakToDir( const char* pak, const char* dir );

extern void FS_AppendResolutionToFilename( const char* infile, char* outfile );

#endif
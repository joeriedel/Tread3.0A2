///////////////////////////////////////////////////////////////////////////////
// mapfile.h
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

#ifndef MAPFILE_H
#define MAPFILE_H

#define MAP_VERSION 1

class CFile;
class CMapObject;
class vec3;
class vec2;
class mat3x3;
class plane3;
class COrientation;
class quat;

#define TREAD_FILE_VERSION(x) (x & 0xffff)
#define PLUGIN_FILE_VERSION(x) ((x>>16)&0xffff)
#define MAKE_FILE_VERSION(x, y) ((x&0xffff) | ((y&0xffff)<<16))

#if defined(__BUILDING_TREAD__)
OS_FNEXP bool WriteTreadMap( CFile* file, CTreadDoc* pDoc );
OS_FNEXP bool ReadTreadMap( CFile* file, CTreadDoc* pDoc );
#endif

OS_FNEXP bool MAP_WriteObjectList( CFile* file, CTreadDoc* pDoc, int nVersion, CLinkedList<CMapObject>* pList );
OS_FNEXP bool MAP_ReadObjectList( CFile* file, CTreadDoc* pDoc, int nVersion, CLinkedList<CMapObject>* pList );

OS_FNEXP bool MAP_WriteObject( CFile* file, CTreadDoc* pDoc, int nVersion, CMapObject* obj );
OS_FNEXP CMapObject* MAP_ReadObject( CFile* file, CTreadDoc* pdoc, int nVersion );

OS_FNEXP const char *MAP_ReadString( CFile* file );
OS_FNEXP void MAP_WriteString( CFile* file, const char* string );

OS_FNEXP int MAP_ReadInt( CFile* file );
OS_FNEXP void MAP_WriteInt( CFile* file, int i );

OS_FNEXP void MAP_ReadVec3( CFile* file, vec3* vec );
OS_FNEXP void MAP_WriteVec3( CFile* file, vec3* vec );

OS_FNEXP void MAP_ReadVec2( CFile* file, vec2* vec );
OS_FNEXP void MAP_WriteVec2( CFile* file, vec2* vec );

OS_FNEXP float MAP_ReadFloat( CFile* file );
OS_FNEXP void MAP_WriteFloat( CFile* file, float f );

OS_FNEXP void MAP_WriteOR( CFile* file, COrientation* pOR );
OS_FNEXP void MAP_ReadOR( CFile* file, COrientation* pOR );

OS_FNEXP void MAP_WriteMat3x3( CFile* file, mat3x3* m );
OS_FNEXP void MAP_ReadMat3x3( CFile* file, mat3x3* m );

OS_FNEXP void MAP_WriteQuat( CFile* file, quat* q );
OS_FNEXP void MAP_ReadQuat( CFile* file, quat* q );

OS_FNEXP void MAP_WritePlane( CFile* file, plane3* p );
OS_FNEXP void MAP_ReadPlane( CFile* file, plane3* p );

#endif
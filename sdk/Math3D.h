///////////////////////////////////////////////////////////////////////////////
// Math3D.h
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008, Joe Riedel
// All rights reserved.
// Original code by Andrew Meggs
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

#ifndef __math3d_h__
#define __math3d_h__

#ifndef PI
#define PI					3.14159265359F
#endif

#define MATH3D_ASSERT(x)

#include "os.h"

#ifndef _MATH_H
#include <math.h>
#endif

#ifndef _STRING_H
#include <string.h>
#endif

#ifndef _STDIO_H
#include <stdio.h>
#endif

class vec2;
class vec3;
class mat3x3;
class mat4x3;
class mat4x4;

typedef float vec_t;
typedef vec3  vec3_t;
typedef float vec4_t[4];

float recip_sqrt( float );


#ifdef __MWERKS__
#pragma mark vec2
#endif


class OS_CLEXP vec2
{
public:
	float x, y;
	
	vec2();
	vec2( float x, float y );

	vec2 operator + ( const vec2 &v ) const;
	vec2 operator - ( const vec2 &v ) const;
	vec2 operator * ( float s ) const;
	vec2 operator / ( float s ) const;
	vec2 operator - () const;
		
	float &operator [] ( int i );
	const float &operator [] ( int i ) const;
	
	operator float * ();
	operator const float * () const;
	
	vec2 &operator += ( const vec2 &v );
	vec2 &operator -= ( const vec2 &v );
	
	vec2 &operator *= ( float s );
	vec2 &operator /= ( float s );
	
	vec2 &operator = ( const vec2 &v );
	
	float length_squared() const;
	float length() const;
	vec2 &normalize();

	static const vec2 zero;
};

float dot( const vec2 &v, const vec2 &w );
vec2 mul_vec( const vec2& v, const vec2& w );
float vec_length_squared( const vec2 &v );
float vec_length( const vec2 &v );
vec2 operator * ( float s, const vec2 &v );
vec2 normalized( const vec2 &v );
vec2 vec_mins( const vec2 &a, const vec2 &b );
vec2 vec_maxs( const vec2 &a, const vec2 &b );

#ifdef __MWERKS__
#pragma mark vec3
#endif

class OS_CLEXP vec3
{
public:
	float x, y, z, padding;
	
	vec3();
	vec3( float x, float y, float z );

	vec3 operator + ( const vec3 &v ) const;
	vec3 operator + ( float f ) const;
	vec3 operator - ( const vec3 &v ) const;
	vec3 operator - ( float f ) const;
	vec3 operator * ( float s ) const;
	vec3 operator * ( const vec3 &v ) const;
	vec3 operator / ( float s ) const;
	vec3 operator / ( const vec3 &v ) const;
	vec3 operator - () const;
		
	float &operator [] ( int i );
	const float &operator [] ( int i ) const;
	
	operator float * ();
	operator const float * () const;
	
	vec3 &operator += ( const vec3 &v );
	vec3 &operator -= ( const vec3 &v );
	
	vec3 &operator *= ( float s );
	vec3 &operator /= ( float s );
	
	vec3 &operator = ( const vec3 &v );
	
	vec3 &operator *= ( const mat4x3 &m );
	vec3  operator *  ( const mat4x3 &m ) const;
	vec3 &operator *= ( const mat3x3 &m );
	vec3  operator *  ( const mat3x3 &m ) const;

	float length_squared() const;
	float length() const;
	vec3 &normalize();

	static const vec3 zero;
	static const vec3 bogus_min;
	static const vec3 bogus_max;
};

float dot( const vec3 &v, const vec3 &w );

/* CHEAT SHEET: :)
	cross(X,Y) == Z
	cross(Y,X) == -Z
	cross(X,Z) == -Y
	cross(Z,X) == Y
	cross(Y,Z) == X
	cross(Z,Y) == -X
*/
vec3 cross( const vec3 &v, const vec3 &w );
vec3 mul_vec( const vec3 &v, const vec3& w );
float vec_length_squared( const vec3 &v );
float vec_length( const vec3 &v );
vec3 operator * ( float s, const vec3 &v );
vec3 normalized( const vec3 &v );
vec3 vec_mins( const vec3 &a, const vec3 &b );
vec3 vec_maxs( const vec3 &a, const vec3 &b );
bool equals( const vec3& a, const vec3& b, float epsilon );
vec3 project_point( const vec3& n, const vec3& v );
vec3 ortho_vec( const vec3& v );
vec3 reflect_vec( const vec3& n, const vec3& v );

//
// returns: v + v2*scale
vec3 scale_add( const vec3& v, float scale, const vec3& v2 );

float sign_vec( const vec3& a );


#ifdef __MWERKS__
#pragma mark plane3
#endif


class OS_CLEXP plane3
{
	// This describes a plane such that a * x + b * y + c * z + d = 0
public:
	float a, b, c, d;
	
	plane3();
	plane3( float a, float b, float c, float d );
	plane3( const vec3 &normal, float d );
	plane3( const vec3 &normal, const vec3 &point );
	plane3( const vec3 &v0, const vec3 &v1, const vec3 &v2 );
	
	plane3 &normalize();
	
	float &operator [] ( int i );
	const float &operator [] ( int i ) const;
	plane3 operator - () const;

	operator const vec3 & () const { return *(const vec3 *)this; }
	
	
	static float tolerance;
};

bool operator < ( const vec3 &v, const plane3 &p );
bool operator > ( const vec3 &v, const plane3 &p );
bool operator <= ( const vec3 &v, const plane3 &p );
bool operator >= ( const vec3 &v, const plane3 &p );
bool on_plane( const vec3 &v, const plane3 &p );
bool equals( const plane3& a, const plane3& b, float normal_epsilon, float dist_epsilon );


#ifdef __MWERKS__
#pragma mark quat
#endif


class quat
{
public:
	float x, y, z, w;
	
	quat();
	quat( const mat3x3 &m );
	quat( float x, float y, float z, float w );
	quat( const vec3 &axis, float angle_radians );
	
	float &operator [] ( int i );
	const float &operator [] ( int i ) const;
	
	operator float * ();
	operator const float * () const;
	
	quat operator - () const;
	
	quat &operator = ( const quat &q );
	quat &operator = ( const mat3x3 &m );
	
	quat operator * ( const quat &q ) const;
	quat &operator *= ( const quat &q );
	
	static const quat zero;
	static const quat identity;
	
	// Note that quaternion multiplication is BACKWARDS relative to matrices,
	// so:  matrix( quat0 * quat1 ) == matrix( quat1 ) * matrix( quat0 )
	// and: quaternion( matrix0 * matrix1 ) == quaterion( matrix1 ) * quaternion( matrix0 )
};


OS_FNEXP void quaternion_to_matrix( mat3x3 *out, const quat *in );
OS_FNEXP void matrix_to_quaternion( quat *out, const mat3x3 *in );
OS_FNEXP void slerp_quaternion( quat *out, const quat *from, const quat *to, float t );
OS_FNEXP void quaternion_mul( quat *out, const quat *q1, const quat *q2 );

#ifdef __MWERKS__
#pragma mark mat4x4
#endif


class OS_CLEXP mat4x4
{
	// A standard 4x4 matrix, stored in column-major order. The layout
	// in memory is thus:
	//
	//     | 0  4  8 12 |                 
	//     | 1  5  9 13 |  
	//     | 2  6 10 14 |
	//     | 3  7 11 15 |
	//
	// or, written with subscripts:
	//
	//     | m[0][0]  m[1][0]  m[2][0]  m[3][0] |
	//     | m[0][1]  m[1][1]  m[2][1]  m[3][1] |
	//     | m[0][2]  m[1][2]  m[2][2]  m[3][2] |
	//     | m[0][3]  m[1][3]  m[2][3]  m[3][3] |
	//
	
public:
	float m[4][4];
	
	mat4x4();
	mat4x4( const mat4x4 &a );
	mat4x4( const mat4x3 &a );
	mat4x4( const mat3x3 &a );
	
	float * operator [] ( int column );
	const float * operator [] ( int column ) const;
	
	mat4x4 & operator = ( const mat4x4 &a );
	mat4x4 & operator = ( const mat4x3 &a );
	mat4x4 & operator = ( const mat3x3 &a );
	
	mat4x4 operator * ( const mat4x4 &b ) const;
	mat4x4 operator * ( const mat4x3 &b ) const;
	mat4x4 operator * ( const mat3x3 &b ) const;
	
	mat4x4 & operator *= ( const mat4x4 &b );
	mat4x4 & operator *= ( const mat4x3 &b );
	mat4x4 & operator *= ( const mat3x3 &b );
	
	operator mat4x3 & () { return *(mat4x3 *)(this); }
	operator const mat4x3 & () const { return *(const mat4x3 *)(this); }
	
	operator mat3x3 & () { return *(mat3x3 *)(this); }
	operator const mat3x3 & () const { return *(const mat3x3 *)(this); }
	
	static const mat4x4 zero;
	static const mat4x4 identity;
};

mat4x4 inverse( const mat4x4 &m );
mat4x4 make_orthonormal( const mat4x4 &m );
void fprint_matrix( FILE *f, const mat4x4 &m );




#ifdef __MWERKS__
#pragma mark mat4x3
#endif


class OS_CLEXP mat4x3
{
	// This is a 4x4 matrix in which only the first 3 columns
	// are stored and the fourth column is implied to be [ 0 0 0 1 ].
	// Alternately, think of it as a 3x3 matrix plus a translation.
	
public:
	float m[3][4];
	
	mat4x3();
	mat4x3( const mat4x4 &a );
	mat4x3( const mat4x3 &a );
	mat4x3( const mat3x3 &a );
	mat4x3( const quat &rotation );
	mat4x3( const vec3 &translation );
	mat4x3( const quat &rotation, const vec3 &translation );
	mat4x3( const vec3 &col0, const vec3 &col1, const vec3 &col2 );
	mat4x3( const vec3 &col0, const vec3 &col1, const vec3 &col2, const vec3 &translation );
	
	float * operator [] ( int column );
	const float * operator [] ( int column ) const;
	
	mat4x3 & operator = ( const mat4x4 &a );
	mat4x3 & operator = ( const mat4x3 &a );
	mat4x3 & operator = ( const mat3x3 &a );
	mat4x3 & operator = ( const quat &rotation );
	mat4x3 & operator = ( const vec3 &translation );
	
	mat4x4 operator * ( const mat4x4 &b ) const;
	mat4x3 operator * ( const mat4x3 &b ) const;
	mat4x3 operator * ( const mat3x3 &b ) const;
	
	mat4x3 & operator *= ( const mat4x4 &b );
	mat4x3 & operator *= ( const mat4x3 &b );
	mat4x3 & operator *= ( const mat3x3 &b );
	
	operator mat3x3 & () { return *(mat3x3 *)(this); }
	operator const mat3x3 & () const { return *(const mat3x3 *)(this); }
	
	static const mat4x3 zero;
	static const mat4x3 identity;
};

vec3 operator * ( const vec3 &a, const mat4x3 &b );
mat4x3 inverse( const mat4x3 &m );
mat4x3 make_orthonormal( const mat4x3 &m );
void fprint_matrix( FILE *f, const mat4x3 &m );
OS_FNEXP void factor_matrix( vec3 *out_scale, quat *out_orient, vec3 *out_offset, const mat4x3 &in );





#ifdef __MWERKS__
#pragma mark mat3x3
#endif


class OS_CLEXP mat3x3
{
	// A simple 3x3 matrix, usually used to represent rotations,
	// shears, scales, and other origin-preserving affine transforms.
	// Note that because each column is padded to 16-byte alignment
	// this takes up the same space as a 4x3 matrix.
	
public:
	float m[3][4];
	
	mat3x3();
	mat3x3( const mat3x3 &a );
	mat3x3( const quat &rotation );
	mat3x3( const vec3 &col0, const vec3 &col1, const vec3 &col2 );
	
	float * operator [] ( int column );
	const float * operator [] ( int column ) const;
	
	mat3x3 & operator = ( const mat3x3 &a );
	mat3x3 & operator = ( const quat &q );
	
	mat4x4 operator * ( const mat4x4 &b ) const;
	mat4x3 operator * ( const mat4x3 &b ) const;
	mat3x3 operator * ( const mat3x3 &b ) const;
	
	mat3x3 & operator *= ( const mat3x3 &b );
	
	static const mat3x3 zero;
	static const mat3x3 identity;
};

mat3x3 inverse( const mat3x3 &m );
mat3x3 make_orthonormal( const mat3x3 &m );
OS_FNEXP vec3 euler_from_matrix( const mat3x3 &m );

void fprint_matrix( FILE *f, const mat3x3 &m );


#ifdef __MWERKS__
#pragma mark general matrix
#endif


// convenience functtions to make matrices for commonly-used purposes of matrices
mat3x3 build_scale_matrix( float scale );
mat3x3 build_scale_matrix( const vec3 &scale );
mat3x3 build_rotation_matrix( const quat &rotation );
mat3x3 build_rotation_matrix( const vec3 &axis, float angle_radians );
mat3x3 build_rotation_matrix( float axis_x, float axis_y, float axis_z, float angle_radians );
mat4x3 build_translation_matrix( const vec3 &translation );
mat4x3 build_translation_matrix( float dx, float dy, float dz );

// some of these inverses are insipidly simple, but are still here for completeness and consistency
mat3x3 build_inverse_scale_matrix( float scale );
mat3x3 build_inverse_scale_matrix( const vec3 &scale );
mat3x3 build_inverse_rotation_matrix( const quat &rotation );
mat3x3 build_inverse_rotation_matrix( const vec3 &axis, float angle_radians );
mat3x3 build_inverse_rotation_matrix( float axis_x, float axis_y, float axis_z, float angle_radians );
mat4x3 build_inverse_translation_matrix( const vec3 &translation );
mat4x3 build_inverse_translation_matrix( float dx, float dy, float dz );




OS_FNEXP void matrix_mul( mat4x4 *out, const mat4x4 *a, const mat4x4 *b );
OS_FNEXP void matrix_mul( mat4x4 *out, const mat4x4 *a, const mat4x3 *b );
OS_FNEXP void matrix_mul( mat4x4 *out, const mat4x4 *a, const mat3x3 *b );
OS_FNEXP void matrix_mul( mat4x4 *out, const mat4x3 *a, const mat4x4 *b );
OS_FNEXP void matrix_mul( mat4x3 *out, const mat4x3 *a, const mat4x3 *b );
OS_FNEXP void matrix_mul( mat4x3 *out, const mat4x3 *a, const mat3x3 *b );
OS_FNEXP void matrix_mul( mat4x4 *out, const mat3x3 *a, const mat4x4 *b );
OS_FNEXP void matrix_mul( mat4x3 *out, const mat3x3 *a, const mat4x3 *b );
OS_FNEXP void matrix_mul( mat3x3 *out, const mat3x3 *a, const mat3x3 *b );
OS_FNEXP void matrix_invert( mat4x4 *out, const mat4x4 *m );
OS_FNEXP void matrix_invert( mat4x3 *out, const mat4x3 *m );
OS_FNEXP void matrix_invert( mat3x3 *out, const mat3x3 *m );
OS_FNEXP void matrix_invert( float *m, int size, int column_stride, int row_stride );
OS_FNEXP void make_orthonormal( float *m, int size, int column_stride, int row_stride );

#include "math3d_inlines.inl"

#endif // #ifndef __math3d_h__


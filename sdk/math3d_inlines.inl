///////////////////////////////////////////////////////////////////////////////
// math3d_inlines.inl
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

#define __math3d_inlines_i__

#ifndef PI
#define PI					3.14159265359F
#endif

//////////////////////////////////////////////////////////////////////
//
// GENERAL UTILITY FUNCTIONS
//
//////////////////////////////////////////////////////////////////////



inline float recip_sqrt( float f )
{
#if defined(__POWERPC__)
	float x = __frsqrte(f);
	f *= 0.5f;
	x = 1.5f * x - f * x * x * x;
	x = 1.5f * x - f * x * x * x;
	return x;
#else
	return 1.0f / sqrtf(f);
#endif
}



//////////////////////////////////////////////////////////////////////
//
// 2D-VECTOR FUNCTIONS
//
//////////////////////////////////////////////////////////////////////



inline vec2::vec2() {}

inline vec2::vec2( float x, float y )
{
	this->x = x;
	this->y = y;
}


inline vec2 vec2::operator + ( const vec2 &v ) const
{
	return vec2( x + v.x, y + v.y );
}


inline vec2 vec2::operator - ( const vec2 &v ) const
{
	return vec2( x - v.x, y - v.y );
}


inline vec2 vec2::operator * ( float s ) const
{
	return vec2( x*s, y*s );
}


inline vec2 vec2::operator / ( float s ) const
{
	float is = 1.0f/s;
	return vec2( x*is, y*is );
}


inline vec2 vec2::operator - () const
{
	return vec2( -x, -y );
}


inline float &vec2::operator [] ( int i )
{
	MATH3D_ASSERT( i >= 0 && i < 2 );
	return ((float *)this)[i];
}


inline const float &vec2::operator [] ( int i ) const
{
	MATH3D_ASSERT( i >= 0 && i < 2 );
	return ((float *)this)[i];
}


inline vec2::operator float * ()
{
	return (float *)this;
}


inline vec2::operator const float * () const
{
	return (float *)this;
}


inline vec2 &vec2::operator += ( const vec2 &v )
{
	x += v.x;
	y += v.y;
	return *this;
}


inline vec2 &vec2::operator -= ( const vec2 &v )
{
	x -= v.x;
	y -= v.y;
	return *this;
}


inline vec2 &vec2::operator *= ( float s )
{
	x *= s;
	y *= s;
	return *this;
}


inline vec2 &vec2::operator /= ( float s )
{
	return *this *= (1.0f/s);
}


inline vec2 &vec2::operator = ( const vec2 &v )
{
	x = v.x;
	y = v.y;
	return *this;
}


inline float vec2::length_squared() const
{
	return x * x + y * y;
}


inline float vec2::length() const
{
	float l2 = length_squared();
	return (l2 > 0.0f ? l2 * recip_sqrt(l2) : 0.0f);
}


inline vec2 &vec2::normalize()
{
	float l2 = length_squared();
	if (l2 > 0.0f) *this *= recip_sqrt(l2);
	return *this;
}


inline float dot( const vec2 &v, const vec2 &w )
{
	return v.x*w.x+v.y*w.y;
}


inline vec2 mul_vec( const vec2 &v, const vec2 &w )
{
	return vec2( v.x * w.x, v.y * w.y );
}

inline float vec_length_squared( const vec2 &v )
{
	return v.x * v.x + v.y * v.y;
}


inline float vec_length( const vec2 &v )
{
	float l2 = vec_length_squared(v);
	return (l2 > 0.0f ? l2 * recip_sqrt(l2) : 0.0f);
}


inline vec2 operator * ( float s, const vec2 &v )
{
	return v*s;
}


inline vec2 normalized( const vec2 &v )
{
	float l2 = vec_length_squared(v);
	return (l2 > 0.0f ? v * recip_sqrt(l2) : v);
}


inline vec2 vec_mins( const vec2 &a, const vec2 &b )
{
	vec2 out;
	out.x = a.x < b.x ? a.x : b.x;
	out.y = a.y < b.y ? a.y : b.y;
	return out;
}


inline vec2 vec_maxs( const vec2 &a, const vec2 &b )
{
	vec2 out;
	out.x = a.x > b.x ? a.x : b.x;
	out.y = a.y > b.y ? a.y : b.y;
	return out;
}




//////////////////////////////////////////////////////////////////////
//
// 3D-VECTOR FUNCTIONS
//
//////////////////////////////////////////////////////////////////////



inline vec3::vec3() {}

inline vec3::vec3( float x, float y, float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
}


inline vec3 vec3::operator + ( const vec3 &v ) const
{
	return vec3( x + v.x, y + v.y, z + v.z );
}


inline vec3 vec3::operator - ( const vec3 &v ) const
{
	return vec3( x - v.x, y - v.y, z - v.z );
}


inline vec3 vec3::operator * ( float s ) const
{
	return vec3( x*s, y*s, z*s );
}

inline vec3 vec3::operator * ( const vec3& v ) const
{
	return vec3( x*v.x, y*v.y, z*v.z );
}

inline vec3 vec3::operator / ( float s ) const
{
	float is = 1.0f/s;
	return vec3( x*is, y*is, z*is );
}

inline vec3 vec3::operator / ( const vec3& v ) const
{
	vec3 out;

	out[0] = x / v.x;
	out[1] = y / v.y;
	out[2] = z / v.z;

	return out;
}

inline vec3 vec3::operator - () const
{
	return vec3( -x, -y, -z );
}


inline float &vec3::operator [] ( int i )
{
	MATH3D_ASSERT( i >= 0 && i < 3 );
	return ((float *)this)[i];
}


inline const float &vec3::operator [] ( int i ) const
{
	MATH3D_ASSERT( i >= 0 && i < 3 );
	return ((float *)this)[i];
}


inline vec3::operator float * ()
{
	return (float *)this;
}


inline vec3::operator const float * () const
{
	return (float *)this;
}


inline vec3 &vec3::operator += ( const vec3 &v )
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}


inline vec3 &vec3::operator -= ( const vec3 &v )
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}


inline vec3 &vec3::operator *= ( float s )
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

inline vec3 &vec3::operator *= ( const mat4x3 &m )
{
	float outx = (x * m.m[0][0]) + (y * m.m[0][1]) + (z * m.m[0][2]) + m.m[0][3];
	float outy = (x * m.m[1][0]) + (y * m.m[1][1]) + (z * m.m[1][2]) + m.m[1][3];
	float outz = (x * m.m[2][0]) + (y * m.m[2][1]) + (z * m.m[2][2]) + m.m[2][3];
	x = outx;
	y = outy;
	z = outz;
	return *this;
}

inline vec3  vec3::operator * ( const mat4x3 &m ) const
{
	float outx = (x * m.m[0][0]) + (y * m.m[0][1]) + (z * m.m[0][2]) + m.m[0][3];
	float outy = (x * m.m[1][0]) + (y * m.m[1][1]) + (z * m.m[1][2]) + m.m[1][3];
	float outz = (x * m.m[2][0]) + (y * m.m[2][1]) + (z * m.m[2][2]) + m.m[2][3];
	
	return vec3( outx, outy, outz );
}

inline vec3 &vec3::operator *= ( const mat3x3 &m )
{
	float outx = (x * m.m[0][0]) + (y * m.m[0][1]) + (z * m.m[0][2]);
	float outy = (x * m.m[1][0]) + (y * m.m[1][1]) + (z * m.m[1][2]);
	float outz = (x * m.m[2][0]) + (y * m.m[2][1]) + (z * m.m[2][2]);
	x = outx;
	y = outy;
	z = outz;
	return *this;
}

inline vec3 vec3::operator * ( const mat3x3 &m ) const
{
	float outx = (x * m.m[0][0]) + (y * m.m[0][1]) + (z * m.m[0][2]);
	float outy = (x * m.m[1][0]) + (y * m.m[1][1]) + (z * m.m[1][2]);
	float outz = (x * m.m[2][0]) + (y * m.m[2][1]) + (z * m.m[2][2]);
	
	return vec3( outx, outy, outz );
}


inline vec3 &vec3::operator /= ( float s )
{
	return *this *= (1.0f/s);
}


inline vec3 &vec3::operator = ( const vec3 &v )
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}


inline float vec3::length_squared() const
{
	return x * x + y * y + z * z;
}


inline float vec3::length() const
{
	float l2 = length_squared();
	return (l2 > 0.0f ? l2 * recip_sqrt(l2) : 0.0f);
}


inline vec3 &vec3::normalize()
{
	float l2 = length_squared();
	if (l2 > 0.0f) *this *= recip_sqrt(l2);
	return *this;
}

inline vec3 vec3::operator - ( float f ) const
{
	vec3 out;

	out[0] = x - f;
	out[1] = y - f;
	out[2] = z - f;

	return out;
}

inline vec3 vec3::operator + ( float f ) const
{
	vec3 out;

	out[0] = x + f;
	out[1] = y + f;
	out[2] = z + f;

	return out;
}

inline float dot( const vec3 &v, const vec3 &w )
{
	return v.x*w.x+v.y*w.y+v.z*w.z;
}


/*
	cross(X,Y) == Z
	cross(Y,X) == -Z
	cross(X,Z) == -Y
	cross(Z,X) == Y
	cross(Y,Z) == X
	cross(Z,Y) == -X
*/
inline vec3 cross( const vec3 &v, const vec3 &w )
{
	return vec3( v.y*w.z - w.y*v.z, w.x*v.z - v.x*w.z, v.x*w.y - w.x*v.y );
}

inline vec3 project_point( const vec3& n, const vec3& v )
{
	vec3 out;
	float inv_d;
	float d;

	inv_d = 1.0f / dot( n, n );
	d = dot( n, v ) * inv_d;

	out[0] = v[0] - d * n[0] * inv_d;
	out[1] = v[1] - d * n[1] * inv_d;
	out[2] = v[2] - d * n[2] * inv_d;

	return out;
}

inline vec3 ortho_vec( const vec3& v )
{
	int i;
	int axis;
	float minlen = 1.0f;

	for(i = 0; i < 3; i++)
	{
		if( fabsf(v[i]) < minlen )
		{
			axis = i;
			minlen = fabsf(v[i]);
		}
	}

	vec3 t = vec3::zero;
	t[axis] = 1.0f;

	t = project_point( v, t );
	t.normalize();

	return t;
}

inline vec3 reflect_vec( const vec3& n, const vec3& v )
{
	vec3 out;
	float d = dot( n, v );

	out[0] = v[0]*d*2 - n[0];
	out[1] = v[1]*d*2 - n[1];
	out[2] = v[2]*d*2 - n[2];

	return out;
}

inline vec3 mul_vec( const vec3 &v, const vec3 &w )
{
	return vec3( v.x * w.x, v.y * w.y, v.z * w.z );
}

inline float sign_vec( const vec3& a )
{
	vec3 f;

	f[0] = fabsf(a[0]);
	f[1] = fabsf(a[1]);
	f[2] = fabsf(a[2]);

	if( f[0] > f[1] && f[0] > f[2] )
		return a[0];
	if( f[1] > f[0] && f[1] > f[2] )
		return a[1];

	return a[2];
}

inline float vec_length_squared( const vec3 &v )
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}


inline float vec_length( const vec3 &v )
{
	float l2 = vec_length_squared(v);
	return (l2 > 0.0f ? l2 * recip_sqrt(l2) : 0.0f);
}


inline vec3 operator * ( float s, const vec3 &v )
{
	return v*s;
}


inline vec3 normalized( const vec3 &v )
{
	float l2 = vec_length_squared(v);
	return (l2 > 0.0f ? v * recip_sqrt(l2) : v);
}


inline vec3 vec_mins( const vec3 &a, const vec3 &b )
{
	vec3 out;
	out.x = a.x < b.x ? a.x : b.x;
	out.y = a.y < b.y ? a.y : b.y;
	out.z = a.z < b.z ? a.z : b.z;
	return out;
}


inline vec3 vec_maxs( const vec3 &a, const vec3 &b )
{
	vec3 out;
	out.x = a.x > b.x ? a.x : b.x;
	out.y = a.y > b.y ? a.y : b.y;
	out.z = a.z > b.z ? a.z : b.z;
	return out;
}

inline vec3 scale_add( const vec3& v, float scale, const vec3& v2 )
{
	vec3 out;

	out.x = v.x + v2.x*scale;
	out.y = v.y + v2.y*scale;
	out.z = v.z + v2.z*scale;

	return out;
}

inline bool equals( const vec3& a, const vec3& b, float epsilon )
{
	if( (a[0] >= (b[0]-epsilon)) &&
		(a[0] <= (b[0]+epsilon)) &&
		(a[1] >= (b[1]-epsilon)) &&
		(a[1] <= (b[1]+epsilon)) &&
		(a[2] >= (b[2]-epsilon)) &&
		(a[2] <= (b[2]+epsilon)) )
	{
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////
//
// QUATERNION FUNCTIONS
//
//////////////////////////////////////////////////////////////////////



inline quat::quat() {}


inline quat::quat( const mat3x3 &m )
{
	matrix_to_quaternion( this, &m );
}


inline quat::quat( float x, float y, float z, float w )
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}


inline quat::quat( const vec3 &axis, float angle_radians )
{
	angle_radians *= 0.5f;
	this->w = cosf( angle_radians );
	float s = sinf( angle_radians );
	this->x = axis.x * s;
	this->y = axis.y * s;
	this->z = axis.z * s;
}


inline float &quat::operator [] ( int i )
{
	MATH3D_ASSERT( i >= 0 && i < 4 );
	return ((float *)this)[i];
}


inline const float &quat::operator [] ( int i ) const
{
	MATH3D_ASSERT( i >= 0 && i < 4 );
	return ((float *)this)[i];
}


inline quat::operator float * ()
{
	return (float *)this;
}


inline quat::operator const float * () const
{
	return (float *)this;
}


inline quat quat::operator - () const
{
	return quat( -x, -y, -z, w );
}


inline quat quat::operator * ( const quat &right ) const
{
	quat out;
	quaternion_mul( &out, this, &right );
	return out;
}


inline quat &quat::operator *= ( const quat &right )
{
	quaternion_mul( this, this, &right );
	return *this;
}


inline quat &quat::operator = ( const quat &q )
{
	this->x = q.x;
	this->y = q.y;
	this->z = q.z;
	this->w = q.w;
	return *this;
}


inline quat &quat::operator = ( const mat3x3 &m )
{
	matrix_to_quaternion( this, &m );
	return *this;
}



//////////////////////////////////////////////////////////////////////
//
// 4x4 MATRIX FUNCTIONS
//
//////////////////////////////////////////////////////////////////////



inline mat4x4::mat4x4() {}
inline mat4x4::mat4x4( const mat4x4 &a ) { *this = a; }
inline mat4x4::mat4x4( const mat4x3 &a ) { *this = a; }
inline mat4x4::mat4x4( const mat3x3 &a ) { *this = a; }

inline float * mat4x4::operator [] ( int column )
{
	MATH3D_ASSERT( column >= 0 && column < 4 );
	return m[column];
}


inline const float * mat4x4::operator [] ( int column ) const
{
	MATH3D_ASSERT( column >= 0 && column < 4 );
	return m[column];
}


inline mat4x4 & mat4x4::operator = ( const mat4x4 &a )
{
	memcpy( this, &a, sizeof(mat4x4) );
	return *this;
}


inline mat4x4 & mat4x4::operator = ( const mat4x3 &a )
{
	memcpy( this, &a, sizeof(mat4x3) );
	
	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;
	
	return *this;
}


inline mat4x4 & mat4x4::operator = ( const mat3x3 &a )
{
	memcpy( this, &a, sizeof(mat3x3) );
	
	m[0][3] = 0.0f;
	m[1][3] = 0.0f;
	m[2][3] = 0.0f;
	
	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	
	m[3][3] = 1.0f;
	
	return *this;
}


inline mat4x4 mat4x4::operator * ( const mat4x4 &right ) const
{
	mat4x4 temp;
	matrix_mul( &temp, this, &right );
	return temp;
}


inline mat4x4 mat4x4::operator * ( const mat4x3 &right ) const
{
	mat4x4 temp;
	matrix_mul( &temp, this, &right );
	return temp;
}


inline mat4x4 mat4x4::operator * ( const mat3x3 &right ) const
{
	mat4x4 temp;
	matrix_mul( &temp, this, &right );
	return temp;
}


inline mat4x4 & mat4x4::operator *= ( const mat4x4 &right )
{
	mat4x4 temp;
	matrix_mul( &temp, this, &right );
	*this = temp;
	return *this;
}


inline mat4x4 & mat4x4::operator *= ( const mat4x3 &right )
{
	mat4x4 temp;
	matrix_mul( &temp, this, &right );
	*this = temp;
	return *this;
}


inline mat4x4 & mat4x4::operator *= ( const mat3x3 &right )
{
	mat4x4 temp;
	matrix_mul( &temp, this, &right );
	*this = temp;
	return *this;
}


inline mat4x4 inverse( const mat4x4 &m )
{
	mat4x4 out = m;
	matrix_invert( &out[0][0], 4, 4, 1 );
	return out;
}


inline mat4x4 make_orthonormal( const mat4x4 &m )
{
	mat4x4 out = m;
	make_orthonormal( &(out[0][0]), 4, sizeof(out[0]), 1 );
	return out;
}




//////////////////////////////////////////////////////////////////////
//
// 4x3 MATRIX FUNCTIONS
//
//////////////////////////////////////////////////////////////////////


inline mat4x3::mat4x3() {}
inline mat4x3::mat4x3( const mat4x4 &a ) { *this = a; }
inline mat4x3::mat4x3( const mat4x3 &a ) { *this = a; }
inline mat4x3::mat4x3( const mat3x3 &a ) { *this = a; }


inline mat4x3::mat4x3( const quat &rotation )
{
	quaternion_to_matrix( (mat3x3 *)this, &rotation );
}


inline mat4x3::mat4x3( const vec3 &translation )
{
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = translation.x;
	
	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[1][3] = translation.y;
	
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
	m[2][3] = translation.z;
}


inline mat4x3::mat4x3( const quat &rotation, const vec3 &translation )
{
	m[0][3] = translation.x;
	m[1][3] = translation.y;
	m[2][3] = translation.z;
	quaternion_to_matrix( (mat3x3 *)this, &rotation );
}


inline mat4x3::mat4x3( const vec3 &col0, const vec3 &col1, const vec3 &col2 )
{
	m[0][0] = col0.x;
	m[0][1] = col0.y;
	m[0][2] = col0.z;
	m[0][3] = 0.0f;
	
	m[1][0] = col1.x;
	m[1][1] = col1.y;
	m[1][2] = col1.z;
	m[1][3] = 0.0f;
	
	m[2][0] = col2.x;
	m[2][1] = col2.y;
	m[2][2] = col2.z;
	m[2][3] = 0.0f;
}


inline mat4x3::mat4x3( const vec3 &col0, const vec3 &col1, const vec3 &col2, const vec3 &translation )
{
	m[0][0] = col0.x;
	m[0][1] = col0.y;
	m[0][2] = col0.z;
	m[0][3] = translation.x;
	
	m[1][0] = col1.x;
	m[1][1] = col1.y;
	m[1][2] = col1.z;
	m[1][3] = translation.y;
	
	m[2][0] = col2.x;
	m[2][1] = col2.y;
	m[2][2] = col2.z;
	m[2][3] = translation.z;
}


inline float * mat4x3::operator [] ( int column )
{
	MATH3D_ASSERT( column >= 0 && column < 3 );
	return m[column];
}

inline const float * mat4x3::operator [] ( int column ) const
{
	MATH3D_ASSERT( column >= 0 && column < 3 );
	return m[column];
}


inline mat4x3 & mat4x3::operator = ( const mat4x4 &a )
{
	memcpy( this, &a, sizeof(mat4x3) );
	return *this;
}


inline mat4x3 & mat4x3::operator = ( const mat4x3 &a )
{
	memcpy( this, &a, sizeof(mat4x3) );
	return *this;
}


inline mat4x3 & mat4x3::operator = ( const mat3x3 &a )
{
	memcpy( this, &a, sizeof(mat3x3) );
	
	m[0][3] = 0.0f;
	m[1][3] = 0.0f;
	m[2][3] = 0.0f;
	
	return *this;
}


inline mat4x3 & mat4x3::operator = ( const quat &rotation )
{
	m[0][3] = 0.0f;
	m[1][3] = 0.0f;
	m[2][3] = 0.0f;
	
	quaternion_to_matrix( (mat3x3 *)this, &rotation );
	
	return *this;
}


inline mat4x3 & mat4x3::operator = ( const vec3 &translation )
{
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = translation.x;
	
	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[1][3] = translation.y;
	
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
	m[2][3] = translation.z;
	
	return *this;
}


inline mat4x4 mat4x3::operator * ( const mat4x4 &right ) const
{
	mat4x4 temp;
	matrix_mul( &temp, this, &right );
	return temp;
}


inline mat4x3 mat4x3::operator * ( const mat4x3 &right ) const
{
	mat4x3 temp;
	matrix_mul( &temp, this, &right );
	return temp;
}


inline mat4x3 mat4x3::operator * ( const mat3x3 &right ) const
{
	mat4x3 temp;
	matrix_mul( &temp, this, &right );
	return temp;
}


inline mat4x3 & mat4x3::operator *= ( const mat4x4 &right )
{
	mat4x3 temp;
	matrix_mul( &temp, this, (const mat4x3 *)&right );
	*this = temp;
	return *this;
}


inline mat4x3 & mat4x3::operator *= ( const mat4x3 &right )
{
	mat4x3 temp;
	matrix_mul( &temp, this, &right );
	*this = temp;
	return *this;
}


inline mat4x3 & mat4x3::operator *= ( const mat3x3 &right )
{
	mat4x3 temp;
	matrix_mul( &temp, this, &right );
	*this = temp;
	return *this;
}


inline mat4x3 inverse( const mat4x3 &m )
{
	mat4x4 out = m;
	matrix_invert( &out[0][0], 4, 4, 1 );
	return out;
}


inline mat4x3 make_orthonormal( const mat4x3 &m )
{
	mat4x3 out = m;
	make_orthonormal( &(out[0][0]), 3, sizeof(out[0]), 1 );
	return out;
}


inline vec3 operator * ( const vec3 &a, const mat4x3 &b )
{
	vec3 out;
	out.x = (a.x * b.m[0][0]) + (a.y * b.m[0][1]) + (a.z * b.m[0][2]) + b.m[0][3];
	out.y = (a.x * b.m[1][0]) + (a.y * b.m[1][1]) + (a.z * b.m[1][2]) + b.m[1][3];
	out.z = (a.x * b.m[2][0]) + (a.y * b.m[2][1]) + (a.z * b.m[2][2]) + b.m[2][3];
	return out;
}


//////////////////////////////////////////////////////////////////////
//
// 3x3 MATRIX FUNCTIONS
//
//////////////////////////////////////////////////////////////////////




inline mat3x3::mat3x3() {}
inline mat3x3::mat3x3( const mat3x3 &a ) { *this = a; }


inline mat3x3::mat3x3( const quat &rotation )
{
	quaternion_to_matrix( this, &rotation );
}


inline mat3x3::mat3x3( const vec3 &col0, const vec3 &col1, const vec3 &col2 )
{
	m[0][0] = col0.x;
	m[0][1] = col0.y;
	m[0][2] = col0.z;
	
	m[1][0] = col1.x;
	m[1][1] = col1.y;
	m[1][2] = col1.z;
	
	m[2][0] = col2.x;
	m[2][1] = col2.y;
	m[2][2] = col2.z;
}


inline float * mat3x3::operator [] ( int column )
{
	MATH3D_ASSERT( column >= 0 && column < 3 );
	return m[column];
}

inline const float * mat3x3::operator [] ( int column ) const
{
	MATH3D_ASSERT( column >= 0 && column < 3 );
	return m[column];
}

inline mat3x3 & mat3x3::operator = ( const mat3x3 &a )
{
	memcpy( this, &a, sizeof(mat3x3) );
	return *this;
}

inline mat3x3 & mat3x3::operator = ( const quat &rotation )
{
	quaternion_to_matrix( this, &rotation );
	return *this;
}

inline mat4x4 mat3x3::operator * ( const mat4x4 &right ) const
{
	mat4x4 temp;
	matrix_mul( &temp, this, &right );
	return temp;
}


inline mat4x3 mat3x3::operator * ( const mat4x3 &right ) const
{
	mat4x3 temp;
	matrix_mul( &temp, this, &right );
	return temp;
}


inline mat3x3 mat3x3::operator * ( const mat3x3 &right ) const
{
	mat3x3 temp;
	matrix_mul( &temp, this, &right );
	return temp;
}


inline mat3x3 & mat3x3::operator *= ( const mat3x3 &right )
{
	mat3x3 temp;
	matrix_mul( &temp, this, &right );
	*this = temp;
	return *this;
}


inline mat3x3 inverse( const mat3x3 &m )
{
	mat3x3 out = m;
	matrix_invert( &out[0][0], 3, 4, 1 );
	return out;
}


inline mat3x3 make_orthonormal( const mat3x3 &m )
{
	mat3x3 out = m;
	make_orthonormal( &(out[0][0]), 3, sizeof(out[0]), 1 );
	return out;
}

//////////////////////////////////////////////////////////////////////
//
// MATRIX-BUILDING UTILITY FUNCTIONS
//
//////////////////////////////////////////////////////////////////////



inline mat3x3 build_scale_matrix( float scale )
{
	mat3x3 m;
	
	m[0][0] = scale;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	
	m[1][0] = 0.0f;
	m[1][1] = scale;
	m[1][2] = 0.0f;
	
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = scale;
	
	return m;
}


inline mat3x3 build_scale_matrix( const vec3 &scale )
{
	mat3x3 m;
	
	m[0][0] = scale.x;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	
	m[1][0] = 0.0f;
	m[1][1] = scale.y;
	m[1][2] = 0.0f;
	
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = scale.z;
	
	return m;
}


inline mat3x3 build_rotation_matrix( const quat &rotation )
{
	return mat3x3(rotation);
}


inline mat3x3 build_rotation_matrix( const vec3 &axis, float angle_radians )
{
	return mat3x3(quat(axis,angle_radians));
}


inline mat3x3 build_rotation_matrix( float axis_x, float axis_y, float axis_z, float angle_radians )
{
	return mat3x3(quat(vec3(axis_x,axis_y,axis_z),angle_radians));
}

inline mat4x3 build_translation_matrix( const vec3 &translation )
{
	mat4x3 m;
	
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = translation.x;
	
	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[1][3] = translation.y;
	
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
	m[2][3] = translation.z;
	
	return m;
}


inline mat4x3 build_translation_matrix( float dx, float dy, float dz )
{
	mat4x3 m;
	
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = dx;
	
	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[1][3] = dy;
	
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
	m[2][3] = dz;
	
	return m;
}


inline mat3x3 build_inverse_scale_matrix( float scale )
{
	mat3x3 m;
	
	scale = 1.0f / scale;
	
	m[0][0] = scale;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	
	m[1][0] = 0.0f;
	m[1][1] = scale;
	m[1][2] = 0.0f;
	
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = scale;
	
	return m;
}


inline mat3x3 build_inverse_scale_matrix( const vec3 &scale )
{
	mat3x3 m;
	
	m[0][0] = 1.0f / scale.x;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	
	m[1][0] = 0.0f;
	m[1][1] = 1.0f / scale.y;
	m[1][2] = 0.0f;
	
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f / scale.z;
	
	return m;
}


inline mat3x3 build_inverse_rotation_matrix( const quat &rotation )
{
	quat inv_rotation( -rotation.x, -rotation.y, -rotation.z, rotation.w );
	return mat3x3(inv_rotation);
}


inline mat3x3 build_inverse_rotation_matrix( const vec3 &axis, float angle_radians )
{
	return mat3x3(quat(axis,-angle_radians));
}


inline mat3x3 build_inverse_rotation_matrix( float axis_x, float axis_y, float axis_z, float angle_radians )
{
	return mat3x3(quat(vec3(axis_x,axis_y,axis_z),-angle_radians));
}

inline mat4x3 build_inverse_translation_matrix( const vec3 &translation )
{
	mat4x3 m;
	
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = -translation.x;
	
	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[1][3] = -translation.y;
	
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
	m[2][3] = -translation.z;
	
	return m;
}


inline mat4x3 build_inverse_translation_matrix( float dx, float dy, float dz )
{
	mat4x3 m;
	
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = -dx;
	
	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[1][3] = -dy;
	
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
	m[2][3] = -dz;
	
	return m;
}



//////////////////////////////////////////////////////////////////////
//
// 3D-PLANE FUNCTIONS
//
//////////////////////////////////////////////////////////////////////



inline plane3::plane3() {}

inline plane3::plane3( float a, float b, float c, float d )
{
	this->a = a;
	this->b = b;
	this->c = c;
	this->d = d;
}


inline plane3::plane3( const vec3 &normal, float d )
{
	this->a = normal.x;
	this->b = normal.y;
	this->c = normal.z;
	this->d = d;
}


inline plane3::plane3( const vec3 &normal, const vec3 &point )
{
	this->a = normal.x;
	this->b = normal.y;
	this->c = normal.z;
	this->d = -normal.x * point.x - normal.y * point.y - normal.z * point.z;
}


inline plane3::plane3( const vec3 &v0, const vec3 &v1, const vec3 &v2 )
{
	vec3 n = cross( normalized(v2 - v0), normalized(v1 - v0) );
	n.normalize();
	this->a = n.x;
	this->b = n.y;
	this->c = n.z;
	this->d = dot( n, v0 );
}


inline plane3 &plane3::normalize()
{
	float l2 = a*a + b*b + c*c;
	if (l2 > 0.0f)
	{
		l2 = recip_sqrt( l2 );
		a *= l2;
		b *= l2;
		c *= l2;
		d *= l2;
	}
	return *this;
}

inline float &plane3::operator [] ( int i )
{
	MATH3D_ASSERT( i >= 0 && i <= 3 );
	return ((float*)this)[i];
}

inline const float &plane3::operator [] ( int i ) const
{	
	MATH3D_ASSERT( i >= 0 && i <= 3 );
	return ((float*)this)[i];
}

inline plane3 plane3::operator - () const
{
	plane3 p;

	p.a = -a;
	p.b = -b;
	p.c = -c;
	p.d = -d;

	return p;
}

inline bool operator < ( const vec3 &v, const plane3 &p )
{
	return p.a * v.x + p.b * v.y + p.c * v.z + p.d < -plane3::tolerance;
}


inline bool operator > ( const vec3 &v, const plane3 &p )
{
	return p.a * v.x + p.b * v.y + p.c * v.z + p.d > plane3::tolerance;
}


inline bool operator <= ( const vec3 &v, const plane3 &p )
{
	return p.a * v.x + p.b * v.y + p.c * v.z + p.d <= plane3::tolerance;
}


inline bool operator >= ( const vec3 &v, const plane3 &p )
{
	return p.a * v.x + p.b * v.y + p.c * v.z + p.d >= -plane3::tolerance;
}


inline bool on_plane( const vec3 &v, const plane3 &p )
{
	return fabsf(p.a * v.x + p.b * v.y + p.c * v.z + p.d) <= plane3::tolerance;
}

inline bool equals( const plane3& a, const plane3& b, float normal_epsilon, float dist_epsilon )
{
	if( (a.d < (b.d-dist_epsilon)) ||
		(a.d > (b.d+dist_epsilon)) )
		return false;

	return equals( vec3( a ), vec3( b ), normal_epsilon );
}



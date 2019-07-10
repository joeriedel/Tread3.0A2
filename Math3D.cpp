///////////////////////////////////////////////////////////////////////////////
// Math3D.cpp
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2008, Joe Riedel
// All rights reserved.
// Original Author: Andrew Meggs
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

#ifndef __math3d_h__
#include "math3d.h"
#endif

// Include inline functions in out-of-line form if mat3d.h
// didn't include them already as inlines.

#ifndef __math3d_inlines_i__
#define inline
#include "math3d_inlines.i"
#undef inline
#endif


static const float zero4x4[] =
	{
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f
	};


static const float ident4x4[] =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};


const mat3x3 mat3x3::zero = *(mat3x3 *)zero4x4;
const mat3x3 mat3x3::identity = *(mat3x3 *)ident4x4;

const mat4x3 mat4x3::zero = *(mat4x3 *)zero4x4;
const mat4x3 mat4x3::identity = *(mat4x3 *)ident4x4;

const mat4x4 mat4x4::zero = *(mat4x4 *)zero4x4;
const mat4x4 mat4x4::identity = *(mat4x4 *)ident4x4;

const quat quat::zero(0,0,0,0);
const quat quat::identity(0,0,0,1);

const vec2 vec2::zero(0,0);
const vec3 vec3::zero(0,0,0);
const vec3 vec3::bogus_max(999999.0f, 999999.0f, 999999.0f);
const vec3 vec3::bogus_min(-999999.0f, -999999.0f, -999999.0f);

float plane3::tolerance = 1.0 / 65536.0;


OS_FNEXP void quaternion_to_matrix( mat3x3 *out, const quat *in )
{
	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
	
	// calculate coefficients
	x2 = in->x + in->x;
	y2 = in->y + in->y; 
	z2 = in->z + in->z;
	
	xx = in->x * x2;
	xy = in->x * y2;
	xz = in->x * z2;
	
	yy = in->y * y2;
	yz = in->y * z2;
	zz = in->z * z2;
	
	wx = in->w * x2;
	wy = in->w * y2;
	wz = in->w * z2;
	
	out->m[0][0] = 1.0f - (yy + zz);
	out->m[0][1] = xy - wz;
	out->m[0][2] = xz + wy;
	
	out->m[1][0] = xy + wz;
	out->m[1][1] = 1.0f - (xx + zz);
	out->m[1][2] = yz - wx;
	
	out->m[2][0] = xz - wy;
	out->m[2][1] = yz + wx;
	out->m[2][2] = 1.0f - (xx + yy);
}

// This code is reversed from what I can find in the literature!
// I feel really strongly that mat2quat( quat2mat( q ) ) should equal q
// and that quat2mat( mat2quat( m ) ) should equal m, but the set
// of functions that I copied from Game Developer instead gives me
// their respective inverses. It's possible I screwed something up
// in the process of copying things over (and the original mat2quat
// is reproduced below if someone wishes to attempt to find my typo),
// and it's possible I completely misunderstand the intention of
// the original functions. Either way, for now I'm making the code
// do what I want it to do by having mat2quat return the inverse of
// what my attempt at transcribing the original code does.

OS_FNEXP void matrix_to_quaternion( quat *out, const mat3x3 *in )
{
	float tr, s, q[4];
	int i, j, k;

	static const int nxt[3] = {1, 2, 0};

	tr = in->m[0][0] + in->m[1][1] + in->m[2][2];

	// check the diagonal
	if (tr > 0.0)
	{
		s = sqrtf(tr + 1.0f);
		out->w = s / 2.0f;
		s = 0.5f / s;
		out->x = -(in->m[1][2] - in->m[2][1]) * s; // added minus sign (see above)
		out->y = -(in->m[2][0] - in->m[0][2]) * s; // added minus sign (see above)
		out->z = -(in->m[0][1] - in->m[1][0]) * s; // added minus sign (see above)
	}
	else
	{		
		// diagonal is negative
		i = 0;
		if (in->m[1][1] > in->m[0][0]) i = 1;
		if (in->m[2][2] > in->m[i][i]) i = 2;
		j = nxt[i];
		k = nxt[j];

		s = sqrtf( (in->m[i][i] - (in->m[j][j] + in->m[k][k])) + 1.0f );

		q[i] = s * 0.5f;

		if (s != 0.0f) s = 0.5f / s;

		q[3] = (in->m[j][k] - in->m[k][j]) * s;
		q[j] = (in->m[i][j] + in->m[j][i]) * s;
		q[k] = (in->m[i][k] + in->m[k][i]) * s;

		out->x = -q[0]; // added minus sign (see above)
		out->y = -q[1]; // added minus sign (see above)
		out->z = -q[2]; // added minus sign (see above)
		out->w = q[3];
	}
}

/* original code (see above)
void matrix_to_quaternion( quat *out, const mat3x3 *in )
{
	float tr, s, q[4];
	int i, j, k;

	static const int nxt[3] = {1, 2, 0};

	tr = in->m[0][0] + in->m[1][1] + in->m[2][2];

	// check the diagonal
	if (tr > 0.0)
	{
		s = sqrtf(tr + 1.0f);
		out->w = s / 2.0f;
		s = 0.5f / s;
		out->x = (in->m[1][2] - in->m[2][1]) * s;
		out->y = (in->m[2][0] - in->m[0][2]) * s;
		out->z = (in->m[0][1] - in->m[1][0]) * s;
	}
	else
	{		
		// diagonal is negative
		i = 0;
		if (in->m[1][1] > in->m[0][0]) i = 1;
		if (in->m[2][2] > in->m[i][i]) i = 2;
		j = nxt[i];
		k = nxt[j];

		s = sqrtf( (in->m[i][i] - (in->m[j][j] + in->m[k][k])) + 1.0f );

		q[i] = s * 0.5f;

		if (s != 0.0f) s = 0.5f / s;

		q[3] = (in->m[j][k] - in->m[k][j]) * s;
		q[j] = (in->m[i][j] + in->m[j][i]) * s;
		q[k] = (in->m[i][k] + in->m[k][i]) * s;

		out->x = q[0];
		out->y = q[1];
		out->z = q[2];
		out->w = q[3];
	}
}
*/

OS_FNEXP vec3 euler_from_matrix( const mat3x3 &m )
{
	float theta;
	float cp;
	float sp;
	vec3 v;
	
	sp = m[2][0];
	
	if( sp > 1.0f )
		sp = 1.0f;
	if( sp < -1.0f )
		sp = -1.0f;
		
	theta = (float)-asin( sp );
	cp = (float)cos( theta );
	
	if( cp > (8192.0f*1.19209289550781250000e-07) )
	{
		v[0] = theta;// * 180.0f / PI;
		v[1] = (float)atan2( m[1][0], m[0][0] );// * 180.0f / PI;
		v[2] = (float)atan2( m[2][1], m[2][2] );// * 180.0f / PI;
	}
	else
	{
		v[0] = theta;// * 180.0f / PI;
		v[1] = (float)-atan2( m[0][1], m[1][1] );// * 180.0f / PI;
		v[2] = 0.0f;
	}
	
	return v;
}

OS_FNEXP void slerp_quaternion( quat *out, const quat *from, const quat *to, float t )
{
	float to1[4];
	double omega, cosom, sinom, scale0, scale1;
	
	// calc cosine
	cosom = (from->x * to->x) + (from->y * to->y) + (from->z * to->z) + (from->w * to->w);
	
	// adjust signs (if necessary)
	if (cosom < 0.0f)
	{
		cosom = -cosom;
		to1[0] = -to->x;
		to1[1] = -to->y;
		to1[2] = -to->z;
		to1[3] = -to->w;
	}
	else
	{
		to1[0] = to->x;
		to1[1] = to->y;
		to1[2] = to->z;
		to1[3] = to->w;
	}
	
	// calculate coefficients
	
	// This particular definition of "close" was chosen
	// to match the keyframe tolerance of the skelanim
	// library.
	if ( (1.0 - cosom) > 0.0006f ) 
	{
		// standard case (slerp)
		omega = acosf((float)cosom);
		sinom = sinf((float)omega);
		scale0 = sinf((float)((1.0f - t) * omega)) / sinom;
		scale1 = sinf((float)(t * omega)) / sinom;
	}
	else
	{        
		// "from" and "to" quaternions are very close 
		//  ... so we can do a linear interpolation
		scale0 = 1.0f - t;
		scale1 = t;
	}
	
	// calculate final values
	out->x = (float)(scale0 * from->x + scale1 * to1[0]);
	out->y = (float)(scale0 * from->y + scale1 * to1[1]);
	out->z = (float)(scale0 * from->z + scale1 * to1[2]);
	out->w = (float)(scale0 * from->w + scale1 * to1[3]);
}


OS_FNEXP void quaternion_mul( quat *out, const quat *q1, const quat *q2 )
{
	float A, B, C, D, E, F, G, H;

	A = (q1->w + q1->x) * (q2->w + q2->x);
	B = (q1->z - q1->y) * (q2->y - q2->z);
	C = (q1->w - q1->x) * (q2->y + q2->z); 
	D = (q1->y + q1->z) * (q2->w - q2->x);
	E = (q1->x + q1->z) * (q2->x + q2->y);
	F = (q1->x - q1->z) * (q2->x - q2->y);
	G = (q1->w + q1->y) * (q2->w - q2->z);
	H = (q1->w - q1->y) * (q2->w + q2->z);

	out->w = B + (-E - F + G + H) * 0.5f;
	out->x = A - (E + F + G + H) * 0.5f; 
	out->y = C + (E - F + G - H) * 0.5f; 
	out->z = D + (E - F - G + H) * 0.5f;
}




#define MAX_INVERTIBLE 8

OS_FNEXP void matrix_invert( float *m, int size, int column_stride, int row_stride )
{
	if (!column_stride) column_stride = 1;
	if (!row_stride) row_stride = size * column_stride;
	
	MATH3D_ASSERT( size > 1 && size <= MAX_INVERTIBLE );
	
	int indxc[MAX_INVERTIBLE];
	int indxr[MAX_INVERTIBLE];
	int ipiv[MAX_INVERTIBLE];
	int i, icol, irow, j, k;
	float big, pivinv, temp;
	
	memset( ipiv, 0, sizeof(ipiv) );
	
	for (i = 0; i < size; ++i)
	{
		big = 0.0f;
		for (j = 0; j < size; ++j)
		{
			if (ipiv[j] != 1)
			{
				for (k = 0; k < size; ++k)
				{
					if (!ipiv[k])
					{
						float fa = (float)fabs( m[ j * row_stride + k * column_stride ] );
						if (fa > big)
						{
							big = fa;
							irow = j;
							icol = k;
						}
					}
					else if (ipiv[k] > 1)
					{
						// error, singular matrix
						return;
					}
				}
			}
		}
		ipiv[icol] += 1;
		
		if (irow != icol)
		{
			for (j = 0; j < size; ++j)
			{
				temp = m[icol * row_stride + j * column_stride];
				m[icol * row_stride + j * column_stride] = m[irow * row_stride + j * column_stride];
				m[irow * row_stride + j * column_stride] = temp;
			}
		}
		
		indxr[i] = irow;
		indxc[i] = icol;
		
		pivinv = m[icol * row_stride + icol * column_stride];
		if (pivinv == 0.0f)
		{
			// error, singular matrix
			return;
		}
		pivinv = 1.0f / pivinv;
		m[icol * row_stride + icol * column_stride] = 1.0f;
		for (j = 0; j < size; ++j)
		{
			m[icol * row_stride + j * column_stride] *= pivinv;
		}
		for (j = 0; j < size; ++j)
		{
			if (j != icol)
			{
				temp = m[j * row_stride + icol * column_stride];
				m[j * row_stride + icol * column_stride] = 0.0f;
				for (k = 0; k < size; ++k)
				{
					m[j * row_stride + k * column_stride] -= m[icol * row_stride + k * column_stride] * temp;
				}
			}
		}
	}
	
	for (i = size-1; i >= 0; --i)
	{
		if (indxr[i] != indxc[i])
		{
			for (j = 0; j < size; ++j)
			{
				temp = m[j * row_stride + indxr[i] * column_stride];
				m[j * row_stride + indxr[i] * column_stride] = m[j * row_stride + indxc[i] * column_stride];
				m[j * row_stride + indxc[i] * column_stride] = temp;
			}
		}
	}
}



OS_FNEXP void matrix_invert( mat4x4 *out, const mat4x4 *in )
{
	if (out != in) *out = *in;
	matrix_invert( &(*out)[0][0], 4, 4, 1 );
}


OS_FNEXP void matrix_invert( mat4x3 *out, const mat4x3 *in )
{
	mat4x4 temp = *in;
	matrix_invert( &temp[0][0], 4, 4, 1 );
	*out = temp;
}


OS_FNEXP void matrix_invert( mat3x3 *out, const mat3x3 *in )
{
	if (out != in) *out = *in;
	matrix_invert( &(*out)[0][0], 3, 4, 1 );
}



OS_FNEXP void matrix_mul( mat4x4 *out, const mat4x4 *left, const mat4x4 *right )
{
	MATH3D_ASSERT( (char *)left >= (char *)out + sizeof(*out) || (char *)out >= (char *)left + sizeof(*left) );
	MATH3D_ASSERT( (char *)right >= (char *)out + sizeof(*out) || (char *)out >= (char *)right + sizeof(*right) );
	
	out->m[0][0] = (left->m[0][0] * right->m[0][0]) + (left->m[1][0] * right->m[0][1]) + (left->m[2][0] * right->m[0][2]) + (left->m[3][0] * right->m[0][3]);
	out->m[0][1] = (left->m[0][1] * right->m[0][0]) + (left->m[1][1] * right->m[0][1]) + (left->m[2][1] * right->m[0][2]) + (left->m[3][1] * right->m[0][3]);
	out->m[0][2] = (left->m[0][2] * right->m[0][0]) + (left->m[1][2] * right->m[0][1]) + (left->m[2][2] * right->m[0][2]) + (left->m[3][2] * right->m[0][3]);
	out->m[0][3] = (left->m[0][3] * right->m[0][0]) + (left->m[1][3] * right->m[0][1]) + (left->m[2][3] * right->m[0][2]) + (left->m[3][3] * right->m[0][3]);
	
	out->m[1][0] = (left->m[0][0] * right->m[1][0]) + (left->m[1][0] * right->m[1][1]) + (left->m[2][0] * right->m[1][2]) + (left->m[3][0] * right->m[1][3]);
	out->m[1][1] = (left->m[0][1] * right->m[1][0]) + (left->m[1][1] * right->m[1][1]) + (left->m[2][1] * right->m[1][2]) + (left->m[3][1] * right->m[1][3]);
	out->m[1][2] = (left->m[0][2] * right->m[1][0]) + (left->m[1][2] * right->m[1][1]) + (left->m[2][2] * right->m[1][2]) + (left->m[3][2] * right->m[1][3]);
	out->m[1][3] = (left->m[0][3] * right->m[1][0]) + (left->m[1][3] * right->m[1][1]) + (left->m[2][3] * right->m[1][2]) + (left->m[3][3] * right->m[1][3]);
	
	out->m[2][0] = (left->m[0][0] * right->m[2][0]) + (left->m[1][0] * right->m[2][1]) + (left->m[2][0] * right->m[2][2]) + (left->m[3][0] * right->m[2][3]);
	out->m[2][1] = (left->m[0][1] * right->m[2][0]) + (left->m[1][1] * right->m[2][1]) + (left->m[2][1] * right->m[2][2]) + (left->m[3][1] * right->m[2][3]);
	out->m[2][2] = (left->m[0][2] * right->m[2][0]) + (left->m[1][2] * right->m[2][1]) + (left->m[2][2] * right->m[2][2]) + (left->m[3][2] * right->m[2][3]);
	out->m[2][3] = (left->m[0][3] * right->m[2][0]) + (left->m[1][3] * right->m[2][1]) + (left->m[2][3] * right->m[2][2]) + (left->m[3][3] * right->m[2][3]);
	
	out->m[3][0] = (left->m[0][0] * right->m[3][0]) + (left->m[1][0] * right->m[3][1]) + (left->m[2][0] * right->m[3][2]) + (left->m[3][0] * right->m[3][3]);
	out->m[3][1] = (left->m[0][1] * right->m[3][0]) + (left->m[1][1] * right->m[3][1]) + (left->m[2][1] * right->m[3][2]) + (left->m[3][1] * right->m[3][3]);
	out->m[3][2] = (left->m[0][2] * right->m[3][0]) + (left->m[1][2] * right->m[3][1]) + (left->m[2][2] * right->m[3][2]) + (left->m[3][2] * right->m[3][3]);
	out->m[3][3] = (left->m[0][3] * right->m[3][0]) + (left->m[1][3] * right->m[3][1]) + (left->m[2][3] * right->m[3][2]) + (left->m[3][3] * right->m[3][3]);
}


OS_FNEXP void matrix_mul( mat4x4 *out, const mat4x4 *left, const mat4x3 *right )
{
	MATH3D_ASSERT( (char *)left >= (char *)out + sizeof(*out) || (char *)out >= (char *)left + sizeof(*left) );
	MATH3D_ASSERT( (char *)right >= (char *)out + sizeof(*out) || (char *)out >= (char *)right + sizeof(*right) );
	
	out->m[0][0] = (left->m[0][0] * right->m[0][0]) + (left->m[1][0] * right->m[0][1]) + (left->m[2][0] * right->m[0][2]) + (left->m[3][0] * right->m[0][3]);
	out->m[0][1] = (left->m[0][1] * right->m[0][0]) + (left->m[1][1] * right->m[0][1]) + (left->m[2][1] * right->m[0][2]) + (left->m[3][1] * right->m[0][3]);
	out->m[0][2] = (left->m[0][2] * right->m[0][0]) + (left->m[1][2] * right->m[0][1]) + (left->m[2][2] * right->m[0][2]) + (left->m[3][2] * right->m[0][3]);
	out->m[0][3] = (left->m[0][3] * right->m[0][0]) + (left->m[1][3] * right->m[0][1]) + (left->m[2][3] * right->m[0][2]) + (left->m[3][3] * right->m[0][3]);
	
	out->m[1][0] = (left->m[0][0] * right->m[1][0]) + (left->m[1][0] * right->m[1][1]) + (left->m[2][0] * right->m[1][2]) + (left->m[3][0] * right->m[1][3]);
	out->m[1][1] = (left->m[0][1] * right->m[1][0]) + (left->m[1][1] * right->m[1][1]) + (left->m[2][1] * right->m[1][2]) + (left->m[3][1] * right->m[1][3]);
	out->m[1][2] = (left->m[0][2] * right->m[1][0]) + (left->m[1][2] * right->m[1][1]) + (left->m[2][2] * right->m[1][2]) + (left->m[3][2] * right->m[1][3]);
	out->m[1][3] = (left->m[0][3] * right->m[1][0]) + (left->m[1][3] * right->m[1][1]) + (left->m[2][3] * right->m[1][2]) + (left->m[3][3] * right->m[1][3]);
	
	out->m[2][0] = (left->m[0][0] * right->m[2][0]) + (left->m[1][0] * right->m[2][1]) + (left->m[2][0] * right->m[2][2]) + (left->m[3][0] * right->m[2][3]);
	out->m[2][1] = (left->m[0][1] * right->m[2][0]) + (left->m[1][1] * right->m[2][1]) + (left->m[2][1] * right->m[2][2]) + (left->m[3][1] * right->m[2][3]);
	out->m[2][2] = (left->m[0][2] * right->m[2][0]) + (left->m[1][2] * right->m[2][1]) + (left->m[2][2] * right->m[2][2]) + (left->m[3][2] * right->m[2][3]);
	out->m[2][3] = (left->m[0][3] * right->m[2][0]) + (left->m[1][3] * right->m[2][1]) + (left->m[2][3] * right->m[2][2]) + (left->m[3][3] * right->m[2][3]);
	
	out->m[3][0] = left->m[3][0];
	out->m[3][1] = left->m[3][1];
	out->m[3][2] = left->m[3][2];
	out->m[3][3] = left->m[3][3];
}


OS_FNEXP void matrix_mul( mat4x4 *out, const mat4x4 *left, const mat3x3 *right )
{
	MATH3D_ASSERT( (char *)left >= (char *)out + sizeof(*out) || (char *)out >= (char *)left + sizeof(*left) );
	MATH3D_ASSERT( (char *)right >= (char *)out + sizeof(*out) || (char *)out >= (char *)right + sizeof(*right) );
	
	out->m[0][0] = (left->m[0][0] * right->m[0][0]) + (left->m[1][0] * right->m[0][1]) + (left->m[2][0] * right->m[0][2]);
	out->m[0][1] = (left->m[0][1] * right->m[0][0]) + (left->m[1][1] * right->m[0][1]) + (left->m[2][1] * right->m[0][2]);
	out->m[0][2] = (left->m[0][2] * right->m[0][0]) + (left->m[1][2] * right->m[0][1]) + (left->m[2][2] * right->m[0][2]);
	out->m[0][3] = (left->m[0][3] * right->m[0][0]) + (left->m[1][3] * right->m[0][1]) + (left->m[2][3] * right->m[0][2]);
	
	out->m[1][0] = (left->m[0][0] * right->m[1][0]) + (left->m[1][0] * right->m[1][1]) + (left->m[2][0] * right->m[1][2]);
	out->m[1][1] = (left->m[0][1] * right->m[1][0]) + (left->m[1][1] * right->m[1][1]) + (left->m[2][1] * right->m[1][2]);
	out->m[1][2] = (left->m[0][2] * right->m[1][0]) + (left->m[1][2] * right->m[1][1]) + (left->m[2][2] * right->m[1][2]);
	out->m[1][3] = (left->m[0][3] * right->m[1][0]) + (left->m[1][3] * right->m[1][1]) + (left->m[2][3] * right->m[1][2]);
	
	out->m[2][0] = (left->m[0][0] * right->m[2][0]) + (left->m[1][0] * right->m[2][1]) + (left->m[2][0] * right->m[2][2]);
	out->m[2][1] = (left->m[0][1] * right->m[2][0]) + (left->m[1][1] * right->m[2][1]) + (left->m[2][1] * right->m[2][2]);
	out->m[2][2] = (left->m[0][2] * right->m[2][0]) + (left->m[1][2] * right->m[2][1]) + (left->m[2][2] * right->m[2][2]);
	out->m[2][3] = (left->m[0][3] * right->m[2][0]) + (left->m[1][3] * right->m[2][1]) + (left->m[2][3] * right->m[2][2]);
	
	out->m[3][0] = left->m[3][0];
	out->m[3][1] = left->m[3][1];
	out->m[3][2] = left->m[3][2];
	out->m[3][3] = left->m[3][3];
}


OS_FNEXP void matrix_mul( mat4x4 *out, const mat4x3 *left, const mat4x4 *right )
{
	MATH3D_ASSERT( (char *)left >= (char *)out + sizeof(*out) || (char *)out >= (char *)left + sizeof(*left) );
	MATH3D_ASSERT( (char *)right >= (char *)out + sizeof(*out) || (char *)out >= (char *)right + sizeof(*right) );
	
	out->m[0][0] = (left->m[0][0] * right->m[0][0]) + (left->m[1][0] * right->m[0][1]) + (left->m[2][0] * right->m[0][2]);
	out->m[0][1] = (left->m[0][1] * right->m[0][0]) + (left->m[1][1] * right->m[0][1]) + (left->m[2][1] * right->m[0][2]);
	out->m[0][2] = (left->m[0][2] * right->m[0][0]) + (left->m[1][2] * right->m[0][1]) + (left->m[2][2] * right->m[0][2]);
	out->m[0][3] = (left->m[0][3] * right->m[0][0]) + (left->m[1][3] * right->m[0][1]) + (left->m[2][3] * right->m[0][2]) + right->m[0][3];
	
	out->m[1][0] = (left->m[0][0] * right->m[1][0]) + (left->m[1][0] * right->m[1][1]) + (left->m[2][0] * right->m[1][2]);
	out->m[1][1] = (left->m[0][1] * right->m[1][0]) + (left->m[1][1] * right->m[1][1]) + (left->m[2][1] * right->m[1][2]);
	out->m[1][2] = (left->m[0][2] * right->m[1][0]) + (left->m[1][2] * right->m[1][1]) + (left->m[2][2] * right->m[1][2]);
	out->m[1][3] = (left->m[0][3] * right->m[1][0]) + (left->m[1][3] * right->m[1][1]) + (left->m[2][3] * right->m[1][2]) + right->m[1][3];
	
	out->m[2][0] = (left->m[0][0] * right->m[2][0]) + (left->m[1][0] * right->m[2][1]) + (left->m[2][0] * right->m[2][2]);
	out->m[2][1] = (left->m[0][1] * right->m[2][0]) + (left->m[1][1] * right->m[2][1]) + (left->m[2][1] * right->m[2][2]);
	out->m[2][2] = (left->m[0][2] * right->m[2][0]) + (left->m[1][2] * right->m[2][1]) + (left->m[2][2] * right->m[2][2]);
	out->m[2][3] = (left->m[0][3] * right->m[2][0]) + (left->m[1][3] * right->m[2][1]) + (left->m[2][3] * right->m[2][2]) + right->m[2][3];
	
	out->m[3][0] = (left->m[0][0] * right->m[3][0]) + (left->m[1][0] * right->m[3][1]) + (left->m[2][0] * right->m[3][2]);
	out->m[3][1] = (left->m[0][1] * right->m[3][0]) + (left->m[1][1] * right->m[3][1]) + (left->m[2][1] * right->m[3][2]);
	out->m[3][2] = (left->m[0][2] * right->m[3][0]) + (left->m[1][2] * right->m[3][1]) + (left->m[2][2] * right->m[3][2]);
	out->m[3][3] = (left->m[0][3] * right->m[3][0]) + (left->m[1][3] * right->m[3][1]) + (left->m[2][3] * right->m[3][2]) + right->m[3][3];
}


OS_FNEXP void matrix_mul( mat4x3 *out, const mat4x3 *left, const mat4x3 *right )
{
	MATH3D_ASSERT( (char *)left >= (char *)out + sizeof(*out) || (char *)out >= (char *)left + sizeof(*left) );
	MATH3D_ASSERT( (char *)right >= (char *)out + sizeof(*out) || (char *)out >= (char *)right + sizeof(*right) );
	
	out->m[0][0] = (left->m[0][0] * right->m[0][0]) + (left->m[1][0] * right->m[0][1]) + (left->m[2][0] * right->m[0][2]);
	out->m[0][1] = (left->m[0][1] * right->m[0][0]) + (left->m[1][1] * right->m[0][1]) + (left->m[2][1] * right->m[0][2]);
	out->m[0][2] = (left->m[0][2] * right->m[0][0]) + (left->m[1][2] * right->m[0][1]) + (left->m[2][2] * right->m[0][2]);
	out->m[0][3] = (left->m[0][3] * right->m[0][0]) + (left->m[1][3] * right->m[0][1]) + (left->m[2][3] * right->m[0][2]) + right->m[0][3];
	
	out->m[1][0] = (left->m[0][0] * right->m[1][0]) + (left->m[1][0] * right->m[1][1]) + (left->m[2][0] * right->m[1][2]);
	out->m[1][1] = (left->m[0][1] * right->m[1][0]) + (left->m[1][1] * right->m[1][1]) + (left->m[2][1] * right->m[1][2]);
	out->m[1][2] = (left->m[0][2] * right->m[1][0]) + (left->m[1][2] * right->m[1][1]) + (left->m[2][2] * right->m[1][2]);
	out->m[1][3] = (left->m[0][3] * right->m[1][0]) + (left->m[1][3] * right->m[1][1]) + (left->m[2][3] * right->m[1][2]) + right->m[1][3];
	
	out->m[2][0] = (left->m[0][0] * right->m[2][0]) + (left->m[1][0] * right->m[2][1]) + (left->m[2][0] * right->m[2][2]);
	out->m[2][1] = (left->m[0][1] * right->m[2][0]) + (left->m[1][1] * right->m[2][1]) + (left->m[2][1] * right->m[2][2]);
	out->m[2][2] = (left->m[0][2] * right->m[2][0]) + (left->m[1][2] * right->m[2][1]) + (left->m[2][2] * right->m[2][2]);
	out->m[2][3] = (left->m[0][3] * right->m[2][0]) + (left->m[1][3] * right->m[2][1]) + (left->m[2][3] * right->m[2][2]) + right->m[2][3];
}


OS_FNEXP void matrix_mul( mat4x3 *out, const mat4x3 *left, const mat3x3 *right )
{
	MATH3D_ASSERT( (char *)left >= (char *)out + sizeof(*out) || (char *)out >= (char *)left + sizeof(*left) );
	MATH3D_ASSERT( (char *)right >= (char *)out + sizeof(*out) || (char *)out >= (char *)right + sizeof(*right) );
	
	out->m[0][0] = (left->m[0][0] * right->m[0][0]) + (left->m[1][0] * right->m[0][1]) + (left->m[2][0] * right->m[0][2]);
	out->m[0][1] = (left->m[0][1] * right->m[0][0]) + (left->m[1][1] * right->m[0][1]) + (left->m[2][1] * right->m[0][2]);
	out->m[0][2] = (left->m[0][2] * right->m[0][0]) + (left->m[1][2] * right->m[0][1]) + (left->m[2][2] * right->m[0][2]);
	out->m[0][3] = (left->m[0][3] * right->m[0][0]) + (left->m[1][3] * right->m[0][1]) + (left->m[2][3] * right->m[0][2]);
	
	out->m[1][0] = (left->m[0][0] * right->m[1][0]) + (left->m[1][0] * right->m[1][1]) + (left->m[2][0] * right->m[1][2]);
	out->m[1][1] = (left->m[0][1] * right->m[1][0]) + (left->m[1][1] * right->m[1][1]) + (left->m[2][1] * right->m[1][2]);
	out->m[1][2] = (left->m[0][2] * right->m[1][0]) + (left->m[1][2] * right->m[1][1]) + (left->m[2][2] * right->m[1][2]);
	out->m[1][3] = (left->m[0][3] * right->m[1][0]) + (left->m[1][3] * right->m[1][1]) + (left->m[2][3] * right->m[1][2]);
	
	out->m[2][0] = (left->m[0][0] * right->m[2][0]) + (left->m[1][0] * right->m[2][1]) + (left->m[2][0] * right->m[2][2]);
	out->m[2][1] = (left->m[0][1] * right->m[2][0]) + (left->m[1][1] * right->m[2][1]) + (left->m[2][1] * right->m[2][2]);
	out->m[2][2] = (left->m[0][2] * right->m[2][0]) + (left->m[1][2] * right->m[2][1]) + (left->m[2][2] * right->m[2][2]);
	out->m[2][3] = (left->m[0][3] * right->m[2][0]) + (left->m[1][3] * right->m[2][1]) + (left->m[2][3] * right->m[2][2]);
}


OS_FNEXP void matrix_mul( mat4x4 *out, const mat3x3 *left, const mat4x4 *right )
{
	MATH3D_ASSERT( (char *)left >= (char *)out + sizeof(*out) || (char *)out >= (char *)left + sizeof(*left) );
	MATH3D_ASSERT( (char *)right >= (char *)out + sizeof(*out) || (char *)out >= (char *)right + sizeof(*right) );
	
	out->m[0][0] = (left->m[0][0] * right->m[0][0]) + (left->m[1][0] * right->m[0][1]) + (left->m[2][0] * right->m[0][2]);
	out->m[0][1] = (left->m[0][1] * right->m[0][0]) + (left->m[1][1] * right->m[0][1]) + (left->m[2][1] * right->m[0][2]);
	out->m[0][2] = (left->m[0][2] * right->m[0][0]) + (left->m[1][2] * right->m[0][1]) + (left->m[2][2] * right->m[0][2]);
	out->m[0][3] = right->m[0][3];
	
	out->m[1][0] = (left->m[0][0] * right->m[1][0]) + (left->m[1][0] * right->m[1][1]) + (left->m[2][0] * right->m[1][2]);
	out->m[1][1] = (left->m[0][1] * right->m[1][0]) + (left->m[1][1] * right->m[1][1]) + (left->m[2][1] * right->m[1][2]);
	out->m[1][2] = (left->m[0][2] * right->m[1][0]) + (left->m[1][2] * right->m[1][1]) + (left->m[2][2] * right->m[1][2]);
	out->m[1][3] = right->m[1][3];
	
	out->m[2][0] = (left->m[0][0] * right->m[2][0]) + (left->m[1][0] * right->m[2][1]) + (left->m[2][0] * right->m[2][2]);
	out->m[2][1] = (left->m[0][1] * right->m[2][0]) + (left->m[1][1] * right->m[2][1]) + (left->m[2][1] * right->m[2][2]);
	out->m[2][2] = (left->m[0][2] * right->m[2][0]) + (left->m[1][2] * right->m[2][1]) + (left->m[2][2] * right->m[2][2]);
	out->m[2][3] = right->m[2][3];
	
	out->m[3][0] = (left->m[0][0] * right->m[3][0]) + (left->m[1][0] * right->m[3][1]) + (left->m[2][0] * right->m[3][2]);
	out->m[3][1] = (left->m[0][1] * right->m[3][0]) + (left->m[1][1] * right->m[3][1]) + (left->m[2][1] * right->m[3][2]);
	out->m[3][2] = (left->m[0][2] * right->m[3][0]) + (left->m[1][2] * right->m[3][1]) + (left->m[2][2] * right->m[3][2]);
	out->m[3][3] = right->m[3][3];
}


OS_FNEXP void matrix_mul( mat4x3 *out, const mat3x3 *left, const mat4x3 *right )
{
	MATH3D_ASSERT( (char *)left >= (char *)out + sizeof(*out) || (char *)out >= (char *)left + sizeof(*left) );
	MATH3D_ASSERT( (char *)right >= (char *)out + sizeof(*out) || (char *)out >= (char *)right + sizeof(*right) );
	
	out->m[0][0] = (left->m[0][0] * right->m[0][0]) + (left->m[1][0] * right->m[0][1]) + (left->m[2][0] * right->m[0][2]);
	out->m[0][1] = (left->m[0][1] * right->m[0][0]) + (left->m[1][1] * right->m[0][1]) + (left->m[2][1] * right->m[0][2]);
	out->m[0][2] = (left->m[0][2] * right->m[0][0]) + (left->m[1][2] * right->m[0][1]) + (left->m[2][2] * right->m[0][2]);
	out->m[0][3] = right->m[0][3];
	
	out->m[1][0] = (left->m[0][0] * right->m[1][0]) + (left->m[1][0] * right->m[1][1]) + (left->m[2][0] * right->m[1][2]);
	out->m[1][1] = (left->m[0][1] * right->m[1][0]) + (left->m[1][1] * right->m[1][1]) + (left->m[2][1] * right->m[1][2]);
	out->m[1][2] = (left->m[0][2] * right->m[1][0]) + (left->m[1][2] * right->m[1][1]) + (left->m[2][2] * right->m[1][2]);
	out->m[1][3] = right->m[1][3];
	
	out->m[2][0] = (left->m[0][0] * right->m[2][0]) + (left->m[1][0] * right->m[2][1]) + (left->m[2][0] * right->m[2][2]);
	out->m[2][1] = (left->m[0][1] * right->m[2][0]) + (left->m[1][1] * right->m[2][1]) + (left->m[2][1] * right->m[2][2]);
	out->m[2][2] = (left->m[0][2] * right->m[2][0]) + (left->m[1][2] * right->m[2][1]) + (left->m[2][2] * right->m[2][2]);
	out->m[2][3] = right->m[2][3];
}


OS_FNEXP void matrix_mul( mat3x3 *out, const mat3x3 *left, const mat3x3 *right )
{
	MATH3D_ASSERT( (char *)left >= (char *)out + sizeof(*out) || (char *)out >= (char *)left + sizeof(*left) );
	MATH3D_ASSERT( (char *)right >= (char *)out + sizeof(*out) || (char *)out >= (char *)right + sizeof(*right) );
	
	out->m[0][0] = (left->m[0][0] * right->m[0][0]) + (left->m[1][0] * right->m[0][1]) + (left->m[2][0] * right->m[0][2]);
	out->m[0][1] = (left->m[0][1] * right->m[0][0]) + (left->m[1][1] * right->m[0][1]) + (left->m[2][1] * right->m[0][2]);
	out->m[0][2] = (left->m[0][2] * right->m[0][0]) + (left->m[1][2] * right->m[0][1]) + (left->m[2][2] * right->m[0][2]);
	
	out->m[1][0] = (left->m[0][0] * right->m[1][0]) + (left->m[1][0] * right->m[1][1]) + (left->m[2][0] * right->m[1][2]);
	out->m[1][1] = (left->m[0][1] * right->m[1][0]) + (left->m[1][1] * right->m[1][1]) + (left->m[2][1] * right->m[1][2]);
	out->m[1][2] = (left->m[0][2] * right->m[1][0]) + (left->m[1][2] * right->m[1][1]) + (left->m[2][2] * right->m[1][2]);
	
	out->m[2][0] = (left->m[0][0] * right->m[2][0]) + (left->m[1][0] * right->m[2][1]) + (left->m[2][0] * right->m[2][2]);
	out->m[2][1] = (left->m[0][1] * right->m[2][0]) + (left->m[1][1] * right->m[2][1]) + (left->m[2][1] * right->m[2][2]);
	out->m[2][2] = (left->m[0][2] * right->m[2][0]) + (left->m[1][2] * right->m[2][1]) + (left->m[2][2] * right->m[2][2]);
}


void fprint_matrix( FILE *f, const mat3x3 &m )
{
	for (int i = 0; i < 3; ++i)
	{
		fprintf( f, "\n     " );
		for (int j = 0; j < 3; ++j)
		{
			fprintf( f, "%10.5f", m[j][i] );
		}
	}
	fprintf( f, "\n" );
}


void fprint_matrix( FILE *f, const mat4x3 &m )
{
	for (int i = 0; i < 4; ++i)
	{
		fprintf( f, "\n     " );
		for (int j = 0; j < 3; ++j)
		{
			fprintf( f, "%10.5f", m[j][i] );
		}
	}
	fprintf( f, "\n" );
}


void fprint_matrix( FILE *f, const mat4x4 &m )
{
	for (int i = 0; i < 4; ++i)
	{
		fprintf( f, "\n     " );
		for (int j = 0; j < 4; ++j)
		{
			fprintf( f, "%10.5f", m[j][i] );
		}
	}
	fprintf( f, "\n" );
}


OS_FNEXP void factor_matrix( vec3 *out_scale, quat *out_orient, vec3 *out_offset, const mat4x3 &in )
{
	int i;
	
	vec3 scale;
	mat4x3 m = in;
	for (i = 0; i < 3; ++i)
	{
		double s = sqrt( in[0][i] * in[0][i] + in[1][i] * in[1][i] + in[2][i] * in[2][i] );
		scale[i] = (float)s;
		m[0][i] = (float)(in[0][i] / s);
		m[1][i] = (float)(in[1][i] / s);
		m[2][i] = (float)(in[2][i] / s);
	}
	if (out_scale) *out_scale = scale;
	
	if (out_orient)
	{
		*out_orient = m;
		mat3x3 test_m(*out_orient);
	}
	
	if (out_offset)
	{
		for (i = 0; i < 3; ++i)
		{
			(*out_offset)[i] = m[i][3];
		}
	}
}

OS_FNEXP void make_orthonormal( float *m, int size, int column_stride, int row_stride );
OS_FNEXP void make_orthonormal( float *m, int size, int column_stride, int row_stride )
{
	int i, j, k;
	for (i = 0; i < size; ++i)
	{
		// make othogonal to earlier columns
		for (j = 0; j < i; ++j)
		{
			// compute the dot product of columns j and i. note that column j is normalized
			float proj = 0.0f;
			for (k = 0; k < size; ++k)
			{
				proj += m[(i*column_stride) + (k*row_stride)] * m[(j*column_stride) + (k*row_stride)];
			}
			
			// subtract out the projection of column j onto column i
			for (k = 0; k < size; ++k)
			{
				m[(i*column_stride) + (k*row_stride)] -= proj * m[(j*column_stride) + (k*row_stride)];
			}
		}
		
		// normalize column i
		float len = 0.0f;
		for (j = 0; j < size; ++j)
		{
			len += m[(i*column_stride) + (j*row_stride)] * m[(i*column_stride) + (j*row_stride)];
		}
		float n = 1.0f / sqrtf(len);
		for (j = 0; j < size; ++j)
		{
			m[(i*column_stride) + (j*row_stride)] *= n;
		}
	}
}




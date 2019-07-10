///////////////////////////////////////////////////////////////////////////////
// lss.h
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

#ifndef LSS_H
#define LSS_H

#include <math.h>
#include <burger.h>

/**********************************

	Solves the set of n linear equations A * X = B. Here a[0..N-1][0..N-1] is
	input, not as the matrix A but rather as its LU decomposition, as determined
	by the routine ludcmp above. indx[0..N-1] is input as the permutation vector
	returned by ludcmp. b[0..N-1] is input as the right-hand side vector B,
	and returns with the solution vector X. a and indx are not modified by
	this routine and can be left in place for successive calls with different
	right-hand sides b.

	The main problem with this algorithm is the #*&$ divides.
	However, I can queue a reciprocal, then perform integer
	operations while the FPU is talking to itself. This way
	I can get from "FREE" code time.

	Yes, the code looks REAL funky, but I've benchmarks this on intel and PPC
	and got a 30% speed increase.

**********************************/

inline void lubksub4(float *a,int *indx,float *b)
{
	float b0,b1,b2,b3;
	float ia15;					/* Reciprocal storage */
	int ip1;					/* Index temp */

	ia15 = 1.0f/a[15];			/* Create the reciprocal and defer */
								/* it for a while */

	/* While the divide is working, perform loads and stores */

	ip1 = indx[0];				/* First index */
	b0 = b[ip1];		  		/* Get my first constant */
	((LongWord *)b)[ip1] = ((LongWord *)b)[0];	/* Copy with integer instructions */

	ip1 = indx[1];				/* I do not care about Address generation interlocks */
								/* The divide gives me lots of time to burn */
	b1 = b[ip1];
	((LongWord *)b)[ip1] = ((LongWord *)b)[1];	/* Use the int unit! */

	ip1 = indx[2];
	b2 = b[ip1];
	((LongWord *)b)[ip1] = ((LongWord *)b)[2];	/* Use the integer instructions */

	/* Now I have to do math operations */

	b1 = b1-(a[4]*b0);		/* Scale the first, sub from second */
	b2 = (b2-(a[8]*b0))-(a[9]*b1);
	b3 = (((b[3]-(a[12] * b0)) - (a[13] * b1)) - (a[14] * b2)) * ia15;	/* Use the divide */
	b2 = (b2 - (a[11] * b3))/a[10];				/* I can't defer */
	b1 = ((b1 - (a[6] * b2))-(a[7]*b3)) / a[5];	/* I can't defer */
	ia15 = 1.0f/a[0];			/* Defer the last divide */
	b[3] = b3;					/* While the divide is going, copy these values */
	b[2] = b2;
	b[1] = b1;
	b[0] = (((b0 - (a[1] * b1))-(a[2] * b2)) - (a[3] * b3)) * ia15;
}

/**********************************

	Given a matrix a[0..3][0..3], this rotuine replaces it by the
	LU decomposition of a rowwise permutation of itself. a is
	input. a is output, rearranged as the pair of decompositions.
	index[0..3] is an output vector that records the row permutation
	effected by the partial pivoting; the return value is +/- 1.0
	depending on whether the number of row exchanges is even or odd,
	respectively. This routine is used in combination with lubksub to
	solve linear equations or invert a matrix.

	Return TRUE if it's ok, FALSE if a math error occured

**********************************/

inline Word ludcmp4(float *a, int *indx)
{
	float vv[4];
	Word i;
	Word j;

	// find highest value in each row for pivot selection

	i = 0;
	do {
		float big;
		float temp;
		big = FloatAbs(a[0]);
		temp = FloatAbs(a[1]);
		if (temp>big) {
			big = temp;
		}
		temp = FloatAbs(a[2]);
		if (temp>big) {
			big = temp;
		}
		temp = FloatAbs(a[3]);
		a+=4;				/* Next row */
		if (temp>big) {
			big = temp;
		}
		if (!big) {			/* Largest is zero? */
			return FALSE;	/* error, singular matrix */
		}
		vv[i] = 1.0f / big;	/* Get the reciprocal */
	} while (++i<4);
	a -= 16;			/* Restore the pointer */

	/* At this point vv[4] has this largest absolute values of each column */

	j = 0;
	do {	// loop over columns
		Word imax;
		imax = j;
		if (j>=2) {				/* Don't do much on the 2 passes */
			i = 1;				/* This index actually matters! */
			do {
				float sum;		/* Running total */
				Word k;
				float *ap1;
				float *ap2;

				ap1 = a+i*4;	/* Initial row index */
				ap2 = a+j;		/* Column index */
				sum = ap1[j];	/* Get the initial value */
				k = i;			/* Iterator count */
				do {
					sum -= ap1[0] * ap2[0];
					ap1 += 1;		/* Next column */
					ap2 += 4;		/* Next row */
				} while (--k);
				a[i*4+j] = sum;		/* Store the new value */
			} while (++i<j);
		}

		float big = 0.0f;			/* Init the largest value */
		i = j;
		do {				/* This is OK, j<4 */
			float sum;
			float *ap1;
			float *ap2;

			ap1 = a+i*4;	/* Initial row index */
			ap2 = a+j;		/* Column index */
			sum = ap1[j];
			if (j) {
				Word k;
				k = j;
				do {
					sum -= ap1[0] * ap2[0];
					ap1 += 1;
					ap2 += 4;
				} while (--k);
			}
			a[i*4+j] = sum;
			sum = vv[i] * FloatAbs(sum);
			if (sum > big) {
				big = sum;
				imax = i;		/* Max index */
			}
		} while (++i<4);

		if (j != imax) {		// do we need to interchange rows?
			float *ap1 = &a[imax*4];
			float *ap2 = &a[j*4];
			LongWord temp,temp2;
			temp = ((LongWord *)ap1)[0];
			temp2 = ((LongWord *)ap2)[0];
			((LongWord *)ap2)[0] = temp;
			((LongWord *)ap1)[0] = temp2;
			temp = ((LongWord *)ap1)[1];
			temp2 = ((LongWord *)ap2)[1];
			((LongWord *)ap2)[1] = temp;
			((LongWord *)ap1)[1] = temp2;
			temp = ((LongWord *)ap1)[2];
			temp2 = ((LongWord *)ap2)[2];
			((LongWord *)ap2)[2] = temp;
			((LongWord *)ap1)[2] = temp2;
			temp = ((LongWord *)ap1)[3];
			temp2 = ((LongWord *)ap2)[3];
			((LongWord *)ap2)[3] = temp;
			((LongWord *)ap1)[3] = temp2;
			vv[imax] = vv[j];	// and update scale factor
		}
		indx[j] = imax;
		if (!a[j*5]) {
			return FALSE;	// singular matrix
		}
		if (j != 3) {		// now, finally, divide by pivot element
			float invpivot;
			float *ap1;
			ap1 = a+(j*5);
			invpivot = 1.0f / ap1[0];	/* Get the reciprocal */
			ap1 = ap1+4;		/* Adjust the pointer */
			i = 3-j;
			do {
				ap1[0] *= invpivot;	/* Perform the divide (1.0f/val) */
				ap1+=4;
			} while (--i);		/* Count down */
		}
	} while (++j<4);			/* All done? */
	return TRUE;
}

#endif
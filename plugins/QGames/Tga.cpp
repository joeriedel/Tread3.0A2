///////////////////////////////////////////////////////////////////////////////
// Tga.cpp
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
#include "Tga.h"
#include "ByteParser.h"
#include <algorithm>

static bool UnpackTGAByte3(unsigned char *DestBuffer,const unsigned char *InputPtr,int Pixels, int buffLength)
{
	int Count;
	do {
		if (buffLength<1) return false;
		Count = *InputPtr++;	/* Get the counter */
		if (Count&0x80) {		/* Packed? */
			if (buffLength<3) return false;
			unsigned short Temp;
			unsigned char Temp2;
			Count = Count-0x7F;	/* Remove the high bit */
			Pixels = Pixels-Count;
			Temp = LoadIntelShort(((const unsigned short *)(InputPtr+1))[0]);	/* Get G,R */
			Temp2 = InputPtr[0];		/* Blue */
			InputPtr = InputPtr+3;
			buffLength -= 3;
			do {
				((unsigned short *)DestBuffer)[0] = Temp;	/* R and G */
				DestBuffer[2] = Temp2;		/* Blue */
				DestBuffer = DestBuffer+3;
			} while (--Count);
		} else {
			++Count;			/* +1 to the count */
			if (buffLength<Count) return false;
			Pixels = Pixels-Count;	/* Adjust amount */
			buffLength -= Count;
			do {
				DestBuffer[0] = InputPtr[2];	/* Red */
				DestBuffer[1] = InputPtr[1];	/* Green */
				DestBuffer[2] = InputPtr[0];	/* Blue */
				DestBuffer = DestBuffer+3;		/* Next index */
				InputPtr = InputPtr+3;
			} while (--Count);		/* All done? */
		}
	} while (Pixels>0);	/* Is there still more? */

	return true;
}

static bool UnpackTGALong(unsigned char *DestBuffer,const unsigned char *InputPtr,int Pixels, int buffLength)
{
	int Count;
	do {
		if (buffLength<1) return false;
		Count = *InputPtr++;	/* Get the counter */
		if (Count&0x80) {		/* Packed? */
			if (buffLength<4) return false;
			unsigned int Temp;
			Count = Count-0x7F;	/* Remove the high bit and add 1 */
			Pixels = Pixels-Count;
			((unsigned char *)&Temp)[0] = InputPtr[2];	/* Red */
			((unsigned char *)&Temp)[1] = InputPtr[1];	/* Green */
			((unsigned char *)&Temp)[2] = InputPtr[0];	/* Blue */
			((unsigned char *)&Temp)[3] = InputPtr[3];	/* Alpha */
			InputPtr = InputPtr+4;
			buffLength -= 4;
			do {
				((unsigned int *)DestBuffer)[0] = Temp;	/* Fill memory */
				DestBuffer = DestBuffer+4;
			} while (--Count);
		} else {
			++Count;				/* +1 to the count */
			if (buffLength<Count) return false;
			Pixels = Pixels-Count;	/* Adjust amount */
			buffLength -= Count;
			do {
				DestBuffer[0] = InputPtr[2];	/* Red */
				DestBuffer[1] = InputPtr[1];	/* Green */
				DestBuffer[2] = InputPtr[0];	/* Blue */
				DestBuffer[3] = InputPtr[3];	/* Alpha */
				InputPtr = InputPtr+4;
				DestBuffer = DestBuffer+4;
			} while (--Count);		/* All done? */
		}
	} while (Pixels>0);	/* Is there still more? */

	return true;
}

bool CTga::IsTga(const void *buff, int len)
{
	if (len < 18) return false;
	const unsigned char *ptr = reinterpret_cast<const unsigned char*>(buff);
	return ptr[2] == 2 || ptr[2] == 10;
}

bool CTga::ReadInfo(const void *buff, int len, int *width, int *height, int *bpp)
{
	if (!IsTga(buff, len)) return false;
	CByteParser parser(buff, len);
	parser.Skip(12);
	*width = parser.ReadShort();
	*height = parser.ReadShort();
	int bits = parser.ReadByte();
	*bpp = bits / 8;
	return true;
}

bool CTga::Read(const void *buff, int len, void **dst, int *width, int *height, int *bpp)
{
	if (!IsTga(buff, len)) return false;
	CByteParser parser(buff, len);
	int tagLen = parser.ReadByte();
	parser.Skip(1);
	int type = parser.ReadByte();
	parser.Skip(9);
	//int xorg = parser.ReadShort();
	//int yorg = parser.ReadShort();
	*width = parser.ReadShort();
	*height = parser.ReadShort();
	int bits = parser.ReadByte();
	*bpp = bits / 8;
	int flags = parser.ReadByte();
	parser.Skip(tagLen);
	int imgLen = *width * *height * *bpp;
	len  = parser.Left();
	if (!len) return false;
	unsigned char *ptr = new unsigned char[len];
	parser.Read(ptr, len);

	switch (type)
	{
	case 2: // BGR uncompressed
		{
			*dst = ptr;
		} break;

	case 10: // BGR rle
		{
			*dst = new unsigned char[imgLen];
			if (*bpp == 3)
			{
				UnpackTGAByte3((unsigned char*)*dst, ptr, imgLen / *bpp, len);
			}
			else
			{
				UnpackTGALong((unsigned char*)*dst, ptr, imgLen / *bpp, len);
			}

			delete [] ptr;
			ptr = (unsigned char*)*dst;
		} break;
	}

	unsigned char *work = ptr;
	for (int i = 0; i < imgLen/(*bpp); ++i)
	{
		std::swap(work[0], work[2]);
		work += *bpp;
	}

	if ((flags&32) == 0)
	{
		int scan = *width * *bpp;
		unsigned char *swap = new unsigned char[scan];
		for (int y = 0; y < *height/2 ; ++y)
		{
			int flipY = *height - y - 1;
			void *a = &ptr[y * scan];
			void *b = &ptr[flipY * scan];
			memcpy(swap, a, scan);
			memcpy(a, b, scan);
			memcpy(b, swap, scan);
		}
		delete [] swap;
	}

	//	a2l

	return true;
}
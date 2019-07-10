///////////////////////////////////////////////////////////////////////////////
// TextureFactory.cpp
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
#include "TextureFactory.h"

///////////////////////////////////////////////////////////////////////////////
// CTextureFactory
///////////////////////////////////////////////////////////////////////////////

void *CTextureFactory::ExpandPalettized(const void *voidSrc, int width, int height, int dstBpp, const void *palette)
{
	unsigned char *dst = new unsigned char[width * height * dstBpp];
	const unsigned char *src = static_cast<const unsigned char*>(voidSrc);
	const unsigned char *pal = static_cast<const unsigned char*>(palette);
	OS_ASSERT(dstBpp == 3 || dstBpp == 4);
	
	unsigned char *work = dst;
	int num = width * height;
	while (num-- > 0)
	{
		int v = static_cast<int>(*src) * 3;
		work[0] = pal[v+0];
		work[1] = pal[v+1];
		work[2] = pal[v+2];
		if (dstBpp == 4) { work[3] = 255; }
		work += dstBpp;
		++src;
	}
	
	return dst;
}

void CTextureFactory::FreeTexture(void *src)
{
	delete [] static_cast<unsigned char*>(src);
}
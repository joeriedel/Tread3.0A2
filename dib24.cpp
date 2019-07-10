///////////////////////////////////////////////////////////////////////////////
// Dib24.cpp
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
#include "dib24.h"

typedef struct
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors;
} BITMAPDIB24;

// Blits the bitmap onto the window.
void BlitDib24(CDC* pDC, CDC* bmDC, RECT size)
{
	// Blit it to the screen.
	pDC->BitBlt(0, 0, size.right, size.bottom, bmDC, 0, 0, SRCCOPY);
}

// Creates a 24 bit bitmaps of RECT size.
HBITMAP CreateDib24(HDC hdc, RECT size)
{
	BYTE* bits;
	HBITMAP bmhandle;
	BITMAPDIB24 BMInfo;
	DEVMODE devmode;

	// Get the current display mode.
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);

	memset(&BMInfo, 0, sizeof(BITMAPDIB24));
	BMInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BMInfo.bmiHeader.biWidth = size.right;
	BMInfo.bmiHeader.biHeight = size.bottom;
	BMInfo.bmiHeader.biPlanes = 1;
	BMInfo.bmiHeader.biBitCount = (devmode.dmBitsPerPel > 8) ? (USHORT)devmode.dmBitsPerPel : (USHORT)16;
	BMInfo.bmiHeader.biCompression = BI_RGB;	// No compression.
	BMInfo.bmiHeader.biSizeImage = NULL;
	BMInfo.bmiHeader.biXPelsPerMeter = NULL;
	BMInfo.bmiHeader.biYPelsPerMeter = NULL;
	BMInfo.bmiHeader.biClrUsed = 0;//256;
	BMInfo.bmiHeader.biClrImportant = 0;//256;

	bmhandle = CreateDIBSection(hdc, (BITMAPINFO*)&BMInfo, DIB_RGB_COLORS, 
		(VOID**)&bits, NULL, NULL);

	return(bmhandle);
}

// Deletes the bitmap.
void DeleteDib24(HBITMAP bm)
{
	DeleteObject(bm);
}
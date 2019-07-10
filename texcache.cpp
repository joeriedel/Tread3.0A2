///////////////////////////////////////////////////////////////////////////////
// texcache.cpp
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
#include "system.h"
#include "r_gl.h"
#include "texcache.h"
#include "files.h"
#include "util.h"

#define KEYOFS(key) (key>>5)
#define KEYBIT(key) (1<<(key&31))

static int c_num_textures=0;
static TC_Texture_t c_textures[MAX_CACHED_TEXTURES];

static void FreeAllTextures();
static int FindFreeTexture();
static void FreeTexture( TC_Texture_t* tex );
static bool PalettizeTexture( TC_Texture_t* tc, int index_components );
static void InvertAlpha( Image_t* img, int index_components );

#define LOOKUP24BITCOLORINIMAP( imap, r, g, b ) ( imap[ (((LongWord)(r))>>3)+((((LongWord)(g))>>3)<<5)+((((LongWord)(b))>>3)<<10) ] )

//
// these routines will be added to burger lib soon...
//
static Byte* PaletteMake16bitIMAP( const Byte* pal )
{
	Word r, g, b;
	Word rl, gl, bl;
	Byte* imap, *out;
	
	imap = out = (Byte*)OS_AllocAPointer( 32*32*32 );
	
	for( b = 0; b < 32; b++ )
	{
		bl = b<<3;
		for( g = 0; g < 32; g++ )
		{
			gl = g<<3;
			for( r = 0; r < 32; r++ )
			{
				rl = r<<3;
				
				out[0] = PaletteFindColorIndex( pal, rl, gl, bl, 256 );
				out++;
			}
		}
	}
	
	return imap;
}

bool TC_Init(void)
{
	c_num_textures=0;
	memset( c_textures, 0, sizeof(TC_Texture_t)*MAX_CACHED_TEXTURES );
	
	return FALSE;
}

bool TC_Shutdown(void)
{
	c_num_textures=0;
	FreeAllTextures();
		
	return FALSE;
}

static int FindFreeTexture(void)
{
	if( c_num_textures >= MAX_CACHED_TEXTURES ) {		/* You poor bastard */
		Sys_printf("TexCache: FindFreeTexture(): MAX_CACHED_TEXTURES!\n");
	} else {
		int i;
		TC_Texture_t *WorkPtr;
		i = 0;
		WorkPtr = c_textures;
		do {
			if (!WorkPtr->used) {
				return i;
			}
			++WorkPtr;
		} while (++i<MAX_CACHED_TEXTURES);
	}
	return BAD_TC_HANDLE;
}

static void FreeAllTextures(void)
{
	int i;
	TC_Texture_t *WorkPtr;
	WorkPtr = c_textures;
	i = MAX_CACHED_TEXTURES;
	do {
		if (WorkPtr->used) {		/* Is this used? */
			FreeTexture(WorkPtr);	/* Zap this texture */
		}
		++WorkPtr;					/* Continue traversal */
	} while (--i);
	c_num_textures = 0;
}

static Byte* PalettizeData( const Byte* rgb, LongWord num_pixels, int index_components )
{
	LongWord i;
	Byte* indices, *out;
	Byte* pal = R_glGetColorTable();

	OS_ASSERT( pal );	
	OS_ASSERT( num_pixels > 0 );
	OS_ASSERT( index_components == GL_COLOR_INDEX || index_components == GL_LUMINANCE || index_components == GL_ALPHA );
	
	indices = out = (Byte*)OS_AllocAPointer( num_pixels );

	if( index_components == GL_COLOR_INDEX )
	{
		/*for(i = 0; i < num_pixels; i++)
		{
			*out = LOOKUP24BITCOLORINIMAP( c_texture_imap, rgb[0], rgb[1], rgb[2] );
						
			out++;
			rgb += 3;
		}*/
	}
	else
	{
		for(i = 0; i < num_pixels; i++)
		{
			*out = rgb[0];
			out++;
			rgb += 3;
		}
	}
		
	return indices;
}

static bool WriteMips( const char* filename, TC_Texture_t* tctex )
{
	if( tctex->mips == 0 )
	{
		if( tctex->data == 0 )
			return FALSE;
		
		Image_t img;
		char buff[1024];
		char buff2[1024];

		img.ImagePtr = (Byte*)tctex->data;
		
		if( tctex->bpp == 1 )
			img.PalettePtr = R_glGetColorTable();
		else
			img.PalettePtr = 0;
			
		img.AlphaPtr = 0;
		img.Width = tctex->width;
		img.Height = tctex->height;
		img.RowBytes = tctex->width*tctex->bpp;
		img.DataType = (tctex->bpp==4)?IMAGE8888:(tctex->bpp==3)?IMAGE888:IMAGE8_PAL;
		
		sprintf(buff, "9:base:temp:miptest%s.tga", filename );
		
		FS_MakeBurgerPath( buff, buff2 );
		CreateDirectoryPath2( buff2 );
		Image2TGAFile( &img, buff2, TRUE );

		return FALSE;	
	}
			
	int i;
	int w, h;
	Image_t img;
	char buff[1024];
	char buff2[1024];

	w = tctex->width;
	h = tctex->height;
	
	for(i=0;i < MAX_MIPMAP_LEVELS;i++)
	{
		if( !tctex->mips[i] )
			break;
			
		img.ImagePtr = (Byte*)tctex->mips[i];
		
		if( tctex->bpp == 1 )
			img.PalettePtr = R_glGetColorTable();
		else
			img.PalettePtr = 0;
			
		img.AlphaPtr = 0;
		img.Width = w;
		img.Height = h;
		img.RowBytes = w*tctex->bpp;
		img.DataType = (tctex->bpp==4)?IMAGE8888:(tctex->bpp==3)?IMAGE888:IMAGE8_PAL;
		
		if( i == 0 )
			sprintf(buff, "9:base:temp:miptest%s.tga", filename );
		else
			sprintf(buff, "9:base:temp:miptest%s_mip%i.tga", filename, i-1 );
		
		FS_MakeBurgerPath( buff, buff2 );
		CreateDirectoryPath2( buff2 );
		Image2TGAFile( &img, buff2, TRUE );
	
		w >>= 1;
		h >>= 1;
		
		if( w < 1 || h < 1 )
			break;
	}
	
	return FALSE;
}

static Byte* ExpandData( Byte* indices, LongWord num_pixels, int index_components )
{
	LongWord i;
	LongWord val;
	Byte* rgb, *out;
	Byte* pal = R_glGetColorTable();
	
	OS_ASSERT( pal );
	OS_ASSERT( num_pixels > 0 );
	OS_ASSERT( index_components == GL_COLOR_INDEX || index_components == GL_LUMINANCE || index_components == GL_ALPHA );
	
	rgb = out = (Byte*)OS_AllocAPointer( num_pixels*3 );
	
	if( index_components == GL_COLOR_INDEX )
	{
		for(i = 0; i < num_pixels; i++)
		{
			val = indices[0];
			val = val+val+val;
			
			out[0] = pal[val+0];
			out[1] = pal[val+1];
			out[2] = pal[val+2];
			
			indices++;
			out+=3;
		}
	}
	else
	{
		for(i = 0; i < num_pixels; i++)
		{
			val = indices[0];
					
			out[0] = (Byte)val;
			out[1] = (Byte)val;
			out[2] = (Byte)val;
			
			indices++;
			out+=3;
		}
	}
		
	return rgb;
}

static Byte* MipDown( Byte* rgb, int width, int height, int bpp )
{
	int w = width>>1;
	int h = height>>1;
	
	OS_ASSERT( w >= 1 && h >= 1 );
	
	Byte* mip, *out;
	Byte* yofs, *xofs;
	int accr, accb, accg, acca;
	int x, y;
	int linestride = width*bpp;
	
	acca = 0;
	
	mip = out = (Byte*)OS_AllocAPointer(w*h*bpp);
	yofs = xofs = rgb;
	for(y = 0; y < h; y++)
	{
		for(x = 0; x < w; x++)
		{			
			//
			// sample 1.
			//			
			accr = xofs[0];
			accg = xofs[1];
			accb = xofs[2];
			
			if( bpp == 4 )
				acca = xofs[3];
			
			//
			// sample 2.
			//
			accr += xofs[bpp+0];
			accg += xofs[bpp+1];
			accb += xofs[bpp+2];
			
			if( bpp == 4 )
				acca += xofs[bpp+3];
				
			//
			// sample 3.
			//
			Byte* ld = xofs+linestride;
			
			accr += ld[0];
			accg += ld[1];
			accb += ld[2];
			
			if( bpp == 4 )
				acca += ld[3];
				
			//
			// sample 4.
			//
			accr += ld[bpp+0];
			accg += ld[bpp+1];
			accb += ld[bpp+2];
			
			if( bpp == 4 )
				acca += ld[bpp+3];
				
			accr >>= 2;
			accg >>= 2;
			accb >>= 2;
			acca >>= 2;
			
			out[0] = (Byte)accr;
			out[1] = (Byte)accg;
			out[2] = (Byte)accb;
			
			if( bpp == 4 )
				out[3] = (Byte)acca;
			
			xofs += bpp<<1;
			out += bpp;
		}
		
		yofs+=linestride<<1;
		xofs = yofs;
	}
	
	return mip;
}

static bool MipMapTexture( TC_Texture_t* tc, int index_components )
{
	OS_ASSERT( tc );
	OS_ASSERT( tc->mips == 0 );
	OS_ASSERT( tc->data );
	
	//
	// are we 8bit?
	//
	bool _8bit = tc->bpp==1?TRUE:FALSE;
	
	if( _8bit )
	{
		Byte* data = ExpandData( (Byte*)tc->data, tc->width*tc->height, index_components );
		
		OS_DeallocAPointer( tc->data );
		tc->data = data;
		tc->bpp = 3;
		tc->components = GL_RGB;
		tc->type = GL_UNSIGNED_BYTE;
	}
	
	//
	// mipdown.
	//
	int i;
	int w = tc->width>>1;
	int h = tc->height>>1;
	
	memset( tc->mip_data, 0, sizeof(void*)*MAX_MIPMAP_LEVELS );
	
	tc->mip_data[0] = tc->data;
	
	for( i = 1; i < MAX_MIPMAP_LEVELS; i++ )
	{
		if( w < 1 || h < 1 )
			break;
			
		tc->mip_data[i] = MipDown( (Byte*)tc->mip_data[i-1], w<<1, h<<1, tc->bpp );
		
		w>>=1;
		h>>=1;
	}
	
	tc->mips = tc->mip_data;
	
	//
	// we may have to palettize it back down.
	//
	if( _8bit )
	{
		if(PalettizeTexture( tc, index_components ))
			return TRUE;
	}
	
	return FALSE;
}

static bool PalettizeTexture( TC_Texture_t* tc, int index_components )
{	
	OS_ASSERT( tc->data );
	OS_ASSERT( tc->bpp > 1 );
	
	if( tc->mips )
	{
		int i;
		int w = tc->width;
		int h = tc->height;
		
		for(i = 0; i < MAX_MIPMAP_LEVELS; i++)
		{
			if( !tc->mip_data[i] )
				break;
						
			Byte* data = PalettizeData( (Byte*)tc->mip_data[i], w*h, index_components );
			
			OS_DeallocAPointer( tc->mip_data[i] );
			tc->mip_data[i] = data;
			
			w>>=1;
			h>>=1;
		}
		
		tc->data = tc->mip_data[0];
	}
	else
	{	
		Byte* data = PalettizeData( (Byte*)tc->data, tc->width*tc->height, index_components );
		
		OS_DeallocAPointer( tc->data );
		tc->data = data;
	}
	
	tc->bpp = 1;
	tc->components = index_components;
	tc->type = GL_UNSIGNED_BYTE;
	
	return FALSE;
}

static bool ExpandTexture( TC_Texture_t* tctex, int index_components )
{
	OS_ASSERT( tctex->bpp == 1 );
	
	if( tctex->mips )
	{
		Byte* data;
		int i;
		int w, h;
		
		w = tctex->width;
		h = tctex->height;
		
		for(i=0;;i++)
		{
			data = ExpandData( (Byte*)tctex->mips[i], w*h, index_components );
			
			if( !data )
				return TRUE;
				
			OS_DeallocAPointer( tctex->mips[i] );
			tctex->mips[i] = data;
			if(i == 0 )
				tctex->data = data;
				
			w >>= 1;
			h >>= 1;
			
			if( w < 1 || h < 1 )
				break;
		}
	
	}
	else
	{
		Byte* data = ExpandData( (Byte*)tctex->data, tctex->width*tctex->height, index_components );
		
		if( !data )
			return TRUE;
			
		OS_DeallocAPointer( tctex->data );
		tctex->data = data;
	}
	
	tctex->bpp = 3;
	tctex->components = GL_RGB;
	tctex->type = GL_UNSIGNED_BYTE;
	
	return FALSE;
}

static bool AutoConvertBump( const char* filename, Image_t* tex, bool invert_alpha )
{
	//
	// try load the alpha...
	//
	char buff[1024];
	Image_t* alpha = 0;
	Byte* expanded;
	int ofs, len;

	strcpy( buff, filename );
	StrSetFileExtension( buff, 0 );
	strcat( buff, "_alpha" );
	
	if( FS_ImageExists( buff, buff ) )
	{
		alpha = FS_LoadImage( buff );
		if( alpha )
		{
			if( alpha->Width != tex->Width || alpha->Height != tex->Height )
			{
				Image_t a, b;

				memset( &a, 0, sizeof(Image_t) );

				a.Width = alpha->Width;
				a.Height = alpha->Height;
				a.DataType = IMAGE888;
				a.RowBytes = a.Width*3;

				a.ImagePtr = ExpandData( alpha->ImagePtr, alpha->Width*alpha->Height, GL_ALPHA );
				
				ImageInit( &b, tex->Width, tex->Height, IMAGE888 );
				ImageStore888( &b, &a ); // Scale A to match B.
				ImageDestroy( &a );
								
				OS_DeallocAPointer( alpha->ImagePtr );
				alpha->Width = b.Width;
				alpha->Height = b.Height;
				alpha->RowBytes = b.Width;
				alpha->ImagePtr = PalettizeData( b.ImagePtr, b.Width*b.Height, GL_ALPHA );
				ImageDestroy( &b );
			}

			//
			// this is *extremely* funny logic, but it is here for a reason.
			// images painted in photoshop that are indexed, appear to have a reversed
			// palette so white becomes color 0, not color 255. we normally invert the image
			// to correct for this. however, if the user wishes it to be inverted, then we 
			// leave it alone.
			//
			if( invert_alpha == FALSE )
				InvertAlpha( alpha, GL_ALPHA );
		}
	}
	
	if( !alpha )
		return FALSE;

	len = tex->Width*tex->Height;

	if( tex->DataType == IMAGE8_PAL )
	{
		expanded = ExpandData( tex->ImagePtr, tex->Height*tex->RowBytes, GL_COLOR_INDEX );
		if( !expanded )
		{
			Sys_printf("ERROR: AutoConvertBump: ExpandData() failed on texture '%s'!\n", filename);
			return TRUE;
		}

		OS_DeallocAPointer( tex->ImagePtr );
		if( tex->PalettePtr )
			OS_DeallocAPointer( tex->PalettePtr );

		tex->ImagePtr = 0;
		tex->PalettePtr = 0;
	}
	else
	{
		expanded = tex->ImagePtr;
	}

	{
		Byte* rgba = (Byte*)OS_AllocAPointer( len*4 );

		for( ofs = 0; ofs < len; ofs++ )
		{
			rgba[(ofs<<2)] = expanded[(ofs+ofs+ofs)];
			rgba[(ofs<<2)+1] = expanded[(ofs+ofs+ofs)+1];
			rgba[(ofs<<2)+2] = expanded[(ofs+ofs+ofs)+2];
			rgba[(ofs<<2)+3] = alpha->ImagePtr[ofs];
		}

		OS_DeallocAPointer( expanded );
		tex->ImagePtr = rgba;
		tex->DataType = IMAGE8888;
		tex->RowBytes = tex->Width*4;
	}
	
	// get rid of the alpha.
	{
		char native[1024];
		char file[1024];
		
		FS_GetFullPath( buff, file, FS_GetFileLoc( buff ) );
		ExpandAPathToBufferNative( native, file );
		SetFileAttributes( native, FILE_ATTRIBUTE_NORMAL ); // might be read only.
		DeleteFile( native );
	}

	//
	// Save the new file.
	//
	{
		char native[1024];
		char file[1024];
		
		FS_GetFullPath( filename, file, FS_GetFileLoc( filename ) );
		ExpandAPathToBufferNative( native, file );

		DWORD attribs = GetFileAttributes( native );
		SetFileAttributes( native, FILE_ATTRIBUTE_NORMAL );
		Image2TGAFile( tex, file, FALSE );
		SetFileAttributes( native, attribs ); // restore attributes.
	}

	Sys_printf("NOTE: '%s' was automatically converted to a 32 bit normal map!\n", filename );
	ImageDelete( alpha );
	
	return FALSE;
}

//
// if the texture cannot be found, then this will try to load "missing_texture.jpg"
//
static bool LoadBaseTexture( const char* filename, TC_Texture_t* tctex, int index_components, bool flip_horz, bool flip_vert, bool invert )
{
	Image_t* tex;
	
	tex = FS_LoadImage( filename );
	if( !tex )
	{
		Sys_printf("ERROR: TexCache: unable to load '%s'\n", filename);
		
		tex = FS_LoadImage("/textures/missing_texture.jpg");
		if( !tex )
		{
			return TRUE;
		}
	}
	
	if( tex->DataType != IMAGE888 &&
		tex->DataType != IMAGE8888 &&
		tex->DataType != IMAGE8_PAL )
	{
		Sys_printf("ERROR: TexCache: unrecognized color depth '%s'\n", filename );
		ImageDelete( tex );
		return TRUE;
	}
	
	if( flip_horz )
		ImageHorizontalFlip( tex );
	if( flip_vert )
		ImageVerticalFlip( tex );
	
	//
	// If this is a palettized bump, then convert it automatically.
	//
	if( index_components == GL_COLOR_INDEX &&
		(tex->DataType == IMAGE8_PAL || tex->DataType == IMAGE888) )
	{
		AutoConvertBump( filename, tex, invert );
	}

	//
	// this is *extremely* funny logic, but it is here for a reason.
	// images painted in photoshop that are indexed, appear to have a reversed
	// palette so white becomes color 0, not color 255. we normally invert the image
	// to correct for this. however, if the user wishes it to be inverted, then we 
	// leave it alone.
	//
	if( invert == FALSE )
		InvertAlpha( tex, index_components );
	
	tctex->original_width = tctex->width = tex->Width;
	tctex->original_height = tctex->height = tex->Height;
	tctex->data = tex->ImagePtr;
		
	if( tex->PalettePtr )
		OS_DeallocAPointer( tex->PalettePtr );
	
	tctex->type = GL_UNSIGNED_BYTE;
	tctex->components =  ( tex->DataType == IMAGE888 ) ? GL_RGB : (tex->DataType == IMAGE8888) ? GL_RGBA : index_components;
	tctex->bpp = (tex->DataType == IMAGE888) ? 3 : (tex->DataType == IMAGE8888) ? 4 : (tex->DataType == IMAGE8_PAL) ? 1 : 0;
	
	OS_DeallocAPointer( tex );
	
	//
	// if this is a GL_COLOR_INDEX texture (i.e. normal map) and we don't support paletted textures (i.e. Radeon 9700)
	// then expand this texture.
	//
	if( R_glImpSupportsEXTPalettedTexture() == FALSE )
	{
		if( tctex->components == GL_COLOR_INDEX )
		{
			ExpandTexture( tctex, GL_COLOR_INDEX );
		}
	}
	
	//
	// need resize?
	//
	int max_size = R_glMaxTextureSize();
	if( tctex->width > max_size || tctex->height > max_size )
	{
		//Sys_printf("TexCache: LoadBaseTexture: resizing texture due to hardware constraints...\n");
		
		bool _8bit = (tctex->bpp==1)?TRUE:FALSE;
		
		if( _8bit )
		{
			if( ExpandTexture( tctex, index_components ) )
			{
				Sys_printf("ERROR: TexCache: LoadBaseTexture: ExpandTexture() failed!\n" );
				return TRUE;
			}
		}
		
		R_glResizeTextureForCard( &tctex->width, &tctex->height, &tctex->type, &tctex->components, &tctex->data );
		
		if( _8bit )
		{
			if( PalettizeTexture( tctex, index_components ) )
			{
				Sys_printf("ERROR: TexCache: LoadBaseTexture: PalettizeTexture() failed!\n" );
				return TRUE;
			}
		}
	}
	
	return FALSE;
}

//
// invert the alpha channel.
//
static void InvertAlpha( Image_t* img, int index_components )
{
	int k;	
	int len;
	int bpp;
	int w, h;
	Byte* pixels;

	w = img->Width;
	h = img->Height;
	
	bpp = (img->DataType == IMAGE888) ? 3 : (img->DataType == IMAGE8888) ? 4 : (img->DataType == IMAGE8_PAL) ? 1 : 0;
	
	if( bpp != 1 )
		return;
	if( index_components != GL_ALPHA )
		return;

	//return;
			
	pixels = img->ImagePtr;
	len = w*h;
	
	for(k = 0; k < len; k++)
	{
		if( bpp == 4 )
		{
			pixels[3] = 255-pixels[3];
		}
		else
		{
			pixels[0] = 255-pixels[0];
		}
		
		pixels += bpp;
	}
}

//
// the mipping works like this:
//
// the /base/mipmaps folder contains mipmaps for all images that pass through here.
//
// an image in the /pics/ folder will be in the /mipmaps/pics folder, etc, etc.
//
// the naming works like the following:
//
// for texture /textures/image1.tga, we look for,
// /mipmaps/textures/image1_mip0.tga - image1_mip2.tga
//
// if the mipmaps are not present, then they will be generated before the function returns.
//
//
static bool LoadMipTextures( const char* inputfile, TC_Texture_t* tctex, int index_components, bool flip_horz, bool flip_vert, bool invert )
{
	int i;
	int w, h;
	int mipcount;
	char buff[1024];
	char filename[1024];
	bool sizeok=FALSE;
	
	strcpy( filename, inputfile );
	StrSetFileExtension( filename, 0 );
	
	//
	// if mips are already loaded for this, then don't do anything.
	//
	if( tctex->mips )
		return FALSE;
	
	//
	// load the base texture.
	//
	if( tctex->data == 0 ) // base image not loaded yet...
	{
		if( LoadBaseTexture( inputfile, tctex, index_components, flip_horz, flip_vert, invert ) )
			return TRUE;
			
		OS_ASSERT( tctex->data );
	}
	
	//
	// how many mips do we need?
	//
	w = tctex->width;
	h = tctex->height;
	
	bool _1d = h==1;
	
	for(i=0;;i++)
	{
		w >>= 1;
		h >>= 1;
		
		if( w < 1 || (h < 1 && !_1d) )
			break;	
	}
	
	mipcount = i;
	if( mipcount > MAX_MIPMAP_LEVELS-1 )
		mipcount = MAX_MIPMAP_LEVELS-1;
	
	//
	// scan for presence of _mip0-_mip2, if they are all indeed necessary.
	//
	for(i = 0; i < mipcount; i++)
	{
		sprintf( buff, "/mipmaps%s_mip%d", filename, i );
		if( !FS_ImageExists( buff ) )
			break;
	}
	
	sizeok = (tctex->width==tctex->original_width)&&(tctex->height==tctex->original_height);
	
	if( i == mipcount && sizeok && mipcount > 0 )
	{
		Image_t* img;
		int bpp;
		
		//
		// load the files.
		//
		w = tctex->width>>1;
		
		if( _1d )
			h = 1;
		else
			h = tctex->height>>1;
		
		tctex->mips = 0;
		memset( tctex->mip_data, 0, sizeof(void*)*MAX_MIPMAP_LEVELS );
		
		tctex->mip_data[0] = tctex->data;
		
		for(i = 0; i < mipcount; i++)
		{
			sprintf( buff, "/mipmaps%s_mip%d", filename, i );
			
			if( !FS_ImageExists( buff, buff ) )
			{
				Sys_printf("ERROR: TexCache: LoadMipTextures: FS_ImageExists() failed, but it worked a second ago. This should never happen!\n" );
				break;
			}
			
			img = FS_LoadImage( buff );
			
			if( !img )
			{
				Sys_printf("ERROR: TexCache: LoadMipTextures: unable to load image '%s'\n", filename );
				break;
			}
			
			//
			// this had better be the same bitdepth as the original...
			//
			bool expand = FALSE;
			
			bpp = (img->DataType==IMAGE8888)?4:(img->DataType==IMAGE888)?3:1;
			
			if( R_glImpSupportsEXTPalettedTexture() == FALSE )
			{
				if( bpp == 1 )
				{
					if( index_components == GL_COLOR_INDEX )
					{
						bpp = 3; // we will expand.
						expand = TRUE;
					}
				}
			}
						
			if( bpp != tctex->bpp )
			{
				Sys_printf("ERROR: TexCache: LoadMipTextures: the bitdepth for mipmap image '%s' does not match the source image bitdepth!\n", buff );
				ImageDelete( img );
				break;
			}
			
			//
			// is it the right size?
			//
			if( w != img->Width || h != img->Height )
			{
				Sys_printf("ERROR: TexCache: LoadMipTextures: the size of mipmap image '%s' is not correct, it should be %dx%d, but is %dx%d\n", buff, w, h, img->Width, img->Height );
				ImageDelete( img );
				break;
			}
			
			if( flip_horz )
				ImageHorizontalFlip( img );
			if( flip_vert )
				ImageVerticalFlip( img );
			
			//
			// this is *extremely* funny logic, but it is here for a reason.
			// images painted in photoshop that are indexed, appear to have a reversed
			// palette so white becomes color 0, not color 255. we normally invert the image
			// to correct for this. however, if the user wishes it to be inverted, then we 
			// leave it alone.
			//
			if( invert == FALSE )
				InvertAlpha( img, index_components );
			
			//
			// i can accept this image.
			//
			if( img->PalettePtr )
			{
				OS_DeallocAPointer( img->PalettePtr );
				img->PalettePtr = 0;
			}
			
			if( expand )
			{
				Byte* data = ExpandData( img->ImagePtr, w*h, GL_COLOR_INDEX );
				if( !data )
				{
					Sys_printf("ERROR: TexCache: LoadMipTexture: failed expanding paletted texture '%s'\n", buff );
					ImageDelete( img );
					break;							
				}
				
				OS_DeallocAPointer( img->ImagePtr );
				img->ImagePtr = data;
			}
			
			tctex->mip_data[i+1] = img->ImagePtr;
			
			OS_DeallocAPointer( img );
			
			w >>= 1;
			
			if( !_1d )
				h >>= 1;
			
			//Sys_printf("TexCache: loaded '%s'\n", buff );
		}
		
		//
		// did it complete?
		//
		if( i == mipcount )
		{
			tctex->mips = tctex->mip_data;
			return FALSE;
		}
		
		//
		// free any used ones.
		//
		for(i = 1; i < MAX_MIPMAP_LEVELS; i++)
		{
			if( tctex->mip_data[i] != 0 )
				OS_DeallocAPointer( tctex->mip_data[i] );
				
			tctex->mip_data[i] = 0;
		}
	}
	
	//
	// we can't mipmap a _1d texture.
	//
	if( _1d )
		return FALSE;
		
	//
	// we have to generate the mipmap images...
	//
	//Sys_printf("TexCache: LoadMipTextures: generating mipmap images for '%s'\n", filename );
	
	if(MipMapTexture( tctex, index_components ))
		return TRUE;

	return FALSE;
}

void TC_CacheTexture( int* user_handle, int* user_key, const char* filename, int index_components, bool mipmap, bool flip_horz, bool flip_vert, bool invert )
{
	//
	// see if it's cached.
	//
	int i=MAX_CACHED_TEXTURES;
	int handle;
	TC_Texture_t* tctex;
	
	*user_handle = BAD_TC_HANDLE;
	*user_key = BAD_TC_HANDLE;
	
	CHKSTRLEN( filename, MAX_TCTEXTURE_NAME_LEN, "texture-cache image filename" );

	if( c_num_textures > 0 )
	{
		for(i = 0; i < MAX_CACHED_TEXTURES; i++)
		{
			tctex = &c_textures[i];
			
			if( tctex->used )
			{
				if(!stricmp(tctex->name, filename))
				{
					if( tctex->loaded )
					{
						if( mipmap && tctex->mips == 0 && tctex->height > 1 )
						{
							Sys_printf("ERROR: TexCache: mips have not been loaded for this image... doing so now...\n");
							if( LoadMipTextures( filename, tctex, index_components, FALSE, FALSE, FALSE ) )
							{
								Sys_printf("ERROR: TexCache: LoadMipTextures() failed!\n");
								return;
							}							
						}
						
						*user_handle = i;
						*user_key = TC_LockTexture( i );
						return;					
					}
					
					handle = i;
					break;
				}
			}
		}
	}
	
	if( i >= MAX_CACHED_TEXTURES )
		handle = FindFreeTexture();
	
	if( handle != BAD_TC_HANDLE )
	{
		tctex = &c_textures[handle];
		memset( tctex, 0, sizeof(TC_Texture_t) );
			
		if( mipmap )
		{
			if( LoadMipTextures( filename, tctex, index_components, flip_horz, flip_vert, invert ) )
			{
				Sys_printf("TexCache: LoadMipTextures() failed!\n");
				return;
			}
		}
		else
		{
			if( LoadBaseTexture( filename, tctex, index_components, flip_horz, flip_vert, invert ) )
			{
				Sys_printf("TexCache: LoadBaseTexture() failed!\n");
				return;
			}
		}
		
		c_num_textures++;
		tctex->loaded = TRUE;
		tctex->used = TRUE;
		strcpy( tctex->name, filename );
		
		*user_handle = handle;
		*user_key = TC_LockTexture( handle );
	}
	else
		OS_BreakMsg("ERROR: TexCache: out of texture handles: '%s'\n", filename );
}

bool TC_TexUpload(  int handle, int flags, RTex_t* tex, bool unlock )
{
	TC_Texture_t* tc = TC_GetTexture( handle );
		
	if( !tc )
		return TRUE;
	
	OS_ASSERT( tex->cache.handle == handle );
	
	R_glUploadTexture( tex, tc->width, tc->height, 1, tc->type, tc->components, flags, 
		tc->data, tc->mips );
	
	if( unlock )
	{
		TC_UnlockTexture( handle, tex->cache.key );
	}
	
	return FALSE;
}

bool TC_TexRebind(  int handle, int key, RTex_t* tex )
{
	return FALSE;
}

bool TC_TexPurge(  int handle, RTex_t* tex )
{
	return FALSE;
}

TC_Texture_t* TC_GetTexture( int handle )
{
	OS_ASSERT( handle >= 0 && handle < MAX_CACHED_TEXTURES );
	
	TC_Texture_t* tex = &c_textures[handle];
	return (tex->used) ? tex : 0;
}

void TC_DeallocRTex( int handle, RTex_t* rtex )
{
	TC_Texture_t* tex = TC_GetTexture( handle );

	if( !tex )
		return;
	
	OS_ASSERT( rtex );
	
	TC_GLock_t* g_lock;
	for( g_lock = tex->g_locks; g_lock; g_lock = g_lock->next )
	{
		if( g_lock->rtex == rtex )
			break;
	}
	
	if( !g_lock )
	{
		//OS_BreakMsg("TC_DeallocRTex: rtex not found!");
		return;
	}
	
	if( g_lock->next )
		g_lock->next->prev = g_lock->prev;
	if( g_lock->prev )
		g_lock->prev->next = g_lock->next;
	if( tex->g_locks == g_lock )
		tex->g_locks = g_lock->next;
	
	R_glDeleteTexture( g_lock->rtex );
	OS_DeallocAPointer( g_lock );
}

RTex_t* TC_AllocRTex( int handle, int key, int type, char* name, RTexTouchProc load_proc, RTexTouchProc rebind_proc, RTexTouchProc purge_proc )
{
	TC_Texture_t* tex = TC_GetTexture( handle );

	if( !tex )
		return 0;
	
	TC_DLock_t* d_lock;
	for( d_lock = tex->d_locks; d_lock; d_lock = d_lock->next )
	{
		if( d_lock->key == key )
			break;
	}
	
	if( !d_lock )
	{
		OS_BreakMsg("TC_AllocRTex: attempted to rtex an unlocked texture!");
		return 0;
	}
	
	TC_GLock_t* g_lock;
	if( type != TCRTEX_UNIQUE )
	{
		for( g_lock = tex->g_locks; g_lock; g_lock = g_lock->next )
		{
			if( g_lock->type == (Byte)type )
				return g_lock->rtex;
		}
	}
		
	if( !name )
		name = "tctex";
		
	//
	// make the RTex wrap this texture.
	//
	RTex_t* rt = R_glCreateTexture( name, tex->width, tex->height, 1, tex->bpp, load_proc, rebind_proc, purge_proc );
	rt->cache.handle = handle;
	rt->cache.key = key;
	
	g_lock = (TC_GLock_t*)OS_AllocAPointer( sizeof(TC_GLock_t) );
	g_lock->rtex = rt;
	g_lock->prev = 0;
	g_lock->type = (Byte)type;
	g_lock->next = tex->g_locks;
	
	if( tex->g_locks )
		tex->g_locks->prev = g_lock;
	tex->g_locks = g_lock;
		
	return rt;
}

int TC_LockTexture( int handle )
{
	TC_Texture_t* tex = TC_GetTexture(handle);
	if( !tex )
		return BAD_TC_HANDLE;
			
	if( !tex->loaded )
	{
		OS_BreakMsg("TC_LockTexture: tried locking an unloaded texture!\n");
		return BAD_TC_HANDLE;
	}
	
	int key = tex->d_key++;
	TC_DLock_t* lock;
	
	lock = (TC_DLock_t*)OS_AllocAPointer( sizeof(TC_DLock_t) );
	lock->key = key;
	
	lock->prev = 0;
	lock->next = tex->d_locks;
	if( tex->d_locks )
		tex->d_locks->prev = lock;
		
	tex->d_locks = lock;
	
	return key;
}

void TC_UnlockTexture( int handle, int key )
{
	TC_Texture_t* tex = TC_GetTexture( handle );
	if( !tex )
		return;
	
	TC_DLock_t* lock;
	
	for( lock = tex->d_locks; lock; lock = lock->next )
	{
		if( lock->key == key )
			break;
	}
	
	//
	// it's not an error to unlock a texture multiple times.
	//
	if( !lock )
		return;
	
	if( lock->next )
		lock->next->prev = lock->prev;
	if( lock->prev )
		lock->prev->next = lock->next;
		
	if( lock == tex->d_locks )
		tex->d_locks = lock->next;
		
	OS_DeallocAPointer( lock );
	
	//
	// has everyone stopped using this?
	//
	if( tex->d_locks == 0 )
	{
		if( tex->mips )
		{
			int i;
			for(i = 0; i < MAX_MIPMAP_LEVELS; i++)
			{
				if( tex->mips[i] )
					OS_DeallocAPointer( tex->mips[i] );
					
				tex->mips[i] = 0;
			}
			
			tex->data = 0;
			tex->mips = 0;
		}
		else
		if( tex->data )
		{
			//
			// free this one, no ones using it.
			//
		
			OS_DeallocAPointer( tex->data );
			tex->data = 0;
		}
		
		tex->loaded = FALSE;
	}
}

void TC_FreeUnlockedTextures()
{
	int i;
	
	for(i = 0; i<MAX_CACHED_TEXTURES; i++)
	{
		if( c_textures[i].used && c_textures[i].d_locks == 0 )
		{
			FreeTexture( &c_textures[i] );
			c_num_textures--;
		}
	}
}

int TC_GetNumLockedTextures( void )
{
	int i;
	int c=0;
	
	for(i = 0; i<MAX_CACHED_TEXTURES; i++)
	{
		TC_Texture_t* tex = &c_textures[i];
		
		if( tex->used && tex->d_locks )
		{
			c++;		
		}
	}
	
	return c;
}

int TC_GetNumLoadedTextures( void )
{
	int i;
	int c=0;
	
	for(i = 0; i<MAX_CACHED_TEXTURES; i++)
	{
		TC_Texture_t* tex = &c_textures[i];
		
		if( tex->used && tex->loaded )
		{
			
			c++;		
		}
	}
	
	return c;
}

static void FreeTexture( TC_Texture_t* tex )
{
	int i;
			
	if( tex->mips )
	{
		for(i = 0; i < MAX_MIPMAP_LEVELS; i++)
		{
			if( tex->mips[i] )
				OS_DeallocAPointer( tex->mips[i] );
				
			tex->mips[i] = 0;
		}
		
		tex->data = 0;
		tex->mips = 0;
	}
	else
	if( tex->data )
	{
		//
		// free this one, no ones using it.
		//
	
		OS_DeallocAPointer( tex->data );
		tex->data = 0;
	}
	
	TC_DLock_t* d_lock, *nd;
	TC_GLock_t* g_lock, *ng;
	
	for( d_lock = tex->d_locks; d_lock; d_lock = nd )
	{
		nd = d_lock->next;
		OS_DeallocAPointer( d_lock );
	}
	tex->d_locks = 0;
	
	for( g_lock = tex->g_locks; g_lock; g_lock = ng )
	{
		ng = g_lock->next;
		if( g_lock->rtex )
		{
			R_glDeleteTexture( g_lock->rtex );
		}
		OS_DeallocAPointer( g_lock );
	}
	tex->g_locks = 0;
	
	Sys_printf("TexCache: purged '%s'\n", tex->name);
	
	tex->name[0] = 0;
	tex->used = FALSE;
	tex->loaded = FALSE;
}

bool TC_ReloadTexture( int handle, int index_components, bool mipmap, bool flip_horz, bool flip_vert, bool invert )
{
	TC_Texture_t* tex = TC_GetTexture( handle );
	if( !tex )
		return TRUE;

	int i;
			
	if( tex->mips )
	{
		for(i = 0; i < MAX_MIPMAP_LEVELS; i++)
		{
			if( tex->mips[i] )
				OS_DeallocAPointer( tex->mips[i] );
				
			tex->mips[i] = 0;
		}
		
		tex->data = 0;
		tex->mips = 0;
	}
	else
	if( tex->data )
	{
		//
		// free this one, no ones using it.
		//
	
		OS_DeallocAPointer( tex->data );
		tex->data = 0;
	}

	tex->loaded = FALSE;

	Sys_printf("TexCache: caching '%s'\n", tex->name );

	if( mipmap )
	{
		if( LoadMipTextures( tex->name, tex, index_components, flip_horz, flip_vert, invert ) )
		{
			Sys_printf("TexCache: LoadMipTextures() failed!\n");
			return TRUE;
		}
	}
	else
	{
		if( LoadBaseTexture( tex->name, tex, index_components, flip_horz, flip_vert, invert ) )
		{
			Sys_printf("TexCache: LoadBaseTexture() failed!\n");
			return TRUE;
		}
	}

	return FALSE;
}

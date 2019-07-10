///////////////////////////////////////////////////////////////////////////////
// jpgload.cpp
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

#include "jpeglib.h"
#include <memory.h>
#include "stdafx.h"
#include <stdio.h>
#include "jpeglib.h"
#include "jerror.h"

static const void *in_ptr;
static int in_length;
static jp_bool in_overrun;

static void *out_ptr;
static int out_length;
static int out_total;

static void discard_error (j_common_ptr cinfo) {}
static void discard_emit_message (j_common_ptr cinfo, int msg_level) {}
static void discard_format_message (j_common_ptr cinfo, char * buffer) {}

static void reset_error_mgr (j_common_ptr cinfo)
{
  cinfo->err->num_warnings = 0;
  /* trace_level is not reset since it is an application-supplied parameter */
  cinfo->err->msg_code = 0;	/* may be useful as a flag for "no error" */
}

static const char * const jpeg_std_message_table[] = {
#include "jerror.h"
  0
};

static struct jpeg_error_mgr * jpeg_my_std_error (struct jpeg_error_mgr * err)
{
  err->error_exit = discard_error;
  err->emit_message = discard_emit_message;
  err->output_message = discard_error;
  err->format_message = discard_format_message;
  err->reset_error_mgr = reset_error_mgr;

  err->trace_level = 0;		/* default = no tracing */
  err->num_warnings = 0;	/* no warnings emitted yet */
  err->msg_code = 0;		/* may be useful as a flag for "no error" */

  /* Initialize message table pointers */
  err->jpeg_message_table = jpeg_std_message_table;
  err->last_jpeg_message = (int) JMSG_LASTMSGCODE - 1;

  err->addon_message_table = 0;
  err->first_addon_message = 0;	/* for safety */
  err->last_addon_message = 0;

  return err;
}

static void init_source( j_decompress_ptr cinfo )
{
	cinfo->src->next_input_byte = (const JOCTET *)in_ptr;
	cinfo->src->bytes_in_buffer = in_length;
	in_overrun = FALSE;
}

static jp_bool fill_input_buffer( j_decompress_ptr cinfo )
{
	static JOCTET eoibuf[2];
	
	in_overrun = TRUE;
	eoibuf[0] = 0xff;
	eoibuf[1] = JPEG_EOI;
	cinfo->src->next_input_byte = eoibuf;
	cinfo->src->bytes_in_buffer = 2;
	
	return TRUE;
}


static void skip_input_data( j_decompress_ptr cinfo, long num_bytes )
{
	if (num_bytes >= cinfo->src->bytes_in_buffer) {
		fill_input_buffer( cinfo );
	} else {
		cinfo->src->next_input_byte += num_bytes;
		cinfo->src->bytes_in_buffer -= num_bytes;
	}
}


static void term_source( j_decompress_ptr cinfo )
{
	cinfo = cinfo; // shut up compiler
	// nothing necessary
}


static struct jpeg_source_mgr jSrc = {
	0,	// next_input_byte
	0,		// bytes_in_buffer
	init_source,
	fill_input_buffer,
	skip_input_data,
	jpeg_resync_to_restart,	// use the default
	term_source
};


static void init_destination( j_compress_ptr cinfo )
{
	out_total = 0;
	cinfo->dest->next_output_byte = ((JOCTET *)out_ptr);
	cinfo->dest->free_in_buffer = out_length;
}


static jp_bool empty_output_buffer( j_compress_ptr cinfo )
{
	out_total += out_length;
	cinfo->dest->next_output_byte = ((JOCTET *)out_ptr);
	cinfo->dest->free_in_buffer = out_length;
	return TRUE;
}


static void term_destination( j_compress_ptr cinfo )
{
	out_total += (int)(out_length - cinfo->dest->free_in_buffer);
}


static struct jpeg_destination_mgr jDst = {
	0,	// next_output_byte
	0,		// free_in_buffer
	init_destination,
	empty_output_buffer,
	term_destination
};


jp_bool get_jpeg_dimensions( const void* in, int in_size, int* width, int* height )
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	
	cinfo.err = jpeg_my_std_error( &jerr );
	jpeg_create_decompress( &cinfo );
	
	cinfo.src = &jSrc;
	in_ptr = in;
	in_length = in_size;
	
	if( jpeg_read_header( &cinfo, TRUE ) != JPEG_HEADER_OK )
		return TRUE;
	
	jpeg_destroy_decompress( &cinfo );
	
	*width = cinfo.image_width;
	*height = cinfo.image_height;
	
	return FALSE;
}

jp_bool decompress_jpeg( void **out, const void *in, int in_size, int* width, int* height, bool hq )
{
	int y;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW *rows;
	
	cinfo.err = jpeg_my_std_error( &jerr );
	jpeg_create_decompress( &cinfo );
	
	cinfo.src = &jSrc;
	in_ptr = in;
	in_length = in_size;
	
	if( jpeg_read_header( &cinfo, TRUE ) != JPEG_HEADER_OK )
	{
		jpeg_destroy_decompress( &cinfo );
		return TRUE;
	}

	*width = cinfo.image_width;
	*height = cinfo.image_height;
		
	cinfo.scale_denom = cinfo.image_width / (*width);
	if (hq) {
		cinfo.dct_method = JDCT_IFAST;
		cinfo.do_fancy_upsampling = TRUE;
	} else {
		cinfo.dct_method = JDCT_IFAST;
		cinfo.do_fancy_upsampling = FALSE;
	}
	
	jpeg_start_decompress( &cinfo );
	
	*out = new unsigned char[((*width)*(*height)*3)];
	
	rows = (JSAMPROW*)malloc(sizeof(JSAMPROW)*(*height));
	if(!rows)
	{
		jpeg_finish_decompress( &cinfo );
		jpeg_destroy_decompress( &cinfo );
		delete[] *out;
		*out = 0;
		return TRUE;
	}

	for (y = 0; y < (*height); ++y) {
		rows[y] = ((JSAMPROW)(*out)) + y * 3 * (*width);
	}
	
	while (cinfo.output_scanline < cinfo.output_height) {
		int outlines = jpeg_read_scanlines( &cinfo, rows + cinfo.output_scanline, (*height) - cinfo.output_scanline );
		if (outlines < 1) break;
	}
	
	free(rows);
	
	jpeg_finish_decompress( &cinfo );
	jpeg_destroy_decompress( &cinfo );
	return FALSE;
}

void LoadJPGBuff(const void *fbuffer, int size, void **pic, int *width, int *height, int *depth )
{
	decompress_jpeg( pic, fbuffer, size, width, height, FALSE );
	*depth = 3;
}

void ReadJPGInfo(const void *fbuffer, int size, int *width, int *height, int *depth )
{
	get_jpeg_dimensions( fbuffer, size, width, height );
	*depth = 3;
}
///////////////////////////////////////////////////////////////////////////////
// ByteParser.h
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

#if !defined(BYTEPARSER_H)
     #define BYTEPARSER_H

#include <string.h>

class CByteParser
{
public:

	CByteParser(const void *data, int len) :
	  m_ptr(reinterpret_cast<const unsigned char*>(data)), m_len(len), m_ofs(0)
  {
  }

  void Skip(int num)
  {
	  m_ofs += num;
	  m_ofs = m_ofs > m_len ? m_len : m_ofs;
  }

  void Read(void *dst, int len)
  {
	  len = (len < Left()) ? len : Left();
	  memcpy(dst, m_ptr+m_ofs, len);
	  m_ofs += len;
  }

  int ReadByte()
  {
	  if (Left() < 1) return 0;
	  ++m_ofs;
	  return (int)m_ptr[m_ofs-1];
  }

  int ReadShort()
  {
	  if (Left() < 2) return 0;
	  int x, y;
	  m_ofs += 2;
	  x = m_ptr[m_ofs-2];
	  y = m_ptr[m_ofs-1];
	  return LoadIntelShort(x | (y<<8));
  }

  int ReadInt()
  {
	  if (Left() < 4) return 0;
	  int x, y, z, w;
	  m_ofs += 4;
	  x = m_ptr[m_ofs-4];
	  y = m_ptr[m_ofs-3];
	  z = m_ptr[m_ofs-2];
	  w = m_ptr[m_ofs-1];
	  return LoadIntelLong(x | (y<<8) | (z<<16) | (w<<24));
  }

  int Left() { return m_len - m_ofs; }

private:

	const unsigned char *m_ptr;
	int m_ofs;
	int m_len;
};

#endif
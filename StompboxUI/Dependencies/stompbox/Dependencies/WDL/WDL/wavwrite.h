/*
    WDL - wavwrite.h
    Copyright (C) 2005 and later, Cockos Incorporated

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
    

*/

/*

  This file provides a simple class for writing basic 8, 16 or 24 bit PCM,
  or 32 or 64 bit floating point WAV files.
 
*/


#ifndef _WAVWRITE_H_
#define _WAVWRITE_H_


//#include "wdlendian.h" // include for faster endian conversion

#include <stdio.h>
#include <string.h>
#include "pcmfmtcvt.h"
#include "wdlstring.h"
#include "win32_utf8.h"
#include "wdltypes.h"

#if defined(WAVEWRITER_MAX_NCH) && WAVEWRITER_MAX_NCH > 64
#include "heapbuf.h"
#endif

class WaveWriter
{
  public:
    // appending doesnt check sample types
    WaveWriter(): m_fp(0), m_data_size(0), m_total_size(0), m_bps(0),
    m_nch(0), m_srate(0) {}

    WaveWriter(const char *filename, int bps, int nch, int srate, int allow_append=1) 
    {
      m_bps=m_srate=m_nch=0;
      Open(filename,bps,nch,srate,allow_append);

    }

    int Open(const char *filename, int bps, int nch, int srate, int allow_append=1)
    {
      m_fn.Set(filename);
      m_fp=0;
      m_data_size=m_total_size=0;
      if (allow_append)
      {
        m_fp=fopen(filename,"r+b");
        if (m_fp)
        {
          fseek(m_fp,0,SEEK_END);
          int pos=ftell(m_fp);
          if (pos < 44)
          {
            char buf[44]={0,};
            fwrite(buf,1,44-pos,m_fp);
          }
        }
      }
      if (!m_fp)
      {
        m_fp=fopen(filename,"wb");
        if (!m_fp) return 0;

        char tbuf[44];
        fwrite(tbuf,1,44,m_fp); // room for header
      }
      m_bps=bps;
      m_nch=nch>1?nch:1;
      m_srate=srate;

      return !!m_fp;
    }

    void Close()
    {
      if (m_fp)
      {
        EndDataChunk();
        fseek(m_fp,0,SEEK_SET);

        // write header
        fwrite("RIFF",1,4,m_fp);
        unsigned int riff_size=m_total_size+44-8;
        fputi32(riff_size,m_fp);
        fwrite("WAVEfmt \x10\0\0\0",1,12,m_fp);
        fwrite(m_bps<32?"\1\0":"\3\0",1,2,m_fp); // PCM or float

        fputi16(m_nch,m_fp); // nch
        fputi32(m_srate,m_fp); // srate
        fputi32(m_nch * (m_bps/8) * m_srate,m_fp); // bytes_per_sec
        int blockalign=m_nch * (m_bps/8);
        fputi16(blockalign,m_fp); // block alignment
        fputi16(m_bps&~7,m_fp); // bits/sample
        fwrite("data",1,4,m_fp);
        fputi32(m_data_size,m_fp); // size

        fclose(m_fp);
        m_fp=0;
      }
    }

    ~WaveWriter()
    {
      Close();
    }

    const char *GetFileName() const { return m_fn.Get(); }

    int Status() const { return !!m_fp; }

    unsigned int BytesWritten() const
    {
      return m_total_size ? m_total_size : m_data_size;
    }

    // Length/size in samples.
    unsigned int GetLength() const { return m_data_size / (m_nch * (m_bps/8)); }
    unsigned int GetSize() const { return m_data_size / (m_bps/8); }

    void WriteRaw(void *buf, unsigned int len)
    {
      if (!m_fp) return;

      len = (unsigned int)fwrite(buf,1,len,m_fp);
      if (m_total_size) m_total_size += len; else m_data_size += len;
    }

    void WriteFloats(float *samples, unsigned int nsamples)
    {
      if (!m_fp) return;

      m_data_size += nsamples * (m_bps/8);

      if (m_bps == 8)
      {
        while (nsamples-->0)
        {
          int a;
          float_TO_UINT8(a,*samples);
          fputc(a,m_fp);
          samples++;
        }
      }
      else if (m_bps == 16)
      {
        while (nsamples-->0)
        {
          short a;
          float_TO_INT16(a,*samples);
          fputi16(a,m_fp);
          samples++;
        }
      }
      else if (m_bps == 24)
      {
        while (nsamples-->0)
        {
          unsigned char a[3];
          float_to_i24(samples,a);
          fwrite(a,1,3,m_fp);
          samples++;
        }
      }
      else if (m_bps == 32)
      {
      #ifdef WDL_LITTLE_ENDIAN
        fwrite(samples,1,nsamples*4,m_fp);
      #else
        while (nsamples-->0)
        {
          unsigned int a;
          memcpy(&a,samples,4);
          fputi32(a,m_fp);
          samples++;
        }
      #endif
      }
      else if (m_bps == 64)
      {
        while (nsamples-->0)
        {
          const double x=*samples;
          WDL_UINT64 a;
          memcpy(&a,&x,8);
          fputi64(a,m_fp);
          samples++;
        }
      }
    }

    void WriteDoubles(double *samples, unsigned int nsamples)
    {
      if (!m_fp) return;

      m_data_size += nsamples * (m_bps/8);

      if (m_bps == 8)
      {
        while (nsamples-->0)
        {
          int a;
          double_TO_UINT8(a,*samples);
          fputc(a,m_fp);
          samples++;
        }
      }
      else if (m_bps == 16)
      {
        while (nsamples-->0)
        {
          short a;
          double_TO_INT16(a,*samples);
          fputi16(a,m_fp);
          samples++;
        }
      }
      else if (m_bps == 24)
      {
        while (nsamples-->0)
        {
          unsigned char a[3];
          double_to_i24(samples,a);
          fwrite(a,1,3,m_fp);
          samples++;
        }
      }
      else if (m_bps == 32)
      {
        while (nsamples-->0)
        {
          const float x=(float)*samples;
          unsigned int a;
          memcpy(&a,&x,4);
          fputi32(a,m_fp);
          samples++;
        }
      }
      else if (m_bps == 64)
      {
      #ifdef WDL_LITTLE_ENDIAN
        fwrite(samples,1,nsamples*8,m_fp);
      #else
        while (nsamples-->0)
        {
          WDL_UINT64 a;
          memcpy(&a,samples,8);
          fputi64(a,m_fp);
          samples++;
        }
      #endif
      }
    }

    void WriteFloatsNI(float **samples, unsigned int offs, unsigned int nsamples, int nchsrc=0)
    {
      if (!m_fp) return;

      m_data_size += nsamples * m_nch * (m_bps/8);

      if (nchsrc < 1) nchsrc=m_nch;

      float *stackbuf[STACKBUF_MAX_NCH], **tmpptrs=GetTmpPtrs(stackbuf,samples,offs,nchsrc);
      if (!tmpptrs) return;

      if (m_bps == 8)
      {
        while (nsamples-->0)
        {
          for (int ch = 0; ch < m_nch; ++ch)
          {
            int a;
            float_TO_UINT8(a,tmpptrs[ch][0]);
            fputc(a,m_fp);
            tmpptrs[ch]++;
          }
        }
      }
      else if (m_bps == 16)
      {
        while (nsamples-->0)
        {          
          for (int ch = 0; ch < m_nch; ++ch)
          {
            short a;
            float_TO_INT16(a,tmpptrs[ch][0]);
            fputi16(a,m_fp);
            tmpptrs[ch]++;
          }
        }
      }
      else if (m_bps == 24)
      {
        while (nsamples-->0)
        {
          for (int ch = 0; ch < m_nch; ++ch)
          {
            unsigned char a[3];
            float_to_i24(tmpptrs[ch],a);
            fwrite(a,1,3,m_fp);
            tmpptrs[ch]++;
          }
        }
      }
      else if (m_bps == 32)
      {
        while (nsamples-->0)
        {
          for (int ch = 0; ch < m_nch; ++ch)
          {
            unsigned int a;
            memcpy(&a,tmpptrs[ch],4);
            fputi32(a,m_fp);
            tmpptrs[ch]++;
          }
        }
      }
      else if (m_bps == 64)
      {
        while (nsamples-->0)
        {
          for (int ch = 0; ch < m_nch; ++ch)
          {
            const double x=tmpptrs[ch][0];
            WDL_UINT64 a;
            memcpy(&a,&x,8);
            fputi64(a,m_fp);
            tmpptrs[ch]++;
          }
        }
      }
    }

    void WriteDoublesNI(double **samples, unsigned int offs, unsigned int nsamples, int nchsrc=0)
    {
      if (!m_fp) return;

      m_data_size += nsamples * m_nch * (m_bps/8);

      if (nchsrc < 1) nchsrc=m_nch;

      double *stackbuf[STACKBUF_MAX_NCH], **tmpptrs=GetTmpPtrs(stackbuf,samples,offs,nchsrc);
      if (!tmpptrs) return;

      if (m_bps == 8)
      {
        while (nsamples-->0)
        {
          for (int ch = 0; ch < m_nch; ++ch)
          {
            int a;
            double_TO_UINT8(a,tmpptrs[ch][0]);
            fputc(a,m_fp);
            tmpptrs[ch]++;
          }
        }
      }
      else if (m_bps == 16)
      {
        while (nsamples-->0)
        {          
          for (int ch = 0; ch < m_nch; ++ch)
          {
            short a;
            double_TO_INT16(a,tmpptrs[ch][0]);
            fputi16(a,m_fp);
            tmpptrs[ch]++;
          }
        }
      }
      else if (m_bps == 24)
      {
        while (nsamples-->0)
        {
          for (int ch = 0; ch < m_nch; ++ch)
          {
            unsigned char a[3];
            double_to_i24(tmpptrs[ch],a);
            fwrite(a,1,3,m_fp);
            tmpptrs[ch]++;
          }
        }
      }
      else if (m_bps == 32)
      {
        while (nsamples-->0)
        {
          for (int ch = 0; ch < m_nch; ++ch)
          {
            const float x=(float)tmpptrs[ch][0];
            unsigned int a;
            memcpy(&a,&x,4);
            fputi32(a,m_fp);
            tmpptrs[ch]++;
          }
        }
      }
      else if (m_bps == 64)
      {
        while (nsamples-->0)
        {
          for (int ch = 0; ch < m_nch; ++ch)
          {
            WDL_UINT64 a;
            memcpy(&a,tmpptrs[ch],8);
            fputi64(a,m_fp);
            tmpptrs[ch]++;
          }
        }
      }
    }

    void WriteChunk(void *buf, unsigned int len)
    {
      EndDataChunk();
      if (m_fp) m_total_size += (unsigned int)fwrite(buf,1,len,m_fp);
    }

    void EndDataChunk()
    {
      if (!m_fp || m_total_size) return;

      m_total_size = m_data_size;
      if (m_total_size & 1)
      {
        m_total_size++;
        fputc(m_bps == 8 ? 0x80 : 0, m_fp);
      }
    }

    int get_nch() const { return m_nch; } 
    int get_srate() const { return m_srate; }
    int get_bps() const { return m_bps; }

  private:
    static int fputi16(unsigned short a, FILE *fp)
    {
    #if defined(WDL_LITTLE_ENDIAN) || defined(WDL_BIG_ENDIAN)
      WDL_BSWAP16_IF_BE(a);
      return (int)fwrite(&a,1,2,fp);
    #else
      unsigned char buf[2];
      buf[0]=a&0xff; buf[1]=a>>8;
      return (int)fwrite(buf,1,2,fp);
    #endif
    }

    static int fputi32(unsigned int a, FILE *fp)
    {
    #if defined(WDL_LITTLE_ENDIAN) || defined(WDL_BIG_ENDIAN)
      WDL_BSWAP32_IF_BE(a);
      return (int)fwrite(&a,1,4,fp);
    #else
      unsigned char buf[4], *p = buf;
      for (int x = 0; x < 32; x += 8) *p++=(a>>x)&0xff;
      return (int)fwrite(buf,1,4,fp);
    #endif
    }

    static int fputi64(WDL_UINT64 a, FILE *fp)
    {
    #if defined(WDL_LITTLE_ENDIAN) || defined(WDL_BIG_ENDIAN)
      WDL_BSWAP64_IF_BE(a);
      return (int)fwrite(&a,1,8,fp);
    #else
      unsigned char buf[8], *p = buf;
      for (int x = 0; x < 64; x += 8) *p++=(a>>x)&0xff;
      return (int)fwrite(buf,1,8,fp);
    #endif
    }

    template <class T> T **GetTmpPtrs(T **stackbuf, T **samples, unsigned int offs, int nchsrc)
    {
    #if defined(WAVEWRITER_MAX_NCH) && WAVEWRITER_MAX_NCH > 64
      T **tmpptrs=m_nch <= STACKBUF_MAX_NCH ? stackbuf : (T**)m_heapbuf.ResizeOK(nchsrc*sizeof(T*),false);
      if (tmpptrs)
    #else
      T **tmpptrs=stackbuf;
    #endif
      for (int ch = 0; ch < m_nch; ++ch) tmpptrs[ch]=samples[ch%nchsrc]+offs;
      return tmpptrs;
    }

    static const int STACKBUF_MAX_NCH=
  #if !defined(WAVEWRITER_MAX_NCH) || WAVEWRITER_MAX_NCH > 64
    64;
  #else
    WAVEWRITER_MAX_NCH;
  #endif

  #if defined(WAVEWRITER_MAX_NCH) && WAVEWRITER_MAX_NCH > 64
    WDL_HeapBuf m_heapbuf;
  #endif

    WDL_String m_fn;
    FILE *m_fp;
    unsigned int m_data_size, m_total_size;
    int m_bps,m_nch,m_srate;
};


#endif//_WAVWRITE_H_

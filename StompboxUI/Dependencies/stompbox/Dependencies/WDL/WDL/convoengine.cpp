/*
  WDL - convoengine.cpp
  Copyright (C) 2006 and later Cockos Incorporated

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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "convoengine.h"

#include "denormal.h"

//#define TIMING
#ifdef TIMING
#include "timing.c"
#endif

#define CONVOENGINE_SILENCE_THRESH 1.0e-12 // -240dB
#define CONVOENGINE_IMPULSE_SILENCE_THRESH 1.0e-15 // -300dB

#if !defined(WDL_CONVO_SSE) && !defined(WDL_CONVO_SSE3)
  #if defined(__SSE3__)
    #define WDL_CONVO_SSE3
  #elif defined(__SSE__) || _M_IX86_FP >= 1 || defined(_M_X64) || defined(_M_AMD64)
    #define WDL_CONVO_SSE
  #endif
#endif

//#if WDL_FFT_REALSIZE != 4 || defined(WDL_CONVO_WANT_FULLPRECISION_IMPULSE_STORAGE)
#undef WDL_CONVO_SSE
#undef WDL_CONVO_SSE3
//#endif

#ifdef WDL_CONVO_SSE // define for SSE optimised CplxMul
#include <xmmintrin.h>
#endif

#ifdef WDL_CONVO_SSE3 // define for SSE3 optimised CplxMul
#include <pmmintrin.h>
#endif

#if defined(WDL_CONVO_SSE) || defined(WDL_CONVO_SSE3)
  #define WDL_CONVO_ALIGN 16
  #define WDL_CONVO_ALIGN_IMPULSEBUFf (WDL_CONVO_ALIGN / sizeof(WDL_CONVO_IMPULSEBUFf))
  #define WDL_CONVO_ALIGN_FFT_REAL (WDL_CONVO_ALIGN / sizeof(WDL_FFT_REAL))
  #define WDL_CONVO_GETALIGNED() GetAligned(WDL_CONVO_ALIGN)
#else
  #define WDL_CONVO_ALIGN 1
  #define WDL_CONVO_ALIGN_IMPULSEBUFf 0
  #define WDL_CONVO_ALIGN_FFT_REAL 0
  #define WDL_CONVO_GETALIGNED() Get()
#endif

#if !defined(WDL_CONVO_SSE) && !defined(WDL_CONVO_SSE3)
static void WDL_CONVO_CplxMul2(WDL_FFT_COMPLEX *c, const WDL_FFT_COMPLEX *a, const WDL_CONVO_IMPULSEBUFCPLXf *b, int n)
{
  WDL_FFT_REAL t1, t2, t3, t4, t5, t6, t7, t8;
  if (n<2 || (n&1)) return;

  do {
    t1 = a[0].re * b[0].re;
    t2 = a[0].im * b[0].im;
    t3 = a[0].im * b[0].re;
    t4 = a[0].re * b[0].im;
    t5 = a[1].re * b[1].re;
    t6 = a[1].im * b[1].im;
    t7 = a[1].im * b[1].re;
    t8 = a[1].re * b[1].im;
    t1 -= t2;
    t3 += t4;
    t5 -= t6;
    t7 += t8;
    c[0].re = t1;
    c[1].re = t5;
    c[0].im = t3;
    c[1].im = t7;
    a += 2;
    b += 2;
    c += 2;
  } while (n -= 2);
}
static void WDL_CONVO_CplxMul3(WDL_FFT_COMPLEX *c, const WDL_FFT_COMPLEX *a, const WDL_CONVO_IMPULSEBUFCPLXf *b, int n)
{
  WDL_FFT_REAL t1, t2, t3, t4, t5, t6, t7, t8;
  if (n<2 || (n&1)) return;

  do {
    t1 = a[0].re * b[0].re;
    t2 = a[0].im * b[0].im;
    t3 = a[0].im * b[0].re;
    t4 = a[0].re * b[0].im;
    t5 = a[1].re * b[1].re;
    t6 = a[1].im * b[1].im;
    t7 = a[1].im * b[1].re;
    t8 = a[1].re * b[1].im;
    t1 -= t2;
    t3 += t4;
    t5 -= t6;
    t7 += t8;
    c[0].re += t1;
    c[1].re += t5;
    c[0].im += t3;
    c[1].im += t7;
    a += 2;
    b += 2;
    c += 2;
  } while (n -= 2);
}

#elif defined(WDL_CONVO_SSE3)
static void WDL_CONVO_CplxMul2(WDL_FFT_COMPLEX *c, const WDL_FFT_COMPLEX *a, const WDL_CONVO_IMPULSEBUFCPLXf *b, int n)
{
  __m128 xmm0, xmm1, xmm2;
  if (n<2 || (n&1)) return;

  do {
    xmm2 = _mm_load_ps((const float*)b);
    xmm1 = _mm_moveldup_ps(_mm_load_ps((const float*)a));
    xmm0 = _mm_movehdup_ps(_mm_load_ps((const float*)a));
    xmm1 = _mm_mul_ps(xmm1, xmm2);
    xmm2 = _mm_shuffle_ps(xmm2, xmm2, 0xB1);
    xmm2 = _mm_mul_ps(xmm2, xmm0);
    xmm1 = _mm_addsub_ps(xmm1, xmm2);
    _mm_store_ps((float*)c, xmm1);
    a += 2;
    b += 2;
    c += 2;
  } while (n -= 2);
}
static void WDL_CONVO_CplxMul3(WDL_FFT_COMPLEX *c, const WDL_FFT_COMPLEX *a, const WDL_CONVO_IMPULSEBUFCPLXf *b, int n)
{
  __m128 xmm0, xmm1, xmm2;
  if (n<2 || (n&1)) return;

  do {
    xmm2 = _mm_load_ps((const float*)b);
    xmm1 = _mm_moveldup_ps(_mm_load_ps((const float*)a));
    xmm0 = _mm_movehdup_ps(_mm_load_ps((const float*)a));
    xmm1 = _mm_mul_ps(xmm1, xmm2);
    xmm2 = _mm_shuffle_ps(xmm2, xmm2, 0xB1);
    xmm2 = _mm_mul_ps(xmm2, xmm0);
    xmm1 = _mm_addsub_ps(xmm1, xmm2);
    xmm1 = _mm_add_ps(xmm1, _mm_load_ps((const float*)c));
    _mm_store_ps((float*)c, xmm1);
    a += 2;
    b += 2;
    c += 2;
  } while (n -= 2);
}

#elif defined(WDL_CONVO_SSE)
static void WDL_CONVO_CplxMul2(WDL_FFT_COMPLEX *c, const WDL_FFT_COMPLEX *a, const WDL_CONVO_IMPULSEBUFCPLXf *b, int n)
{
  __m128 xmm0, xmm1, xmm2, xmm3, xmm4;
  if (n<2 || (n&1)) return;

  xmm4 = _mm_set_ps(1, -1, 1, -1);
  do {
    xmm3 = _mm_load_ps((const float*)a);
    xmm2 = _mm_load_ps((const float*)b);
    xmm0 = xmm3;
    xmm1 = xmm2;
    xmm0 = _mm_shuffle_ps(xmm0, xmm3, 0xB1);
    xmm1 = _mm_shuffle_ps(xmm1, xmm2, 0xF5);
    xmm1 = _mm_mul_ps(xmm1, xmm0);
    xmm2 = _mm_shuffle_ps(xmm2, xmm2, 0xA0);
    xmm2 = _mm_mul_ps(xmm2, xmm3);
    xmm1 = _mm_mul_ps(xmm1, xmm4);
    xmm1 = _mm_add_ps(xmm1, xmm2);
    _mm_store_ps((float*)c, xmm1);
    a += 2;
    b += 2;
    c += 2;
  } while (n -= 2);
}
static void WDL_CONVO_CplxMul3(WDL_FFT_COMPLEX *c, const WDL_FFT_COMPLEX *a, const WDL_CONVO_IMPULSEBUFCPLXf *b, int n)
{
  __m128 xmm0, xmm1, xmm2, xmm3, xmm4;
  if (n<2 || (n&1)) return;

  xmm4 = _mm_set_ps(1, -1, 1, -1);
  do {
    xmm3 = _mm_load_ps((const float*)a);
    xmm2 = _mm_load_ps((const float*)b);
    xmm0 = xmm3;
    xmm1 = xmm2;
    xmm0 = _mm_shuffle_ps(xmm0, xmm3, 0xB1);
    xmm1 = _mm_shuffle_ps(xmm1, xmm2, 0xF5);
    xmm1 = _mm_mul_ps(xmm1, xmm0);
    xmm2 = _mm_shuffle_ps(xmm2, xmm2, 0xA0);
    xmm2 = _mm_mul_ps(xmm2, xmm3);
    xmm1 = _mm_mul_ps(xmm1, xmm4);
    xmm1 = _mm_add_ps(xmm1, xmm2);
    xmm1 = _mm_add_ps(xmm1, _mm_load_ps((const float*)c));
    _mm_store_ps((float*)c, xmm1);
    a += 2;
    b += 2;
    c += 2;
  } while (n -= 2);
}
#endif // WDL_CONVO_SSE

static bool CompareQueueToBuf(WDL_FastQueue *q, const void *data, int len)
{
  int offs=0;
  while (len>0)
  {
    void *td=NULL;
    int sz=q->GetPtr(offs,&td);
    if (sz<1) return true; // not enough data = not equal!
    if (sz>len) sz=len;
    
    int i=sz/sizeof(WDL_FFT_REAL);
    WDL_FFT_REAL *a1=(WDL_FFT_REAL*)td;
    WDL_FFT_REAL *b1=(WDL_FFT_REAL*)data;
    while (i--) 
    {
      if (fabs(*a1-*b1)>CONVOENGINE_SILENCE_THRESH) return true;
      a1++;
      b1++;
    }

    data = ((char *)data)+sz;
    offs+=sz;
    len-=sz;
  }
  return false;
}


WDL_ConvolutionEngine::WDL_ConvolutionEngine()
{
  WDL_fft_init();
  m_fft_size=0;
  m_impdata.Add(new ImpChannelInfo);
  m_impulse_len=0;
  m_proc_nch=0;
}

WDL_ConvolutionEngine::~WDL_ConvolutionEngine()
{
  m_impdata.Empty(true);
  m_proc.Empty(true);
}

int WDL_ConvolutionEngine::SetImpulse(WDL_ImpulseBuffer *impulse, int fft_size, int impulse_sample_offset, int max_imp_size, bool forceBrute)
{
  int impulse_len=0;
  int x;
  int nch=impulse->GetNumChannels();
  for (x = 0; x < nch; x ++)
  {
    int l=impulse->impulses[x].GetSize()-impulse_sample_offset;
    if (max_imp_size && l>max_imp_size) l=max_imp_size;
    if (impulse_len < l) impulse_len=l;
  }

  if (nch>1) // detect mono signals pretending to be multichannel
  {
    for (x = 1; x < nch; x ++)
    {
      if (impulse->impulses[x].GetSize()!=impulse->impulses[0].GetSize()||
          memcmp(impulse->impulses[x].Get(),impulse->impulses[0].Get(),
            impulse->impulses[0].GetSize()*sizeof(WDL_FFT_REAL)))
            break;
    }
    if (x >= nch) nch=1;
  }

  m_impulse_len=impulse_len;
  m_proc_nch=-1;

  while (m_impdata.GetSize() > nch)
    m_impdata.Delete(m_impdata.GetSize()-1,true);
  while (m_impdata.GetSize() < nch)
    m_impdata.Add(new ImpChannelInfo);

  if (forceBrute)
  {
    m_fft_size=0;

    // save impulse
    for (x = 0; x < m_impdata.GetSize(); x ++)
    {
      WDL_FFT_REAL *imp=impulse->impulses[x].Get()+impulse_sample_offset;
      int lenout=impulse->impulses[x].GetSize()-impulse_sample_offset;  
      if (max_imp_size && lenout>max_imp_size) lenout=max_imp_size;

      ImpChannelInfo *impdata = m_impdata.Get(x);
      impdata->imp.Resize(lenout>0 ? lenout+WDL_CONVO_ALIGN_IMPULSEBUFf : 0);
      WDL_CONVO_IMPULSEBUFf *impout=impdata->imp.WDL_CONVO_GETALIGNED()+lenout;
      while (lenout-->0) *--impout = (WDL_CONVO_IMPULSEBUFf) *imp++;
    }

    for (x = 0; x < m_proc.GetSize(); x ++)
    {
      ProcChannelInfo *inf = m_proc.Get(x);
      inf->samplesin.Clear();
      inf->samplesin2.Clear();
      inf->samplesout.Clear();
    }

    return 0;
  }

 
  if (fft_size<=0)
  {
    int msz=fft_size<=-16? -fft_size*2 : 32768;

    fft_size=32;
    while (fft_size < impulse_len*2 && fft_size < msz) fft_size*=2;
  }

  m_fft_size=fft_size;

  int impchunksize=fft_size/2;
  int nblocks=(impulse_len+impchunksize-1)/impchunksize;
  //wdl_log("il=%d, ffts=%d, cs=%d, nb=%d\n",impulse_len,fft_size,impchunksize,nblocks);

  const bool smallerSizeMode=sizeof(WDL_CONVO_IMPULSEBUFf)!=sizeof(WDL_FFT_REAL);
 
  WDL_FFT_REAL scale=(WDL_FFT_REAL) (1.0/fft_size);
  for (x = 0; x < m_impdata.GetSize(); x ++)
  {
    WDL_FFT_REAL *imp=impulse->impulses[x].Get()+impulse_sample_offset;

    WDL_FFT_REAL *imp2=x < m_impdata.GetSize()-1 ? impulse->impulses[x+1].Get()+impulse_sample_offset : NULL;

    int nb=nblocks+!!smallerSizeMode;
    ImpChannelInfo *impdata = m_impdata.Get(x);
    impdata->imp.Resize(nb>0 ? nb*fft_size*2+WDL_CONVO_ALIGN_IMPULSEBUFf : 0);
    WDL_CONVO_IMPULSEBUFf *impout=impdata->imp.WDL_CONVO_GETALIGNED();
    char *zbuf=m_impdata.Get(x)->zflag.Resize(nblocks);
    int lenout=impulse->impulses[x].GetSize()-impulse_sample_offset;  
    if (max_imp_size && lenout>max_imp_size) lenout=max_imp_size;
      
    int bl;
    for (bl = 0; bl < nblocks; bl ++)
    {

      int thissz=lenout;
      if (thissz > impchunksize) thissz=impchunksize;

      lenout -= thissz;
      int i=0;    
      WDL_FFT_REAL mv=0.0;
      WDL_FFT_REAL mv2=0.0;
      WDL_FFT_REAL *imptmp = (WDL_FFT_REAL *)impout; //-V615

      for (; i < thissz; i ++)
      {
        WDL_FFT_REAL v=*imp++;
        WDL_FFT_REAL v2=(WDL_FFT_REAL)fabs(v);
        if (v2 > mv) mv=v2;

        imptmp[i*2]=denormal_filter_aggressive(v * scale);

        if (imp2)
        {
          v=*imp2++;
          v2=(WDL_FFT_REAL)fabs(v);
          if (v2>mv2) mv2=v2;
          imptmp[i*2+1]=denormal_filter_aggressive(v*scale);
        }
        else imptmp[i*2+1]=0.0;
      }
      for (; i < fft_size; i ++)
      {
        imptmp[i*2]=0.0;
        imptmp[i*2+1]=0.0;
      }
      if (mv>CONVOENGINE_IMPULSE_SILENCE_THRESH||mv2>CONVOENGINE_IMPULSE_SILENCE_THRESH)
      {
        *zbuf++=mv>CONVOENGINE_IMPULSE_SILENCE_THRESH ? 2 : 1; // 1 means only second channel has content
        WDL_fft((WDL_FFT_COMPLEX*)impout,fft_size,0);

        if (smallerSizeMode)
        {
          int x,n=fft_size*2;
          for(x=0;x<n;x++) impout[x]=(WDL_CONVO_IMPULSEBUFf)imptmp[x];
        }
      }
      else *zbuf++=0;

      impout+=fft_size*2;
    }
  }
  return m_fft_size/2;
}


void WDL_ConvolutionEngine::Reset() // clears out any latent samples
{
  for (int x = 0; x < m_proc.GetSize(); x ++)
  {
    ProcChannelInfo *inf = m_proc.Get(x);
    inf->samplesin.Clear();
    inf->samplesin2.Clear();
    inf->samplesout.Clear();
    inf->hist_pos = 0;
    memset(inf->samplehist_zflag.Get(),0,inf->samplehist_zflag.GetSize());
    memset(inf->samplehist.Get(),0,inf->samplehist.GetSize()*sizeof(WDL_FFT_REAL));
    memset(inf->overlaphist.Get(),0,inf->overlaphist.GetSize()*sizeof(WDL_FFT_REAL));
  }
}

void WDL_ConvolutionEngine::Add(WDL_FFT_REAL **bufs, int len, int nch)
{
  while (m_proc.GetSize() < nch) m_proc.Add(new ProcChannelInfo);
  while (m_proc.GetSize() > nch) m_proc.Delete(m_proc.GetSize()-1,true);

  if (m_fft_size<1)
  {
    m_proc_nch=nch;

    for (int ch = 0; ch < nch; ch ++)
    {
      int wch = ch % m_impdata.GetSize();
      ImpChannelInfo *impdata = m_impdata.Get(wch);
      WDL_CONVO_IMPULSEBUFf *imp=impdata->imp.WDL_CONVO_GETALIGNED();
      int imp_len = impdata->imp.GetSize()-WDL_CONVO_ALIGN_IMPULSEBUFf;
      ProcChannelInfo *pinf = m_proc.Get(ch);

      if (imp_len>0) 
      {
        if (pinf->samplesin2.Available()<imp_len*(int)sizeof(WDL_FFT_REAL))
        {
          int sza=imp_len*sizeof(WDL_FFT_REAL)-pinf->samplesin2.Available();
          memset(pinf->samplesin2.Add(NULL,sza),0,sza);
        }
        WDL_FFT_REAL *psrc;
        
        if (bufs && bufs[ch])
          psrc=(WDL_FFT_REAL*)pinf->samplesin2.Add(bufs[ch],len*sizeof(WDL_FFT_REAL));
        else
        {
          psrc=(WDL_FFT_REAL*)pinf->samplesin2.Add(NULL,len*sizeof(WDL_FFT_REAL));
          memset(psrc,0,len*sizeof(WDL_FFT_REAL));
        }

        WDL_FFT_REAL *pout=(WDL_FFT_REAL*)pinf->samplesout.Add(NULL,len*sizeof(WDL_FFT_REAL));
        int x;
        int len1 = len&~1;
        for (x=0; x < len1 ; x += 2)
        {
          int i=imp_len;
          double sum=0.0,sum2=0.0;
          WDL_FFT_REAL *sp=psrc+x-imp_len + 1;
          WDL_CONVO_IMPULSEBUFf *ip=imp;
          int j=i/4; i&=3;
          while (j--) // produce 2 samples, 4 impulse samples at a time
          {
            double a = ip[0],b=ip[1],aa=ip[2],bb=ip[3];
            double c = sp[1],d=sp[2],cc=sp[3];
            sum+=a * sp[0] + b * c + aa * d + bb * cc;
            sum2+=a * c + b * d + aa * cc + bb * sp[4];
            ip+=4;
            sp+=4;
          }

          while (i--)
          {
            double a = *ip++;
            sum+=a * sp[0];
            sum2+=a * sp[1];
            sp++;
          }
          pout[x]=(WDL_FFT_REAL) sum;
          pout[x+1]=(WDL_FFT_REAL) sum2;
        }
        for(;x<len;x++) // any odd samples left
        {
          int i=imp_len;
          double sum=0.0;
          WDL_FFT_REAL *sp=psrc+x-imp_len + 1;
          WDL_CONVO_IMPULSEBUFf *ip=imp;
          int j=i/4; i&=3;
          while (j--)
          {
            sum+=ip[0] * sp[0] + ip[1] * sp[1] + ip[2] * sp[2] + ip[3] * sp[3];
            ip+=4;
            sp+=4;
          }

          while (i--) sum+=*ip++ * *sp++;
          pout[x]=(WDL_FFT_REAL) sum;
        }
        pinf->samplesin2.Advance(len*sizeof(WDL_FFT_REAL));
        pinf->samplesin2.Compact();
      }
      else
      {
        if (bufs && bufs[ch]) pinf->samplesout.Add(bufs[ch],len*sizeof(WDL_FFT_REAL));
        else
        {
          memset(pinf->samplesout.Add(NULL,len*sizeof(WDL_FFT_REAL)),0,len*sizeof(WDL_FFT_REAL));
        }
      }

    }
    return;
  }


  int impchunksize=m_fft_size/2;
  int nblocks=(m_impulse_len+impchunksize-1)/impchunksize;

  if (m_proc_nch != nch)
  {
    m_proc_nch=nch;

    int mso=0;
    for (int ch = 0; ch < nch; ch ++)
    {
      ProcChannelInfo *pinf = m_proc.Get(ch);
      pinf->hist_pos = 0;
      int so=pinf->samplesin.Available() + pinf->samplesout.Available();
      if (so>mso) mso=so;

      if (m_impulse_len<1||!nblocks) 
      {
        if (pinf->samplesin.Available())
        {
          int s=pinf->samplesin.Available();
          void *buf=pinf->samplesout.Add(NULL,s);
          pinf->samplesin.GetToBuf(0,buf,s);
          pinf->samplesin.Clear();
        }
      }

      if (so < mso)
      {
        memset(pinf->samplesout.Add(NULL,mso-so),0,mso-so);
      }
      
      const int sz=nblocks*m_fft_size;

      memset(pinf->samplehist_zflag.Resize(nblocks),0,nblocks);
      pinf->samplehist.Resize(sz>0 ? sz*2+WDL_CONVO_ALIGN_FFT_REAL : 0);
      pinf->overlaphist.Resize(m_fft_size/2);
      memset(pinf->samplehist.Get(),0,pinf->samplehist.GetSize()*sizeof(WDL_FFT_REAL));
      memset(pinf->overlaphist.Get(),0,pinf->overlaphist.GetSize()*sizeof(WDL_FFT_REAL));
    }
  }

  if (m_impulse_len<1||!nblocks) 
  {
    for (int ch = 0; ch < nch; ch ++)
    {
      ProcChannelInfo *pinf = m_proc.Get(ch);
      if (bufs && bufs[ch])
        pinf->samplesout.Add(bufs[ch],len*sizeof(WDL_FFT_REAL));
      else
        memset(pinf->samplesout.Add(NULL,len*sizeof(WDL_FFT_REAL)),0,len*sizeof(WDL_FFT_REAL));
    }
    // pass through
    return;
  }

  for (int ch = 0; ch < nch; ch ++)
  {
    ProcChannelInfo *pinf = m_proc.Get(ch);
    if (pinf->samplehist.GetSize()<WDL_CONVO_ALIGN_FFT_REAL || !pinf->overlaphist.GetSize()) continue;
    pinf->samplesin.Add(bufs ? bufs[ch] : NULL,len*sizeof(WDL_FFT_REAL));
  }
}

void WDL_ConvolutionEngine::AddSilenceToOutput(int len)
{  
  for (int ch = 0; ch < m_proc_nch; ch++)
  {
    ProcChannelInfo *pinf = m_proc.Get(ch);
    memset(pinf->samplesout.Add(NULL,len*sizeof(WDL_FFT_REAL)),0,len*sizeof(WDL_FFT_REAL));
  }
}

int WDL_ConvolutionEngine::Avail(int want)
{
  if (m_fft_size<1)
  {
    ProcChannelInfo *pinf = m_proc.Get(0);
    return pinf ? pinf->samplesout.Available()/sizeof(WDL_FFT_REAL) : 0;
  }

  const int sz=m_fft_size/2;
  const int chunksize=m_fft_size/2;
  const int nblocks=(m_impulse_len+chunksize-1)/chunksize;
  // clear combining buffer
  m_combinebuf.Resize(m_fft_size*4+WDL_CONVO_ALIGN_FFT_REAL); // temp space
  WDL_FFT_REAL *workbuf2 = m_combinebuf.WDL_CONVO_GETALIGNED();

  int ch;

  for (ch = 0; ch < m_proc_nch; ch ++)
  {
    ProcChannelInfo *pinf = m_proc.Get(ch);
    ProcChannelInfo *pinf2 = ch+1 < m_proc_nch ? m_proc.Get(ch+1) : NULL;

    if (pinf->samplehist.GetSize()<WDL_CONVO_ALIGN_FFT_REAL || !pinf->overlaphist.GetSize()) continue;
    int srcc=ch % m_impdata.GetSize();

    bool allow_mono_input_mode=true;
    bool mono_impulse_mode=false;

    if (m_impdata.GetSize()==1 && pinf2 &&
        pinf2->samplehist.GetSize()>=WDL_CONVO_ALIGN_FFT_REAL &&
        pinf2->overlaphist.GetSize() &&
        pinf->samplesin.Available()==pinf2->samplesin.Available() &&
        pinf->samplesout.Available()==pinf2->samplesout.Available()
        )
    { // 2x processing mode
      mono_impulse_mode=true;
      allow_mono_input_mode=false;
    }


    const int in_needed=sz;

    // useSilentList[x] = 1 for mono signal, 2 for stereo, 0 for silent
    char *useSilentList=pinf->samplehist_zflag.GetSize()==nblocks ? pinf->samplehist_zflag.Get() : NULL;
    while (pinf->samplesin.Available()/(int)sizeof(WDL_FFT_REAL) >= sz &&
           pinf->samplesout.Available() < want*(int)sizeof(WDL_FFT_REAL))
    {
      int histpos;
      if ((histpos=++pinf->hist_pos) >= nblocks) histpos=pinf->hist_pos=0;

      // get samples from input, to history
      WDL_FFT_REAL *optr = pinf->samplehist.WDL_CONVO_GETALIGNED()+histpos*m_fft_size*2;

      pinf->samplesin.GetToBuf(0,optr+sz,in_needed*sizeof(WDL_FFT_REAL));
      pinf->samplesin.Advance(in_needed*sizeof(WDL_FFT_REAL));


      bool mono_input_mode=false;

      bool nonzflag=false;
      if (mono_impulse_mode)
      {
        if (++pinf2->hist_pos >= nblocks) pinf2->hist_pos=0;
        pinf2->samplesin.GetToBuf(0,workbuf2,sz*sizeof(WDL_FFT_REAL));
        pinf2->samplesin.Advance(sz*sizeof(WDL_FFT_REAL));
        int i;
        for (i = 0; i < sz; i ++) // unpack samples
        {
          WDL_FFT_REAL f = optr[i*2]=denormal_filter_aggressive(optr[sz+i]);
          if (!nonzflag && (f<-CONVOENGINE_SILENCE_THRESH || f>CONVOENGINE_SILENCE_THRESH)) nonzflag=true;
          f=optr[i*2+1]=denormal_filter_aggressive(workbuf2[i]);
          if (!nonzflag && (f<-CONVOENGINE_SILENCE_THRESH || f>CONVOENGINE_SILENCE_THRESH)) nonzflag=true;
        }
      }
      else
      {
        if (allow_mono_input_mode && 
          pinf2 &&
          srcc<m_impdata.GetSize()-1 &&
          !CompareQueueToBuf(&pinf2->samplesin,optr+sz,sz*sizeof(WDL_FFT_REAL))
          )
        {
          mono_input_mode=true;
        }
        else 
        {
          allow_mono_input_mode=false;
        }

        int i;
        for (i = 0; i < sz; i ++) // unpack samples
        {
          WDL_FFT_REAL f=optr[i*2]=denormal_filter_aggressive(optr[sz+i]);
          optr[i*2+1]=0.0;
          if (!nonzflag && (f<-CONVOENGINE_SILENCE_THRESH || f>CONVOENGINE_SILENCE_THRESH)) nonzflag=true;
        }
      }

      int i;
      for (i = 1; mono_input_mode && i < nblocks; i ++) // start @ 1, since hist[histpos] is no longer used for here
      {
        int srchistpos = histpos-i;
        if (srchistpos < 0) srchistpos += nblocks;
        if (!useSilentList || useSilentList[srchistpos]==2) mono_input_mode=false;
      }

      if (nonzflag||!useSilentList) memset(optr+sz*2,0,sz*2*sizeof(WDL_FFT_REAL));


#ifdef WDLCONVO_ZL_ACCOUNTING
      m_zl_fftcnt++;
#endif

      if (nonzflag) WDL_fft((WDL_FFT_COMPLEX*)optr,m_fft_size,0);

      if (useSilentList) useSilentList[histpos]=nonzflag ? (mono_input_mode ? 1 : 2) : 0;
    
      int mzfl=2;
      if (mono_input_mode)
      {
        mzfl=1;

        pinf2->samplesin.Advance(sz*sizeof(WDL_FFT_REAL));

        // save a valid copy in sample hist incase we switch from mono to stereo
        if (++pinf2->hist_pos >= nblocks) pinf2->hist_pos=0;
        if (pinf2->samplehist_zflag.GetSize()==nblocks)
          pinf2->samplehist_zflag.Get()[pinf2->hist_pos] = nonzflag ? 1 : 0;

        WDL_FFT_REAL *optr2 = pinf2->samplehist.WDL_CONVO_GETALIGNED()+pinf2->hist_pos*m_fft_size*2;
        memcpy(optr2,optr,m_fft_size*2*sizeof(WDL_FFT_REAL));
      }

      int applycnt=0;
      char *useImpSilentList=m_impdata.Get(srcc)->zflag.GetSize() == nblocks ? m_impdata.Get(srcc)->zflag.Get() : NULL;

      WDL_CONVO_IMPULSEBUFf *impulseptr=m_impdata.Get(srcc)->imp.WDL_CONVO_GETALIGNED();
      for (i = 0; i < nblocks; i ++, impulseptr+=m_fft_size*2)
      {
        int srchistpos = histpos-i;
        if (srchistpos < 0) srchistpos += nblocks;

        if (useImpSilentList && useImpSilentList[i]<mzfl) continue;
        if (useSilentList && !useSilentList[srchistpos]) continue; // silent block

        WDL_FFT_REAL *samplehist=pinf->samplehist.WDL_CONVO_GETALIGNED() + m_fft_size*srchistpos*2;

        if (applycnt++) // add to output
          WDL_CONVO_CplxMul3((WDL_FFT_COMPLEX*)workbuf2,(WDL_FFT_COMPLEX*)samplehist,(WDL_CONVO_IMPULSEBUFCPLXf*)impulseptr,m_fft_size);   
        else // replace output
          WDL_CONVO_CplxMul2((WDL_FFT_COMPLEX*)workbuf2,(WDL_FFT_COMPLEX*)samplehist,(WDL_CONVO_IMPULSEBUFCPLXf*)impulseptr,m_fft_size);  

      }
      if (!applycnt)
        memset(workbuf2,0,m_fft_size*2*sizeof(WDL_FFT_REAL));
      else
        WDL_fft((WDL_FFT_COMPLEX*)workbuf2,m_fft_size,1);

      WDL_FFT_REAL *olhist=pinf->overlaphist.Get(); // errors from last time
      WDL_FFT_REAL *p1=workbuf2,*p3=workbuf2+m_fft_size,*p1o=workbuf2;

      if (mono_impulse_mode||mono_input_mode)
      {
        WDL_FFT_REAL *p2o=workbuf2+m_fft_size*2;
        WDL_FFT_REAL *olhist2=pinf2->overlaphist.Get(); // errors from last time
        int s=sz/2;
        while (s--)
        {
          p2o[0] = p1[1]+olhist2[0];
          p2o[1] = p1[3]+olhist2[1];
          p1o[0] = p1[0]+olhist[0];
          p1o[1] = p1[2]+olhist[1];
          p1o+=2;
          p2o+=2;
          p1+=4;

          olhist[0]=p3[0];
          olhist[1]=p3[2];
          olhist2[0]=p3[1];
          olhist2[1]=p3[3];
          p3+=4;

          olhist+=2;
          olhist2+=2;
        }
        // add samples to output
        pinf->samplesout.Add(workbuf2,sz*sizeof(WDL_FFT_REAL));
        pinf2->samplesout.Add(workbuf2+m_fft_size*2,sz*sizeof(WDL_FFT_REAL));
      }
      else
      {
        int s=sz/2;
        while (s--)
        {
          p1o[0] = p1[0]+olhist[0];
          p1o[1] = p1[2]+olhist[1];
          p1o+=2;
          p1+=4;

          olhist[0]=p3[0];
          olhist[1]=p3[2];
          p3+=4;

          olhist+=2;
        }
        // add samples to output
        pinf->samplesout.Add(workbuf2,sz*sizeof(WDL_FFT_REAL));
      }
    } // while available

    if (mono_impulse_mode) ch++;
  }

  int mv = want;
  for (ch=0;ch<m_proc_nch;ch++)
  {
    const ProcChannelInfo *pinf = m_proc.Get(ch);
    int v = pinf ? pinf->samplesout.Available()/sizeof(WDL_FFT_REAL) : 0;
    if (!ch || v<mv)mv=v;
  }
  return mv;
}

WDL_FFT_REAL **WDL_ConvolutionEngine::Get() 
{
  WDL_FFT_REAL **ret = m_get_tmpptrs.ResizeOK(m_proc_nch,false);
  if (WDL_NORMALLY(ret))
    for (int x = 0; x < m_proc_nch; x ++) ret[x]=(WDL_FFT_REAL *)m_proc.Get(x)->samplesout.Get();
  return ret;
}

void WDL_ConvolutionEngine::Advance(int len)
{
  for (int ch = 0; ch < m_proc_nch; ch ++)
  {
    ProcChannelInfo *pinf = m_proc.Get(ch);
    pinf->samplesout.Advance(len*sizeof(WDL_FFT_REAL));
    pinf->samplesout.Compact();
  }
}



/****************************************************************
**  low latency version
*/

WDL_ConvolutionEngine_Div::WDL_ConvolutionEngine_Div()
{
#ifdef TIMING
  timingInit();
#endif
  for (int x = 0; x < 2; x ++) m_sout.Add(new WDL_Queue);
  m_need_feedsilence=true;
}

int WDL_ConvolutionEngine_Div::SetImpulse(WDL_ImpulseBuffer *impulse, int maxfft_size, int known_blocksize, int max_imp_size, int impulse_offset, int latency_allowed)
{
  m_need_feedsilence=true;

  m_engines.Empty(true);
  if (maxfft_size<0)maxfft_size=-maxfft_size;
  maxfft_size*=2;
  if (!maxfft_size || maxfft_size>32768) maxfft_size=32768;


  const int MAX_SIZE_FOR_BRUTE=64;

  int fftsize = MAX_SIZE_FOR_BRUTE;
  int impulsechunksize = MAX_SIZE_FOR_BRUTE;

  if (known_blocksize && !(known_blocksize&(known_blocksize-1)) && known_blocksize>MAX_SIZE_FOR_BRUTE*2)
  {
    fftsize=known_blocksize/2;
    impulsechunksize=known_blocksize/2;
  }
  if (latency_allowed*2 > fftsize)
  {
    int x = 16;
    while (x <= latency_allowed) x*=2;
    if (x>32768) x=32768;
    fftsize=impulsechunksize=x;
  }

  int offs=0;
  int samplesleft=impulse->impulses[0].GetSize()-impulse_offset;
  if (max_imp_size>0 && samplesleft>max_imp_size) samplesleft=max_imp_size;

  do
  {
    WDL_ConvolutionEngine *eng=new WDL_ConvolutionEngine;

    bool wantBrute = !latency_allowed && !offs;
    if (impulsechunksize*(wantBrute ? 2 : 3) >= samplesleft) impulsechunksize=samplesleft; // early-out, no point going to a larger FFT (since if we did this, we wouldnt have enough samples for a complete next pass)
    if (fftsize>=maxfft_size) { impulsechunksize=samplesleft; fftsize=maxfft_size; } // if FFTs are as large as possible, finish up

    eng->SetImpulse(impulse,fftsize,offs+impulse_offset,impulsechunksize, wantBrute);
    eng->m_zl_delaypos = offs;
    eng->m_zl_dumpage=0;
    m_engines.Add(eng);

#ifdef WDLCONVO_ZL_ACCOUNTING
    wdl_log("ce%d: offs=%d, len=%d, fftsize=%d\n",m_engines.GetSize(),offs,impulsechunksize,fftsize);
#endif

    samplesleft -= impulsechunksize;
    offs+=impulsechunksize;

#if 1 // this seems about 10% faster (maybe due to better cache use from less sized ffts used?)
    impulsechunksize=offs*3;
    fftsize=offs*2;
#else
    impulsechunksize=fftsize;

    fftsize*=2;
#endif
  }
  while (samplesleft > 0);
  
  return GetLatency();
}

int WDL_ConvolutionEngine_Div::GetLatency()
{
  return m_engines.GetSize() ? m_engines.Get(0)->GetLatency() : 0;
}


void WDL_ConvolutionEngine_Div::Reset()
{
  int x;
  for (x = 0; x < m_engines.GetSize(); x ++)
  {
    WDL_ConvolutionEngine *eng=m_engines.Get(x);
    eng->Reset();
  }
  for (x = 0; x < m_sout.GetSize(); x ++)
  {
    m_sout.Get(x)->Clear();
  }

  m_need_feedsilence=true;
}

WDL_ConvolutionEngine_Div::~WDL_ConvolutionEngine_Div()
{
#ifdef TIMING
  timingPrint();
#endif
  m_engines.Empty(true);
  m_sout.Empty(true);
}

void WDL_ConvolutionEngine_Div::Add(WDL_FFT_REAL **bufs, int len, int nch)
{
  while (m_sout.GetSize() < nch)
    m_sout.Add(new WDL_Queue);
  while (m_sout.GetSize() > nch)
    m_sout.Delete(m_sout.GetSize()-1,true);

  bool ns=m_need_feedsilence;
  m_need_feedsilence=false;

  int x;
  for (x = 0; x < m_engines.GetSize(); x ++)
  {
    WDL_ConvolutionEngine *eng=m_engines.Get(x);
    if (ns)
    {
      eng->m_zl_dumpage = (x>0 && x < m_engines.GetSize()-1) ? (eng->GetLatency()/4) : 0; // reduce max number of ffts per block by staggering them

      if (eng->m_zl_dumpage>0)
        eng->Add(NULL,eng->m_zl_dumpage,nch); // added silence to input (to control when fft happens)
    }

    eng->Add(bufs,len,nch);

    if (ns) eng->AddSilenceToOutput(eng->m_zl_delaypos); // add silence to output (to delay output to its correct time)

  }
}
WDL_FFT_REAL **WDL_ConvolutionEngine_Div::Get() 
{
  WDL_FFT_REAL **ret = m_get_tmpptrs.ResizeOK(m_sout.GetSize(),false);
  if (WDL_NORMALLY(ret))
    for (int x = 0; x < m_sout.GetSize(); x ++) ret[x]=(WDL_FFT_REAL *)m_sout.Get(x)->Get();
  return ret;
}

void WDL_ConvolutionEngine_Div::Advance(int len)
{
  int x;
  for (x = 0; x < m_sout.GetSize(); x ++)
  {
    WDL_Queue *q = m_sout.Get(x);
    q->Advance(len*sizeof(WDL_FFT_REAL));
    q->Compact();
  }
}

int WDL_ConvolutionEngine_Div::Avail(int wantSamples)
{
#ifdef TIMING
  timingEnter(1);
#endif
  int wso=wantSamples;
  int x;
#ifdef WDLCONVO_ZL_ACCOUNTING
  int cnt=0;
  static int maxcnt=-1;
  int h=0;
#endif
  for (x = 0; x < m_engines.GetSize(); x ++)
  {
    WDL_ConvolutionEngine *eng=m_engines.Get(x);
#ifdef WDLCONVO_ZL_ACCOUNTING
    eng->m_zl_fftcnt=0;
#endif
    int a=eng->Avail(wso+eng->m_zl_dumpage) - eng->m_zl_dumpage;
#ifdef WDLCONVO_ZL_ACCOUNTING
    cnt += !!eng->m_zl_fftcnt;

#if 0
    if (eng->m_zl_fftcnt)
      h|=1<<x;
    
    if (eng->m_zl_fftcnt && x==m_engines.GetSize()-1 && cnt>1)
    {
      wdl_log("fft flags=%08x (%08x=max)\n",h,1<<x);
    }
#endif
#endif
    if (a < wantSamples) wantSamples=a;
  }

#ifdef WDLCONVO_ZL_ACCOUNTING
  static DWORD lastt=0;
  if (cnt>maxcnt)maxcnt=cnt;
  if (GetTickCount()>lastt+1000)
  {
    lastt=GetTickCount();
    wdl_log("maxcnt=%d\n",maxcnt);
    maxcnt=-1;
  }
#endif
  if (wantSamples>0)
  {
    const int add_sz = wantSamples*sizeof(WDL_FFT_REAL);
    for (x =0; x < m_sout.GetSize(); x ++)
    {
      void *add = m_sout.Get(x)->Add(NULL,add_sz);
      if (WDL_NORMALLY(add != NULL)) memset(add,0,add_sz);
    }

    for (x = 0; x < m_engines.GetSize(); x ++)
    {
      WDL_ConvolutionEngine *eng=m_engines.Get(x);
      if (eng->m_zl_dumpage>0) { eng->Advance(eng->m_zl_dumpage); eng->m_zl_dumpage=0; }

      WDL_FFT_REAL **p=eng->Get();
      if (p)
      {
        int i;
        for (i =0; i < m_sout.GetSize(); i ++)
        {
          WDL_Queue *q = m_sout.Get(i);
          const int qsz = q->Available();
          if (WDL_NORMALLY(qsz >= add_sz))
          {
            WDL_FFT_REAL *o=(WDL_FFT_REAL *)((char *)q->Get() + qsz - add_sz);
            const WDL_FFT_REAL *in=p[i];
            int j=wantSamples;
            while (j-->0) *o++ += *in++;
          }
        }
      }
      eng->Advance(wantSamples);
    }
  }
#ifdef TIMING
  timingLeave(1);
#endif

  WDL_Queue *q0 = m_sout.Get(0);
  int av=WDL_NORMALLY(q0 != NULL) ? (int) (q0->Available()/sizeof(WDL_FFT_REAL)) : 0;
  return av>wso ? wso : av;
}


/****************************************************************
**  threaded low latency version
*/

#ifdef WDL_CONVO_THREAD

#ifdef _WIN32

#define WDL_CONVO_thread_state (m_thread && m_thread_state)

#else

#define WDL_CONVO_thread_state m_thread_state

static void WDL_CONVO_cond_init(bool *isSignal, pthread_cond_t *cond, pthread_mutex_t *mutex)
{
  *isSignal = false;
  pthread_mutex_init(mutex,NULL);
  pthread_cond_init(cond,NULL);
}

static void WDL_CONVO_cond_signal(bool *isSignal, pthread_cond_t *cond, pthread_mutex_t *mutex)
{
  pthread_mutex_lock(mutex);
  if (!*isSignal)
  {
    *isSignal = true;
    pthread_cond_signal(cond);
  }
  pthread_mutex_unlock(mutex);
}

static void WDL_CONVO_cond_wait(bool *isSignal, pthread_cond_t *cond, pthread_mutex_t *mutex)
{
  pthread_mutex_lock(mutex);
  while (!*isSignal) pthread_cond_wait(cond,mutex);
  *isSignal=false;
  pthread_mutex_unlock(mutex);
}

static void WDL_CONVO_cond_destroy(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
  pthread_cond_destroy(cond);
  pthread_mutex_destroy(mutex);
}

#endif // _WIN32

WDL_ConvolutionEngine_Thread::WDL_ConvolutionEngine_Thread()
{
  m_proc_nch=2;
  m_need_feedsilence=true;

  m_thread_enable = true;
#ifdef _WIN32
  m_thread = NULL;
  m_signal_thread = CreateEvent(NULL, FALSE, FALSE, NULL);
  m_signal_main = CreateEvent(NULL, FALSE, FALSE, NULL);

  m_thread_state = m_signal_thread && m_signal_main;
#else
  m_thread_state = false;
  WDL_CONVO_cond_init(&m_signal_thread, &m_signal_thread_cond, &m_signal_thread_mutex);
  WDL_CONVO_cond_init(&m_signal_main, &m_signal_main_cond, &m_signal_main_mutex);
#endif
}

int WDL_ConvolutionEngine_Thread::SetImpulse(WDL_ImpulseBuffer *impulse, int maxfft_size, int known_blocksize, int max_imp_size, int impulse_offset, int latency_allowed)
{
  if (!m_thread_enable) CloseThread();
  Reset();

  if (maxfft_size<0)maxfft_size=-maxfft_size;
  if (!maxfft_size || maxfft_size>16384) maxfft_size=16384;

  maxfft_size = 1024;

  int samplesleft=impulse->GetLength()-impulse_offset;
  if (max_imp_size>0 && samplesleft>max_imp_size) samplesleft=max_imp_size;

  int impulsechunksize = maxfft_size;
  if (impulsechunksize >= samplesleft || !m_thread_enable) impulsechunksize=samplesleft;

  m_zl_engine.SetImpulse(impulse, maxfft_size, known_blocksize, impulsechunksize, impulse_offset, latency_allowed);

  samplesleft -= impulsechunksize;

  m_thread_engine.SetImpulse(impulse, maxfft_size*2, impulse_offset + impulsechunksize, samplesleft);
  m_thread_engine.m_zl_delaypos = samplesleft > 0 ? impulsechunksize : -1;
  m_thread_engine.m_zl_dumpage=0;

  return GetLatency();
}

void WDL_ConvolutionEngine_Thread::Reset()
{
  if (WDL_CONVO_thread_state)
  {
#ifdef _WIN32
    SetEvent(m_signal_thread);
    WaitForSingleObject(m_signal_main, INFINITE);
#else
    WDL_CONVO_cond_signal(&m_signal_thread, &m_signal_thread_cond, &m_signal_thread_mutex);
    WDL_CONVO_cond_wait(&m_signal_main, &m_signal_main_cond, &m_signal_main_mutex);
#endif
  }

  m_zl_engine.Reset();
  m_thread_engine.Reset();

  int x;
  for (x = 0; x < WDL_CONVO_MAX_PROC_NCH; x ++)
  {
    m_samplesin[x].Clear();
    m_samplesin2[x].Clear();
    m_samplesout[x].Clear();
    m_samplesout2[x].Clear();
  }

  m_need_feedsilence=true;
}

WDL_ConvolutionEngine_Thread::~WDL_ConvolutionEngine_Thread()
{
  CloseThread();
#ifdef _WIN32
  if (m_signal_thread) CloseHandle(m_signal_thread);
  if (m_signal_main) CloseHandle(m_signal_main);
#else
  WDL_CONVO_cond_destroy(&m_signal_thread_cond, &m_signal_thread_mutex);
  WDL_CONVO_cond_destroy(&m_signal_main_cond, &m_signal_main_mutex);
#endif
}

void WDL_ConvolutionEngine_Thread::Add(WDL_FFT_REAL **bufs, int len, int nch)
{
  m_proc_nch=nch;

  if (m_thread_engine.m_zl_delaypos >= 0 && CreateThread())
  {
    int ch;
    m_samplesin_lock.Enter();
    for (ch = 0; ch < nch; ch ++)
    {
      m_samplesin[ch].Add(bufs ? bufs[ch] : NULL,len*sizeof(WDL_FFT_REAL));
    }
    m_samplesin_lock.Leave();
#ifdef _WIN32
    SetEvent(m_signal_thread);
#else
    WDL_CONVO_cond_signal(&m_signal_thread, &m_signal_thread_cond, &m_signal_thread_mutex);
#endif
  }

  m_zl_engine.Add(bufs,len,nch);
}

WDL_FFT_REAL **WDL_ConvolutionEngine_Thread::Get()
{
  int x;
  for (x = 0; x < m_proc_nch; x ++)
  {
    m_get_tmpptrs[x]=(WDL_FFT_REAL *)m_samplesout2[x].Get();
  }
  return m_get_tmpptrs;
}

void WDL_ConvolutionEngine_Thread::Advance(int len)
{
  int x;
  for (x = 0; x < m_proc_nch; x ++)
  {
    m_samplesout2[x].Advance(len*sizeof(WDL_FFT_REAL));
    m_samplesout2[x].Compact();
  }
}

int WDL_ConvolutionEngine_Thread::Avail(int wantSamples)
{
  int wso=wantSamples;
  int x;

  int av=m_samplesout2[0].Available()/sizeof(WDL_FFT_REAL);
  if (av >= wantSamples) return av;
  wantSamples -= av;

  av=m_zl_engine.Avail(wantSamples);
  if (av < wantSamples) wantSamples=av;

  if (m_thread_engine.m_zl_delaypos >= 0)
  {
    m_samplesout_lock.Enter();
    av=m_samplesout[0].Available();
    m_samplesout_lock.Leave();
    while (av < wantSamples)
    {
      int a;
      if (WDL_CONVO_thread_state)
      {
#ifdef _WIN32
        SetEvent(m_signal_thread);
        WaitForSingleObject(m_signal_main, INFINITE);
#else
        WDL_CONVO_cond_signal(&m_signal_thread, &m_signal_thread_cond, &m_signal_thread_mutex);
        WDL_CONVO_cond_wait(&m_signal_main, &m_signal_main_cond, &m_signal_main_mutex);
#endif

        m_samplesout_lock.Enter();
        a=m_samplesout[0].Available();
        m_samplesout_lock.Leave();
      }
      else
      {
        a=av;
      }
      if (a>av) av=a; else wantSamples=av;
    }
  }

  if (wantSamples>0)
  {
    WDL_FFT_REAL *tp[WDL_CONVO_MAX_PROC_NCH];
    for (x =0; x < m_proc_nch; x ++)
    {
      memset(tp[x]=(WDL_FFT_REAL*)m_samplesout2[x].Add(NULL,wantSamples*sizeof(WDL_FFT_REAL)),0,wantSamples*sizeof(WDL_FFT_REAL));
    }

    WDL_FFT_REAL **p=m_zl_engine.Get();
    if (p)
    {
      int i;
      for (i =0; i < m_proc_nch; i ++)
      {
        WDL_FFT_REAL *o=tp[i];
        WDL_FFT_REAL *in=p[i];
        int j=wantSamples;
        while (j-->0) *o++ += *in++;
      }
    }
    m_zl_engine.Advance(wantSamples);

    if (m_thread_engine.m_zl_delaypos >= 0)
    {
      m_samplesout_lock.Enter();
      {
        int i;
        for (i =0; i < m_proc_nch; i ++)
        {
          WDL_FFT_REAL *o=tp[i];
          WDL_FFT_REAL *in=(WDL_FFT_REAL *)m_samplesout[i].Get();
          int j=wantSamples;
          while (j-->0) *o++ += *in++;

          m_samplesout[i].Advance(wantSamples*sizeof(WDL_FFT_REAL));
          m_samplesout[i].Compact();
        }
      }
      m_samplesout_lock.Leave();
    }
  }

  av=m_samplesout2[0].Available()/sizeof(WDL_FFT_REAL);
  return av>wso ? wso : av;
}

bool WDL_ConvolutionEngine_Thread::CreateThread()
{
#ifdef _WIN32
  if (!m_thread && m_thread_state)
  {
    m_thread = ::CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
    if (m_thread)
      SetThreadPriority(m_thread, THREAD_PRIORITY_ABOVE_NORMAL);
    else
      m_thread_state = false;
  }
#else
  if (!m_thread_state)
  {
    m_thread_state = true;

    m_thread=0;
    pthread_create(&m_thread,NULL,ThreadProc,this);

    static const int prio = 1;
    int pol;
    struct sched_param param;
    if (!pthread_getschedparam(m_thread,&pol,&param))
    {

      param.sched_priority = 31 + prio;
      int mt=sched_get_priority_min(pol);
      if (param.sched_priority<mt||param.sched_priority > (mt=sched_get_priority_max(pol)))param.sched_priority=mt;

      pthread_setschedparam(m_thread,pol,&param);
    }
  }
#endif
  return m_thread_state;
}

void WDL_ConvolutionEngine_Thread::CloseThread()
{
#ifdef _WIN32
  if (m_thread)
  {
    const bool state = m_thread_state;
    if (state)
    {
      m_thread_state = false;
      SetEvent(m_signal_thread);
      WaitForSingleObject(m_thread, INFINITE);
    }
    if (CloseHandle(m_thread)) m_thread_state = state;
    m_thread = NULL;
  }
#else
  if (m_thread_state)
  {
    m_thread_state = false;
    WDL_CONVO_cond_signal(&m_signal_thread, &m_signal_thread_cond, &m_signal_thread_mutex);
    void *tmp;
    pthread_join(m_thread,&tmp);
    pthread_detach(m_thread);
  }
#endif
}

#ifdef _WIN32
DWORD WINAPI WDL_ConvolutionEngine_Thread::ThreadProc(LPVOID lpParam)
#else
void *WDL_ConvolutionEngine_Thread::ThreadProc(void *lpParam)
#endif
{
  WDL_ConvolutionEngine_Thread* _this = (WDL_ConvolutionEngine_Thread*)lpParam;

  do
  {
#ifdef _WIN32
    if (WaitForSingleObject(_this->m_signal_thread, INFINITE) != WAIT_OBJECT_0)
    {
      _this->m_thread_state = false;
    }
#else
    WDL_CONVO_cond_wait(&_this->m_signal_thread, &_this->m_signal_thread_cond, &_this->m_signal_thread_mutex);
#endif

    if (_this->m_thread_state)
    {
      int avail, av, x;

      _this->m_samplesin_lock.Enter();
      avail = _this->m_samplesin[0].Available();
      while (avail > 0)
      {
        int sz;
        for (x = 0; x < _this->m_proc_nch; x ++)
        {
          void *buf=NULL;
          sz=_this->m_samplesin[x].GetPtr(0,&buf);
          _this->m_samplesin2[x].Add(buf,sz);
          _this->m_samplesin[x].Advance(sz);
        }
        avail -= sz;
      }
      _this->m_samplesin_lock.Leave();

      av = avail = _this->m_samplesin2[0].Available();
      while (avail > 0)
      {
        WDL_FFT_REAL *tp[WDL_CONVO_MAX_PROC_NCH];
        int sz;
        for (x = 0; x < _this->m_proc_nch; x ++)
        {
          sz=_this->m_samplesin2[x].GetPtr(0,(void**)&tp[x]);
        }
        _this->m_thread_engine.Add(tp,sz/sizeof(WDL_FFT_REAL),_this->m_proc_nch);
        for (x = 0; x < _this->m_proc_nch; x ++)
        {
          _this->m_samplesin2[x].Advance(sz);
        }
        if (_this->m_need_feedsilence)
        {
          if (_this->m_thread_engine.m_zl_delaypos > 0)
          {
            _this->m_thread_engine.AddSilenceToOutput(_this->m_thread_engine.m_zl_delaypos); // , _this->m_proc_nch); // add silence to output (to delay output to its correct time)
          }
          _this->m_need_feedsilence=false;
        }
        avail -= sz;
      }

      if (av) av = _this->m_thread_engine.Avail(av/sizeof(WDL_FFT_REAL));
      if (av)
      {
        WDL_FFT_REAL **p=_this->m_thread_engine.Get();
        _this->m_samplesout_lock.Enter();
        for (x = 0; x < _this->m_proc_nch; x ++)
        {
          _this->m_samplesout[x].Add(p[x],av*sizeof(WDL_FFT_REAL));
        }
        _this->m_samplesout_lock.Leave();
        _this->m_thread_engine.Advance(av);
      }
    }

#ifdef _WIN32
    SetEvent(_this->m_signal_main);
#else
    WDL_CONVO_cond_signal(&_this->m_signal_main, &_this->m_signal_main_cond, &_this->m_signal_main_mutex);
#endif
  }
  while (_this->m_thread_state);

#ifndef _WIN32
  pthread_exit(0);
#endif
  return 0;
}

#endif // WDL_CONVO_THREAD



#ifdef WDL_TEST_CONVO

#include <stdio.h>

int main(int argc, char **argv)
{
  if (argc!=5)
  {
    printf("usage: convoengine fftsize implen oneoffs pingoffs\n");
    return -1;
  }

  int fftsize=atoi(argv[1]);
  int implen = atoi(argv[2]);
  int oneoffs = atoi(argv[3]);
  int pingoffs=atoi(argv[4]);

  if (implen < 1 || oneoffs < 0 || oneoffs >= implen || pingoffs < 0)
  {
    printf("invalid parameters\n");
    return -1;
  }

  WDL_ImpulseBuffer imp;
  imp.nch=1;
  memset(imp.impulses[0].Resize(implen),0,implen*sizeof(WDL_FFT_REAL));
  imp.impulses[0].Get()[oneoffs]=1.0;


#if WDL_TEST_CONVO==2
  WDL_ConvolutionEngine_Div engine;
#else
  WDL_ConvolutionEngine engine;
#endif
  engine.SetImpulse(&imp,fftsize);
  WDL_TypedBuf<WDL_FFT_REAL> m_tmpbuf;
  memset(m_tmpbuf.Resize(pingoffs+1),0,pingoffs*sizeof(WDL_FFT_REAL));
  m_tmpbuf.Get()[pingoffs]=1.0;
  WDL_FFT_REAL *p=m_tmpbuf.Get();
  engine.Add(&p,pingoffs+1,1);
  
  p=m_tmpbuf.Resize(4096);
  memset(p,0,m_tmpbuf.GetSize()*sizeof(WDL_FFT_REAL));

  int avail;
  while ((avail=engine.Avail(pingoffs+oneoffs + 8192)) < pingoffs+oneoffs + 8192)
  {
    engine.Add(&p,4096,1);
  }
  WDL_FFT_REAL **output = engine.Get();
  if (!output || !*output)
  {
    printf("cant get output\n");
    return -1;
  }
  int x;
  for (x = 0; x < avail; x ++)
  {
    WDL_FFT_REAL val=output[0][x];
    WDL_FFT_REAL expval = (x==pingoffs+oneoffs) ? 1.0:0.0;
    if (fabs(val-expval)>0.000000001)
    {
      printf("%d: %.4fdB - %f %f\n",x,log10(max(val,0.000000000001))*20.0 - log10(max(expval,0.000000000001))*20.0,val,expval);
    }
  }

  return 0;
}

#endif


int WDL_ImpulseBuffer::SetLength(int samples)
{
  const int nch = impulses.list.GetSize();
  if (!nch) return 0;
  for (int x=0;x<nch;x++)
  {
    int cursz=impulses[x].GetSize();
    if (cursz!=samples) 
    {
      impulses[x].Resize(samples,false);

      if (impulses[x].GetSize()!=samples) // validate length!
      {
        // ERROR! FREE ALL!
        for(x=0;x<impulses.list.GetSize();x++) impulses[x].Resize(0);
        return 0;
      }
    }

    if (cursz<samples)
      memset(impulses[x].Get()+cursz,0,(samples-cursz)*sizeof(WDL_FFT_REAL));     
  }
  return impulses[0].GetSize();
}


void WDL_ImpulseBuffer::SetNumChannels(int usench, bool duplicateExisting)
{
  if (usench<1) usench=1;

  const int old_nch = impulses.list.GetSize();
  if (usench > old_nch)
  {
    while (impulses.list.GetSize() < usench)
      impulses.list.Add(new WDL_CONVO_IMPULSE_TYPED_BUF);

    const int len = SetLength(GetLength());

    int x,ax=0;
    if (duplicateExisting && len>0 && old_nch>0) for(x=old_nch;x<usench;x++)
    {
      memcpy(impulses[x].Get(),impulses[ax].Get(),len*sizeof(WDL_FFT_REAL)); // duplicate channels
      if (++ax>=old_nch) ax=0;
    }
  }
  else while (usench<impulses.list.GetSize())
  {
    impulses.list.Delete(impulses.list.GetSize()-1,true);
  }
}


void WDL_ImpulseBuffer::Set(const WDL_FFT_REAL** bufs, int samples, int usench)
{
#ifdef WDL_CONVO_USE_CONST_HEAP_BUF
  if (usench<1) usench=1;

  const int old_nch = impulses.list.GetSize();
  if (usench > old_nch)
  {
    while (impulses.list.GetSize() < usench)
      impulses.list.Add(new WDL_CONVO_IMPULSE_TYPED_BUF);
  }
  else while (usench<impulses.list.GetSize())
  {
    impulses.list.Delete(impulses.list.GetSize()-1,true);
  }

  for (int x = 0; x < usench; ++x) impulses[x].Set(bufs[x], samples);

#else
  SetLength(samples);
  SetNumChannels(usench,false);
  usench = GetNumChannels();
  if (GetLength() > 0) for (int x = 0; x < usench; ++x)
  {
    memcpy(impulses[x].Get(), bufs[x], samples * sizeof(WDL_FFT_REAL));
  }
#endif
}

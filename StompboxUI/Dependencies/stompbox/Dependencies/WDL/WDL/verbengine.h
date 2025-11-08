#ifndef _VERBENGINE_H_
#define _VERBENGINE_H_


/*
    WDL - verbengine.h
    Copyright (C) 2007 and later Cockos Incorporated

    This is based on the public domain FreeVerb source:
      by Jezar at Dreampoint, June 2000
      http://www.dreampoint.co.uk

    Filter tweaks and general guidance thanks to Thomas Scott Stillwell.

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
#include "heapbuf.h"


#include "denormal.h"


// #define WDL_REVERB_MONO for 1 instead of 2 channels
#ifdef WDL_REVERB_MONO
#define WDL_REVERB_NCH 1
#else
#define WDL_REVERB_NCH 2
#endif

// #define WDL_REVERB_FIXED_WIDTH for fixed width of 1.0
#if defined(WDL_REVERB_MONO) && !defined(WDL_REVERB_FIXED_WIDTH)
#define WDL_REVERB_FIXED_WIDTH
#endif

// #define WDL_REVERB_ENABLE_SET_NCH to enable SetNumChannels()
#ifdef WDL_REVERB_ENABLE_SET_NCH
#define WDL_REVERB_if_m_nch_gt_1 if (m_nch > 1)
#else
#define WDL_REVERB_if_m_nch_gt_1
#endif


class WDL_ReverbAllpass
{
public:
  WDL_ReverbAllpass() { feedback=0.5; setsize(1); }
  ~WDL_ReverbAllpass() { }

  void setsize(int size)
  {
    if (size<1)size=1;
    if (buffer.GetSize()!=size)
    {
      bufidx=0;
      buffer.Resize(size);
      Reset();
    }
  }

	double process(double inp)
  {
    double *bptr=buffer.Get()+bufidx;

	  double bufout = *bptr;
	  
	  double output = bufout - inp;
	  *bptr = denormal_filter_double(inp + (bufout*feedback));

	  if(++bufidx>=buffer.GetSize()) bufidx = 0;

	  return output;
  }
  void Reset() { memset(buffer.Get(),0,buffer.GetSize()*sizeof(double)); }
  void setfeedback(double val) { feedback=val; }

private:
	double	feedback;
	WDL_TypedBuf<double> buffer;
	int		bufidx;
public:
  int __pad;

} WDL_FIXALIGN;

  
class WDL_ReverbComb
{
public:
  WDL_ReverbComb() { feedback=0.5; damp=0.5; filterstore=0; setsize(1); }
  ~WDL_ReverbComb() { }

  void setsize(int size)
  {
    if (size<1)size=1;
    if (buffer.GetSize()!=size)
    {
      bufidx=0;
      buffer.Resize(size);
      Reset();
    }
  }

	double process(double inp)
  {
    double *bptr=buffer.Get()+bufidx;
	  double output = *bptr;
	  filterstore = denormal_filter_double((output*(1-damp)) + (filterstore*damp));

	  *bptr = denormal_filter_double(inp + (filterstore*feedback));

	  if(++bufidx>=buffer.GetSize()) bufidx = 0;

	  return output;
  }
  void Reset() { filterstore=0; memset(buffer.Get(),0,buffer.GetSize()*sizeof(double)); }
  void setdamp(double val) { damp=val;  }
  void setfeedback(double val) { feedback=val; }

private:

	double	feedback;
	double	filterstore;
	double	damp;
	WDL_TypedBuf<double> buffer;
	int		bufidx;
public:
  int __pad;
} WDL_FIXALIGN;

  // these represent lengths in samples at 44.1khz but are scaled accordingly
#ifndef WDL_REVERB_MONO
const int wdl_verb__stereospread=23;
#endif
const short wdl_verb__combtunings[]={1116,1188,1277,1356,1422,1491,1557,1617,1685,1748};
const short wdl_verb__allpasstunings[]={556,441,341,225,180,153};


class WDL_ReverbEngine
{
public:
  WDL_ReverbEngine()
  {
    m_srate=44100.0;
    m_roomsize=0.5;
    m_damp=0.5;
  #ifndef WDL_REVERB_FIXED_WIDTH
    SetWidth(1.0);
  #endif
  #ifdef WDL_REVERB_ENABLE_SET_NCH
    SetNumChannels(2);
  #endif
    Reset(false);
  }
  ~WDL_ReverbEngine()
  {
  }
  void SetSampleRate(double srate)
  {
    if (m_srate!=srate)
    {
      m_srate=srate;
      Reset(true);
    }
  }

  double GetSampleRate() const { return m_srate; }

  #ifdef WDL_REVERB_MONO
  void ProcessSampleBlock(double *spl0, double *outp0, int ns)
  #else
  void ProcessSampleBlock(double *spl0, double *spl1, double *outp0, double *outp1, int ns)
  #endif
  {
    int x;
    memset(outp0,0,ns*sizeof(double));
    #ifndef WDL_REVERB_MONO
    WDL_REVERB_if_m_nch_gt_1 memset(outp1,0,ns*sizeof(double));
    #endif

    for (x = 0; x < sizeof(wdl_verb__combtunings)/sizeof(wdl_verb__combtunings[0]); x += 2)
    {
      int i=ns;
      double *p0=outp0,*i0=spl0
      #ifndef WDL_REVERB_MONO
      ,*p1=outp1,*i1=spl1
      #endif
      ;
      while (i--)
      {        
        double a=*i0++
        #ifdef WDL_REVERB_ENABLE_SET_NCH
        ,b; if (m_nch > 1) b=*i1++
        #elif !defined(WDL_REVERB_MONO)
        ,b=*i1++
        #endif
        ;
        *p0+=m_combs[x][0].process(a); 
        #ifndef WDL_REVERB_MONO
        WDL_REVERB_if_m_nch_gt_1 *p1+=m_combs[x][1].process(b);
        #endif
        *p0+++=m_combs[x+1][0].process(a); 
        #ifndef WDL_REVERB_MONO
        WDL_REVERB_if_m_nch_gt_1 *p1+++=m_combs[x+1][1].process(b);
        #endif
      }
    }
    for (x = 0; x < sizeof(wdl_verb__allpasstunings)/sizeof(wdl_verb__allpasstunings[0])-2; x += 2)
    {
      int i=ns;
      double *p0=outp0
      #ifndef WDL_REVERB_MONO
      ,*p1=outp1
      #endif
      ;
      while (i--)
      {        
        double tmp=m_allpasses[x][0].process(*p0);

        #ifndef WDL_REVERB_MONO
        #ifdef WDL_REVERB_ENABLE_SET_NCH
        double tmp2; if (m_nch > 1) tmp2=
        #else
        double tmp2=
        #endif
        m_allpasses[x][1].process(*p1);
        #endif

        *p0++=m_allpasses[x+1][0].process(tmp);
        #ifndef WDL_REVERB_MONO
        WDL_REVERB_if_m_nch_gt_1 *p1++=m_allpasses[x+1][1].process(tmp2);
        #endif
      }
    }
    int i=ns;
    double *p0=outp0
    #ifndef WDL_REVERB_MONO
    ,*p1=outp1
    #endif
          ;
    while (i--)
    {        
      double a=m_allpasses[x+1][0].process(m_allpasses[x][0].process(*p0))*0.015;

      #ifndef WDL_REVERB_MONO
      #ifdef WDL_REVERB_ENABLE_SET_NCH
      double b; if (m_nch > 1) b=
      #else
      double b=
      #endif
      m_allpasses[x+1][1].process(m_allpasses[x][1].process(*p1))*0.015;
      #endif

    #ifdef WDL_REVERB_FIXED_WIDTH
      *p0 = a;
      #ifndef WDL_REVERB_MONO
      WDL_REVERB_if_m_nch_gt_1 *p1 = b;
      #endif
    #else
      #ifdef WDL_REVERB_ENABLE_SET_NCH
      if (m_nch == 1)
      {
        *p0 = a;
      }
      else
      #endif
      if (m_wid<0)
      {
        double m=-m_wid;
        *p0 = b*m + a*(1.0-m);
        *p1 = a*m + b*(1.0-m);
      }
      else
      {
        double m=m_wid;
        *p0 = a*m + b*(1.0-m);
        *p1 = b*m + a*(1.0-m);
      }
    #endif
      p0++;
      #ifndef WDL_REVERB_MONO
      WDL_REVERB_if_m_nch_gt_1 p1++;
      #endif
    }
    
  }

  #ifdef WDL_REVERB_MONO
  void ProcessSample(double *spl0)
  #else
  void ProcessSample(double *spl0, double *spl1)
  #endif
  {
    int x;
    double in0=*spl0 * 0.015;

    #ifndef WDL_REVERB_MONO
    #ifdef WDL_REVERB_ENABLE_SET_NCH
    double in1; if (m_nch > 1) in1=
    #else
    double in1=
    #endif
    *spl1 * 0.015;
    #endif

    double out0=0.0;
    #ifndef WDL_REVERB_MONO
    double out1=0.0;
    #endif
    for (x = 0; x < sizeof(wdl_verb__combtunings)/sizeof(wdl_verb__combtunings[0]); x ++)
    {
      out0+=m_combs[x][0].process(in0);
      #ifndef WDL_REVERB_MONO
      WDL_REVERB_if_m_nch_gt_1 out1+=m_combs[x][1].process(in1);
      #endif
    }
    for (x = 0; x < sizeof(wdl_verb__allpasstunings)/sizeof(wdl_verb__allpasstunings[0]); x ++)
    {
      out0=m_allpasses[x][0].process(out0);
      #ifndef WDL_REVERB_MONO
      WDL_REVERB_if_m_nch_gt_1 out1=m_allpasses[x][1].process(out1);
      #endif
    }

  #ifdef WDL_REVERB_FIXED_WIDTH
    *spl0 = out0;
    #ifndef WDL_REVERB_MONO
    WDL_REVERB_if_m_nch_gt_1 *spl1 = out1;
    #endif
  #else
    #ifdef WDL_REVERB_ENABLE_SET_NCH
    if (m_nch == 1)
    {
      *spl0 = out0;
    }
    else
    #endif
    if (m_wid<0)
    {
      double m=-m_wid;
      *spl0 = out1*m + out0*(1.0-m);
      *spl1 = out0*m + out1*(1.0-m);
    }
    else
    {
      double m=m_wid;
      *spl0 = out0*m + out1*(1.0-m);
      *spl1 = out1*m + out0*(1.0-m);
    }
  #endif
  }

  void Reset(bool doclear=false) // call this after changing roomsize or dampening
  {
    int x;
    double sc=m_srate / 44100.0;
    for (x = 0; x < sizeof(wdl_verb__allpasstunings)/sizeof(wdl_verb__allpasstunings[0]); x ++)
    {
      m_allpasses[x][0].setsize((int) (wdl_verb__allpasstunings[x] * sc));
      #ifndef WDL_REVERB_MONO
      m_allpasses[x][1].setsize((int) ((wdl_verb__allpasstunings[x]+wdl_verb__stereospread) * sc));
      #endif
      m_allpasses[x][0].setfeedback(0.5);
      #ifndef WDL_REVERB_MONO
      m_allpasses[x][1].setfeedback(0.5);
      #endif
      if (doclear)
      {
        m_allpasses[x][0].Reset();
        #ifndef WDL_REVERB_MONO
        m_allpasses[x][1].Reset();
        #endif
      }
    }
    double damp=m_damp*0.4;
    if (damp > 0.0 && sc != 1.0) damp = exp(log(damp) / sc);
    for (x = 0; x < sizeof(wdl_verb__combtunings)/sizeof(wdl_verb__combtunings[0]); x ++)
    {
      m_combs[x][0].setsize((int) (wdl_verb__combtunings[x] * sc));
      #ifndef WDL_REVERB_MONO
      m_combs[x][1].setsize((int) ((wdl_verb__combtunings[x]+wdl_verb__stereospread) * sc));
      #endif
      m_combs[x][0].setfeedback(m_roomsize);
      #ifndef WDL_REVERB_MONO
      m_combs[x][1].setfeedback(m_roomsize);
      #endif
      m_combs[x][0].setdamp(damp);
      #ifndef WDL_REVERB_MONO
      m_combs[x][1].setdamp(damp);
      #endif
      if (doclear)
      {
        m_combs[x][0].Reset();
        #ifndef WDL_REVERB_MONO
        m_combs[x][1].Reset();
        #endif
      }
    }

  }

  void SetRoomSize(double sz) { m_roomsize=sz;; } // 0.3..0.99 or so
  void SetDampening(double dmp) { m_damp=dmp; } // 0..1

  double GetRoomSize() const { return m_roomsize; }
  double GetDampening() const { return m_damp; }

#ifndef WDL_REVERB_FIXED_WIDTH
  void SetWidth(double wid) 
  {  
    if (wid<-1) wid=-1; 
    else if (wid>1) wid=1; 
    wid*=0.5;
    if (wid>=0.0) wid+=0.5;
    else wid-=0.5;
    m_wid=wid;
  } // -1..1

  double GetWidth() const { return m_wid<0 ? 2*m_wid+1 : 2*m_wid-1; }
#endif

#ifdef WDL_REVERB_ENABLE_SET_NCH
  void SetNumChannels(int n) { m_nch = n; } // 1 or 2
  int GetNumChannels() const { return m_nch; }
#endif

private:
#ifndef WDL_REVERB_FIXED_WIDTH
  double m_wid;
#endif
  double m_roomsize;
  double m_damp;
  double m_srate;
#ifdef WDL_REVERB_ENABLE_SET_NCH
  int m_nch;
#endif
  WDL_ReverbAllpass m_allpasses[sizeof(wdl_verb__allpasstunings)/sizeof(wdl_verb__allpasstunings[0])][WDL_REVERB_NCH];
  WDL_ReverbComb m_combs[sizeof(wdl_verb__combtunings)/sizeof(wdl_verb__combtunings[0])][WDL_REVERB_NCH];

} WDL_FIXALIGN;


#endif

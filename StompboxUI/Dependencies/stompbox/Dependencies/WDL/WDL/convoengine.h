/*
  WDL - convoengine.h
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
  

  This file provides an interface to the WDL fast convolution engine. This engine can convolve audio streams using
  either brute force (for small impulses), or a partitioned FFT scheme (for larger impulses). 

  Note that this library needs to have lookahead ability in order to process samples. Calling Add(somevalue) may produce Avail() < somevalue.

*/


#ifndef _WDL_CONVOENGINE_H_
#define _WDL_CONVOENGINE_H_

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef WDL_CONVO_THREAD
#ifndef _WIN32
#include <pthread.h>
#endif
#include "mutex.h"
#endif

#include "queue.h"
#include "fastqueue.h"
#include "fft.h"

#ifdef WDL_CONVO_USE_CONST_HEAP_BUF // define this for const impulse buffer support, see WDL_ImpulseBuffer::Set()

#include "constheapbuf.h"
typedef WDL_ConstTypedBuf<WDL_FFT_REAL> WDL_CONVO_IMPULSE_TYPED_BUF;

#else

typedef WDL_TypedBuf<WDL_FFT_REAL> WDL_CONVO_IMPULSE_TYPED_BUF;

#endif

#if defined(WDL_CONVO_THREAD) && !defined(WDL_CONVO_MAX_PROC_NCH)
#define WDL_CONVO_MAX_PROC_NCH 2
#endif

//#define WDL_CONVO_WANT_FULLPRECISION_IMPULSE_STORAGE // define this for slowerness with -138dB error difference in resulting output (+-1 LSB at 24 bit)

#ifdef WDL_CONVO_WANT_FULLPRECISION_IMPULSE_STORAGE 

typedef WDL_FFT_REAL WDL_CONVO_IMPULSEBUFf;
typedef WDL_FFT_COMPLEX WDL_CONVO_IMPULSEBUFCPLXf;

#else
typedef float WDL_CONVO_IMPULSEBUFf;
typedef struct
{
  WDL_CONVO_IMPULSEBUFf re, im;
}
WDL_CONVO_IMPULSEBUFCPLXf;
#endif

class WDL_ImpulseBuffer
{
public:
  WDL_ImpulseBuffer()
  {
    samplerate=44100.0;
    impulses.list.Add(new WDL_CONVO_IMPULSE_TYPED_BUF);
  }
  ~WDL_ImpulseBuffer()
  {
    impulses.list.Empty(true);
  }

  int GetLength() { return impulses.list.GetSize() ? impulses[0].GetSize() : 0; }
  int SetLength(int samples); // resizes/clears all channels accordingly, returns actual size set (can be 0 if error)
  void SetNumChannels(int usench, bool duplicateExisting=true); // handles allocating/converting/etc
  int GetNumChannels() { return impulses.list.GetSize(); }

  void Set(const WDL_FFT_REAL** bufs, int samples, int usench); // call instead of SetLength() and SetNumChannels() to use const instead of heap buffer

  double samplerate; // TN: Not used, remove?
  struct  {
    WDL_PtrList<WDL_CONVO_IMPULSE_TYPED_BUF  > list;

    WDL_CONVO_IMPULSE_TYPED_BUF &operator[](size_t i) const
    {
      WDL_CONVO_IMPULSE_TYPED_BUF *p = list.Get(i);
      if (WDL_NORMALLY(p != NULL)) return *p;
      return *list.Get(0); // if for some reason an out of range index was passed, return first item rather than crash
    }
  } impulses;

};

class WDL_ConvolutionEngine
{
public:
  WDL_ConvolutionEngine();
  ~WDL_ConvolutionEngine();

  int SetImpulse(WDL_ImpulseBuffer *impulse, int fft_size=-1, int impulse_sample_offset=0, int max_imp_size=0, bool forceBrute=false);
 
  int GetFFTSize() { return m_fft_size; }
  int GetLatency() { return m_fft_size/2; }
  
  void Reset(); // clears out any latent samples

  void Add(WDL_FFT_REAL **bufs, int len, int nch);

  int Avail(int wantSamples);
  WDL_FFT_REAL **Get(); // returns length valid
  void Advance(int len);

private:

  struct ImpChannelInfo {
    WDL_TypedBuf<WDL_CONVO_IMPULSEBUFf> imp;
    WDL_TypedBuf<char> zflag;
  };

  struct ProcChannelInfo {
    WDL_Queue samplesout;
    WDL_Queue samplesin2;
    WDL_FastQueue samplesin;

    WDL_TypedBuf<WDL_FFT_REAL> samplehist; // FFT'd sample blocks per channel
    WDL_TypedBuf<char> samplehist_zflag;
    WDL_TypedBuf<WDL_FFT_REAL> overlaphist;

    int hist_pos;
  };


  WDL_PtrList<ImpChannelInfo> m_impdata;

  int m_impulse_len;
  int m_fft_size;

  int m_proc_nch;
  WDL_PtrList<ProcChannelInfo> m_proc;


  WDL_TypedBuf<WDL_FFT_REAL> m_combinebuf;
  WDL_TypedBuf<WDL_FFT_REAL *> m_get_tmpptrs;

public:

  // _div stuff
  int m_zl_delaypos;
  int m_zl_dumpage;

//#define WDLCONVO_ZL_ACCOUNTING
#ifdef WDLCONVO_ZL_ACCOUNTING
  int m_zl_fftcnt;//removeme (testing of benchmarks)
#endif
  void AddSilenceToOutput(int len);

} WDL_FIXALIGN;

// low latency version
class WDL_ConvolutionEngine_Div
{
public:
  WDL_ConvolutionEngine_Div();
  ~WDL_ConvolutionEngine_Div();

  int SetImpulse(WDL_ImpulseBuffer *impulse, int maxfft_size=0, int known_blocksize=0, int max_imp_size=0, int impulse_offset=0, int latency_allowed=0);

  int GetLatency();
  void Reset();

  void Add(WDL_FFT_REAL **bufs, int len, int nch);

  int Avail(int wantSamples);
  WDL_FFT_REAL **Get(); // returns length valid
  void Advance(int len);

private:
  WDL_PtrList<WDL_ConvolutionEngine> m_engines;

  WDL_PtrList<WDL_Queue> m_sout;
  WDL_TypedBuf<WDL_FFT_REAL *> m_get_tmpptrs;

  bool m_need_feedsilence;

} WDL_FIXALIGN;

#ifdef WDL_CONVO_THREAD // define for threaded low latency support
class WDL_ConvolutionEngine_Thread
{
public:
  WDL_ConvolutionEngine_Thread();
  ~WDL_ConvolutionEngine_Thread();

  int SetImpulse(WDL_ImpulseBuffer *impulse, int maxfft_size=0, int known_blocksize=0, int max_imp_size=0, int impulse_offset=0, int latency_allowed=0);

  int GetLatency() { return m_zl_engine.GetLatency(); }
  void Reset();

  void Add(WDL_FFT_REAL **bufs, int len, int nch);

  int Avail(int wantSamples);
  WDL_FFT_REAL **Get(); // returns length valid
  void Advance(int len);

  // threading isn't actually enabled/disabled until next SetImpulse() call
  void EnableThread(bool enable) { m_thread_enable = enable; }
  bool ThreadEnabled() const { return m_thread_enable; }

private:
  bool CreateThread();
  void CloseThread();

  WDL_ConvolutionEngine_Div m_zl_engine;
  WDL_ConvolutionEngine m_thread_engine;

  WDL_Queue m_samplesout[WDL_CONVO_MAX_PROC_NCH];
  WDL_Queue m_samplesout2[WDL_CONVO_MAX_PROC_NCH];
  WDL_FastQueue m_samplesin[WDL_CONVO_MAX_PROC_NCH];
  WDL_FastQueue m_samplesin2[WDL_CONVO_MAX_PROC_NCH];

  WDL_FFT_REAL *m_get_tmpptrs[WDL_CONVO_MAX_PROC_NCH];

  WDL_Mutex m_samplesout_lock, m_samplesin_lock;

  int m_proc_nch;

#ifdef _WIN32
  HANDLE m_thread, m_signal_thread, m_signal_main;
  static DWORD WINAPI ThreadProc(LPVOID lpParam);
#else
  pthread_t m_thread;

  pthread_cond_t m_signal_thread_cond, m_signal_main_cond;
  pthread_mutex_t m_signal_thread_mutex, m_signal_main_mutex;
  bool m_signal_thread, m_signal_main;

  static void *ThreadProc(void *lpParam);
#endif
  bool m_thread_enable, m_thread_state;

  bool m_need_feedsilence;

} WDL_FIXALIGN;
#endif // WDL_CONVO_THREAD


#endif

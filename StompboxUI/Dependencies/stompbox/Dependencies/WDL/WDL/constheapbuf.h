/*
  WDL - constheapbuf.h
  (c) Theo Niessink 2014
  <http://www.taletn.com/>

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
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


  This file provides a minimalistic (hint: could be extended) interface to
  either a const or a heap buffer.

*/

#ifndef _WDL_CONSTHEAPBUF_H_
#define _WDL_CONSTHEAPBUF_H_

#include "heapbuf.h"

class WDL_ConstHeapBuf
{
public:
	WDL_ConstHeapBuf(): m_size(0) {}

	void* Get() const { return m_size ? (void*)m_ptr : m_hb.Get(); }
	int GetSize() const { return m_size ? m_size : m_hb.GetSize(); }

	void* Resize(int newsize, bool resizedown = true)
	{
		m_size = 0;
		return m_hb.Resize(newsize, resizedown);
	}

	void Set(const void* ptr, int size)
	{
		m_size = size;
		m_ptr = ptr;
	}

private:
	WDL_HeapBuf m_hb;

	const void* m_ptr;
	int m_size;
};

template <class PTRTYPE> class WDL_ConstTypedBuf
{
public:
	PTRTYPE* Get() const { return (PTRTYPE*)m_hb.Get(); }
	int GetSize() const { return m_hb.GetSize() / (unsigned int)sizeof(PTRTYPE); }

	PTRTYPE* Resize(int newsize, bool resizedown = true) { return (PTRTYPE*)m_hb.Resize(newsize * sizeof(PTRTYPE), resizedown); }

	void Set(const PTRTYPE* ptr, int size) { m_hb.Set(ptr, size * sizeof(PTRTYPE)); }

private:
	WDL_ConstHeapBuf m_hb;
};

#endif // _WDL_CONSTHEAPBUF_H_

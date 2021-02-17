#include "stdafx.h"
#include "ScopePoolObj.h"
#ifdef __TBB_malloc_proxy
#include "tbb/tbbmalloc_proxy.h"
#else
#include "tcmalloc.h"
#endif


CScopePoolObj::CScopePoolObj(size_t size_) :
	m_size(size_)
{
#ifdef __TBB_malloc_proxy
	m_buffer = new char[size_];
#else
	m_buffer = tc_malloc(size_);
#endif
	if (m_buffer == NULL)
	{
		// ·Î±×
	}
}


CScopePoolObj::~CScopePoolObj()
{
#ifdef __TBB_malloc_proxy
	delete[] m_buffer;
#else
	tc_free(m_buffer);
#endif
}

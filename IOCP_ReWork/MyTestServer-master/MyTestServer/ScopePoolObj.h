#pragma once
#include <stdlib.h>


///////////////////////////////////////////////////////////////////////////////
// 오브젝트 풀을 스코프 영역에서 1회성으로 사용하기 위한 클래스
///////////////////////////////////////////////////////////////////////////////
class CScopePoolObj
{
private:
	void* m_buffer;     ///< 오브젝트 풀에서 꺼내올 버퍼    
	size_t m_size;      ///< 버퍼 사이즈

public:
	/// 버퍼를 가지고 온다.
	inline void* GetBuffer() { return m_buffer; }

public:
	CScopePoolObj(size_t size_);
	~CScopePoolObj();
};
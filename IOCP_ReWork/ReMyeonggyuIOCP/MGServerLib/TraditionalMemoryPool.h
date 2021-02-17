#pragma once

#include <list>
#include "Synchronized.h"

// 이 풀은 32, 64, 128, 256, 512, 1024, 1536, 2048, 3072, 4096 크기의 메모리를 관리한다.
//	요청한 메모리블럭의 크기가 33이라면 64크기짜리 메모리를 리턴하게 된다는 의미이다.
class TraditionalMemoryPool
{
public:
	TraditionalMemoryPool(void) = default;

	virtual ~TraditionalMemoryPool(void)
	{
		p_0032.clear();
		p_0064.clear();
		p_0128.clear();
		p_0256.clear();
		p_0512.clear();
		p_1024.clear();
		p_1536.clear();
		p_2048.clear();
		p_3072.clear();
		p_4096.clear();
		p_8192.clear();
	}

	void* Retreive(int size)
	{
		if (0 >= size) {
			return 0;
		}

		Synchronized synch(&m_EntireSynch);
		
		if (32 >= size)
		{
			auto iter = p_0032.begin();

			if (iter != p_0032.end())
			{
				void* p = *iter;
				p_0032.erase(iter);
				return p;
			}
			
			char* cp = new char[32];
			return (void*)cp;
		}
		else if (64 >= size)
		{
			auto iter = p_0064.begin();
			
			if (iter != p_0064.end())
			{
				void* p = *iter;
				p_0064.erase(iter);
				return p;
			}

			char* cp = new char[64];
			return (void*)cp;
		}
		else if (128 >= size)
		{
			std::list<void*>::iterator iter = p_0128.begin();
			if (iter != p_0128.end())
			{
				void* p = *iter;
				p_0128.erase(iter);
				return p;
			}
			
			char* cp = new char[128];
			return (void*)cp;
		}
		else if (256 >= size)
		{
			auto iter = p_0256.begin();
			
			if (iter != p_0256.end())
			{
				void* p = *iter;
				p_0256.erase(iter);
				return p;
			}

			char* cp = new char[256];
			return (void*)cp;
		}
		else if (512 >= size)
		{
			auto iter = p_0512.begin();
			
			if (iter != p_0512.end())
			{
				void* p = *iter;
				p_0512.erase(iter);
				return p;
			}

			char* cp = new char[512];
			return (void*)cp;
		}
		else if (1024 >= size)
		{
			auto iter = p_1024.begin();

			if (iter != p_1024.end())
			{
				void* p = *iter;
				p_1024.erase(iter);
				return p;
			}
			char* cp = new char[1024];
			return (void*)cp;
		}
		else if (1536 >= size)
		{
			auto iter = p_1536.begin();

			if (iter != p_1536.end())
			{
				void* p = *iter;
				p_1536.erase(iter);
				return p;
			}

			char* cp = new char[1536];
			return (void*)cp;
		}
		else if (2048 >= size)
		{
			auto iter = p_2048.begin();

			if (iter != p_2048.end())
			{
				void* p = *iter;
				p_2048.erase(iter);
				return p;
			}
			char* cp = new char[2048];
			return (void*)cp;
		}
		else if (3072 >= size)
		{
			auto iter = p_3072.begin();

			if (iter != p_3072.end())
			{
				void* p = *iter;
				p_3072.erase(iter);
				return p;
			}
			char* cp = new char[3072];
			return (void*)cp;
		}
		else if (4096 >= size)
		{
			auto iter = p_4096.begin();

			if (iter != p_4096.end())
			{
				void* p = *iter;
				p_4096.erase(iter);
				return p;
			}
			
			char* cp = new char[4096];
			return (void*)cp;
		}
		else if (8192 >= size)
		{
			auto iter = p_8192.begin();

			if (iter != p_8192.end())
			{
				void* p = *iter;
				p_8192.erase(iter);
				return p;
			}
			
			char* cp = new char[8192];
			return (void*)cp;
		}

		return 0;
	}


	void Release(void* p, int size)
	{
		if (0 >= size || 0 == p) {
			return;
		}

		Synchronized synch(&m_EntireSynch);

		if (32 >= size)
		{
			p_0032.push_back(p);
		}
		else if (64 >= size)
		{
			p_0064.push_back(p);
		}
		else if (128 >= size)
		{
			p_0128.push_back(p);
		}
		else if (256 >= size)
		{
			p_0256.push_back(p);
		}
		else if (512 >= size)
		{
			p_0512.push_back(p);
		}
		else if (1024 >= size)
		{
			p_1024.push_back(p);
		}
		else if (1536 >= size)
		{
			p_1536.push_back(p);
		}
		else if (2048 >= size)
		{
			p_2048.push_back(p);
		}
		else if (3072 >= size)
		{
			p_3072.push_back(p);
		}
		else if (4096 >= size)
		{
			p_4096.push_back(p);
		}
		else if (8192 >= size)
		{
			p_8192.push_back(p);
		}
	}


private:
	std::list<void*>	p_0032;
	std::list<void*>	p_0064;
	std::list<void*>	p_0128;
	std::list<void*>	p_0256;
	std::list<void*>	p_0512;
	std::list<void*>	p_1024;
	std::list<void*>	p_1536;
	std::list<void*>	p_2048;
	std::list<void*>	p_3072;
	std::list<void*>	p_4096;
	std::list<void*>	p_8192;

	CriticalSectionLockWrapper	m_EntireSynch;
};

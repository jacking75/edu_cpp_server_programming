#pragma once

#include <deque>
#include <cstdint>

class UniqueSeqNumGenerator
{
public:
	typedef std::deque<uint64_t>			tREUSELIST;
	typedef std::deque<uint64_t>::iterator	tREUSELISTITER;
	static const uint64_t cDefaultStart = 0;
	static const uint64_t cMinMultiple = 1;
	static const uint64_t cMaxMultiple = 10000;


public:
	UniqueSeqNumGenerator(uint64_t start=cDefaultStart,
		uint64_t multiple=cMinMultiple) :
														reuseList(), 
														currentNumber(start), 
														multiple(multiple)
	{
		if (cMinMultiple > multiple || cMaxMultiple < multiple) {
			throw "multiple error";
		}
	}

	virtual ~UniqueSeqNumGenerator()
	{
		reset();
	}

	void reset(uint64_t start=cDefaultStart)
	{
		reuseList.clear();
		currentNumber = start;
	}

	uint64_t retrieve()
	{
		if(reuseList.empty())
		{
			currentNumber += multiple;
			return currentNumber;
		}

		uint64_t n = reuseList.front();
		reuseList.erase(reuseList.begin());
		return n;
	}

	bool release(uint64_t n, bool bValid=false)
	{
		if(bValid)
		{
			for(auto iter = reuseList.begin() ; iter != reuseList.end() ; ++iter )
			{
				if (n == *iter) {
					return false;
				}
			}			
		}

		reuseList.push_back(n);
		return true;
	}


private:
	tREUSELIST		reuseList;
	uint64_t		multiple;
	uint64_t		currentNumber;
};


//class RandomGenerator
//{
//	unsigned int		w;
//	unsigned int		z;
//public:
//
//	RandomGenerator(unsigned int sw, unsigned int sz) : w(sw), z(sz)
//	{
//	}
//
//	~RandomGenerator()
//	{
//	}
//
//	unsigned int Get()
//	{
//		z = 36969 * (z & 65535) + (z >> 16);
//		w = 18000 * (w & 65535) + (w >> 16);
//		return (z << 16) + w;
//	}
//
//};


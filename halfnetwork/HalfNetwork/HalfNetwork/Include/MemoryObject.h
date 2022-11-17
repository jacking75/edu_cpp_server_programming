#ifndef __memoryobject_h__
#define __memoryobject_h__

#pragma once

#include "PoolStrategy.h"

namespace HalfNetwork
{

	typedef ACE_Singleton<FlexibleSizePoolT<MemoryBlockPool, void>, ACE_Thread_Mutex> FlexibleSizePoolSingleton;
	#define MemoryPoolInstance FlexibleSizePoolSingleton::instance()

	class MemoryObject
	{
	public:
		virtual ~MemoryObject() {}

	#ifdef _MSC_VER
		static void * operator new ( size_t size )
	#else
		static void * operator new ( size_t size ) throw ( std::bad_alloc )
	#endif
		{
			return MemoryPoolInstance->Allocate(size);
		}

		static void * operator new ( size_t size, const std::nothrow_t & ) throw ()
		{
			//return MemoryPoolInstance->Allocate(size);
			return ::operator new( size );
		}

		inline static void * operator new ( size_t size, void * place )
		{
			return ::operator new( size, place );
		}

		static void operator delete ( void * p, size_t size ) throw ()
		{
			return MemoryPoolInstance->Deallocate(p, size);
		}

		static void operator delete ( void * p, const std::nothrow_t & ) throw()
		{
			::operator delete ( p );
		}

		inline static void operator delete ( void * p, void * place )
		{
			::operator delete ( p, place );
		}

	protected:
		inline MemoryObject( void ) {}

	private:
		/// Copy-constructor is not implemented.
		MemoryObject( const MemoryObject & );
		/// Copy-assignment operator is not implemented.
		MemoryObject & operator = ( const MemoryObject & );
	}; 

} // namespace HalfNetwork

#endif // __memoryobject_h__
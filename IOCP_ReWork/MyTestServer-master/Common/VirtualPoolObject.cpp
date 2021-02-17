#include "stdafx.h"
#include "VirtualPoolObject.h"


bool CVirtualPoolObject::SetUnAvailableState()
{
	if (m_bUsed)
	{
		return false;
	}
	m_bUsed = true;

	return true;	
	//return (m_bUsed.compare_and_swap(true, false) == false);
}


bool CVirtualPoolObject::SetAvailableState()
{

	if (!m_bUsed)
	{
		return false;
	}
	m_bUsed = false;

	return true;
	//return (m_bUsed.compare_and_swap(false, true) == true);
}


bool CVirtualPoolObject::IsUsed() const
{
	return m_bUsed;
}


bool CVirtualPoolObject::Leave(void* initParam_)
{
	// Don't call this function.
	return true;
}


bool CVirtualPoolObject::Enter()
{
	// Don't call this function.
	return true;
}


CVirtualPoolObject::CVirtualPoolObject()
	: m_bUsed(false)
{
}


CVirtualPoolObject::~CVirtualPoolObject()
{
}

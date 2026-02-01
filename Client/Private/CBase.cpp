#include "CBase.h"

CBase::CBase()
{
}

uint CBase::AddRef()
{
	return ++m_iRefCnt;
}

uint CBase::Release()
{
	if (m_iRefCnt != 0)
	{
		return m_iRefCnt--;
	}
	else
	{
		Free();
		delete this;
		return 0;
	}
}

void CBase::Free()
{
}

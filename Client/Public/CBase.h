#pragma once

#include "Defines.h"

BEGIN(Client)

class CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	uint AddRef();
	uint Release();
	uint GetRef() { return m_iRefCnt; }

protected:
	uint	m_iRefCnt = { 0 };

public:
	virtual void Free();
};

END
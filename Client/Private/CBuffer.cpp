#include "CBuffer.h"

CBuffer::CBuffer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice{ _pDevice }, m_pContext{ _pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CBuffer::Initialize()
{
	m_iMaxParticle = 2 << 24;
	m_iEmitNum = 100;//100000;

	return S_OK;
}

void CBuffer::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

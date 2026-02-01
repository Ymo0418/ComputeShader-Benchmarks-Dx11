#include "CEmitter.h"
#include "CBuffer.h"
#include "CShader.h"
#include "CCamera.h"

CEmitter::CEmitter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice{ _pDevice }, m_pContext{ _pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

void CEmitter::Update(float _fTimeDelta)
{
    Ready_PerFrameInfo(_fTimeDelta);

    m_pBuffer->Update(_fTimeDelta);
}

void CEmitter::Bind_UI(void* _pData)
{
    m_pBuffer->Bind_UI(_pData);
}

void CEmitter::Free()
{
    Safe_Release(m_pPerFrameInfo_Buffer);
    Safe_Release(m_pShader);
    Safe_Release(m_pBuffer);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
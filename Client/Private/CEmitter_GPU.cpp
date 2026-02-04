#include "CEmitter_GPU.h"
#include "CBuffer_GPU.h"
#include "CShader.h"
#include "CCamera.h"

CEmitter_GPU::CEmitter_GPU(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CEmitter{ _pDevice, _pContext }
{
}

HRESULT CEmitter_GPU::Initialize()
{
    CHECK_NULPTR((m_pShader = CShader::Create(m_pDevice, m_pContext, 
        TEXT("../ShaderFiles/Shader_GPU.hlsl"), PARTICLE_DATA::Elements, PARTICLE_DATA::iNumElement)));
    CHECK_NULPTR((m_pBuffer = CBuffer_GPU::Create(m_pDevice, m_pContext)));

    D3D11_BUFFER_DESC Desc;
    ZeroMemory(&Desc, sizeof Desc);
    Desc.Usage = D3D11_USAGE_DYNAMIC;
    Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Desc.ByteWidth = sizeof(m_PerFrameInfo);
    CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, &m_pPerFrameInfo_Buffer));

    D3D11_SAMPLER_DESC DescSampler{};
    DescSampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    DescSampler.AddressU = DescSampler.AddressV = DescSampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    DescSampler.MaxAnisotropy = 1;
    DescSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    DescSampler.MaxLOD = D3D11_FLOAT32_MAX;
    CHECK_FAILED(m_pDevice->CreateSamplerState(&DescSampler, &m_pSampler));

    return S_OK;
}

HRESULT CEmitter_GPU::Render()
{
    CHECK_FAILED(m_pShader->Begin(0));

    m_pContext->VSSetConstantBuffers(0, 1, &m_pPerFrameInfo_Buffer);
    m_pContext->PSSetConstantBuffers(0, 1, &m_pPerFrameInfo_Buffer);
    m_pContext->GSSetConstantBuffers(0, 1, &m_pPerFrameInfo_Buffer);
    m_pContext->CSSetSamplers(0, 1, &m_pSampler);

    CHECK_FAILED(m_pBuffer->Render());

    return S_OK;
}

HRESULT CEmitter_GPU::Ready_PerFrameInfo(float _fTimeDelta)
{
    m_PerFrameInfo.fTimeDelta = _fTimeDelta;

    const float fAccumLimit = 10.f;
    if ((m_PerFrameInfo.fAccumTime += m_PerFrameInfo.fTimeDelta) > fAccumLimit)
        m_PerFrameInfo.fAccumTime -= fAccumLimit;

    CCamera::GetInstance()->Get_CameraPos(&m_PerFrameInfo.vCamPos);
    CCamera::GetInstance()->Get_CameraMat(&m_PerFrameInfo.matView, CCamera::MAT_VIEW);
    CCamera::GetInstance()->Get_CameraMat(&m_PerFrameInfo.matProj, CCamera::MAT_PROJ);

    D3D11_MAPPED_SUBRESOURCE MappedResource;
    if (FAILED(m_pContext->Map(m_pPerFrameInfo_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)))
        __debugbreak();

    memcpy(MappedResource.pData, &m_PerFrameInfo, sizeof PER_FRAME_INFO);

    m_pContext->Unmap(m_pPerFrameInfo_Buffer, 0);

    m_pContext->CSSetConstantBuffers(0, 1, &m_pPerFrameInfo_Buffer);
    m_pContext->CSSetSamplers(0, 1, &m_pSampler);

    return S_OK;
}

CEmitter_GPU* CEmitter_GPU::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CEmitter_GPU* pInstance = new CEmitter_GPU(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CEmitter_GPU");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEmitter_GPU::Free()
{
    __super::Free();

    Safe_Release(m_pSampler);
}
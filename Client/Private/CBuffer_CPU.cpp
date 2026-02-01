#include "CBuffer_CPU.h"

CBuffer_CPU::CBuffer_CPU(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CBuffer{ _pDevice ,_pContext }
{
}

HRESULT CBuffer_CPU::Initialize()
{
	CHECK_FAILED(__super::Initialize());

	{
		m_tConstInfos.vStartColor = { 1.f, 1.f, 1.f, 1.f };
		m_tConstInfos.vEndColor = { 1.f, 1.f, 1.f, 1.f };
		m_tConstInfos.fLifeTimeFrom = 1.f;
		m_tConstInfos.fLifeTimeTo = 4.f;
		m_tConstInfos.fScale = 0.4f;
		m_tConstInfos.fSpeed = 8.f;
		m_tConstInfos.vPivot = { 0.f, 0.f, 0.f };
		m_tConstInfos.vRange = { 10.f, 10.f, 10.f };
	}

	D3D11_BUFFER_DESC Desc{};
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.ByteWidth = sizeof(PARTICLE) * m_iMaxParticle;
	Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, &m_pVertex_Buffer));

	m_Particles = new PARTICLE[m_iMaxParticle];

	return S_OK;
}

void CBuffer_CPU::Update(float _fTimeDelta)
{
	for (uint i = 0; i < m_iLiveParticle;)
	{
		m_Particles[i].vLifeTime.y -= _fTimeDelta;
		
		if (m_Particles[i].vLifeTime.y <= 0.f)
		{
			m_Particles[i] = m_Particles[m_iLiveParticle - 1];
			--m_iLiveParticle;
		}
		else
		{
			float fLifeRatio = 1.f - (m_Particles[i].vLifeTime.y / m_Particles[i].vLifeTime.x);
			
			XMStoreFloat4(&m_Particles[i].vColor, XMVectorLerp(m_tConstInfos.vStartColor, m_tConstInfos.vEndColor, fLifeRatio));

			XMStoreFloat3(&m_Particles[i].vWorldPos,
				XMLoadFloat3(&m_Particles[i].vWorldPos) + XMLoadFloat3(&m_Particles[i].vWorldVelocity) * _fTimeDelta);
			++i;
		}		
	}

	Update_Emit(_fTimeDelta);
}

HRESULT CBuffer_CPU::Render()
{
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pContext->Map(m_pVertex_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	memcpy(MappedResource.pData, m_Particles, sizeof(PARTICLE) * m_iLiveParticle);

	m_pContext->Unmap(m_pVertex_Buffer, 0);

	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	UINT stride = sizeof PARTICLE;
	UINT offset = 0;
	m_pContext->IASetVertexBuffers(0, 1, &m_pVertex_Buffer, &stride, &offset);
	m_pContext->DrawInstanced(1, m_iLiveParticle, 0, 0);

	return S_OK;
}

void CBuffer_CPU::Update_Emit(float _fTimeDelta)
{
	m_vEmitTimer.y -= _fTimeDelta;

	if (m_vEmitTimer.y <= 0.f)
	{
		m_vEmitTimer.y = m_vEmitTimer.x;

#ifdef _DEBUG
		if (m_iLiveParticle + m_iEmitNum >= m_iMaxParticle)
		{
			/* ----- Over Emitting ----- */
			__debugbreak();
		}
#endif // _DEBUG

		for (uint i = 0; i < m_iEmitNum; ++i)
		{
			int Idx = m_iLiveParticle + i;

			XMStoreFloat4(&m_Particles[Idx].vColor, m_tConstInfos.vStartColor);

			random_device					 randomDevice;
			mt19937							 gen(randomDevice());
			uniform_real_distribution<float> dis(-1.f, 1.f);

			_vector vPos = { 0.1f, 0.1f, 0.1f, 1.f };
			//_vector vPos = {
			//	(((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f) * m_tConstInfos.vRange.x,
			//	(((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f) * m_tConstInfos.vRange.y,
			//	(((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f) * m_tConstInfos.vRange.z, 1.f };
			XMStoreFloat3(&m_Particles[Idx].vWorldPos, vPos);

			_vector vVel = XMVector3Normalize(vPos - m_tConstInfos.vPivot) * m_tConstInfos.fSpeed;
			XMStoreFloat3(&m_Particles[Idx].vWorldVelocity, vVel);

			float fLifeTime = rand() / (float)RAND_MAX;
			fLifeTime = fLifeTime * (m_tConstInfos.fLifeTimeTo - m_tConstInfos.fLifeTimeFrom) + m_tConstInfos.fLifeTimeFrom;
			XMStoreFloat2(&m_Particles[Idx].vLifeTime, { fLifeTime, fLifeTime });

			m_Particles[Idx].fScale = m_tConstInfos.fScale;
		}

		m_iLiveParticle += m_iEmitNum;
	}
}

void CBuffer_CPU::Bind_UI(void* _pData)
{
	const uint** pData = static_cast<const uint**>(_pData);

	pData[0] = &m_iMaxParticle;
	pData[1] = &m_iLiveParticle;
}

CBuffer_CPU* CBuffer_CPU::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBuffer_CPU* pInstance = new CBuffer_CPU(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CBuffer_CPU");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBuffer_CPU::Free()
{
	__super::Free();

	Safe_Delete_Array(m_Particles);
	Safe_Release(m_pVertex_Buffer);
}
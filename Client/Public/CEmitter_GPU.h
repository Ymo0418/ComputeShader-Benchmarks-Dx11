#pragma once
#include "CEmitter.h"

BEGIN(Client)

class CEmitter_GPU : public CEmitter
{
private:
	typedef struct tagPerFrameInfo
	{
		_float4x4	matView;
		_float4x4	matProj;
		_float4x4	matViewProj;
		_float4		vCamPos;
		float		fAccumTime;
		float		fTimeDelta;
		_float2		PerFrameInfo_Pad;
	}PER_FRAME_INFO;

protected:
	CEmitter_GPU(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CEmitter_GPU() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Render() override;

protected:
	virtual HRESULT Ready_PerFrameInfo(float _fTimeDelta) override;

protected:
	ID3D11SamplerState*		m_pSampler				= { nullptr };
	PER_FRAME_INFO			m_PerFrameInfo			= {};

public:
	static CEmitter_GPU* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END
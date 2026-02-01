#pragma once
#include "CEmitter.h"

BEGIN(Client)

class CEmitter_CPU : public CEmitter
{
private:
	typedef struct tagPerFrameInfo
	{
		_float4x4	matView;
		_float4x4	matProj;
	}PER_FRAME_INFO;

protected:
	CEmitter_CPU(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CEmitter_CPU() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Render() override;
	
protected:
	virtual HRESULT Ready_PerFrameInfo(float _fTimeDelta) override;

protected:
	PER_FRAME_INFO			m_PerFrameInfo			= {};

public:
	static CEmitter_CPU* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END
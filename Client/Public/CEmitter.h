#pragma once
#include "CBase.h"

BEGIN(Client)

class CEmitter : public CBase
{
protected:
	CEmitter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CEmitter() = default;

public:
	virtual HRESULT Initialize() = 0;
	virtual void	Update(float _fTimeDelta);
	virtual HRESULT Render() = 0;
	virtual void	Bind_UI(void* _pData);

protected:
	virtual HRESULT Ready_PerFrameInfo(float _fTimeDelta) = 0;

protected:
    ID3D11Device*			m_pDevice				= { nullptr };
    ID3D11DeviceContext*	m_pContext				= { nullptr };

	ID3D11Buffer*			m_pPerFrameInfo_Buffer	= { nullptr };

	class CShader*			m_pShader				= { nullptr };
	class CBuffer*			m_pBuffer				= { nullptr };

public:
	virtual void Free();
};

END
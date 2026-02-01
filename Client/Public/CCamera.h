#pragma once
#include "CBase.h"

BEGIN(Client)

class CCamera : public CBase
{
public:
	enum MATRIX_STATE { MAT_VIEW, MAT_PROJ, MAT_END };

private:
	static CCamera* m_pInstance;

public:
	static CCamera* GetInstance();
	static uint		DestroyInstance();

private:
	CCamera()							 = default;
	virtual ~CCamera()					 = default;
	CCamera(const CCamera&)				 = delete;
	CCamera& operator = (const CCamera&) = delete;

public:
	virtual HRESULT Initialize();
	virtual void	Update(float _fTimeDelta);
	virtual HRESULT Render();

	void			Get_CameraPos(_float4* _pOut);
	void			Get_CameraMat(_float4x4* _pOut, MATRIX_STATE _eState);

protected:	
	_vector					m_vEye			= { };
	_vector					m_vAt			= { };
	float					m_fFovy			= { 0.f };
	float					m_fAspect		= { 0.f };
	float					m_fNear			= { 0.f };
	float					m_fFar			= { 0.f };
	
	_float4x4				m_matFormation[MAT_END]		= {};

public:
	virtual void Free();
};

END
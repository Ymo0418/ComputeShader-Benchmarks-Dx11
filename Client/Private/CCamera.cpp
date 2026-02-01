#include "CCamera.h"
#include "CInput_Device.h"

CCamera* CCamera::m_pInstance = nullptr;
CCamera* CCamera::GetInstance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = DBG_NEW CCamera;
		m_pInstance->Initialize();
	}
	return m_pInstance;
}
uint CCamera::DestroyInstance()
{
	uint iRefCnt = { 0 };
	if (nullptr != m_pInstance) 
	{
		iRefCnt = m_pInstance->Release();
		if (0 == iRefCnt)
			m_pInstance = nullptr;
	}

	return iRefCnt;
}

HRESULT CCamera::Initialize() 
{
	m_vEye = { 0.f, 50.f, -150.f, 1.f };
	m_vAt  = { 0.f, 0.f, 0.f, 1.f };
	m_fFovy = XMConvertToRadians(60.f);
	m_fAspect = (float)g_iWinSizeX / g_iWinSizeY;
	m_fNear = 0.1f;
	m_fFar = 1000.f;

	_float4x4 m_matWorld = {};

	_vector vLook = m_vAt - m_vEye;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);
	
	XMStoreFloat4((_float4*)(&(m_matWorld.m[0][0])), XMVector3Normalize(vRight));
	XMStoreFloat4((_float4*)(&(m_matWorld.m[1][0])), XMVector3Normalize(vUp));
	XMStoreFloat4((_float4*)(&(m_matWorld.m[2][0])), XMVector3Normalize(vLook));
	XMStoreFloat4((_float4*)(&(m_matWorld.m[3][0])), m_vEye);

	XMStoreFloat4x4(&m_matFormation[MAT_VIEW], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_matWorld)));
	XMStoreFloat4x4(&m_matFormation[MAT_PROJ], XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));

	return S_OK;
}

void CCamera::Update(float _fTimeDelta) 
{
	if (CInput_Device::GetInstance()->Get_DIKey(DIK_W))
		m_vEye = XMVectorSetZ(m_vEye, XMVectorGetZ(m_vEye) + 1);
	if (CInput_Device::GetInstance()->Get_DIKey(DIK_S))
		m_vEye = XMVectorSetZ(m_vEye, XMVectorGetZ(m_vEye) - 1);
	if (CInput_Device::GetInstance()->Get_DIKey(DIK_A))
		m_vEye = XMVectorSetY(m_vEye, XMVectorGetY(m_vEye) + 1);
	if (CInput_Device::GetInstance()->Get_DIKey(DIK_D))
		m_vEye = XMVectorSetY(m_vEye, XMVectorGetY(m_vEye) - 1);

	_float4x4 m_matWorld = {};

	_vector vLook = m_vAt - m_vEye;
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	XMStoreFloat4((_float4*)(&(m_matWorld.m[0][0])), XMVector3Normalize(vRight));
	XMStoreFloat4((_float4*)(&(m_matWorld.m[1][0])), XMVector3Normalize(vUp));
	XMStoreFloat4((_float4*)(&(m_matWorld.m[2][0])), XMVector3Normalize(vLook));
	XMStoreFloat4((_float4*)(&(m_matWorld.m[3][0])), m_vEye);

	XMStoreFloat4x4(&m_matFormation[MAT_VIEW], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_matWorld)));
	XMStoreFloat4x4(&m_matFormation[MAT_PROJ], XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Get_CameraPos(_float4* _pOut)
{
	XMStoreFloat4(_pOut, m_vEye);
}

void CCamera::Get_CameraMat(_float4x4* _pOut, MATRIX_STATE _eState)
{
	XMStoreFloat4x4(_pOut, XMMatrixTranspose(XMLoadFloat4x4(&m_matFormation[_eState])));
}

void CCamera::Free()
{
}
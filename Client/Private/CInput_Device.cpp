#include "CInput_Device.h"

CInput_Device* CInput_Device::m_pInstance = nullptr;
CInput_Device* CInput_Device::GetInstance()
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = DBG_NEW CInput_Device;
		m_pInstance->Initialize();
	}
	return m_pInstance;
}
uint CInput_Device::DestroyInstance()
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

HRESULT CInput_Device::Initialize()
{
	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(g_hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK, nullptr)))
		return E_FAIL;


	// 키보드 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;


	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	m_pKeyBoard->SetCooperativeLevel(g_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();


	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(g_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pMouse->Acquire();

	return S_OK;
}

void CInput_Device::Update_InputDev()
{
	memcpy(m_byKeyStateBefore, m_byKeyState, sizeof(m_byKeyState));
	memcpy(&m_tMouseStaetBefore, &m_tMouseState, sizeof(m_tMouseState));

	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
}

bool CInput_Device::Get_DIKey(ubyte _byKeyID)
{
	return (m_byKeyState[_byKeyID] & 0x80);
}
bool CInput_Device::Get_DIKeyUp(ubyte _byKeyID)
{
	return (!(m_byKeyState[_byKeyID] & 0x80) && (m_byKeyStateBefore[_byKeyID] & 0x80));
}
bool CInput_Device::Get_DIKeyDown(ubyte _byKeyID)
{
	return ((m_byKeyState[_byKeyID] & 0x80) && !(m_byKeyStateBefore[_byKeyID] & 0x80));
}
bool CInput_Device::Get_DIKeyPressing(ubyte _byKeyID)
{
	return ((m_byKeyState[_byKeyID] & 0x80) && (m_byKeyStateBefore[_byKeyID] & 0x80));
}

long CInput_Device::Get_DIMouseMove(MOUSEMOVESTATE _eMouseState)
{
	return *(((long*)&m_tMouseState) + _eMouseState);
}
bool CInput_Device::Get_DIMouseUp(MOUSEKEYSTATE _eMouse)
{
	return (!(m_tMouseState.rgbButtons[_eMouse] & 0x80) && (m_tMouseStaetBefore.rgbButtons[_eMouse] & 0x80));
}
bool CInput_Device::Get_DIMouseDown(MOUSEKEYSTATE _eMouse)
{
	return ((m_tMouseState.rgbButtons[_eMouse] & 0x80) && !(m_tMouseStaetBefore.rgbButtons[_eMouse] & 0x80));
}
bool CInput_Device::Get_DIMousePressing(MOUSEKEYSTATE _eMouse)
{
	return ((m_tMouseState.rgbButtons[_eMouse] & 0x80) && (m_tMouseStaetBefore.rgbButtons[_eMouse] & 0x80));
}

void CInput_Device::Free()
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}
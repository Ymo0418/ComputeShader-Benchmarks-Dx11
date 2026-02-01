#pragma once
#include "CBase.h"

BEGIN(Client)

class CInput_Device final : public CBase
{
private:
	static CInput_Device*	m_pInstance;

public:
	static CInput_Device*	GetInstance();
	static uint				DestroyInstance();

private:
	CInput_Device()										= default;
	virtual ~CInput_Device()							= default;
	CInput_Device(const CInput_Device&)					= delete;
	CInput_Device& operator = (const CInput_Device&)	= delete;

public:
	HRESULT Initialize();
	void	Update_InputDev();

	/* KeyState */
	bool Get_DIKey(ubyte _byKeyID);
	bool Get_DIKeyUp(ubyte _byKeyID);
	bool Get_DIKeyDown(ubyte _byKeyID);
	bool Get_DIKeyPressing(ubyte _byKeyID);

	/* MouseState */
	long Get_DIMouseMove(MOUSEMOVESTATE _eMouseState);
	bool Get_DIMouseUp(MOUSEKEYSTATE _eMouse);
	bool Get_DIMouseDown(MOUSEKEYSTATE _eMouse);
	bool Get_DIMousePressing(MOUSEKEYSTATE _eMouse);

private:
	LPDIRECTINPUT8			m_pInputSDK				= { nullptr };

	LPDIRECTINPUTDEVICE8	m_pKeyBoard				= { nullptr };
	byte					m_byKeyStateBefore[256] = {};
	byte					m_byKeyState[256]		= {};

	LPDIRECTINPUTDEVICE8	m_pMouse				= { nullptr };
	DIMOUSESTATE			m_tMouseStaetBefore		= {};
	DIMOUSESTATE			m_tMouseState			= {};

public:
	virtual void Free(void);
};

END
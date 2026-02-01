#pragma once

#pragma warning (disable : 4251)
#pragma warning (disable : 4251)

#include <d3d11.h>
#include <d3dcompiler.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "DirectXTK\DDSTextureLoader.h"
#include "DirectXTK\WICTextureLoader.h"
#include "DirectXTK\VertexTypes.h"
#include "DirectXTK\PrimitiveBatch.h"
#include "DirectXTK\SpriteBatch.h"
#include "DirectXTK\SpriteFont.h"
#include "DirectXTK\Effects.h"
#include "DirectXTK\ScreenGrab.h"
#include "Effects11\d3dx11effect.h"

#include <DirectXMath.h>

#include <process.h>
#include "framework.h"

#include <string>
#include <vector>
#include <list>
#include <deque>
#include <random>

#ifdef _DEBUG
#include "iostream"
#endif // _DEBUG

using namespace DirectX;
using namespace std;

#include "Typedef.h"
#include "Function.h"
#include "Macro.h"
#include "Struct.h"

namespace Client
{
	const unsigned int g_iWinSizeX = 1208;
	const unsigned int g_iWinSizeY = 720;

	enum MOUSEKEYSTATE { DIMK_LB, DIMK_RB, DIMK_MB, DIMK_END };
	enum MOUSEMOVESTATE { DIMM_X, DIMM_Y, DIMM_Z, DIMM_END };
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace Client;

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // _DEBUG

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#else
#define DBG_NEW new
#endif // _DEBUG

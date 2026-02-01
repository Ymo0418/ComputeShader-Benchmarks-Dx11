#pragma once

namespace Client
{
	struct WINDOW_DESC
	{
		HWND		hWnd;
		HINSTANCE	hInst;
		uint		iWinSizeX, iWinSizeY;
		bool		isWindowed;
	};
	
	struct PARTICLE_DATA
	{
		_float4 vColor;
		_float3 vViewPosition;
		float	fScale;

		static const unsigned int				iNumElement = 3;
		static const D3D11_INPUT_ELEMENT_DESC	Elements[iNumElement];
	};
}
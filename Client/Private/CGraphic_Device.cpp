#include "CGraphic_Device.h"

CGraphic_Device::CGraphic_Device()
{
}

HRESULT CGraphic_Device::Initialize(HWND _hWnd, bool _isWindowed, uint _iWinSizeX, uint _iWinSizeY, _Inout_ ID3D11Device** _ppDevice, _Inout_ ID3D11DeviceContext** _ppContext)
{
	uint iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif // DEBUG
	D3D_FEATURE_LEVEL FeatureLV;

	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0, D3D11_SDK_VERSION,
		&m_pDevice, &FeatureLV, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_SwapChain(_hWnd, _isWindowed, _iWinSizeX, _iWinSizeY)))
		return E_FAIL;

	if (FAILED(Ready_BackBufferRenderTargetView()))
		return E_FAIL;

	if (FAILED(Ready_DepthStencilView(_iWinSizeX, _iWinSizeY)))
		return E_FAIL;

	ID3D11RenderTargetView* pRTVs[1] = {
		m_pBackBufferRTV
	};

	m_pContext->OMSetRenderTargets(1, pRTVs, m_pDepthStencilView);

	D3D11_VIEWPORT ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(ViewPortDesc));

	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (float)_iWinSizeX;
	ViewPortDesc.Height = (float)_iWinSizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	*_ppContext = m_pContext;
	*_ppDevice = m_pDevice;

	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_BackBuffer_View(_float4 _vClearColor)
{
	if (!m_pContext)
		return E_FAIL;

	m_pContext->ClearRenderTargetView(m_pBackBufferRTV, (float*)&_vClearColor);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_DepthStecil_View()
{
	if (!m_pContext)
		return E_FAIL;

	m_pContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT CGraphic_Device::Present()
{
	if (!m_pSwapChain)
		return E_FAIL;

	return m_pSwapChain->Present(1, 0);
}

D3D11_VIEWPORT CGraphic_Device::Get_ViewPorts()
{
	D3D11_VIEWPORT View{};
	uint iNum = 1;

	m_pContext->RSGetViewports(&iNum, &View);

	return View;
}

HRESULT CGraphic_Device::Ready_SwapChain(HWND _hWnd, bool _isWindowed, uint _iWinSizeX, uint _iWinSizeY)
{
	IDXGIDevice* pDevice = nullptr;
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDevice);

	IDXGIAdapter* pAdapter = nullptr;
	pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);

	IDXGIFactory* pFactory = nullptr;
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	DXGI_SWAP_CHAIN_DESC SwapChain;
	ZeroMemory(&SwapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

	SwapChain.BufferDesc.Width = _iWinSizeX;
	SwapChain.BufferDesc.Height = _iWinSizeY;

	SwapChain.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	SwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChain.BufferCount = 1;

	SwapChain.BufferDesc.RefreshRate.Numerator = 60;
	SwapChain.BufferDesc.RefreshRate.Denominator = 1;

	SwapChain.SampleDesc.Quality = 0;
	SwapChain.SampleDesc.Count = 1;

	SwapChain.OutputWindow = _hWnd;
	SwapChain.Windowed = _isWindowed;
	SwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	if (FAILED(pFactory->CreateSwapChain(m_pDevice, &SwapChain, &m_pSwapChain)))
	{
		MSG_BOX("Failed : CreateSwapChain");
		return E_FAIL;
	}

	Safe_Release(pFactory);
	Safe_Release(pAdapter);
	Safe_Release(pDevice);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_BackBufferRenderTargetView()
{
	if (!m_pDevice)
		return E_FAIL;

	ID3D11Texture2D* pBackBufferTexture = nullptr;

	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture, nullptr, &m_pBackBufferRTV)))
		return E_FAIL;

	Safe_Release(pBackBufferTexture);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_DepthStencilView(uint _iWinSizeX, uint _iWinSizeY)
{
	if (!m_pDevice)
		return E_FAIL;

	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(TextureDesc));

	TextureDesc.Width = _iWinSizeX;
	TextureDesc.Height = _iWinSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;


	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, NULL, &m_pDepthStencilView)))
		return E_FAIL;


	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

CGraphic_Device* CGraphic_Device::Create(HWND _hWnd, bool _isWindowed, uint _iWinSizeX, uint _iWinSizeY, ID3D11Device** _ppDevice, ID3D11DeviceContext** _ppContext)
{
	CGraphic_Device* pInstance = new CGraphic_Device();

	if (FAILED(pInstance->Initialize(_hWnd, _isWindowed, _iWinSizeX, _iWinSizeY, _ppDevice, _ppContext)))
	{
		MSG_BOX("Failed to Created : CGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGraphic_Device::Free()
{
	Safe_Release(m_pSwapChain);
	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pContext);
	Safe_Release(m_pDepthTexture);

#if defined(DEBUG) || defined(_DEBUG)
	ID3D11Debug* d3dDebug;
	HRESULT hr = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	if (SUCCEEDED(hr))
	{
		OutputDebugStringW(L"\r----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker \r ");
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");

		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

		OutputDebugStringW(L"\r----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker END \r ");
		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	}
	if (d3dDebug != nullptr)            d3dDebug->Release();
#endif

	Safe_Release(m_pDevice);
}

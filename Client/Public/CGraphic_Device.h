#pragma once
#include "CBase.h"

BEGIN(Client)

class CGraphic_Device : public CBase
{
private:
    CGraphic_Device();
    virtual ~CGraphic_Device() = default;

public:
    HRESULT Initialize(HWND _hWnd, bool _isWindowed, uint _iWinSizeX, uint _iWinSizeY,
        _Inout_ ID3D11Device** _ppDevice, _Inout_ ID3D11DeviceContext** _ppContext);
    HRESULT Clear_BackBuffer_View(_float4 _vClearColor);
    HRESULT Clear_DepthStecil_View();
    HRESULT Present();

    //¿¬½À
    D3D11_VIEWPORT Get_ViewPorts();

private:
    HRESULT Ready_SwapChain(HWND _hWnd, bool _isWindowed, uint _iWinSizeX, uint _iWinSizeY);
    HRESULT Ready_BackBufferRenderTargetView();
    HRESULT Ready_DepthStencilView(uint _iWinSizeX, uint _iWinSizeY);

private:
    ID3D11Device*           m_pDevice           = { nullptr };
    ID3D11DeviceContext*    m_pContext          = { nullptr };

    IDXGISwapChain*         m_pSwapChain        = { nullptr };
    ID3D11RenderTargetView* m_pBackBufferRTV    = { nullptr };

    ID3D11Texture2D*        m_pDepthTexture     = { nullptr };
    ID3D11DepthStencilView* m_pDepthStencilView = { nullptr };

public:
    static CGraphic_Device* Create(HWND _hWnd, bool _isWindowed, uint _iWinSizeX, uint _iWinSizeY,
        ID3D11Device** _ppDevice, ID3D11DeviceContext** _ppContext);
    virtual void Free() override;
};

END

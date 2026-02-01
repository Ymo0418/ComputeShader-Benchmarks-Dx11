#pragma once
#include "Defines.h"
#include "CTimer.h"
#include "ImGUI/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

BEGIN(Client)

class CMainApp : public CBase
{
private:
    CMainApp() = default;
    virtual ~CMainApp() = default;

public:
    HRESULT Initialize();
    void    Update(float _fTimeDelta);
    void    Render(CTimer* _pTimer);

private:
    void    Render_Objects();
    void    Render_UI();

private:
    class CGraphic_Device*  m_pGraphic_Device       = { nullptr };
	ID3D11Device*		    m_pDevice 	            = { nullptr };
	ID3D11DeviceContext*    m_pContext	            = { nullptr };

    deque<float>            DequeFrameTimes;
    float                   m_fAccum                = { 0.f };
    const float             m_fRecordTime           = { 10.f };
    bool                    m_bEmit_CPU             = { false };
    const                   uint* m_vParticleNum[2] = { nullptr, nullptr };

    class CEmitter*         m_pEmitter              = { nullptr };

public:
    static CMainApp* Create();
    virtual void Free();
};

END
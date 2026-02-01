#pragma once
#include "CBase.h"
#include "ImGUI/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

BEGIN(Client)

class CImGUI : public CBase
{
private:
    CImGUI() = default;
    virtual ~CImGUI() = default;

public:
    virtual HRESULT Initialize();
    virtual void    Update(float _fTimeDelta);
    virtual HRESULT Render();

    void* Get_DataPtr() { return m_vParticleNum; }

private:
    const uint* m_vParticleNum[2] = { nullptr, nullptr };

public:
    static CImGUI* Create();
	void Free();
};

END
#pragma once
#include "CBase.h"

BEGIN(Client)

class CBuffer : public CBase
{
protected:
    CBuffer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CBuffer() = default;

public:
    virtual HRESULT Initialize();
    virtual void    Update(float _fTimeDelta) = 0;
    virtual HRESULT Render() = 0;
    virtual void    Bind_UI(void* _pData) = 0;

protected:
    virtual void    Update_Emit(float _fTimeDelta) = 0;

protected:
    uint                        m_iEmitNum                  = { 0 };
    uint                        m_iMaxParticle              = { 0 };
    uint                        m_iLiveParticle             = { 0 };
    _float2                     m_vEmitTimer                = { 0.1f, 0.1f };

    ID3D11Device*               m_pDevice                   = { nullptr };
    ID3D11DeviceContext*        m_pContext                  = { nullptr };
        
public:
    virtual void                Free();
};

END
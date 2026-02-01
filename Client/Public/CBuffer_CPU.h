#pragma once
#include "CBuffer.h"

BEGIN(Client)

class CBuffer_CPU : public CBuffer
{
    typedef struct tagParticleInfo {
        _float4 vColor;         
        _float3 vWorldPos;
        float   fScale;
        _float3 vWorldVelocity;
        _float2 vLifeTime;
    }PARTICLE;
    typedef struct tagConstantInfo {
        uint    iMaxParticle;
        _vector vPivot;
        _float3 vRange;
        _vector vStartColor = { 1.f, 1.f, 1.f, 1.f };
        _vector vEndColor = { 1.f, 1.f, 1.f, 1.f };
        float   fLifeTimeFrom, fLifeTimeTo;
        float   fSpeed;
        float   fScale;
    }CONST_INFO;

protected:
    CBuffer_CPU(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CBuffer_CPU() = default;

public:
    virtual HRESULT Initialize();
    virtual void    Update(float _fTimeDelta);
    virtual HRESULT Render();
    virtual void    Bind_UI(void* _pData);
    
protected:
    void    Update_Emit(float _fTimeDelta);

protected:
    PARTICLE*                   m_Particles                 = { };
    CONST_INFO                  m_tConstInfos               = { };

    ID3D11Buffer*               m_pVertex_Buffer            = { nullptr };

public:
    static CBuffer_CPU*         Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual void                Free();
};

END
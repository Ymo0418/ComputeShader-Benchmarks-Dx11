#pragma once
#include "CBuffer.h"

BEGIN(Client)

class CBuffer_GPU : public CBuffer
{
public:
    typedef struct tagForRender{
        _float4 vColor;
        _float3 vWorldPos;
        float  fScale;
    }FOR_RENDER;
    typedef struct tagForUpdate {
        _float3 vWorldVelocity;
        _float2 vLifeTime;
    }FOR_UPDATE;
    typedef struct tagUpdateConst {
        _float4 vStartColor;
        _float4 vEndColor;
        float   fSpeed;
        float   fScale;
        _float2 UpdateConst_Pad;
    }UPDATE_CONST;
    typedef struct tagEmitConst {
        uint    iMaxParticle;
        _float3 vPivot;
        _float4 vRange;
        _float2 vLifeTime;
        _float2 EmitConst_Pad;
    }EMIT_CONST;
    typedef struct tagEmitInfo{
        _float4x4 matWorld;
        uint iCount;
        uint iOffset;
        _float2 EmitInfo_Pad;
    }EMIT_INFO;

protected:
    CBuffer_GPU(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CBuffer_GPU() = default;

public:
    virtual HRESULT Initialize();
    virtual void    Update(float _fTimeDelta);
    virtual HRESULT Render();
    virtual void    Bind_UI(void* _pData);

protected:
    HRESULT Initialize_DrawArg();
    HRESULT Initialize_Count();
    HRESULT Initialize_Index();
    HRESULT Initialize_ParticleResources(const UPDATE_CONST& _Update);
    HRESULT Initialize_EmitInfo(const EMIT_CONST& _Emit);
    HRESULT Initialize_Random();
    HRESULT Initialize_ComputeShader();
    HRESULT Dispatch_Reset();

    HRESULT Check_CS(HRESULT _hr, ID3DBlob* _pShaderBlob, ID3DBlob* _pErrorBlob);
    HRESULT Create_CS(ID3D11ComputeShader** _pComputeShader, ID3DBlob* _pShaderBlob);
    void    Update_Emit(float _fTimeDelta);
    uint    Get_UAVCounter(ID3D11UnorderedAccessView* _pUAV);

protected:
    /* Draw Arg */
    ID3D11Buffer*               m_pDrawArgs_Buffer          = { nullptr };
    ID3D11UnorderedAccessView*  m_pDrawArgs_UAV             = { nullptr };

    /* Count */
    ID3D11Buffer*               m_pCount_Buffer             = { nullptr };

    /* Alive Dead Index Buffer */
    ID3D11Buffer*               m_pAliveIndex_Buffer        = { nullptr };
    ID3D11ShaderResourceView*   m_pAliveIndex_SRV           = { nullptr };
    ID3D11UnorderedAccessView*  m_pAliveIndex_UAV           = { nullptr };
    ID3D11Buffer*               m_pDeadIndex_Buffer         = { nullptr };
    ID3D11UnorderedAccessView*  m_pDeadIndex_UAV            = { nullptr };

    /* Particle Data - Datas that need at Update & Render */
    uint                        m_iUpdateStride             = { 0 };
    uint                        m_iRenderStride             = { 0 };
    ID3D11Buffer*               m_pParticleUpdate_Buffer    = { nullptr };
    ID3D11UnorderedAccessView*  m_pParticleUpdate_UAV       = { nullptr };
    ID3D11Buffer*               m_pParticleRender_Buffer    = { nullptr };
    ID3D11ShaderResourceView*   m_pParticleRender_SRV       = { nullptr };
    ID3D11UnorderedAccessView*  m_pParticleRender_UAV       = { nullptr };
    ID3D11Buffer*               m_pUpdateConstant_Buffer    = { nullptr };

    /* Emit Data - Datas that need at Emit */
    ID3D11Buffer*               m_pEmitInfo_Buffer          = { nullptr };
    ID3D11Buffer*               m_pEmitConstant_Buffer      = { nullptr };
    
    /* Randomness */
    ID3D11Texture2D*            m_pRandom_Texture           = { nullptr };
    ID3D11ShaderResourceView*   m_pRandom_SRV               = { nullptr };

    /* Compute Shader */
    ID3D11ComputeShader*        m_pCS_Emit                  = { nullptr };
    ID3D11ComputeShader*        m_pCS_Update                = { nullptr };
    ID3D11ComputeShader*        m_pCS_Reset                 = { nullptr };
        
public:
    static uint                 OffsetForRandom;
    static CBuffer_GPU*             Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual void                Free();
};

END
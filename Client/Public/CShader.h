#pragma once
#include "CBase.h"

BEGIN(Client)

class CShader final : public CBase
{
private:
    CShader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CShader() = default;

public:
    virtual HRESULT Initialize(const _tchar* _pFilePath, const D3D11_INPUT_ELEMENT_DESC* _Elements, uint _iNumElements, D3D_SHADER_MACRO* _pMacros = nullptr);

    HRESULT Begin(uint _iPassIdx);
    HRESULT Bind_Matrix(const char* _pConstantName, const _float4x4* _pMatrix);
    //HRESULT Bind_Matrices(const char* _pConstantName, const _float4x4* _pMatrix, uint _iNumMatrices);
    //HRESULT Bind_Float(const char* _pConstantName, const float _fValue);
    //HRESULT Bind_SRV(const char* _pConstantName, ID3D11ShaderResourceView* _pSRV);
    //HRESULT Bind_RawData(const char* _pConstantName, const void* pData, uint _iLength);

private:
    uint                        m_iNumPasses;

    vector<ID3D11InputLayout*>  m_vecInputLayout;
    ID3DX11Effect*              m_pEffect           = { nullptr };
    ID3D11Device*               m_pDevice           = { nullptr };
    ID3D11DeviceContext*        m_pContext          = { nullptr };

public:
    static CShader* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pFilePath, const D3D11_INPUT_ELEMENT_DESC* _Elements, uint _iNumElements, D3D_SHADER_MACRO* _pMacros = nullptr);
    virtual void Free() override;
};

END

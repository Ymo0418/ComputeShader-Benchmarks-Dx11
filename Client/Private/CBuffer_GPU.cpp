#include "CBuffer_GPU.h"

uint CBuffer_GPU::OffsetForRandom = { 0 };

CBuffer_GPU::CBuffer_GPU(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CBuffer{ _pDevice ,_pContext }
{
}

HRESULT CBuffer_GPU::Initialize()
{
	CHECK_FAILED(__super::Initialize());

	UPDATE_CONST UpdateConst{};
	{
		UpdateConst.vStartColor = { 1.f, 1.f, 1.f, 1.f };
		UpdateConst.vEndColor = { 1.f, 1.f, 1.f, 1.f };
		UpdateConst.fSpeed = { 8.f };
		UpdateConst.fScale = { 0.4f };
	}
	EMIT_CONST EmitConst{};
	{
		EmitConst.iMaxParticle = m_iMaxParticle;
		EmitConst.vPivot = { 0.f, 0.f, 0.f };
		EmitConst.vRange = { 10.f, 10.f, 10.f, 0.f };
		EmitConst.vLifeTime = { 1.f, 4.f };
		EmitConst.EmitConst_Pad;
	}

	m_iUpdateStride = sizeof FOR_UPDATE;
	m_iRenderStride = sizeof FOR_RENDER;

	CHECK_FAILED(Initialize_DrawArg());
	CHECK_FAILED(Initialize_Count());
	CHECK_FAILED(Initialize_Index());
	CHECK_FAILED(Initialize_ParticleResources(UpdateConst));
	CHECK_FAILED(Initialize_EmitInfo(EmitConst));
	CHECK_FAILED(Initialize_Random());
	CHECK_FAILED(Initialize_ComputeShader());
	CHECK_FAILED(Dispatch_Reset());

	return S_OK;
}

void CBuffer_GPU::Update(float _fTimeDelta)
{
	ID3D11UnorderedAccessView* UAVs[] = { m_pParticleRender_UAV, m_pParticleUpdate_UAV, m_pDeadIndex_UAV, m_pAliveIndex_UAV, m_pDrawArgs_UAV };
	uint InitialCounter[] = { (uint)-1, (uint)-1, (uint)-1, 0, (uint)-1 };
	m_pContext->CSSetUnorderedAccessViews(0, _ARRAYSIZE(UAVs), UAVs, InitialCounter);

	ID3D11ShaderResourceView* SRVs[] = { m_pRandom_SRV };
	m_pContext->CSSetShaderResources(0, _ARRAYSIZE(SRVs), SRVs);

	ID3D11Buffer* CSTs[] = { m_pEmitConstant_Buffer, m_pEmitInfo_Buffer };

	m_pContext->CSSetConstantBuffers(1, _ARRAYSIZE(CSTs), CSTs);
	m_pContext->CSSetShader(m_pCS_Emit, nullptr, 0);

	Update_Emit(_fTimeDelta);

	if (m_iLiveParticle)
	{
		CSTs[0] = m_pUpdateConstant_Buffer;	
		CSTs[1] = nullptr;
		m_pContext->CSSetConstantBuffers(1, _ARRAYSIZE(CSTs), CSTs);

		m_pContext->CSSetShader(m_pCS_Update, nullptr, 0);
		m_pContext->Dispatch(ALIGN_TO_MULTIPLIER(m_iMaxParticle, 1024) / 1024, 1, 1);
	}

	// Reset Binded Resources
	ZeroMemory(SRVs, sizeof SRVs);
	m_pContext->CSSetShaderResources(0, _ARRAYSIZE(SRVs), SRVs);

	ZeroMemory(UAVs, sizeof UAVs);
	m_pContext->CSSetUnorderedAccessViews(0, _ARRAYSIZE(UAVs), UAVs, nullptr);

	ZeroMemory(CSTs, sizeof CSTs);
	m_pContext->CSSetConstantBuffers(1, _ARRAYSIZE(CSTs), CSTs);

	m_iLiveParticle = Get_UAVCounter(m_pAliveIndex_UAV);
}

HRESULT CBuffer_GPU::Render()
{
	if (!m_iLiveParticle)
		return S_OK;

	ID3D11Buffer* vb = nullptr;
	uint stride = 0;
	uint offset = 0;
	m_pContext->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	m_pContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	m_pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	ID3D11ShaderResourceView* VS_SRVs[] = { m_pParticleRender_SRV, m_pAliveIndex_SRV };
	//ID3D11ShaderResourceView* PS_SRVs[] = { m_pTexture->Get_SRV(0) };

	m_pContext->VSSetShaderResources(0, _ARRAYSIZE(VS_SRVs), VS_SRVs);
	//m_pContext->PSSetShaderResources(0, _ARRAYSIZE(PS_SRVs), PS_SRVs);

	m_pContext->DrawInstancedIndirect(m_pDrawArgs_Buffer, 0);

	ZeroMemory(VS_SRVs, sizeof VS_SRVs);
	m_pContext->VSSetShaderResources(0, _ARRAYSIZE(VS_SRVs), VS_SRVs);

	//ZeroMemory(PS_SRVs, sizeof PS_SRVs);
	//m_pContext->PSSetShaderResources(0, _ARRAYSIZE(PS_SRVs), PS_SRVs);

	return S_OK;
}

HRESULT CBuffer_GPU::Initialize_DrawArg()
{
	D3D11_BUFFER_DESC Desc{};
	D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV{};

	ZeroMemory(&Desc, sizeof Desc);
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	Desc.ByteWidth = sizeof(uint) * 4;
	Desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, &m_pDrawArgs_Buffer));

	ZeroMemory(&DescUAV, sizeof DescUAV);
	DescUAV.Format = DXGI_FORMAT_R32_UINT;
	DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	DescUAV.Buffer.FirstElement = 0;
	DescUAV.Buffer.NumElements = 4;
	DescUAV.Buffer.Flags = 0;
	CHECK_FAILED(m_pDevice->CreateUnorderedAccessView(m_pDrawArgs_Buffer, &DescUAV, &m_pDrawArgs_UAV));

	return S_OK;
}

HRESULT CBuffer_GPU::Initialize_Count()
{
	D3D11_BUFFER_DESC Desc{};

	ZeroMemory(&Desc, sizeof Desc);
	Desc.Usage = D3D11_USAGE_STAGING;
	Desc.BindFlags = 0;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	Desc.ByteWidth = sizeof(uint);
	Desc.MiscFlags = 0;
	CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, &m_pCount_Buffer));

	return S_OK;
}

HRESULT CBuffer_GPU::Initialize_Index()
{
	D3D11_BUFFER_DESC Desc{};
	D3D11_SHADER_RESOURCE_VIEW_DESC DescSRV{};
	D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV{};

	ZeroMemory(&Desc, sizeof Desc);
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	Desc.StructureByteStride = sizeof(uint);
	Desc.ByteWidth = sizeof(uint) * m_iMaxParticle;
	CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, &m_pDeadIndex_Buffer));

	ZeroMemory(&DescUAV, sizeof DescUAV);
	DescUAV.Format = DXGI_FORMAT_UNKNOWN;
	DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	DescUAV.Buffer.FirstElement = 0;
	DescUAV.Buffer.NumElements = m_iMaxParticle;
	DescUAV.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
	CHECK_FAILED(m_pDevice->CreateUnorderedAccessView(m_pDeadIndex_Buffer, &DescUAV, &m_pDeadIndex_UAV));

	ZeroMemory(&Desc, sizeof Desc);
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	Desc.StructureByteStride = sizeof(uint);
	Desc.ByteWidth = sizeof(uint) * m_iMaxParticle;
	CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, &m_pAliveIndex_Buffer));

	ZeroMemory(&DescSRV, sizeof DescSRV);
	DescSRV.Format = DXGI_FORMAT_UNKNOWN;
	DescSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	DescSRV.Buffer.ElementOffset = 0;
	DescSRV.Buffer.ElementWidth = m_iMaxParticle;
	CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pAliveIndex_Buffer, &DescSRV, &m_pAliveIndex_SRV));

	ZeroMemory(&DescUAV, sizeof DescUAV);
	DescUAV.Format = DXGI_FORMAT_UNKNOWN;
	DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	DescUAV.Buffer.FirstElement = 0;
	DescUAV.Buffer.NumElements = m_iMaxParticle;
	DescUAV.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
	CHECK_FAILED(m_pDevice->CreateUnorderedAccessView(m_pAliveIndex_Buffer, &DescUAV, &m_pAliveIndex_UAV));

	return S_OK;
}

HRESULT CBuffer_GPU::Initialize_ParticleResources(const UPDATE_CONST& _Update)
{
	D3D11_BUFFER_DESC Desc{};
	D3D11_SHADER_RESOURCE_VIEW_DESC DescSRV{};
	D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV{};

	ZeroMemory(&Desc, sizeof Desc);
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	Desc.StructureByteStride = m_iRenderStride;
	Desc.ByteWidth = m_iMaxParticle * m_iRenderStride;
	CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, &m_pParticleRender_Buffer));

	Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	Desc.StructureByteStride = m_iUpdateStride;
	Desc.ByteWidth = m_iMaxParticle * m_iUpdateStride;
	CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, &m_pParticleUpdate_Buffer));

	ZeroMemory(&DescSRV, sizeof DescSRV);
	DescSRV.Format = DXGI_FORMAT_UNKNOWN;
	DescSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	DescSRV.Buffer.ElementOffset = 0;
	DescSRV.Buffer.ElementWidth = m_iMaxParticle;
	CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pParticleRender_Buffer, &DescSRV, &m_pParticleRender_SRV));

	ZeroMemory(&DescUAV, sizeof DescUAV);
	DescUAV.Format = DXGI_FORMAT_UNKNOWN;
	DescUAV.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	DescUAV.Buffer.FirstElement = 0;
	DescUAV.Buffer.NumElements = m_iMaxParticle;
	DescUAV.Buffer.Flags = 0;
	CHECK_FAILED(m_pDevice->CreateUnorderedAccessView(m_pParticleRender_Buffer, &DescUAV, &m_pParticleRender_UAV));
	CHECK_FAILED(m_pDevice->CreateUnorderedAccessView(m_pParticleUpdate_Buffer, &DescUAV, &m_pParticleUpdate_UAV));

	D3D11_SUBRESOURCE_DATA InitData{};
	ZeroMemory(&InitData, sizeof InitData);
	InitData.pSysMem = &_Update;
	ZeroMemory(&Desc, sizeof Desc);
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.ByteWidth = sizeof(UPDATE_CONST);
	CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, &InitData, &m_pUpdateConstant_Buffer));

	return S_OK;
}

HRESULT CBuffer_GPU::Initialize_EmitInfo(const EMIT_CONST& _Emit)
{
	D3D11_BUFFER_DESC Desc{};
	D3D11_SUBRESOURCE_DATA InitData{};

	ZeroMemory(&Desc, sizeof Desc);
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Desc.ByteWidth = sizeof(EMIT_INFO);
	m_pDevice->CreateBuffer(&Desc, nullptr, &m_pEmitInfo_Buffer);

	ZeroMemory(&InitData, sizeof InitData);
	InitData.pSysMem = &_Emit;
	ZeroMemory(&Desc, sizeof Desc);
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.ByteWidth = sizeof(EMIT_CONST);
	m_pDevice->CreateBuffer(&Desc, &InitData, &m_pEmitConstant_Buffer);

	return S_OK;
}

HRESULT CBuffer_GPU::Initialize_Random()
{
	D3D11_TEXTURE2D_DESC Desc{};
	ZeroMemory(&Desc, sizeof Desc);
	Desc.Width = 1024;
	Desc.Height = 1024;
	Desc.ArraySize = 1;
	Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Desc.Usage = D3D11_USAGE_IMMUTABLE;
	Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	Desc.MipLevels = 1;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;

	float* values = new float[Desc.Width * Desc.Height * 4];
	float* ptr = values;
	for (UINT i = 0; i < Desc.Width * Desc.Height; i++)
	{
		ptr[0] = rand() / (float)RAND_MAX;
		ptr[1] = rand() / (float)RAND_MAX;
		ptr[2] = rand() / (float)RAND_MAX;
		ptr[3] = rand() / (float)RAND_MAX;
		ptr += 4;
	}

	D3D11_SUBRESOURCE_DATA SRD{};
	SRD.pSysMem = values;
	SRD.SysMemPitch = Desc.Width * 16;
	SRD.SysMemSlicePitch = 0;

	CHECK_FAILED(m_pDevice->CreateTexture2D(&Desc, &SRD, &m_pRandom_Texture));

	Safe_Delete_Array(values);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRV{};
	SRV.Format = Desc.Format;
	SRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRV.Texture2D.MipLevels = 1;
	SRV.Texture2D.MostDetailedMip = 0;

	CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pRandom_Texture, &SRV, &m_pRandom_SRV));

	return S_OK;
}

HRESULT CBuffer_GPU::Initialize_ComputeShader()
{
	uint iHlslFlag = 0;
	HRESULT hr;
	ID3DBlob* pShaderBlob = { nullptr };
	ID3DBlob* pErrorBlob = { nullptr };

#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	hr = D3DCompileFromFile(TEXT("../ShaderFiles/CS_Emit.hlsl"), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "CS_EMIT", "cs_5_0", iHlslFlag, 0, &pShaderBlob, &pErrorBlob);
	CHECK_FAILED(Check_CS(hr, pShaderBlob, pErrorBlob));
	CHECK_FAILED(Create_CS(&m_pCS_Emit, pShaderBlob));

	hr = D3DCompileFromFile(TEXT("../ShaderFiles/CS_Update.hlsl"), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "CS_UPDATE", "cs_5_0", iHlslFlag, 0, &pShaderBlob, &pErrorBlob);
	CHECK_FAILED(Check_CS(hr, pShaderBlob, pErrorBlob));
	CHECK_FAILED(Create_CS(&m_pCS_Update, pShaderBlob));

	hr = D3DCompileFromFile(TEXT("../ShaderFiles/CS_Reset.hlsl"), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, "CS_RESET", "cs_5_0", iHlslFlag, 0, &pShaderBlob, &pErrorBlob);
	CHECK_FAILED(Check_CS(hr, pShaderBlob, pErrorBlob));
	CHECK_FAILED(Create_CS(&m_pCS_Reset, pShaderBlob));

	return S_OK;
}

HRESULT CBuffer_GPU::Dispatch_Reset()
{
	uint InitialData[4] = { m_iMaxParticle, 0, 0, 0 };
	D3D11_SUBRESOURCE_DATA SRD{};
	SRD.pSysMem = InitialData;

	ID3D11Buffer* ConstBuffer = { nullptr };
	D3D11_BUFFER_DESC Desc{};
	ZeroMemory(&Desc, sizeof Desc);
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.ByteWidth = 4 * sizeof(UINT);
	m_pDevice->CreateBuffer(&Desc, &SRD, &ConstBuffer);

	uint InitialCount[] = { 0 };
	m_pContext->CSSetUnorderedAccessViews(0, 1, &m_pDeadIndex_UAV, InitialCount);
	m_pContext->CSSetConstantBuffers(0, 1, &ConstBuffer);

	m_pContext->CSSetShader(m_pCS_Reset, nullptr, 0);
	m_pContext->Dispatch(ALIGN_TO_MULTIPLIER(m_iMaxParticle, 256) / 256, 1, 1);

	Safe_Release(ConstBuffer);

	return S_OK;
}

void CBuffer_GPU::Update_Emit(float _fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE MappedResource{};

	m_vEmitTimer.y -= _fTimeDelta;

	if (m_vEmitTimer.y <= 0.f)
	{
		ZeroMemory(&MappedResource, sizeof MappedResource);
		m_vEmitTimer.y = m_vEmitTimer.x;
		m_iLiveParticle += m_iEmitNum;

#ifdef _DEBUG
		if (m_iLiveParticle >= m_iMaxParticle)
		{
			/* ----- Over Emitting ----- */
			__debugbreak();
		}
#endif // _DEBUG

		m_pContext->Map(m_pEmitInfo_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

		EMIT_INFO* Infos = (EMIT_INFO*)MappedResource.pData;
		XMStoreFloat4x4(&Infos->matWorld, XMMatrixIdentity());
		Infos->iCount= m_iEmitNum;
		Infos->iOffset = OffsetForRandom;
		OffsetForRandom += Infos->iCount;
		OffsetForRandom = (OffsetForRandom % 1024);

		m_pContext->Unmap(m_pEmitInfo_Buffer, 0);
		m_pContext->Dispatch(ALIGN_TO_MULTIPLIER(Infos->iCount, 256) / 256, 1, 1);
	}
}

HRESULT CBuffer_GPU::Check_CS(HRESULT _hr, ID3DBlob* _pShaderBlob, ID3DBlob* _pErrorBlob)
{
	if (FAILED(_hr))
	{
		if (_pErrorBlob)
		{
			const char* pChar = static_cast<const char*>(_pErrorBlob->GetBufferPointer());
			MessageBox(NULL, _wstring(pChar, pChar + _pErrorBlob->GetBufferSize()).c_str(), L"System Message", MB_OK);

			OutputDebugStringA((char*)_pErrorBlob->GetBufferPointer());
			_pErrorBlob->Release();
		}

		if (_pShaderBlob)
			_pShaderBlob->Release();

		return _hr;
	}

	return S_OK;
}

HRESULT CBuffer_GPU::Create_CS(ID3D11ComputeShader** _pComputeShader, ID3DBlob* _pShaderBlob)
{
	return m_pDevice->CreateComputeShader(_pShaderBlob->GetBufferPointer(), _pShaderBlob->GetBufferSize(), nullptr, _pComputeShader);
}

uint CBuffer_GPU::Get_UAVCounter(ID3D11UnorderedAccessView* _pUAV)
{
	m_pContext->CopyStructureCount(m_pCount_Buffer, 0, _pUAV);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_pContext->Map(m_pCount_Buffer, 0, D3D11_MAP_READ, 0, &mappedResource);
	uint counterValue = *reinterpret_cast<uint*>(mappedResource.pData);
	m_pContext->Unmap(m_pCount_Buffer, 0);

	return counterValue;
}

void CBuffer_GPU::Bind_UI(void* _pData)
{
	const uint** pData = static_cast<const uint**>(_pData);

	pData[0] = &m_iMaxParticle;
	pData[1] = &m_iLiveParticle;
}

CBuffer_GPU* CBuffer_GPU::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CBuffer_GPU* pInstance = new CBuffer_GPU(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CBuffer_GPU");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBuffer_GPU::Free()
{
	__super::Free();

	Safe_Release(m_pCS_Emit);
	Safe_Release(m_pCS_Update);
	Safe_Release(m_pCS_Reset);

	Safe_Release(m_pRandom_SRV);
	Safe_Release(m_pRandom_Texture);

	Safe_Release(m_pEmitInfo_Buffer);
	Safe_Release(m_pEmitConstant_Buffer);

	Safe_Release(m_pUpdateConstant_Buffer);
	Safe_Release(m_pParticleUpdate_UAV);
	Safe_Release(m_pParticleUpdate_Buffer);
	Safe_Release(m_pParticleRender_SRV);
	Safe_Release(m_pParticleRender_UAV);
	Safe_Release(m_pParticleRender_Buffer);

	Safe_Release(m_pAliveIndex_SRV);
	Safe_Release(m_pAliveIndex_UAV);
	Safe_Release(m_pAliveIndex_Buffer);
	Safe_Release(m_pDeadIndex_UAV);
	Safe_Release(m_pDeadIndex_Buffer);

	Safe_Release(m_pCount_Buffer);

	Safe_Release(m_pDrawArgs_UAV);
	Safe_Release(m_pDrawArgs_Buffer);
}

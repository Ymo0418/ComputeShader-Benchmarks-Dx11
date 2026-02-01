#include "CImGUI.h"

HRESULT CImGUI::Initialize()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	return S_OK;
}

void CImGUI::Update(float _fTimeDelta)
{
}

HRESULT CImGUI::Render()
{

	return S_OK;
}

CImGUI* CImGUI::Create()
{
	CImGUI* pInstance = new CImGUI();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CImGUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImGUI::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
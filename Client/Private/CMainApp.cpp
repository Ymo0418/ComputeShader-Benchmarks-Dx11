#include "CMainApp.h"
#include "CGraphic_Device.h"
#include "CInput_Device.h"
#include "CCamera.h"
#include "CEmitter_GPU.h"
#include "CEmitter_CPU.h"
#include "CImGUI.h"

HRESULT CMainApp::Initialize()
{
	//ShowCursor(false);
	srand((uint)(time(NULL)));

	CHECK_NULPTR((m_pGraphic_Device = CGraphic_Device::Create(g_hWnd, true, g_iWinSizeX, g_iWinSizeY, &m_pDevice, &m_pContext)));
	CHECK_NULPTR((m_pEmitter = CEmitter_GPU::Create(m_pDevice, m_pContext)));
	m_pEmitter->Bind_UI(m_vParticleNum);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	return S_OK;
}

void CMainApp::Update(float _fTimeDelta)
{
	CInput_Device::GetInstance()->Update_InputDev();

	CCamera::GetInstance()->Update(_fTimeDelta);
	m_pEmitter->Update(_fTimeDelta);
}

void CMainApp::Render(CTimer* _pTimer)
{
	if (!m_pGraphic_Device)
		MSG_BOX("No Graphic Device Exist");

	m_pGraphic_Device->Clear_BackBuffer_View({ 0.f, 0.5f, 1.f, 1.0f });
	m_pGraphic_Device->Clear_DepthStecil_View();

	Render_Objects();
	
	m_pGraphic_Device->Present();
}

void CMainApp::Render_Objects()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	m_pEmitter->Render();
	Render_UI();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CMainApp::Render_UI()
{
	ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
	ImGui::SetNextWindowSize(ImVec2(300.f, (float)g_iWinSizeY * 0.5f));
	ImGui::Begin("System Info", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	ImGui::Separator();

	if (m_vParticleNum[0] && m_vParticleNum[1])
	{
		float fPerc = float(*(m_vParticleNum[1])) / float(*(m_vParticleNum[0]));
		ImGui::Text("Max Particle : %d", *(m_vParticleNum[0]));
		ImGui::Text("Cur Particle : %d", *(m_vParticleNum[1]));
		ImGui::Text("Used Percent : %.1f %%", fPerc * 100.f);
	}

	float fCurTime = ImGui::GetIO().DeltaTime;
	DequeFrameTimes.push_back(fCurTime);
	m_fAccum += fCurTime;

	while (m_fAccum > m_fRecordTime && !DequeFrameTimes.empty())
	{
		m_fAccum -= DequeFrameTimes.front();
		DequeFrameTimes.pop_front();
	}

	float fAverage = 0.0f;
	if (!DequeFrameTimes.empty() && m_fAccum > 0.0f) {
		fAverage = (float)DequeFrameTimes.size() / m_fAccum;
	}

	ImGui::Text("Cur  FPS : %.2f", ImGui::GetIO().Framerate);
	ImGui::Text("Aver FPS: %.2f", fAverage);

	if (ImGui::Button(m_bEmit_CPU ? "CPU" : "GPU"))
	{
		m_bEmit_CPU = !m_bEmit_CPU;
		Safe_Release(m_pEmitter);

		if (m_bEmit_CPU)
			m_pEmitter = CEmitter_CPU::Create(m_pDevice, m_pContext);
		else
			m_pEmitter = CEmitter_GPU::Create(m_pDevice, m_pContext);

		m_pEmitter->Bind_UI(m_vParticleNum);
	}

	ImGui::End();
}

CMainApp* CMainApp::Create()
{
	CMainApp* pMainApp = new CMainApp();

	if (FAILED(pMainApp->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pMainApp);
	}

	return pMainApp;
}

void CMainApp::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pEmitter);
	
	CCamera::DestroyInstance();
	CInput_Device::DestroyInstance();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGraphic_Device);
}
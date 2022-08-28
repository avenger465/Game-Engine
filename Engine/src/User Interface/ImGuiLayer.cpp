#include "epch.h"
#include "ImGuiLayer.h"

#include <imgui.h>
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Renderer/DirectX11Renderer/DirectX11Renderer.h"

Engine::ImGuiLayer::ImGuiLayer()
	: Layer("imGuilayer")
{
}

void Engine::ImGuiLayer::OnAttach(IRenderer* renderer)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; //Enable docking of the viewports 
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; //Enable each window to be a seperate viewport

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	SetDarkThemeColours();

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(m_WindowProps.Hwnd);

	DirectX11Renderer* currentRenderer = static_cast<DirectX11Renderer*>(renderer);
	ImGui_ImplDX11_Init(currentRenderer->GetDevice(), currentRenderer->GetDeviceContext());

}

void Engine::ImGuiLayer::OnDetach()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Engine::ImGuiLayer::Begin()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Engine::ImGuiLayer::End()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void Engine::ImGuiLayer::SetDarkThemeColours()
{
}

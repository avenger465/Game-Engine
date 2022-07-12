#pragma once

#include "System\Interfaces\IWindow.h"
#include "System\Interfaces\IRenderer.h"
#include "TerrainGenerationScene\TerrainGenerationScene.h"
#include "Utility\Timer.h"

namespace Engine
{
	class Window : public IWindow
	{
	public:
		Window(WindowProperties& WindowProps, IRenderer* renderer);
		virtual ~Window();

		virtual void Update(IRenderer* renderer) override;
		virtual unsigned int GetWidth() const override { return m_WindowProps.Width; }
		virtual unsigned int GetHeight() const override { return m_WindowProps.Height; }

		virtual WindowProperties GetWindowProperties() const override { return m_WindowProps; }

		static LRESULT CALLBACK WindowProc(
			HWND hWnd,
			UINT msg,
			WPARAM wParam,
			LPARAM lParam
		);

		const HWND GetWindowHandle() { return m_hWnd; }

	private:
		virtual BOOL Init(WindowProperties& WindowProps, IRenderer* renderer);

		virtual void Shutdown();

		HRESULT CreateApplicationWindow(WindowProperties& WindowProps, IRenderer* renderer);
		HRESULT Run(IRenderer* renderer);	

		void RenderScene(IRenderer* renderer);
		void RenderSceneFromCamera(IRenderer* renderer);

	private:
		HWND m_hWnd;
		HRESULT m_Window;
		TerrainGenerationScene* Scene;

		WindowProperties m_WindowProps;
		Timer m_Timer;

	};

	static HINSTANCE m_hInstance;
	static std::wstring m_windowClassName;
}

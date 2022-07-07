#include "epch.h"
#include "WindowsPlatform.h"
//#include "System\Interfaces\IWindow.h"

#include "Utility\Input.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "Renderer\Renderer.h"

#include "Utility\GraphicsHelpers.h"
#include "Utility\ColourRGBA.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


namespace Engine
{
	IWindow* IWindow::Create(WindowProperties& props)
	{
		return new Window(props);
	}
	/*IWindow* Create(WindowProperties& WindowProps)
	{
		return new Window(WindowProps);
	}*/

	/*Window::Window(Window)*/

	Window::Window(WindowProperties& WindowProps)
	{
		Init(WindowProps);
	}

	Window::~Window()
	{
		Shutdown();
	}

	BOOL Window::Init(WindowProperties& props)
	{
		m_windowClassName = L"WindowClass";
		m_Window = S_OK;

		m_Window = CreateApplicationWindow(props);

		return TRUE;
	}

	void Window::Update(IRenderer* renderer)
	{
		if (SUCCEEDED(m_Window))
		{
			m_Window = Run(renderer);
		}
	}

	void Window::Shutdown()
	{
		DestroyWindow(m_hWnd);
	}

	HRESULT Window::CreateApplicationWindow(WindowProperties& props)
	{
		// Get a stock icon to show on the taskbar for this program.
		SHSTOCKICONINFO stockIcon;
		stockIcon.cbSize = sizeof(stockIcon);
		if (SHGetStockIconInfo(SIID_APPLICATION, SHGSI_ICON, &stockIcon) != S_OK) // Returns false on failure
		{
			return false;
		}

		// Register window class. Defines various UI features of the window for our application.
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Window::WindowProc;    // Which function deals with windows messages
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0; SIID_APPLICATION;
		wcex.hInstance = m_hInstance;
		wcex.hIcon = stockIcon.hIcon; // Which icon to use for the window
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW); // What cursor appears over the window
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = m_windowClassName.c_str();
		wcex.hIconSm = stockIcon.hIcon;
		if (!RegisterClassEx(&wcex)) // Returns false on failure
		{
			return false;
		}


		// Select the type of window to show our application in
		DWORD windowStyle = WS_OVERLAPPEDWINDOW; // Standard window
		//DWORD windowStyle = WS_POPUP;          // Alternative: borderless. If you also set the viewport size to the monitor resolution, you 
												 // get a "fullscreen borderless" window, which works better with alt-tab than DirectX fullscreen,
												 // which is an option in Direct3DSetup.cpp. DirectX fullscreen has slight better performance though.

		// Calculate overall dimensions for the window. We will render to the *inside* of the window. But the
		// overall winder will be larger because it includes the borders, title bar etc. This code calculates
		// the overall size of the window given our choice of viewport size.
		RECT rc = { 0, 0, props.Width, props.Height };
		AdjustWindowRect(&rc, windowStyle, FALSE);

		// Create window, the second parameter is the text that appears in the title bar

		std::wstring wTitle = std::wstring(props.Title.begin(), props.Title.end());
		const wchar_t* wcharTitle = wTitle.c_str();
		m_hWnd = CreateWindow(m_windowClassName.c_str(), wcharTitle, windowStyle,
			CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, m_hInstance, nullptr);
		if (!m_hWnd)
		{
			return false;
		}

		props.Hwnd = m_hWnd;
		m_WindowProps = props;

		ShowWindow(m_hWnd, 1);
		UpdateWindow(m_hWnd);

		return TRUE;
	}

	HRESULT Window::Run(IRenderer* renderer)
	{
		HRESULT hr = S_OK;


		//// Prepare TL-Engine style input functions
		InitInput();

		//IMGUI
		//*******************************
		// Initialise ImGui
		//*******************************

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		// ImGui::StyleColorsClassic();

		// Setup Platform/Renderer bindings
		ImGui_ImplWin32_Init(m_WindowProps.Hwnd);

		Renderer* currentRenderer = static_cast<Renderer*>(renderer);
		ImGui_ImplDX11_Init(currentRenderer->GetDevice(), currentRenderer->GetDeviceContext());

		

		if (!currentRenderer)
		{
			return 0;
		}
		// Initialise scene
		//if (!m_SceneManager->LoadFirstScene())
		//{
		//	/*ErrorLogger errorLog;
		//	errorLog.ErrorMessage(m_WindowProps, "Error Loading first scene");*/
		//	return 0;
		//}


		m_Timer.Start();
		// Main message loop - this is a Windows equivalent of the loop in a TL-Engine application
		MSG msg = {};
		while (msg.message != WM_QUIT) // As long as window is open
		{
			// Check for and deal with any window messages (input, window resizing, minimizing, etc.).
			// The actual message processing happens in the function WndProc below
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				// Deal with messages
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			else // When no windows messages left to process then render & update our scene
			{
				// Update the scene by the amount of time since the last frame
				/*float frameTime = 0;

				while (frameTime < 1 / 60.0f)
				{
					frameTime += m_Timer.GetLapTime();
				}
				frameTime = 1 / 60.0f;
				m_SceneManager->SceneLoop(frameTime);*/

				// Update the scene by the amount of time since the last frame
				float frameTime = m_Timer.GetLapTime();
				//Scene->UpdateScene(frameTime, HWnd);

				//// Render the scene
				//Scene->RenderScene(frameTime);


				if (KeyHit(Key_Escape))
				{
					DestroyWindow(m_hWnd); // This will close the window and ultimately exit this loop
				}

			}
		}

		//IMGUI
		 //*******************************
		// Shutdown ImGui
		//*******************************

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		//*******************************
	}


	LRESULT Window::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) // IMGUI this line passes user input to ImGUI
			return true;

		switch (msg)
		{
		case WM_PAINT: // A necessary message to ensure the window content is displayed
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break;

		case WM_DESTROY: // Another necessary message to deal with the window being closed
			PostQuitMessage(0);
			break;


			// The WM_KEYXXXX messages report keyboard input to our window.
			// This application has added some simple functions (not DirectX) to process these messages (all in Input.cpp/h)
			// so you don't need to change this code. Instead simply use KeyHit, KeyHeld etc.
		case WM_KEYDOWN:
			KeyDownEvent(static_cast<KeyCode>(wParam));
			break;

		case WM_KEYUP:
			KeyUpEvent(static_cast<KeyCode>(wParam));
			break;


			// The following WM_XXXX messages report mouse movement and button presses
			// Use KeyHit to get mouse buttons, GetMouseX, GetMouseY for its position
		case WM_MOUSEMOVE:
		{
			MouseMoveEvent(LOWORD(lParam), HIWORD(lParam));
			break;
		}
		case WM_LBUTTONDOWN:
		{
			KeyDownEvent(Mouse_LButton);
			break;
		}
		case WM_LBUTTONUP:
		{
			KeyUpEvent(Mouse_LButton);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			KeyDownEvent(Mouse_RButton);
			break;
		}
		case WM_RBUTTONUP:
		{
			KeyUpEvent(Mouse_RButton);
			break;
		}
		case WM_MBUTTONDOWN:
		{
			KeyDownEvent(Mouse_MButton);
			break;
		}
		case WM_MBUTTONUP:
		{
			KeyUpEvent(Mouse_MButton);
			break;
		}


		// Any messages we don't handle are passed back to Windows default handling
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}

	void Window::RenderScene(IRenderer* renderer)
	{

		Renderer* currentRenderer = static_cast<Renderer*>(renderer);
		//IMGUI
		//*******************************
		// Prepare ImGUI for this frame
		//*******************************

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//*******************************


		//// Common settings ////
		if (currentRenderer->GetRenderingType() == ERenderingType::DirectX11) // Checks the correct renderer
		{
			//DirectX11Renderer* d11Renderer = static_cast<DirectX11Renderer*>(m_Renderer); // Casts the renderer to the correct renderer

			// Sets the correct scene settings
			currentRenderer->PerFrameConstants.ambientColour = CVector3{0.5, 0.5, 0.5};// m_Scenes[m_SceneIndex]->GetAmbientColour();
			currentRenderer->PerFrameConstants.specularPower = 0.3f; //m_Scenes[m_SceneIndex]->GetSpecularPower();
			//currentRenderer->PerFrameConstants.cameraPosition = m_Scenes[m_SceneIndex]->GetCamera()->Position();

			//// Main scene rendering ////

			// Set the back buffer as the target for rendering and select the main depth buffer.
			// When finished the back buffer is sent to the "front buffer" - which is the monitor.
			ID3D11RenderTargetView* backBuffer = currentRenderer->GetBackBuffer();
			currentRenderer->GetDeviceContext()->OMSetRenderTargets(1, &backBuffer, currentRenderer->GetDepthStencil());

			// Clear the back buffer to a fixed colour and the depth buffer to the far distance
			ColourRGBA backgroundColour = ColourRGBA{0.5f, 0.5f, 0.5f};// m_Scenes[m_SceneIndex]->GetBackgroundColour();
			currentRenderer->GetDeviceContext()->ClearRenderTargetView(currentRenderer->GetBackBuffer(), &backgroundColour.r);
			currentRenderer->GetDeviceContext()->ClearDepthStencilView(currentRenderer->GetDepthStencil(), D3D11_CLEAR_DEPTH, 1.0f, 0);

			// Setup the viewport to the size of the main window
			D3D11_VIEWPORT vp;
			vp.Width = static_cast<FLOAT>(m_WindowProps.Width);
			vp.Height = static_cast<FLOAT>(m_WindowProps.Height);
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			currentRenderer->GetDeviceContext()->RSSetViewports(1, &vp);

			// Render the scene from the main camera
			RenderSceneFromCamera(renderer);



			ImGui::Render();
			currentRenderer->GetDeviceContext()->OMSetRenderTargets(1, &backBuffer, nullptr);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			//// Scene completion ////

			// When drawing to the off-screen back buffer is complete, we "present" the image to the front buffer (the screen)
			// Set first parameter to 1 to lock to vsync (typically 60fps)
			currentRenderer->GetSwapChain()->Present(true ? 1 : 0, 0);

		}
	}

	void Window::RenderSceneFromCamera(IRenderer* renderer)
	{

		Renderer* currentRenderer = static_cast<Renderer*>(renderer);
		if (currentRenderer->GetRenderingType() == ERenderingType::DirectX11)
		{
			// Set camera matrices in the constant buffer and send over to GPU
			//currentRenderer->PerFrameConstants.viewMatrix = m_Scenes[m_SceneIndex]->GetCamera()->ViewMatrix();
			//currentRenderer->PerFrameConstants.projectionMatrix = m_Scenes[m_SceneIndex]->GetCamera()->ProjectionMatrix();
			//currentRenderer->PerFrameConstants.viewProjectionMatrix = m_Scenes[m_SceneIndex]->GetCamera()->ViewProjectionMatrix();
			UpdateConstantBuffer(currentRenderer->GetDeviceContext(), currentRenderer->PerFrameConstantBuffer, currentRenderer->PerFrameConstants);

			// Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
			currentRenderer->GetDeviceContext()->VSSetConstantBuffers(0, 1, &currentRenderer->PerFrameConstantBuffer); // First parameter must match constant buffer number in the shader 
			currentRenderer->GetDeviceContext()->PSSetConstantBuffers(0, 1, &currentRenderer->PerFrameConstantBuffer);

		}
	}
}

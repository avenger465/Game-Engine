#include "epch.h"

#include "DirectX11Renderer.h"
#pragma comment(lib, "dxgi.lib")


namespace Engine
{
	DirectX11Renderer::~DirectX11Renderer()
	{
		ShutdownRenderer();
	}

	bool DirectX11Renderer::InitRenderer(WindowProperties& WindowProps)
	{
		// Many DirectX functions return a "HRESULT" variable to indicate success or failure. Microsoft code often uses
		// the FAILED macro to test this variable, you'll see it throughout the code - it's fairly self explanatory.
		HRESULT hr = S_OK;

		m_WindowProps = WindowProps;

		//Display the Hardware information of the computer
		GetHardwareInfo();
		GetCpuInfo();

		//// Initialise DirectX ////

		// Create a Direct3D device (i.e. initialise D3D) and create a swap-chain (create a back buffer to render to)
		DXGI_SWAP_CHAIN_DESC swapDesc = {};
		swapDesc.OutputWindow = m_WindowProps.Hwnd;                           // Target window
	    swapDesc.Windowed = TRUE;
		swapDesc.BufferCount = 1;
		swapDesc.BufferDesc.Width = m_WindowProps.Width;             // Target window size
		swapDesc.BufferDesc.Height = m_WindowProps.Height;            // --"--
		swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Pixel format of target window


		if (m_WindowProps.VSync)
		{
			swapDesc.BufferDesc.RefreshRate.Numerator = numerator;        
			swapDesc.BufferDesc.RefreshRate.Denominator = denominator;    
			ENGINE_CORE_TRACE("Refresh Rate: {0} Hz", swapDesc.BufferDesc.RefreshRate.Numerator / swapDesc.BufferDesc.RefreshRate.Denominator);
		}
		else
		{
			swapDesc.BufferDesc.RefreshRate.Numerator = 0;
			swapDesc.BufferDesc.RefreshRate.Denominator = 1;

			ENGINE_CORE_TRACE("Refresh Rate: {0} Hz", swapDesc.BufferDesc.RefreshRate.Numerator / swapDesc.BufferDesc.RefreshRate.Denominator);
		}
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.SampleDesc.Count = 1;
		swapDesc.SampleDesc.Quality = 0;

		UINT flags = D3D11_CREATE_DEVICE_DEBUG; // Set this to D3D11_CREATE_DEVICE_DEBUG to get more debugging information (in the "Output" window of Visual Studio)
		hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, flags, 0, 0, D3D11_SDK_VERSION,
			&swapDesc, &m_SwapChain, &m_D3DDevice, nullptr, &m_D3DContext);
		if (FAILED(hr))
		{
			ENGINE_CORE_ERROR("Failure creating DirectX11 Device");
			return false;
		}

		// Get a "render target view" of back-buffer - standard behaviour
		ID3D11Texture2D* backBuffer;
		hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
		if (FAILED(hr))
		{
			ENGINE_CORE_ERROR("Failure creating Swap Chain");
			return false;
		}
		hr = m_D3DDevice->CreateRenderTargetView(backBuffer, NULL, &m_BackBufferRenderTarget);
		backBuffer->Release();
		if (FAILED(hr))
		{
			ENGINE_CORE_ERROR("Failure creating Render Target View");
			return false;
		}

		//// Create depth buffer to go along with the back buffer ////

		// First create a texture to hold the depth buffer values
		D3D11_TEXTURE2D_DESC dbDesc = {};
		dbDesc.Width = m_WindowProps.Width; // Same size as viewport / back-buffer
		dbDesc.Height = m_WindowProps.Height;
		dbDesc.MipLevels = 1;
		dbDesc.ArraySize = 1;
		dbDesc.Format = DXGI_FORMAT_D32_FLOAT; // Each depth value is a single float
		dbDesc.SampleDesc.Count = 1;
		dbDesc.SampleDesc.Quality = 0;
		dbDesc.Usage = D3D11_USAGE_DEFAULT;
		dbDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dbDesc.CPUAccessFlags = 0;
		dbDesc.MiscFlags = 0;
		hr = m_D3DDevice->CreateTexture2D(&dbDesc, nullptr, &m_DepthStencilTexture);
		if (FAILED(hr))
		{
			ENGINE_CORE_ERROR("Failure creating Depth Buffer Texture");
			return false;
		}

		// Create the depth stencil view - an object to allow us to use the texture
		// just created as a depth buffer
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = dbDesc.Format;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		hr = m_D3DDevice->CreateDepthStencilView(m_DepthStencilTexture, &dsvDesc,
			&m_DepthStencil);
		if (FAILED(hr))
		{
			ENGINE_CORE_ERROR("Failure creating Depth Stencil View");
			return false;
		}

		// Create GPU-side constant buffers to receive the gPerFrameConstants and gPerModelConstants structures above
		// These allow us to pass data from CPU to shaders such as lighting information or matrices
		// See the comments above where these variable are declared and also the UpdateScene function
		PerFrameConstantBuffer = CreateConstantBuffer(sizeof(PerFrameConstants));
		PerModelConstantBuffer = CreateConstantBuffer(sizeof(PerModelConstants));
		if (PerFrameConstantBuffer == nullptr || PerModelConstantBuffer == nullptr)
		{
			ENGINE_CORE_ERROR("Failure creating Constant Buffers");
			return false;
		}

		//**** Create Scene Texture ****//

		// Using a helper function to load textures from files above. Here we create the Scene texture manually
		// as we are creating a special kind of texture (one that we can render to). Many settings to prepare:
		D3D11_TEXTURE2D_DESC SceneDesc = {};
		SceneDesc.Width =  m_WindowProps.Width;  // Size of the portal texture determines its quality
		SceneDesc.Height = m_WindowProps.Height;
		SceneDesc.MipLevels = 1; // No mip-maps when rendering to textures (or we would have to render every level)
		SceneDesc.ArraySize = 1;
		SceneDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // RGBA texture (8-bits each)
		SceneDesc.SampleDesc.Count = 1;
		SceneDesc.SampleDesc.Quality = 0;
		SceneDesc.Usage = D3D11_USAGE_DEFAULT;
		SceneDesc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE; // IMPORTANT: Indicate we will use texture as render target, and pass it to shaders
		SceneDesc.CPUAccessFlags = 0;
		SceneDesc.MiscFlags = 0;
		if (FAILED(m_D3DDevice->CreateTexture2D(&SceneDesc, NULL, &SceneTexture)))
		{
			ENGINE_CORE_CRITICAL("Error creating Scene Texture");
			return false;
		}

		// We created the Scene texture above, now we get a "view" of it as a render target, i.e. get a special pointer to the texture that
		// we use when rendering to it (see RenderScene function below)
		if (FAILED(m_D3DDevice->CreateRenderTargetView(SceneTexture, NULL, &SceneRenderTarget)))
		{
			ENGINE_CORE_CRITICAL("Error creating Scene Render Target");
			return false;
		}

		// We also need to send this texture (resource) to the shaders. To do that we must create a shader-resource "view"
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc = {};
		srDesc.Format = SceneDesc.Format;
		srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc.Texture2D.MostDetailedMip = 0;
		srDesc.Texture2D.MipLevels = 1;
		if (FAILED(m_D3DDevice->CreateShaderResourceView(SceneTexture, &srDesc, &SceneTextureSRV)))
		{
			ENGINE_CORE_CRITICAL("Error creating Scene Texture View");
			return false;
		}

		//**** Create Scene Depth Buffer ****//
		//**** This depth buffer can be shared with any other textures of the same size
		SceneDesc = {};
		SceneDesc.Width =  m_WindowProps.Width; 
		SceneDesc.Height = m_WindowProps.Height;
		SceneDesc.MipLevels = 1;
		SceneDesc.ArraySize = 1;
		SceneDesc.Format = DXGI_FORMAT_D32_FLOAT; // Depth buffers contain a single float per pixel
		SceneDesc.SampleDesc.Count = 1;
		SceneDesc.SampleDesc.Quality = 0;
		SceneDesc.Usage = D3D11_USAGE_DEFAULT;
		SceneDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
		SceneDesc.CPUAccessFlags = 0;
		SceneDesc.MiscFlags = 0;
		if (FAILED(m_D3DDevice->CreateTexture2D(&SceneDesc, NULL, &SceneDepthStencil)))
		{
			ENGINE_CORE_CRITICAL("Error creating Scene Depth Stencil View");
			return false;
		}

		// Create the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC sceneDescDSV = {};
		sceneDescDSV.Format = SceneDesc.Format;
		sceneDescDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		sceneDescDSV.Texture2D.MipSlice = 0;
		sceneDescDSV.Flags = 0;
		if (FAILED(m_D3DDevice->CreateDepthStencilView(SceneDepthStencil, &sceneDescDSV, &SceneDepthStencilView)))
		{
			ENGINE_CORE_CRITICAL("Error creating Scene Depth Stencil View");
			return false;
		}

		return true;
	}

	void DirectX11Renderer::ShutdownRenderer()
	{
		if (m_D3DContext)
		{
			m_D3DContext->ClearState(); // This line is also needed to reset the GPU before shutting down DirectX
			m_D3DContext->Release();
		}
		if (m_DepthStencil)           m_DepthStencil->Release();
		if (m_DepthStencilTexture)    m_DepthStencilTexture->Release();
		if (m_BackBufferRenderTarget) m_BackBufferRenderTarget->Release();
		if (m_SwapChain)              m_SwapChain->Release();
		if (m_D3DDevice)              m_D3DDevice->Release();
		if (PerFrameConstantBuffer)   PerFrameConstantBuffer->Release();
		if (PerModelConstantBuffer)   PerModelConstantBuffer->Release();
	}

	void DirectX11Renderer::BeginScene(ColourRGBA Background, CVector3 Position)
	{
		PerFrameConstants.ambientColour = CVector3{ 0.5, 0.5, 0.5 };// m_Scenes[m_SceneIndex]->GetAmbientColour();
		PerFrameConstants.specularPower = 0.3f; //m_Scenes[m_SceneIndex]->GetSpecularPower();
		PerFrameConstants.cameraPosition = Position;

		m_D3DContext->OMSetRenderTargets(1, &SceneRenderTarget, SceneDepthStencilView);
		m_D3DContext->ClearRenderTargetView(SceneRenderTarget, &Background.r);
		m_D3DContext->ClearDepthStencilView(SceneDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		D3D11_VIEWPORT vp;
		vp.Width = static_cast<FLOAT>(m_WindowProps.Width);
		vp.Height = static_cast<FLOAT>(m_WindowProps.Height);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		m_D3DContext->RSSetViewports(1, &vp);
	}

	void DirectX11Renderer::EndScene(bool VSync)
	{
		m_SwapChain->Present(m_WindowProps.VSync ? 1 : 0, 0);
	}

	ID3D11Buffer* DirectX11Renderer::CreateConstantBuffer(int size)
	{
		D3D11_BUFFER_DESC cbDesc;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.ByteWidth = 16 * ((size + 15) / 16);     // Constant buffer size must be a multiple of 16 - this maths rounds up to the nearest multiple
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;             // Indicates that the buffer is frequently updated
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU is only going to write to the constants (not read them)
		cbDesc.MiscFlags = 0;
		ID3D11Buffer* constantBuffer;
		HRESULT hr = m_D3DDevice->CreateBuffer(&cbDesc, nullptr, &constantBuffer);
		if (FAILED(hr))
		{
			return nullptr;
		}

		return constantBuffer;
	}
	bool DirectX11Renderer::LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
	{
		return false;
	}
	void DirectX11Renderer::GetRenderStats(float frameTime, std::string& FPS, std::string& FrameTime)
	{
		const float fpsUpdateTime = 0.5f; // How long between updates (in seconds)
		static float totalFrameTime = 0;
		static int frameCount = 0;
		totalFrameTime += frameTime;
		++frameCount;

		if (totalFrameTime > fpsUpdateTime)
		{
			// Displays FPS rounded to nearest int, and frame time (more useful for developers) in milliseconds to 2 decimal places
			float avgFrameTime = totalFrameTime / frameCount;
			std::ostringstream frameTimeMs;
			frameTimeMs.precision(2);
			frameTimeMs << std::fixed << avgFrameTime * 1000;
			FrameTime = frameTimeMs.str();
			totalFrameTime = 0;
			frameCount = 0;
			FPS = std::to_string(static_cast<int>(1 / avgFrameTime + 0.5f));
		}
	}
	void DirectX11Renderer::GetHardwareInfo()
	{
		IDXGIFactory* factory = nullptr;
		IDXGIAdapter* pAdapter;
		DXGI_ADAPTER_DESC desc;
		IDXGIOutput* adapterOutput;

		
		HRESULT result = CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)&factory);

		factory->EnumAdapters(0, &pAdapter);

		

		pAdapter->EnumOutputs(0, &adapterOutput);

		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
		if (FAILED(result))
		{
			ENGINE_CORE_ERROR("Failure to get display mode list");
		}

		displayModeList = new DXGI_MODE_DESC[numModes];
		if (!displayModeList)
		{
			ENGINE_CORE_ERROR("Failure to initialise display mode list");
		}

		// Now fill the display mode list structures.
		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
		if (FAILED(result))
		{
			ENGINE_CORE_ERROR("Failure to initialise display mode list");
		}


		// Now go through all the display modes and find the one that matches the screen width and height.
		// When a match is found store the numerator and denominator of the refresh rate for that monitor.
		for (i = 0; i < numModes; i++)
		{
			if (displayModeList[i].Width == (unsigned int)m_WindowProps.Width)
			{
				if (displayModeList[i].Height == (unsigned int)m_WindowProps.Height)
				{
					numerator = displayModeList[i].RefreshRate.Numerator;
					denominator = displayModeList[i].RefreshRate.Denominator;
				}
			}
		}

		pAdapter->GetDesc(&desc);
		m_videoCardMemory = (int)(desc.DedicatedVideoMemory / 1024 / 1024);
		ENGINE_CORE_INFO("{0} MB", m_videoCardMemory);

		size_t stringLength = 0;
		wcstombs_s(&stringLength, m_videoCardDescription, 128, desc.Description, 128);
		ENGINE_CORE_INFO("{0}", m_videoCardDescription);

		SAFE_RELEASE(pAdapter);
		SAFE_RELEASE(factory);
		SAFE_RELEASE(adapterOutput);
	}

	std::string DirectX11Renderer::GetCpuInfo()
	{
		// 4 is essentially hardcoded due to the __cpuid function requirements.
		// NOTE: Results are limited to whatever the sizeof(int) * 4 is...
		std::array<int, 4> integerBuffer = {};
		constexpr size_t sizeofIntegerBuffer = sizeof(int) * integerBuffer.size();

		std::array<char, 64> charBuffer = {};

		// The information you wanna query __cpuid for.
		// https://docs.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex?view=vs-2019
		constexpr std::array<int, 3> functionIds = {
			// Manufacturer
			//  EX: "Intel(R) Core(TM"
			0x8000'0002,
			// Model
			//  EX: ") i7-8700K CPU @"
			0x8000'0003,
			// Clockspeed
			//  EX: " 3.70GHz"
			0x8000'0004
		};

		std::string cpu;

		for (int id : functionIds)
		{
			// Get the data for the current ID.
			__cpuid(integerBuffer.data(), id);

			// Copy the raw data from the integer buffer into the character buffer
			std::memcpy(charBuffer.data(), integerBuffer.data(), sizeofIntegerBuffer);

			// Copy that data into a std::string
			cpu += std::string(charBuffer.data());
		}

		ENGINE_CORE_INFO("{0}", cpu);

		return cpu;
	}
}



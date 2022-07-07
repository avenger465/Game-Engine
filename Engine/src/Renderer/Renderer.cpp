#include "epch.h"
#include "Renderer.h"

namespace Engine
{
	Renderer::~Renderer()
	{
		ShutdownRenderer();
	}

	bool Renderer::InitRenderer(WindowProperties& WindowProps)
	{
		// Many DirectX functions return a "HRESULT" variable to indicate success or failure. Microsoft code often uses
		// the FAILED macro to test this variable, you'll see it throughout the code - it's fairly self explanatory.
		HRESULT hr = S_OK;

		m_WindowProps = WindowProps;

		//// Initialise DirectX ////

		// Create a Direct3D device (i.e. initialise D3D) and create a swap-chain (create a back buffer to render to)
		DXGI_SWAP_CHAIN_DESC swapDesc = {};
		swapDesc.OutputWindow = m_WindowProps.Hwnd;                           // Target window
		swapDesc.Windowed = TRUE;
		swapDesc.BufferCount = 1;
		swapDesc.BufferDesc.Width = m_WindowProps.Width;             // Target window size
		swapDesc.BufferDesc.Height = m_WindowProps.Height;            // --"--
		swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Pixel format of target window
		swapDesc.BufferDesc.RefreshRate.Numerator = 60;        // Refresh rate of monitor (provided as fraction = 60/1 here)
		swapDesc.BufferDesc.RefreshRate.Denominator = 1;         // --"--
		swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapDesc.SampleDesc.Count = 1;
		swapDesc.SampleDesc.Quality = 0;
		UINT flags = 0; // Set this to D3D11_CREATE_DEVICE_DEBUG to get more debugging information (in the "Output" window of Visual Studio)
		hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, flags, 0, 0, D3D11_SDK_VERSION,
			&swapDesc, &m_SwapChain, &m_D3DDevice, nullptr, &m_D3DContext);
		if (FAILED(hr))
		{
			/*ErrorLogger errorLog;
			errorLog.ErrorMessage(m_Props, "Error creating Direct3D device");*/
			return false;
		}


		// Get a "render target view" of back-buffer - standard behaviour
		ID3D11Texture2D* backBuffer;
		hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
		if (FAILED(hr))
		{
			/*ErrorLogger errorLog;
			errorLog.ErrorMessage(m_Props, "Error creating swap chain");*/
			return false;
		}
		hr = m_D3DDevice->CreateRenderTargetView(backBuffer, NULL, &m_BackBufferRenderTarget);
		backBuffer->Release();
		if (FAILED(hr))
		{
			/*ErrorLogger errorLog;
			errorLog.ErrorMessage(m_Props, "Error creating render target view");*/
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
			/*ErrorLogger errorLog;
			errorLog.ErrorMessage(m_Props, "Error creating depth buffer texture");*/
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
			/*ErrorLogger errorLog;
			errorLog.ErrorMessage(m_Props, "Error creating depth buffer view");*/
			return false;
		}

		// Create GPU-side constant buffers to receive the gPerFrameConstants and gPerModelConstants structures above
		// These allow us to pass data from CPU to shaders such as lighting information or matrices
		// See the comments above where these variable are declared and also the UpdateScene function
		PerFrameConstantBuffer = CreateConstantBuffer(sizeof(PerFrameConstants));
		PerModelConstantBuffer = CreateConstantBuffer(sizeof(PerModelConstants));
		if (PerFrameConstantBuffer == nullptr || PerModelConstantBuffer == nullptr)
		{
			/*ErrorLogger errorLog;
			errorLog.ErrorMessage(m_Props, "Error creating constant buffers");*/
			return false;
		}


		return true;
	}


	void Renderer::ShutdownRenderer()
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

	ID3D11Buffer* Renderer::CreateConstantBuffer(int size)
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
	bool Renderer::LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
	{
		return false;
	}
}



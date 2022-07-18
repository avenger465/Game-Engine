#pragma once
#include "System\Interfaces\IRenderer.h"
#include "Math\CVector3.h"
#include "Math\CMatrix4x4.h"

#include <winnt.h>


struct PerFrameConstants
{
	// These are the matrices used to position the camera
	CMatrix4x4 viewMatrix;
	CMatrix4x4 projectionMatrix;
	CMatrix4x4 viewProjectionMatrix; // The above two matrices multiplied together to combine their effects

	CVector3   light1Position; // 3 floats: x, y z
	float      padding1;       // Pad above variable to float4 (HLSL requirement - which we must duplicate in this the C++ version of the structure)
	CVector3   light1Colour;
	float      padding2;

	CVector3   ambientColour;
	float      specularPower;

	CVector3   cameraPosition;
	float      padding4;
};

struct PerModelConstants
{
	CMatrix4x4 worldMatrix;
	CVector3   objectColour; // Allows each light model to be tinted to match the light colour they cast
	float      paddingA;
};


namespace Engine
{
	class Renderer : public IRenderer
	{
	public:

		~Renderer();
		
		virtual bool InitRenderer(WindowProperties& WindowProps) override;

		virtual void ShutdownRenderer() override;

		virtual const ERenderingAPI GetRenderingType() override {return ERenderingAPI::DirectX11;}
		
		virtual WindowProperties GetWindowProperties() override { return m_WindowProps; }


	public:
		ID3D11Device* GetDevice() { return m_D3DDevice; } // Returns the DirectX11 device
		ID3D11DeviceContext* GetDeviceContext() { return m_D3DContext; } // Returns the DirectX11 device context
		IDXGISwapChain* GetSwapChain() { return m_SwapChain; } // Returns the DirectX11 Swapchain
		ID3D11RenderTargetView* GetBackBuffer() { return m_BackBufferRenderTarget; } // Returns the DirectX11 BackBuffer
		ID3D11DepthStencilView* GetDepthStencil() { return m_DepthStencil; } // Returns the DirectX11 Depth Stencil

		ID3D11RenderTargetView* GetSceneRenderTarget() { return SceneRenderTarget; } // Returns the DirectX11 BackBuffer
		ID3D11ShaderResourceView* GetSceneShaderResourceView() { return SceneTextureSRV; } // Returns the DirectX11 BackBuffer
		ID3D11DepthStencilView* GetSceneDepthStencil() { return SceneDepthStencilView; } // Returns the DirectX11 Depth Stencil

		ID3D11Buffer* CreateConstantBuffer(int size); // Function used for creating a constant buffer

		bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);

	private:
		void GetHardwareInfo();
		std::string GetCpuInfo();

	public:
		PerFrameConstants PerFrameConstants; // Used for setting per frame constant variables and sending them to the GPU
		ID3D11Buffer* PerFrameConstantBuffer;

		PerModelConstants PerModelConstants;  // Used for setting per model constant variables and sending them to the GPU
		ID3D11Buffer* PerModelConstantBuffer;


	private:

		// The main Direct3D (D3D) variables
		ID3D11Device* m_D3DDevice = nullptr; // D3D device for overall features
		ID3D11DeviceContext* m_D3DContext = nullptr; // D3D context for specific rendering tasks

		// Swap chain and back buffer
		IDXGISwapChain* m_SwapChain = nullptr;
		ID3D11RenderTargetView* m_BackBufferRenderTarget = nullptr;

		// Depth buffer (can also contain "stencil" values, which we will see later)
		ID3D11Texture2D* m_DepthStencilTexture = nullptr; // The texture holding the depth values
		ID3D11DepthStencilView* m_DepthStencil = nullptr; // The depth buffer referencing above texture

		// The scene texture - each frame it is rendered to, then it is used as a texture for model
		ID3D11Texture2D* SceneTexture = nullptr; // This object represents the memory used by the texture on the GPU
		ID3D11RenderTargetView* SceneRenderTarget = nullptr; // This object is used when we want to render to the texture above
		ID3D11ShaderResourceView* SceneTextureSRV = nullptr; // This object is used to give shaders access to the texture above (SRV = shader resource view)

		ID3D11Texture2D* SceneDepthStencil = nullptr; // This object represents the memory used by the texture on the GPU
		ID3D11DepthStencilView* SceneDepthStencilView = nullptr; // This object is used when we want to use the texture above as the depth buffer

		WindowProperties m_WindowProps; // Used for getting the window properties

		char m_videoCardDescription[128];
		int m_videoCardMemory;
	};
}

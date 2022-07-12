#pragma once
#include "System\Interfaces\IShader.h"

namespace Engine
{
	enum class EVertexShader
	{
		PixelLightingVertexShader = 0,
		BasicTransformVertexShader,
		SkinningVertexShader
	};

	enum class EPixelShader
	{
		PixelLightingPixelShader = 0,
		LightModelPixelShader
	};

	class DirectX11Shader : public IShader
	{
	public:
		~DirectX11Shader();

		// Initialize the shaders 
		virtual bool InitShaders(IRenderer* renderer) override;

		// Release the shaders
		virtual void ReleaseShaders() override;

		// Returns the selected vertex shader
		ID3D11VertexShader* GetVertexShader(EVertexShader vertexShader);

		// Returns the selected pixel shader
		ID3D11PixelShader* GetPixelShader(EPixelShader pixelShader);

	private:

		ID3D11VertexShader* LoadVertexShader(std::string shaderName); // Loads the vertex shader
		ID3D11PixelShader* LoadPixelShader(std::string shaderName); // Loads the pixel shader

	private:
		IRenderer* m_Renderer; // Used for getting the renderer in use

		// Vertex shader member variables
		ID3D11VertexShader* m_PixelLightingVertexShader;
		ID3D11VertexShader* m_BasicTransformVertexShader;
		ID3D11VertexShader* m_SkinningVertexShader;

		// Pixel shader member variables
		ID3D11PixelShader* m_LightModelPixelShader;
		ID3D11PixelShader* m_PixelLightingPixelShader;

	};
}

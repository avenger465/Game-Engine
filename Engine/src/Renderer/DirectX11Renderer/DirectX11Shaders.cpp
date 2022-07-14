#include "epch.h"
#include "DirectX11Shaders.h"
#include "Renderer\DirectX11Renderer\Renderer.h"

namespace Engine
{
	DirectX11Shader::~DirectX11Shader()
	{
		delete m_Renderer;

		ReleaseShaders();
	}

	bool DirectX11Shader::InitShaders(IRenderer* renderer)
	{
		m_Renderer = renderer;
		if (m_Renderer->GetRenderingType() == ERenderingAPI::DirectX11)
		{
			std::filesystem::path MainPath = std::filesystem::current_path(); // Gets the current working directory

		// Creates the path to the shader path
			std::filesystem::path shaderPath = std::filesystem::current_path().parent_path().append("Engine\\src\\Renderer\\DirectX11Renderer\\Shaders\\");

			std::filesystem::current_path(shaderPath); // Sets the current path to the shader path

			// Loads the vertex shaders
			m_PixelLightingVertexShader = LoadVertexShader("PixelLighting_vs");
			m_BasicTransformVertexShader = LoadVertexShader("BasicTransform_vs");

			// Loads the pixel shaders
			m_LightModelPixelShader = LoadPixelShader("LightModel_ps");
			m_PixelLightingPixelShader = LoadPixelShader("PixelLighting_ps");

			std::filesystem::current_path(MainPath); // Resets path to the main directory

			// CHecks that the shaders are not nullptr
			if (m_PixelLightingVertexShader == nullptr || m_PixelLightingPixelShader == nullptr ||
				m_BasicTransformVertexShader == nullptr || m_SkinningVertexShader == nullptr || m_LightModelPixelShader == nullptr)
			{
				E_CORE_ERROR("Error Creating Shaders");
				return false;
			}
		}

		return true;
	}

	void DirectX11Shader::ReleaseShaders()
	{
	}

	ID3D11VertexShader* DirectX11Shader::GetVertexShader(EVertexShader vertexShader)
	{
		// Returns the selected vertex shader
		switch (vertexShader)
		{
		case EVertexShader::PixelLightingVertexShader:
			return m_PixelLightingVertexShader;
			break;
		case EVertexShader::BasicTransformVertexShader:
			return m_BasicTransformVertexShader;
			break;
		case EVertexShader::SkinningVertexShader:
			return m_SkinningVertexShader;
			break;
		default:
			return m_BasicTransformVertexShader;
		}
	}

	ID3D11PixelShader* DirectX11Shader::GetPixelShader(EPixelShader pixelShader)
	{
		// Returns the selected pixel shader
		switch (pixelShader)
		{
		case EPixelShader::PixelLightingPixelShader:
			return m_PixelLightingPixelShader;
			break;
		case EPixelShader::LightModelPixelShader:
			return m_LightModelPixelShader;
			break;
		default:
			return m_LightModelPixelShader;
		}
	}

	ID3D11VertexShader* DirectX11Shader::LoadVertexShader(std::string shaderName)
	{
		// Open compiled shader object file
		std::ifstream shaderFile(shaderName + ".cso", std::ios::in | std::ios::binary | std::ios::ate);
		if (!shaderFile.is_open())
		{
			return nullptr;
		}

		// Read file into vector of chars
		std::streamoff fileSize = shaderFile.tellg();
		shaderFile.seekg(0, std::ios::beg);
		std::vector<char> byteCode(fileSize);
		shaderFile.read(&byteCode[0], fileSize);
		if (shaderFile.fail())
		{
			return nullptr;
		}

		// Create shader object from loaded file (we will use the object later when rendering)
		ID3D11VertexShader* shader;

		if (m_Renderer->GetRenderingType() == ERenderingAPI::DirectX11)
		{
			Renderer* dx11Renderer = static_cast<Renderer*>(m_Renderer);

			HRESULT hr = dx11Renderer->GetDevice()->CreateVertexShader(byteCode.data(), byteCode.size(), nullptr, &shader);
			if (FAILED(hr))
			{
				return nullptr;
			}

			return shader;
		}

	}

	ID3D11PixelShader* DirectX11Shader::LoadPixelShader(std::string shaderName)
	{
		// Open compiled shader object file
		std::ifstream shaderFile(shaderName + ".cso", std::ios::in | std::ios::binary | std::ios::ate);
		if (!shaderFile.is_open())
		{
			return nullptr;
		}

		// Read file into vector of chars
		std::streamoff fileSize = shaderFile.tellg();
		shaderFile.seekg(0, std::ios::beg);
		std::vector<char>byteCode(fileSize);
		shaderFile.read(&byteCode[0], fileSize);
		if (shaderFile.fail())
		{
			return nullptr;
		}

		// Create shader object from loaded file (we will use the object later when rendering)
		ID3D11PixelShader* shader;

		if (m_Renderer->GetRenderingType() == ERenderingAPI::DirectX11)
		{
			Renderer* dx11Renderer = static_cast<Renderer*>(m_Renderer);

			HRESULT hr = dx11Renderer->GetDevice()->CreatePixelShader(byteCode.data(), byteCode.size(), nullptr, &shader);
			if (FAILED(hr))
			{
				return nullptr;
			}
		}
		return shader;
	}
}

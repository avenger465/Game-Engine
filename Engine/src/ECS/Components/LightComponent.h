#pragma once
#include "Data\Mesh.h"
#include "Data\Model.h"
#include "ECS\Entity.h"
#include "ECS\BaseComponent.h"
#include "ECS\Components\MeshComponent.h"
#include "System\Interfaces\IShader.h"
#include "System\Interfaces\IState.h"
#include "Renderer\DirectX11Renderer\DirectX11Shaders.h"
#include "Renderer\DirectX11Renderer\DirectX11States.h"
#include "Utility\GraphicsHelpers.h"

namespace Engine
{
	class LightComponent : public BaseEntityComponent
	{
	public:
		LightComponent(IRenderer* renderer, Entity* entity, EntityUID UID, IShader* shader, IState* state, std::string filePath = "media/Flare.jpg",
			CVector3 colour = CVector3(0.0f, 0.0f, 1.0f), float strength = 10.0f) : BaseEntityComponent("Light Renderer", UID, entity)
		{
			m_Renderer = renderer;
			m_Colour = colour;
			m_Strength = strength;

			m_Entity = entity;

			if (entity->GetComponent("Mesh") && m_Renderer->GetRenderingType() == ERenderingAPI::DirectX11)
			{
				Renderer* dx11Renderer = static_cast<Renderer*>(m_Renderer);
				MeshComponent* comp = static_cast<MeshComponent*>(entity->GetComponent("Mesh"));

				DirectX11Shader* dx11Shader = static_cast<DirectX11Shader*>(shader);
				DirectX11States* dx11State = static_cast<DirectX11States*>(state);



				if (comp->GetMeshPath() != "")
				{

					std::filesystem::path MainPath = std::filesystem::current_path();

					std::filesystem::path meshPath = std::filesystem::current_path().parent_path().append("Engine\\");
					std::filesystem::current_path(meshPath);

					m_Mesh = new Mesh(dx11Renderer, comp->GetMeshPath());
					m_Model = new Model(m_Mesh);


					if (!LoadTexture(dx11Renderer, filePath, &m_TextureResource, &m_TextureSRV))
					{
						// Error Check here
					}

					std::filesystem::current_path(MainPath);

					m_PixelShader = dx11Shader->GetPixelShader(EPixelShader::LightModelPixelShader);
					m_VertexShader = dx11Shader->GetVertexShader(EVertexShader::BasicTransformVertexShader);

					m_BlendState = dx11State->GetBlendState(EBlendingState::AdditiveBlendingState);
					m_DepthStencilState = dx11State->GetDepthStencilState(EDepthStencilState::DepthReadOnlyState);
					m_RasterizerState = dx11State->GetRasterizerState(ERasterizerState::CullNoneState);
					m_SamplerState = dx11State->GetSamplerState(ESamplingState::Anisotropic4xSampler);
				}
				else
				{
					m_Mesh = nullptr;
					m_Model = nullptr;
				}
			}
		}

		~LightComponent();
		virtual bool Update(float frametime) override;
		void Render();
		Model* GetModel() { return m_Model; }
		const float GetStrength() { return m_Strength; }

	private:
		IRenderer* m_Renderer;

		CVector3 m_Colour;
		float m_Strength;
		Entity* m_Entity;
		Mesh* m_Mesh;
		Model* m_Model;

		ID3D11Resource* m_TextureResource;
		ID3D11ShaderResourceView* m_TextureSRV;

		ID3D11PixelShader* m_PixelShader;
		ID3D11VertexShader* m_VertexShader;

		ID3D11BlendState* m_BlendState;
		ID3D11DepthStencilState* m_DepthStencilState;
		ID3D11RasterizerState* m_RasterizerState;
		ID3D11SamplerState* m_SamplerState;
	};
}

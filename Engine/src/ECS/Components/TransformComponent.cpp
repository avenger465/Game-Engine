#include "epch.h"
#include "TransformComponent.h"

namespace Engine
{
	TransformComponent::~TransformComponent()
	{
		//if(m_Entity != nullptr) delete m_Entity;
	}
	bool TransformComponent::Update(float frameTime)
	{
		if (m_Entity->GetComponent("Mesh") && m_Entity->GetComponent("Renderer"))
		{
			Engine::RendererComponent* comp = static_cast<Engine::RendererComponent*>(m_Entity->GetComponent("Renderer"));
			comp->GetModel()->SetPosition(m_Position);
			comp->GetModel()->SetRotation(m_Rotation);
			comp->GetModel()->SetScale(m_Scale);
		}
		else if (m_Entity->GetComponent("Mesh") && m_Entity->GetComponent("Light Renderer"))
		{
			LightComponent* comp = static_cast<LightComponent*>(m_Entity->GetComponent("Light Renderer"));
			comp->GetModel()->SetPosition(m_Position);
			comp->GetModel()->SetRotation(m_Rotation);

			comp->GetModel()->SetScale(m_Scale);
		}
		return true;
	}
}

#pragma once
#include "ECS\Entity.h"
#include "ECS\BaseComponent.h"
#include "ECS\Components\LightComponent.h"
#include "ECS\Components\RendererComponent.h"
#include "Math\CVector3.h"
#include "Math\CMatrix4x4.h"

namespace Engine
{
	class TransformComponent : public BaseEntityComponent
	{
	public:
		TransformComponent(Entity* entity, EntityUID UID, CVector3 position = CVector3(0.0f, 0.0f, 0.0f),
			CVector3 rotation = CVector3(0.0f, 0.0f, 0.0f), CVector3 scale = CVector3(1.0f, 1.0f, 1.0f)) : BaseEntityComponent("Transform", UID, entity)
		{
			m_Entity = entity;
			m_Position = position;
			m_Rotation = rotation;
			m_Scale = scale;

			if (m_Entity->GetComponent("Mesh") && m_Entity->GetComponent("Renderer"))
			{
				Engine::RendererComponent* comp = static_cast<Engine::RendererComponent*>(m_Entity->GetComponent("Renderer"));
				comp->GetModel()->SetPosition(m_Position);
				comp->GetModel()->SetRotation(m_Rotation);
				comp->GetModel()->SetScale(m_Scale);
			}
			if (m_Entity->GetComponent("Mesh") && m_Entity->GetComponent("Light Renderer"))
			{
				LightComponent* comp = static_cast<LightComponent*>(m_Entity->GetComponent("Light Renderer"));
				comp->GetModel()->SetPosition(m_Position);
				comp->GetModel()->SetRotation(m_Rotation);
				comp->GetModel()->SetScale(m_Scale);

			}

		}

		~TransformComponent();

		void SetPosition(CVector3 pos) { m_Position = pos; }
		void SetRotation(CVector3 rot, int node = 0)
		{
			m_Rotation = rot;

		}
		void SetRotationFromQuat(CVector3 quat, float w, int node = 0)
		{
			CMatrix4x4 m = CMatrix4x4(quat, w);

			if (node != 0 && m_Entity->GetComponent("Mesh") && m_Entity->GetComponent("Renderer"))
			{
				Engine::RendererComponent* comp = static_cast<Engine::RendererComponent*>(m_Entity->GetComponent("Renderer"));
				comp->GetModel()->SetRotation(m.GetEulerAngles(), node);
			}
			else
			{
				m_Rotation = m.GetEulerAngles();
			}
		}
		void SetScale(CVector3 scale) { m_Scale = scale; }

		const CVector3 GetPosition() { return m_Position; }
		const CVector3 GetRotation() { return m_Rotation; }
		const CVector3 GetScale() { return m_Scale; }

		const CVector3 GetFacingVector(int node = 0) // Returns facing vector
		{
			if (m_Entity->GetComponent("Mesh") && m_Entity->GetComponent("Renderer"))
			{
				Engine::RendererComponent* comp = static_cast<Engine::RendererComponent*>(m_Entity->GetComponent("Renderer"));
				return comp->GetModel()->GetWorldMatrix(node).GetZAxis();
			}
			if (m_Entity->GetComponent("Mesh") && m_Entity->GetComponent("Light Renderer"))
			{
				LightComponent* comp = static_cast<LightComponent*>(m_Entity->GetComponent("Light Renderer"));
				comp->GetModel()->GetWorldMatrix(node);
				return comp->GetModel()->GetWorldMatrix(node).GetZAxis();
			}
		}

		CVector3 GetYAxis(int node = 0)
		{
			if (m_Entity->GetComponent("Mesh") && m_Entity->GetComponent("Renderer"))
			{
				Engine::RendererComponent* comp = static_cast<Engine::RendererComponent*>(m_Entity->GetComponent("Renderer"));
				return comp->GetModel()->GetWorldMatrix(node).GetZAxis();
			}
			if (m_Entity->GetComponent("Mesh") && m_Entity->GetComponent("Light Renderer"))
			{
				LightComponent* comp = static_cast<LightComponent*>(m_Entity->GetComponent("Light Renderer"));
				comp->GetModel()->GetWorldMatrix(node);
				return comp->GetModel()->GetWorldMatrix(node).GetZAxis();
			}
		}

		virtual bool Update(float frameTime) override;
	private:
		Entity* m_Entity;
		CVector3 m_Position;
		CVector3 m_Rotation;
		CVector3 m_Scale;
	};
}

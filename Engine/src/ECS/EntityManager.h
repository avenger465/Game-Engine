#pragma once
#include "Utility\HashTable.h"
#include "Entity.h"
#include "System\Interfaces\IRenderer.h"
#include "System\Interfaces\IShader.h"
#include "System\Interfaces\IState.h"
#include "Renderer\DirectX11Renderer\DirectX11Shaders.h"
#include "Renderer\DirectX11Renderer\DirectX11States.h"
#include "ECS/Components/TransformComponent.h"
#include "Math\CVector3.h"

namespace Engine
{
	struct EntityTransform
	{
		CVector3 Position;
		CVector3 Rotation;
		CVector3 Scale;

		EntityTransform(CVector3 pos = CVector3(0.0f, 0.0f, 0.0f), CVector3 rot = CVector3(0.0f, 0.0f, 0.0f), CVector3 scale = CVector3(1.0f, 1.0f, 1.0f))
			: Position(pos), Rotation(rot), Scale(scale)
		{}
		EntityTransform(float x, float y, float z) : Position({ x, y, z }), Rotation(CVector3(0.0f, 0.0f, 0.0f)), Scale(CVector3(1.0f, 1.0f, 1.0f))
		{
		}
	};

	class EntityManager
	{
	public:
		EntityManager(IRenderer* renderer);
		~EntityManager();

	public:
		EntityUID CreateEntity(const std::string& name);
		EntityUID CreateModelEntity(const std::string& name, std::string fileMeshPath, bool isRendered, std::string filePath,
			EntityTransform transform = EntityTransform(),
			EPixelShader pixelShader = EPixelShader::PixelLightingPixelShader,
			EVertexShader vertexShader = EVertexShader::PixelLightingVertexShader,
			EBlendingState blendingState = EBlendingState::NoBlendingState,
			EDepthStencilState depthStencilState = EDepthStencilState::UseDepthBufferState,
			ERasterizerState rasterizerState = ERasterizerState::CullNoneState,
			ESamplingState samplingState = ESamplingState::Anisotropic4xSampler);

		EntityUID CreateLightEntity(const std::string& name);

		bool DestroyEntity(EntityUID UID);

		void DestroyAllEntities();

		const EntityUID GetNewUID() { return m_NextUID++; }

		const TInt32 NumEntities()
		{
			return static_cast<TInt32>(m_Entities.size());
		}

		const Entity* GetEntityAtIndex(TInt32 index)
		{
			return m_Entities[index];
		}

		Entity* GetEntity(EntityUID UID)
		{
			TInt32 entityIndex;
			if (!m_EntityUIDMap->LookUpKey(UID, &entityIndex))
			{
				return nullptr;
			}
			return m_Entities[entityIndex];
		}

		// Return the entity with the given name & optionally the given template name & type
		Entity* GetEntity(const std::string& name)
		{
			TEntityIter entity = m_Entities.begin();
			while (entity != m_Entities.end())
			{
				if ((*entity)->GetName() == name)
				{
					return (*entity);
				}
				++entity;
			}
			return nullptr;
		}

		void BeginEnumEntities(const std::string& name)
		{
			m_IsEnumerating = true;
			m_EnumEntity = m_Entities.begin();
			m_EnumName = name;
		}

		void EndEnumEntities()
		{
			m_IsEnumerating = false;
		}

		const Entity* EnumEntity()
		{
			if (!m_IsEnumerating)
			{
				return nullptr;
			}

			while (m_EnumEntity != m_Entities.end())
			{
				if ((m_EnumName.length() == 0 || (*m_EnumEntity)->GetName() == m_EnumName))
				{
					Entity* foundEntity = *m_EnumEntity;
					++m_EnumEntity;
					return foundEntity;
				}
				++m_EnumEntity;
			}

			m_IsEnumerating = false;
			return nullptr;

		}

		void UpdateAllEntities(float updateTime);

		void RenderAllEntities();

		const IShader* GetShader() { return m_Shader; }

	private:
		EntityManager(const EntityManager&);
		EntityManager& operator=(const EntityManager&);

	private:
		IRenderer* m_Renderer;

		typedef std::vector<Entity*> TEntities;
		typedef TEntities::iterator TEntityIter;

		TEntities m_Entities;

		CHashTable<EntityUID, TUInt32>* m_EntityUIDMap;

		EntityUID m_NextUID;

		bool m_IsEnumerating;
		TEntityIter m_EnumEntity;
		std::string m_EnumName;

		IShader* m_Shader;
		IState* m_State;

		bool m_IsPhysics = false;
	};
}

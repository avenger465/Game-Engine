#include "epch.h"
#include "EntityManager.h"


namespace Engine
{
	EntityManager::EntityManager(IRenderer* renderer)
	{
		m_Renderer = renderer;
		m_Entities.reserve(1024);
		m_EntityUIDMap = new CHashTable<EntityUID, TUInt32>(2048, JOneAtATimeHash);

		m_NextUID = 0;
		m_IsEnumerating = true;
		

		ENGINE_CORE_INFO("Initialising Shaders");
		m_Shader = SetShader(m_Renderer->GetRenderingType());
		if(!m_Shader->InitShaders(m_Renderer)) ENGINE_CORE_ERROR("Error Initialising Shaders");

		ENGINE_CORE_INFO("Initialising Shaders");
		m_State = SetStates(m_Renderer->GetRenderingType());
		if(!m_State->InitStates(m_Renderer)) ENGINE_CORE_ERROR("Error Initialising States");
	}

	EntityManager::~EntityManager()
	{
		DestroyAllEntities();
		m_Shader->ReleaseShaders();
		m_State->ReleaseStates();
	}

	EntityUID EntityManager::CreateEntity(const std::string& name)
	{
		// Create new entity with next UID
		Entity* newEntity = new Entity(m_NextUID, name);

		// Add transform component to all entities 
		TransformComponent* comp = new TransformComponent(newEntity, GetNewUID());
		newEntity->AddComponent(comp);

		// Get vector index for new entity and add it to vector
		TUInt32 entityIndex = static_cast<TUInt32>(m_Entities.size());
		m_Entities.push_back(newEntity);

		// Add mapping from UID to entity index into hash map
		m_EntityUIDMap->SetKeyValue(m_NextUID, entityIndex);

		m_IsEnumerating = false; // Cancel any entity enumeration (entity list has changed)

		// Return UID of new entity then increase it ready for next entity
		return m_NextUID++;
	}

	EntityUID EntityManager::CreateModelEntity(const std::string & name, std::string fileMeshPath, bool isRendered, std::string filePath, EntityTransform transform,
		EPixelShader pixelShader, EVertexShader vertexShader, EBlendingState blendState, EDepthStencilState depthStencilState,
		ERasterizerState rasterizerState, ESamplingState samplerState)
	{
		// Create new entity with next UID
		Entity* newEntity = new Entity(m_NextUID, name);


		BaseEntityComponent* comp = new MeshComponent(fileMeshPath, newEntity, GetNewUID());
		newEntity->AddComponent(comp);

		comp = new RendererComponent(isRendered, m_Renderer, newEntity, GetNewUID(), m_Shader, m_State, filePath, pixelShader,
			vertexShader, blendState, depthStencilState, rasterizerState, samplerState);
		newEntity->AddComponent(comp);

		// Add transform component to all entities 
		comp = new TransformComponent(newEntity, GetNewUID(), transform.Position, transform.Rotation, transform.Scale);
		newEntity->AddComponent(comp);

		// Get vector index for new entity and add it to vector
		TUInt32 entityIndex = static_cast<TUInt32>(m_Entities.size());
		m_Entities.push_back(newEntity);

		// Add mapping from UID to entity index into hash map
		m_EntityUIDMap->SetKeyValue(m_NextUID, entityIndex);

		m_IsEnumerating = false; // Cancel any entity enumeration (entity list has changed)

		// Return UID of new entity then increase it ready for next entity
		return m_NextUID++;
	}


	EntityUID EntityManager::CreateLightEntity(const std::string& name)
	{
		// Create new entity with next UID
		Entity* newEntity = new Entity(m_NextUID, name);



		std::string lightPath = "media/Light.x";
		BaseEntityComponent* comp = new MeshComponent(lightPath, newEntity, GetNewUID());
		newEntity->AddComponent(comp);

		comp = new LightComponent(m_Renderer, newEntity, GetNewUID(), m_Shader, m_State);
		newEntity->AddComponent(comp);

		// Add transform component to all entities 
		comp = new TransformComponent(newEntity, GetNewUID());
		newEntity->AddComponent(comp);
		// Get vector index for new entity and add it to vector
		TUInt32 entityIndex = static_cast<TUInt32>(m_Entities.size());
		m_Entities.push_back(newEntity);

		// Add mapping from UID to entity index into hash map
		m_EntityUIDMap->SetKeyValue(m_NextUID, entityIndex);

		m_IsEnumerating = false; // Cancel any entity enumeration (entity list has changed)

		// Return UID of new entity then increase it ready for next entity
		return m_NextUID++;
	}

	bool EntityManager::DestroyEntity(EntityUID UID)
	{
		TUInt32 entityIndex;
		if (!m_EntityUIDMap->LookUpKey(UID, &entityIndex))
		{
			// Quit if not found
			return false;
		}

		// Delete the given entity and remove from UID map
		delete m_Entities[entityIndex];
		m_EntityUIDMap->RemoveKey(UID);

		// If not removing last entity...
		if (entityIndex != m_Entities.size() - 1)
		{
			// ...put the last entity into the empty entity slot and update UID map
			m_Entities[entityIndex] = m_Entities.back();
			m_EntityUIDMap->SetKeyValue(m_Entities.back()->GetUID(), entityIndex);
		}
		m_Entities.pop_back(); // Remove last entity

		m_IsEnumerating = false; // Cancel any entity enumeration (entity list has changed)
		return true;
	}

	void EntityManager::DestroyAllEntities()
	{
		m_EntityUIDMap->RemoveAllKeys();
		while (m_Entities.size())
		{

			delete m_Entities.back();
			m_Entities.pop_back();

		}

		m_IsEnumerating = false;
	}

	void EntityManager::UpdateAllEntities(float updateTime)
	{
		TUInt32 entity = 0;
		while (entity < m_Entities.size())
		{
			// Update entity, if it returns false, then destroy it
			if (!m_Entities[entity]->Update(updateTime))
			{
				DestroyEntity(m_Entities[entity]->GetUID());
			}
			else
			{
				++entity;
			}
		}
	}

	void EntityManager::RenderAllEntities()
	{
		TEntityIter entity = m_Entities.begin();
		while (entity != m_Entities.end())
		{
			if ((*entity)->GetComponent("Renderer") != nullptr)
			{
				RendererComponent* comp = static_cast<RendererComponent*>((*entity)->GetComponent("Renderer"));
				comp->Render();
			}
			if ((*entity)->GetComponent("Light Renderer") != nullptr)
			{
				LightComponent* comp = static_cast<LightComponent*>((*entity)->GetComponent("Light Renderer"));
				comp->Render();
			}
			++entity;
		}
	}
}

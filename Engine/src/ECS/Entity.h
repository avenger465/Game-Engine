#pragma once

#include "BaseComponent.h"

namespace Engine
{
	class Entity
	{
	public:
		Entity(EntityUID UID, const std::string& name) { m_UID = UID; m_Name = name; }
		virtual ~Entity()
		{
			RemoveAllComponents();
		}
	public:
		EntityUID GetUID() { return m_UID; }
		std::string& GetName() { return m_Name; }

		void AddComponent(BaseEntityComponent* component);
		BaseEntityComponent* GetComponent(EntityUID UID);
		BaseEntityComponent* GetComponent(const std::string& type);
		bool RemoveComponent(EntityUID UID);
		bool RemoveComponent(const std::string& type);

		void RemoveAllComponents();

		virtual bool Update(float frameTime);

	private:
		Entity(const Entity&);
		Entity& operator=(const Entity&);

	private:
		EntityUID m_UID;
		std::string m_Name;

		std::map<EntityUID, BaseEntityComponent*> m_Components;
	};
}

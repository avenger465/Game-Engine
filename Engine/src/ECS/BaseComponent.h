#pragma once
//#include <string>
#include <Common\EngineProperties.h>

namespace Engine
{
	class Entity;

	class BaseEntityComponent
	{
	public:
		BaseEntityComponent(const std::string& type, EntityUID UID, Entity* entity)
		{
			m_Type = type;
			m_UID = UID;
			m_Entity = entity;
		}

		const std::string& GetType() { return m_Type; }
		const EntityUID getUID() { return m_UID; }
		const Entity* GetEntity() { return m_Entity; }

		virtual bool Update(float updatetime) = 0;

	private:
		std::string m_Type;
		EntityUID m_UID;
		Entity* m_Entity;
	};
}

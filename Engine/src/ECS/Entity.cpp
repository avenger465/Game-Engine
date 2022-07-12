#include "epch.h"
#include "Entity.h"

namespace Engine
{
	void Entity::AddComponent(BaseEntityComponent* component)
	{
		m_Components.insert(std::pair<EntityUID, BaseEntityComponent*>(component->getUID(), component));
	}

	BaseEntityComponent* Entity::GetComponent(EntityUID UID)
	{
		auto i = m_Components.find(UID);
		if (i != m_Components.end())
		{
			return (*i).second;
		}
		else
		{
			return nullptr;
		}
	}

	BaseEntityComponent* Entity::GetComponent(const std::string& type)
	{
		std::map<EntityUID, BaseEntityComponent*>::iterator it = m_Components.begin();
		while (it != m_Components.end())
		{
			if ((*it).second->GetType() == type)
			{
				return (*it).second;
			}
		}
		return nullptr;
	}

	bool Entity::RemoveComponent(EntityUID UID)
	{
		auto i = m_Components.find(UID);
		if (i != m_Components.begin())
		{
			delete (*i).second;
			m_Components.erase(i);
			return true;
		}
		return false;
	}

	bool Entity::RemoveComponent(const std::string& type)
	{
		std::map<EntityUID, BaseEntityComponent*>::iterator it = m_Components.begin();
		while (it != m_Components.end())
		{
			if ((*it).second->GetType() == type)
			{
				delete (*it).second;
				m_Components.erase(it);
				return true;
			}
		}
		return false;
	}

	void Entity::RemoveAllComponents()
	{
		std::map<EntityUID, BaseEntityComponent*>::iterator it = m_Components.begin();
		while (it != m_Components.end())
		{
			delete (*it).second;
			it++;
		}
	}

	bool Entity::Update(float frameTime)
	{
		// While loop for recieving messages

		std::map<EntityUID, BaseEntityComponent*>::iterator it = m_Components.begin();
		while (it != m_Components.end())
		{
			if (!(*it).second->Update(frameTime))
			{
				it = m_Components.erase(it);
			}
			else
			{
				it++;
			}
		}

		return true;
	}

}



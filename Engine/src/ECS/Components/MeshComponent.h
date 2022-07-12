#pragma once
#include "ECS\BaseComponent.h"

namespace Engine
{
	class MeshComponent : public BaseEntityComponent
	{
	public:

		MeshComponent(std::string& meshFilePath, Entity* entity, EntityUID UID, int index = 0) : BaseEntityComponent("Mesh", UID, entity)
		{
			m_filePath = meshFilePath;
			m_Index = index;
		}
		virtual bool Update(float frameTime) override;

		const std::string GetMeshPath() {
			return m_filePath;
		}

		~MeshComponent() {}

	private:
		std::string m_filePath;
		int m_Index;
	};
}

#include "pch.h"
#include "Entity.h"

namespace Snowstorm
{
	Entity::Entity(const entt::entity handle, World* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}
}

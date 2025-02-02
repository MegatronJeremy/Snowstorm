#pragma once

#include "Entity.h"

namespace Snowstorm
{
	class ScriptableEntity : NonCopyable
	{
	public:
		template <typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}

	protected:
		virtual void OnCreate()
		{
		}

		virtual void OnDestroy()
		{
		}

		virtual void OnUpdate(Timestep ts)
		{
		}

	private:
		Entity m_Entity;

		friend class Scene;
		friend class ScriptSystem;
	};
}

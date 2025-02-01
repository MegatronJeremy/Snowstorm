#include "ScriptSystem.hpp"

#include "Snowstorm/Scene/Components.h"
#include "Snowstorm/Scene/ScriptableEntity.h"

namespace Snowstorm
{
	void ScriptSystem::Execute(const Timestep ts)
	{
		const auto nativeScriptView = View<NativeScriptComponent>();
		const auto nativeScriptInitView = InitView<NativeScriptComponent>();
		const auto nativeScriptFiniView = FiniView<NativeScriptComponent>();

		// Instantiate scripts if not already created
		for (const auto entity : nativeScriptInitView)
		{
			auto [scriptComponent] = nativeScriptView.get(entity);
			SS_CORE_ASSERT(scriptComponent.Instance == nullptr);

			scriptComponent.InstantiateScript();
			scriptComponent.Instance->m_Entity = Entity{entity, m_Scene};
			scriptComponent.Instance->onCreate();
		}

		// Update all scripts
		for (const auto entity : nativeScriptView)
		{
			auto [scriptComponent] = nativeScriptView.get(entity);
			SS_CORE_ASSERT(scriptComponent.Instance);

			scriptComponent.Instance->onUpdate(ts);
		}

		// Destroy scripts that are removed
		for (const auto entity : nativeScriptFiniView)
		{
			auto [scriptComponent] = nativeScriptView.get(entity);
			SS_CORE_ASSERT(scriptComponent.Instance);

			scriptComponent.Instance->onDestroy();
			scriptComponent.DestroyScript();
		}
	}
}

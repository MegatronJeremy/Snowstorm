#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include "SceneHieararchyPanel.hpp"
#include "Snowstorm/Core/Log.h"
#include "Snowstorm/Scene/Components.hpp"

namespace Snowstorm
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		setContext(context);
	}

	void SceneHierarchyPanel::setContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void SceneHierarchyPanel::onImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		// TODO fix this
		for (const auto [entityID] : m_Context->GetRegistry().m_Registry.storage<entt::entity>().each())
		{
			const Entity entity{entityID, m_Context.get()};
			drawEntityNode(entity);
		}


		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			m_SelectionContext = {};
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext)
		{
			drawComponents(m_SelectionContext);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::drawEntityNode(Entity entity)
	{
		const auto& tag = entity.GetComponent<TagComponent>().Tag;

		const ImGuiTreeNodeFlags flags =
		((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0)
		| ImGuiTreeNodeFlags_OpenOnArrow;

		const bool opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(TransformComponent).hash_code()),
		                                      flags,
		                                      tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		if (opened)
		{
			ImGui::TreePop();
		}
	}

	void SceneHierarchyPanel::drawComponents(Entity entity)
	{
		if (entity.hasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256] = {0};
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		if (entity.hasComponent<TransformComponent>())
		{
			if (ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(TransformComponent).hash_code()),
			                      ImGuiTreeNodeFlags_DefaultOpen,
			                      "Transform"))
			{
				auto& position = entity.GetComponent<TransformComponent>().Position;
				ImGui::DragFloat3("Position", value_ptr(position), 0.5f);

				ImGui::TreePop();
			}
		}

		if (entity.hasComponent<CameraComponent>())
		{
			if (ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(CameraComponent).hash_code()),
			                      ImGuiTreeNodeFlags_DefaultOpen,
			                      "Camera"))
			{
				auto& cameraComponent = entity.GetComponent<CameraComponent>();
				auto& camera = cameraComponent.Camera;

				ImGui::Checkbox("Primary", &cameraComponent.Primary);

				const char* projectionTypeStrings[] = {"Perspective", "Orthographic"};
				const char* currentProjectionTypeString = projectionTypeStrings[static_cast<int>(camera.
					getProjectionType())];

				if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						const bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
						{
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.setProjectionType(static_cast<SceneCamera::ProjectionType>(i));
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				if (camera.getProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float perspectiveFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
					if (ImGui::DragFloat("FOV", &perspectiveFOV))
						camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveFOV));

					float perspectiveNear = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near", &perspectiveNear))
						camera.SetPerspectiveNearClip(perspectiveNear);

					float perspectiveFar = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far", &perspectiveFar))
						camera.SetPerspectiveFarClip(perspectiveFar);
				}

				if (camera.getProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = camera.getOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
						camera.setOrthographicSize(orthoSize);

					float orthoNear = camera.getOrthographicNearClip();
					if (ImGui::DragFloat("Near", &orthoNear))
						camera.setOrthographicNearClip(orthoNear);

					float orthoFar = camera.getOrthographicFarClip();
					if (ImGui::DragFloat("Far", &orthoFar))
						camera.setOrthographicFarClip(orthoFar);

					ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);
				}

				ImGui::TreePop();
			}
		}

	}
}

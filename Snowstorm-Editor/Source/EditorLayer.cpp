#include "EditorLayer.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include "Snowstorm/Events/KeyEvent.h"
#include "Snowstorm/Events/MouseEvent.h"

namespace Snowstorm
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
	}

	void EditorLayer::OnAttach()
	{
		SS_PROFILE_FUNCTION();

		m_ActiveScene = CreateRef<Scene>();

		// Entities
		m_FramebufferEntity = m_ActiveScene->CreateEntity("Framebuffer");
		m_FramebufferEntity.addComponent<ViewportComponent>(glm::vec2{1280.0f, 720.0f});

		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_FramebufferEntity.addComponent<FramebufferComponent>(Framebuffer::Create(fbSpec));

		auto checkerboardSquare = m_ActiveScene->CreateEntity("Amazing Square");

		checkerboardSquare.addComponent<TransformComponent>();

		auto checkerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");
		checkerboardSquare.addComponent<SpriteRendererComponent>(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f}, checkerboardTexture, 5.0f);
		checkerboardSquare.addComponent<RenderTargetComponent>(m_FramebufferEntity);

		checkerboardSquare.getComponent<TransformComponent>().Position[0] += 2.0f;

		auto redSquare = m_ActiveScene->CreateEntity("Red Square");

		redSquare.addComponent<TransformComponent>();

		redSquare.addComponent<SpriteRendererComponent>(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
		redSquare.addComponent<RenderTargetComponent>(m_FramebufferEntity);

		m_SquareEntity = checkerboardSquare;

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
		m_CameraEntity.addComponent<TransformComponent>();
		m_CameraEntity.addComponent<CameraComponent>();
		m_CameraEntity.addComponent<CameraControllerComponent>();
		m_CameraEntity.addComponent<RenderTargetComponent>(m_FramebufferEntity);

		m_SecondCamera = m_ActiveScene->CreateEntity("Clip-Space Entity");
		m_SecondCamera.addComponent<TransformComponent>();
		auto& cc = m_SecondCamera.addComponent<CameraComponent>();
		m_SecondCamera.addComponent<CameraControllerComponent>();
		m_SecondCamera.addComponent<RenderTargetComponent>(m_FramebufferEntity);
		cc.Primary = false;

		m_SceneHierarchyPanel.setContext(m_ActiveScene);
	}

	void EditorLayer::OnDetach()
	{
		SS_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(const Timestep ts)
	{
		SS_PROFILE_FUNCTION();

		// Update scene
		m_ActiveScene->OnUpdate(ts);
	}

	void EditorLayer::OnImGuiRender()
	{
		SS_PROFILE_FUNCTION();

		static bool dockspaceOpen = true;
		static bool optFullscreen = true;
		static bool optPadding = false;
		static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (optFullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove;
			windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspaceFlags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
			windowFlags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!optPadding)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		}
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, windowFlags);
		if (!optPadding)
		{
			ImGui::PopStyleVar();
		}

		if (optFullscreen)
		{
			ImGui::PopStyleVar(2);
		}

		// Submit the DockSpace
		if (const ImGuiIO& io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			const ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					Application::Get().Close();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::End();

		m_SceneHierarchyPanel.onImGuiRender();

		ImGui::Begin("Settings");

		const auto stats = Renderer::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		if (m_SquareEntity)
		{
			ImGui::Separator();
			ImGui::Text("%s", m_SquareEntity.getComponent<TagComponent>().Tag.c_str());

			auto& squareColor = m_SquareEntity.getComponent<SpriteRendererComponent>().Color;
			ImGui::ColorEdit4("Square Color", value_ptr(squareColor));
			ImGui::Separator();
		}

		ImGui::DragFloat3("Camera Position", value_ptr(
			                  m_CameraEntity.getComponent<TransformComponent>().Position));

		if (ImGui::Checkbox("Camera A", &m_PrimaryCamera))
		{
			m_CameraEntity.getComponent<CameraComponent>().Primary = m_PrimaryCamera;
			m_SecondCamera.getComponent<CameraComponent>().Primary = !m_PrimaryCamera;
		}

		{
			auto& camera = m_SecondCamera.getComponent<CameraComponent>().Camera;
			float orthoSize = camera.getOrthographicSize();
			if (ImGui::DragFloat("Second Camera Ortho Size", &orthoSize))
				camera.setOrthographicSize(orthoSize);
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
		ImGui::Begin("Viewport");

		// TODO move this to some sort of event system?
		auto& viewportComponent = m_FramebufferEntity.getComponent<ViewportComponent>();
		const auto& framebufferComponent = m_FramebufferEntity.getComponent<FramebufferComponent>();

		viewportComponent.Focused = ImGui::IsWindowFocused();
		viewportComponent.Hovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!viewportComponent.Focused || !viewportComponent.Hovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		viewportComponent.Size = {viewportPanelSize.x, viewportPanelSize.y};

		const uint32_t textureID = framebufferComponent.Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<ImTextureID>(textureID), ImVec2{viewportComponent.Size.x, viewportComponent.Size.y}, ImVec2{0, 1},
		             ImVec2{1, 0});
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::OnEvent(Event& event)
	{
		auto& eventsHandler = m_ActiveScene->getSingletonManager().GetSingleton<EventsHandlerSingleton>();

		// TODO have to make this better
		static const std::unordered_map<EventType, std::function<void(Event&)>> eventMap = {
			{
				EventType::MouseScrolled, [&eventsHandler](Event& e)
				{
					eventsHandler.pushEvent<MouseScrolledEvent>(dynamic_cast<MouseScrolledEvent&>(e));
				}
			}
		};

		// Check if the event type exists in the map
		if (const auto it = eventMap.find(event.getEventType()); it != eventMap.end())
		{
			it->second(event); // Call the corresponding function
		}
		else
		{
			// Don't do this for now... mouse moved events etc...
			// SS_ASSERT(false, "Unknown event");
		}
	}
}

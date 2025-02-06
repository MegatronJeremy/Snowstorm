#include "EditorLayer.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include "Examples/MandelbrotSet/MandelbrotControllerComponent.hpp"
#include "Examples/MandelbrotSet/MandelbrotMaterial.hpp"
#include "Examples/MandelbrotSet/MandelbrotControllerSystem.hpp"

#include "Snowstorm/ECS/SystemManager.hpp"
#include "Snowstorm/Events/KeyEvent.h"
#include "Snowstorm/Events/MouseEvent.h"
#include "Snowstorm/Render/MeshLibrarySingleton.hpp"

namespace Snowstorm
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer")
	{
	}

	void EditorLayer::OnAttach()
	{
		SS_PROFILE_FUNCTION();

		m_ActiveWorld = CreateRef<World>();

		m_SceneHierarchyPanel.setContext(m_ActiveWorld);

		auto& shaderLibrary = m_ActiveWorld->GetSingleton<ShaderLibrarySingleton>();
		auto& meshLibrary = m_ActiveWorld->GetSingleton<MeshLibrarySingleton>();

		// Framebuffer setup
		{
			// TODO set this up with the current screen size
			m_FramebufferEntity = m_ActiveWorld->CreateEntity("Framebuffer");
			m_FramebufferEntity.AddComponent<ViewportComponent>(glm::vec2{1280.0f, 720.0f});

			FramebufferSpecification fbSpec;
			fbSpec.Width = 1280;
			fbSpec.Height = 720;
			m_FramebufferEntity.AddComponent<FramebufferComponent>(Framebuffer::Create(fbSpec));
		}

		// 3D Entities
		{
			const Ref<Shader> mandelbrotShader = shaderLibrary.Load("assets/shaders/Mandelbrot.glsl");
			const Ref<Shader> basicShader = shaderLibrary.Load("assets/shaders/Material.glsl");

			const Ref<MandelbrotMaterial> mandelbrotMaterial = CreateRef<MandelbrotMaterial>(mandelbrotShader);
			const Ref<Material> redMaterial = CreateRef<Material>(basicShader);
			const Ref<Material> blueMaterial = CreateRef<Material>(basicShader);

			auto cubeMesh = meshLibrary.Load("assets/meshes/cube.obj");
			auto girlMesh = meshLibrary.Load("assets/meshes/girl.obj");

			redMaterial->SetColor({1.0f, 0.0f, 0.0f, 1.0f});
			blueMaterial->SetColor({0.0f, 0.0f, 1.0f, 1.0f});

			auto blueCube = m_ActiveWorld->CreateEntity("Blue Cube");

			blueCube.AddComponent<TransformComponent>();
			blueCube.AddComponent<MaterialComponent>(blueMaterial);
			blueCube.AddComponent<MeshComponent>(girlMesh);
			blueCube.AddComponent<RenderTargetComponent>(m_FramebufferEntity);

			blueCube.GetComponent<TransformComponent>().Position -= 3.0f;

			auto redCube = m_ActiveWorld->CreateEntity("Red Cube");

			redCube.AddComponent<TransformComponent>();
			redCube.AddComponent<MaterialComponent>(mandelbrotMaterial);
			redCube.AddComponent<MeshComponent>(girlMesh);
			redCube.AddComponent<RenderTargetComponent>(m_FramebufferEntity);

			redCube.GetComponent<TransformComponent>().Position += 3.0f;

			auto girlCube = m_ActiveWorld->CreateEntity("Girl Cube");

			girlCube.AddComponent<TransformComponent>();
			girlCube.AddComponent<MaterialComponent>(redMaterial);
			girlCube.AddComponent<MeshComponent>(cubeMesh);
			girlCube.AddComponent<RenderTargetComponent>(m_FramebufferEntity);

			girlCube.GetComponent<TransformComponent>().Position -= 6.0f;

			auto mandelbrotQuad = m_ActiveWorld->CreateEntity("Mandelbrot Quad");
			mandelbrotQuad.AddComponent<TransformComponent>();
			mandelbrotQuad.AddComponent<MaterialComponent>(mandelbrotMaterial);
			mandelbrotQuad.AddComponent<MeshComponent>(meshLibrary.CreateQuad());
			mandelbrotQuad.AddComponent<RenderTargetComponent>(m_FramebufferEntity);

			mandelbrotQuad.GetComponent<TransformComponent>().Scale *= 10.0f;

			auto mandelbrotControllerEntity = m_ActiveWorld->CreateEntity("Mandelbrot Controller Entity");
			mandelbrotControllerEntity.AddComponent<MandelbrotControllerComponent>(mandelbrotMaterial);
		}

		// Camera Entities
		{
			m_CameraEntity = m_ActiveWorld->CreateEntity("Camera Entity");
			m_CameraEntity.AddComponent<TransformComponent>();
			m_CameraEntity.AddComponent<CameraComponent>();
			m_CameraEntity.AddComponent<CameraControllerComponent>();
			m_CameraEntity.AddComponent<RenderTargetComponent>(m_FramebufferEntity);

			m_CameraEntity.GetComponent<CameraComponent>().Camera.SetProjectionType(
				SceneCamera::ProjectionType::Perspective);
			m_CameraEntity.GetComponent<CameraComponent>().Camera.SetOrthographicFarClip(1000.0f);
			m_CameraEntity.GetComponent<TransformComponent>().Position.z = 15.0f;

			m_SecondCamera = m_ActiveWorld->CreateEntity("Clip-Space Entity");
			m_SecondCamera.AddComponent<TransformComponent>();
			auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
			m_SecondCamera.AddComponent<CameraControllerComponent>();
			m_SecondCamera.AddComponent<RenderTargetComponent>(m_FramebufferEntity);
			cc.Primary = false;
		}
	}

	void EditorLayer::OnDetach()
	{
		SS_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(const Timestep ts)
	{
		SS_PROFILE_FUNCTION();

		// Update scene
		m_ActiveWorld->OnUpdate(ts);
	}

	void EditorLayer::PostUpdate(const Timestep ts)
	{
		m_ActiveWorld->PostUpdate(ts);
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

		const auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		if (m_SquareEntity)
		{
			ImGui::Separator();
			ImGui::Text("%s", m_SquareEntity.GetComponent<TagComponent>().Tag.c_str());

			auto& squareColor = m_SquareEntity.GetComponent<SpriteComponent>().TintColor;
			ImGui::ColorEdit4("Square Color", value_ptr(squareColor));
			ImGui::Separator();
		}

		ImGui::DragFloat3("Camera Position", value_ptr(
			                  m_CameraEntity.GetComponent<TransformComponent>().Position));

		if (ImGui::Checkbox("Camera A", &m_PrimaryCamera))
		{
			m_CameraEntity.GetComponent<CameraComponent>().Primary = m_PrimaryCamera;
			m_SecondCamera.GetComponent<CameraComponent>().Primary = !m_PrimaryCamera;
		}

		{
			auto& camera = m_SecondCamera.GetComponent<CameraComponent>().Camera;
			float orthoSize = camera.GetOrthographicSize();
			if (ImGui::DragFloat("Second Camera Ortho Size", &orthoSize))
				camera.SetOrthographicSize(orthoSize);
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
		ImGui::Begin("Viewport");

		// TODO move this to some sort of event system?
		auto& viewportComponent = m_FramebufferEntity.GetComponent<ViewportComponent>();
		const auto& framebufferComponent = m_FramebufferEntity.GetComponent<FramebufferComponent>();

		viewportComponent.Focused = ImGui::IsWindowFocused();
		viewportComponent.Hovered = ImGui::IsWindowHovered();
		// BlockEvents(!viewportComponent.Focused || !viewportComponent.Hovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		viewportComponent.Size = {viewportPanelSize.x, viewportPanelSize.y};

		const uint32_t textureID = framebufferComponent.Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<ImTextureID>(textureID),
		             ImVec2{viewportComponent.Size.x, viewportComponent.Size.y}, ImVec2{0, 1},
		             ImVec2{1, 0});
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void EditorLayer::OnEvent(Event& event)
	{
		auto& eventsHandler = m_ActiveWorld->GetSingleton<EventsHandlerSingleton>();

		// TODO have to make this better
		static const std::unordered_map<EventType, std::function<void(Event&)>> eventMap = {
			{
				EventType::MouseScrolled, [&eventsHandler](Event& e)
				{
					eventsHandler.PushEvent<MouseScrolledEvent>(dynamic_cast<MouseScrolledEvent&>(e));
				}
			}
		};

		// Check if the event type exists in the map
		if (const auto it = eventMap.find(event.GetEventType()); it != eventMap.end())
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

#include "Sandbox2D.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

namespace
{
	constexpr uint32_t s_MapWidth = 24;
	const char* s_MapTiles =
		"WWWWWWWWWWWWWWWWWWWWWWWW"
		"WWWWWWDDDDDDWWWWWWWWWWWW"
		"WWWWDDDWDDDDDDWWWWWWWWWW"
		"WWWDDDDDDDDDDDDDDDWWWWWW"
		"WWWWDDDDDDDDDDDWWWDDDDWW"
		"WWWWWWWDDDDDDDDDDWWDDDWW"
		"WWWWDDDDDDDDDDDDDDDDDDDW"
		"WWWWWWDDDDDWWWWWWWDDDDDW"
		"WWWWWWWWDDDDDWWWWDDDDDWW"
		"WWWWWWWWWWWWDDDDDDDDWWWW"
		"WWWWWWWWWWWWWWDDDDWWWWWW"
		"WWWWWCWWWDDDDDDDDDDWWWWW"
		"WWDDDDDDDDDDDDWWWWWWWWWW"
		"WWWWWDDDDDDDDDDDDDWWWWWW";
}

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"),
	  m_CameraController(1280.0f / 720.0f, true), m_Particle(), m_MapWidth(0), m_MapHeight(0)
{
}

void Sandbox2D::OnAttach()
{
	SS_PROFILE_FUNCTION();

	m_CheckerboardTexture = Snowstorm::Texture2D::Create("assets/textures/Checkerboard.png");

	m_SpriteSheet = Snowstorm::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");

	m_TextureStairs = Snowstorm::SubTexture2D::CreateFromCoords(m_SpriteSheet, {0, 11}, {128, 128});
	m_TextureTree = Snowstorm::SubTexture2D::CreateFromCoords(m_SpriteSheet, {2, 1}, {128, 128}, {1, 2});

	m_MapWidth = s_MapWidth;
	m_MapHeight = strlen(s_MapTiles) / s_MapWidth;

	s_TextureMap['D'] = Snowstorm::SubTexture2D::CreateFromCoords(m_SpriteSheet, {6, 11}, {128, 128});
	s_TextureMap['W'] = Snowstorm::SubTexture2D::CreateFromCoords(m_SpriteSheet, {11, 11}, {128, 128});

	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 5.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };

	m_CameraController.SetZoomLevel(5.0f);
}

void Sandbox2D::OnDetach()
{
	SS_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(const Snowstorm::Timestep ts)
{
	SS_PROFILE_FUNCTION();

	// Update
	m_CameraController.OnUpdate(ts);

	// Render
	Snowstorm::Renderer2D::ResetStats();
	{
		SS_PROFILE_SCOPE("Renderer Prep");
		Snowstorm::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
		Snowstorm::RenderCommand::Clear();
	}

	{
		static float rotation = 0.0f;
		rotation += ts * 50.0f;

		SS_PROFILE_SCOPE("Renderer Draw");
		Snowstorm::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Snowstorm::Renderer2D::DrawRotatedQuad({1.0f, 0.0f}, {0.8f, 0.8f}, glm::radians(- 45.0f), {0.8f, 0.2f, 0.3f, 1.0f});
		Snowstorm::Renderer2D::DrawQuad({-1.0f, 0.0f}, {0.8f, 0.8f}, {0.8f, 0.2f, 0.3f, 1.0f});
		Snowstorm::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.0f});
		Snowstorm::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_CheckerboardTexture, 10.0f);
		Snowstorm::Renderer2D::DrawRotatedQuad({-2.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, glm::radians(rotation),  m_CheckerboardTexture, 20.0f,
		glm::vec4(1.0f, 0.9f, 0.9f, 1.0f));
		Snowstorm::Renderer::EndScene();

		Snowstorm::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = {(x + 5.0f) / 10.0f, 0.4f,(y+5.0f) / 10.0f, 0.7f};
				Snowstorm::Renderer2D::DrawQuad({x, y}, {0.45f, 0.45f}, color);
			}
		}
		Snowstorm::Renderer::EndScene();
	}

#if 1
	// Particle system
	if (Snowstorm::Input::IsMouseButtonPressed(Snowstorm::Mouse::ButtonLeft))
	{
		auto [x, y] = Snowstorm::Input::GetMousePosition();
		const auto width = Snowstorm::Application::Get().GetWindow().GetWidth();
		const auto height = Snowstorm::Application::Get().GetWindow().GetHeight();

		const auto bounds = m_CameraController.GetBounds();
		const auto pos = m_CameraController.GetCamera().GetPosition();
		x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
		y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
		m_Particle.Position = { x + pos.x, y + pos.y };
		for (int i = 0; i < 5; i++)
			m_ParticleSystem.Emit(m_Particle);
	}

	m_ParticleSystem.OnUpdate(ts);
	m_ParticleSystem.OnRender(m_CameraController.GetCamera());

	Snowstorm::Renderer2D::BeginScene(m_CameraController.GetCamera());

	for (uint32_t y = 0; y < m_MapHeight; y++)
	{
		for (uint32_t x = 0; x < m_MapHeight; x++)
		{
			char tileType = s_MapTiles[x + y * m_MapWidth];
			Snowstorm::Ref<Snowstorm::SubTexture2D> texture;

			if (s_TextureMap.find(tileType) != s_TextureMap.end())
				texture = s_TextureMap[tileType];
			else
				texture = m_TextureStairs;

			Snowstorm::Renderer2D::DrawQuad({ x - m_MapWidth / 2.0f, m_MapHeight / 2.0f - y, 0.5f }, { 1.0f, 1.0f }, texture);
		}
	}
	Snowstorm::Renderer::EndScene();
#endif
}

void Sandbox2D::OnImGuiRender()
{
	SS_PROFILE_FUNCTION();

	ImGui::Begin("Settings");

	const auto stats = Snowstorm::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	ImGui::ColorEdit4("Square Color", value_ptr(m_SquareColor));

	ImGui::End();
}

void Sandbox2D::OnEvent(Snowstorm::Event& event)
{
	m_CameraController.OnEvent(event);
}

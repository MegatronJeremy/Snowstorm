#pragma once

#include <Snowstorm.h>

#include "ParticleSystem.h"

class Sandbox2D final : public Snowstorm::Layer
{
public:
	Sandbox2D();
	~Sandbox2D() override = default;

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(Snowstorm::Timestep ts) override;
	void OnImGuiRender() override;
	void OnEvent(Snowstorm::Event& event) override;

private:
	// Snowstorm::OrthographicCameraController m_CameraController;
	Snowstorm::Ref<Snowstorm::World> m_scene;

	// Temp
	Snowstorm::Ref<Snowstorm::VertexArray> m_SquareVA;
	Snowstorm::Ref<Snowstorm::Shader> m_FlatColorShader;

	Snowstorm::Ref<Snowstorm::Texture2D> m_CheckerboardTexture;
	Snowstorm::Ref<Snowstorm::Texture2D> m_SpriteSheet;
	Snowstorm::Ref<Snowstorm::SubTexture2D> m_TextureStairs, m_TextureBarrel, m_TextureTree;

	glm::vec4 m_SquareColor = {0.2f, 0.3f, 0.8f, 1.0f};

	ParticleSystem m_ParticleSystem;
	ParticleProps m_Particle;

	uint32_t m_MapWidth, m_MapHeight;
	std::unordered_map<char, Snowstorm::Ref<Snowstorm::SubTexture2D>> s_TextureMap;
};

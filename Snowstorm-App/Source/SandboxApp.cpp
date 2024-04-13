#include <Snowstorm.h>
#include <Snowstorm/Core/EntryPoint.h>

#include <imgui.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"

class ExampleLayer final : public Snowstorm::Layer
{
public:
	ExampleLayer()
		: Layer("Example"),
		  m_CameraController(1280.0f / 720.0f, true)
	{
		m_VertexArray = Snowstorm::VertexArray::Create();

		constexpr float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			0.0f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
		};

		const Snowstorm::Ref<Snowstorm::VertexBuffer>
			vertexBuffer(Snowstorm::VertexBuffer::Create(vertices, sizeof(vertices)));
		const Snowstorm::BufferLayout layout = {
			{Snowstorm::ShaderDataType::Float3, "a_Position"},
			{Snowstorm::ShaderDataType::Float4, "a_Color"}
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		constexpr unsigned int indices[3] = {
			0, 1, 2
		};
		const Snowstorm::Ref<Snowstorm::IndexBuffer> indexBuffer(Snowstorm::IndexBuffer::Create(indices, std::size(indices)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA = Snowstorm::VertexArray::Create();

		constexpr float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f, 0.5f, 0.0f, 0.0f, 1.0f
		};

		const Snowstorm::Ref<Snowstorm::VertexBuffer> squareVB(
			Snowstorm::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{Snowstorm::ShaderDataType::Float3, "a_Position"},
			{Snowstorm::ShaderDataType::Float2, "a_TexCoord"}
		});
		m_SquareVA->AddVertexBuffer(squareVB);

		constexpr unsigned int squareIndices[6] = {
			0, 1, 2, 2, 3, 0
		};
		const Snowstorm::Ref<Snowstorm::IndexBuffer> squareIB(
			Snowstorm::IndexBuffer::Create(squareIndices, std::size(squareIndices)));
		m_SquareVA->SetIndexBuffer(squareIB);

		const std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}			
		)";

		const std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}			
		)";

		m_Shader = Snowstorm::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

		const std::string flatColorShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}			
		)";

		const std::string flatColorShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}			
		)";

		m_FlatColorShader = Snowstorm::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

		const auto textureShader = m_ShaderLibrary.Load("Assets/Shaders/Texture.glsl");

		m_Texture = Snowstorm::Texture2D::Create("Assets/Textures/Checkerboard.png");
		m_ChernoLogoTexture = Snowstorm::Texture2D::Create("Assets/Textures/ChernoLogo.png");

		textureShader->Bind();
		textureShader->SetInt("u_Texture", 0);
	}

	void OnUpdate(const Snowstorm::Timestep ts) override
	{
		// Update
		m_CameraController.OnUpdate(ts);

		// Render
		Snowstorm::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
		Snowstorm::RenderCommand::Clear();

		Snowstorm::Renderer::BeginScene(m_CameraController.GetCamera());

		static float dynScale = 0.1f;
		static bool goingUp = true;
		const glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		m_FlatColorShader->Bind();
		m_FlatColorShader->SetFloat3("u_Color", m_SquareColor);

		for (int x = 0; x < 20; x++)
		{
			for (int y = 0; y < 20; y++)
			{
				glm::vec3 pos(x * (dynScale + 0.1f * dynScale), y * (dynScale + 0.1f * dynScale), 0.0f);
				const glm::mat4 transform = translate(glm::mat4(1.0f), pos) * scale;
				Snowstorm::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);

				if (dynScale >= 0.12f) goingUp = false;
				else if (dynScale <= 0.1f) goingUp = true;

				if (goingUp) dynScale += 0.00001f * ts;
				else dynScale -= 0.00001f * ts;
			}
		}

		const auto textureShader = m_ShaderLibrary.Get("Texture");

		m_Texture->Bind();
		Snowstorm::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		m_ChernoLogoTexture->Bind();
		Snowstorm::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		// Triangle
		// Snowstorm::Renderer::Submit(m_Shader, m_VertexArray);

		Snowstorm::Renderer::EndScene();
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(Snowstorm::Event& e) override
	{
		m_CameraController.OnEvent(e);
	}

private:
	Snowstorm::ShaderLibrary m_ShaderLibrary;
	Snowstorm::Ref<Snowstorm::Shader> m_Shader;
	Snowstorm::Ref<Snowstorm::VertexArray> m_VertexArray;

	Snowstorm::Ref<Snowstorm::Shader> m_FlatColorShader;
	Snowstorm::Ref<Snowstorm::VertexArray> m_SquareVA;

	Snowstorm::Ref<Snowstorm::Texture2D> m_Texture, m_ChernoLogoTexture;

	Snowstorm::OrthographicCameraController m_CameraController;

	glm::vec3 m_SquareColor = {0.2f, 0.3f, 0.8f};
};

class Sandbox final : public Snowstorm::Application
{
public:
	Sandbox()
	{
		// PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox() override
	= default;
};

Snowstorm::Application* Snowstorm::CreateApplication()
{
	return new Sandbox();
}

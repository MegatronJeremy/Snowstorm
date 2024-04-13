#pragma once

#include <glm/glm.hpp>

#include "Snowstorm/Renderer/Shader.h"

// TODO: REMOVE!
using GLenum = unsigned int;

namespace Snowstorm
{
	class OpenGLShader final : public Shader
	{
	public:
		explicit OpenGLShader(const std::string& filepath);
		OpenGLShader(std::string name, const std::string& vertexSrc, const std::string& fragmentSrc);
		~OpenGLShader() override;

		OpenGLShader(const OpenGLShader& other) = delete;
		OpenGLShader(OpenGLShader&& other) = delete;
		OpenGLShader& operator=(const OpenGLShader& other) = delete;
		OpenGLShader& operator=(OpenGLShader&& other) = delete;

		void Bind() const override;
		void Unbind() const override;

		void SetInt(const std::string& name, int value) override;
		void SetIntArray(const std::string& name, int* values, uint32_t count) override;
		void SetFloat(const std::string& name, float value) override;
		void SetFloat3(const std::string& name, const glm::vec3& value) override;
		void SetFloat4(const std::string& name, const glm::vec4& value) override;
		void SetMat4(const std::string& name, const glm::mat4& value) override;

		const std::string& GetName() const override { return m_Name; }

		void UploadUniformInt(const std::string& name, int value) const;
		void UploadUniformIntArray(const std::string& name, const int* values, uint32_t count) const;

		void UploadUniformFloat(const std::string& name, float value) const;
		void UploadUniformFloat2(const std::string& name, const glm::vec2& values) const;
		void UploadUniformFloat3(const std::string& name, const glm::vec3& values) const;
		void UploadUniformFloat4(const std::string& name, const glm::vec4& values) const;

		void UploadUniformMat3(const std::string& name, const glm::mat3& matrix) const;
		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const;

	private:
		static std::string ReadFile(const std::string& filepath);
		static std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(std::unordered_map<GLenum, std::string>& shaderSources);

	private:
		uint32_t m_RendererID;
		std::string m_Name;
	};
}

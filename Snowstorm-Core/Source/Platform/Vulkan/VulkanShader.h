#pragma once
#include "VulkanDevice.h"
#include "Snowstorm/Renderer/Shader.h"

namespace Snowstorm
{
	class VulkanShader final : public Shader
	{
	public:
		explicit VulkanShader(const std::string& filepath);
		~VulkanShader() override;

		VulkanShader(const VulkanShader& other) = delete;
		VulkanShader(VulkanShader&& other) = delete;
		VulkanShader& operator=(const VulkanShader& other) = delete;
		VulkanShader& operator=(VulkanShader&& other) = delete;

		void Bind() const override;
		void Unbind() const override;

		void UploadUniform(const std::string& name, int value) override;
		void UploadUniform(const std::string& name, const std::vector<int>& value) override;
		void UploadUniform(const std::string& name, float value) override;
		void UploadUniform(const std::string& name, const glm::vec3& value) override;
		void UploadUniform(const std::string& name, const glm::vec4& value) override;
		void UploadUniform(const std::string& name, const glm::mat4& value) override;

		const std::string& GetName() const override { return m_Name; }

	private:
		VkDevice m_Device;

		std::string m_Name;
	};
}

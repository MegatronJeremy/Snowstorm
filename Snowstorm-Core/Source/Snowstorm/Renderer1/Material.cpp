#include "pch.h"
#include "Material.h"

#include "Snowstorm/Platform/OpenGL/OpenGLMaterial.h"

#include "Snowstorm/Renderer/RendererAPI.h"

namespace Snowstorm {

	Ref<Material> Material::Create(const Ref<Shader>& shader, const std::string& name)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None: return nullptr;
			case RendererAPIType::OpenGL: return Ref<OpenGLMaterial>::Create(shader, name);
		}
		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
	
	Ref<Material> Material::Copy(const Ref<Material>& other, const std::string& name)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None: return nullptr;
			case RendererAPIType::OpenGL: return Ref<OpenGLMaterial>::Create(other, name);
		}
		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}
#include "pch.h"

#include "UniformBufferSet.h"

#include "Snowstorm/Renderer/Renderer.h"

#include "StorageBufferSet.h"

#include "Snowstorm/Platform/OpenGL/OpenGLStorageBufferSet.h"
#include "Snowstorm/Renderer/RendererAPI.h"

namespace Snowstorm {

	Ref<StorageBufferSet> StorageBufferSet::Create(const StorageBufferSpecification& specification, uint32_t size, uint32_t framesInFlight)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:   return nullptr;
			case RendererAPIType::OpenGL: return Ref<OpenGLStorageBufferSet>::Create(specification, size, framesInFlight);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
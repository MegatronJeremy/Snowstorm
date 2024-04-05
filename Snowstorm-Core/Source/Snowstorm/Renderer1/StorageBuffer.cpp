#include "pch.h"
#include "StorageBuffer.h"

#include "Snowstorm/Platform/OpenGL/OpenGLStorageBuffer.h"
#include "Snowstorm/Renderer/RendererAPI.h"

namespace Snowstorm {

	Ref<StorageBuffer> StorageBuffer::Create(uint32_t size, const StorageBufferSpecification& specification)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:     return nullptr;
			case RendererAPIType::OpenGL:  return Ref<OpenGLStorageBuffer>::Create(size, specification);
		}
		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}

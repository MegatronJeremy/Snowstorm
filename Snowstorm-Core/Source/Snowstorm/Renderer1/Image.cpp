#include "pch.h"
#include "Image.h"

#include "Snowstorm/Platform/Vulkan/VulkanImage.h"

#include "Snowstorm/Renderer/RendererAPI.h"

namespace Snowstorm {

	Ref<Image2D> Image2D::Create(const ImageSpecification& specification, Buffer buffer)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None: return nullptr;
			case RendererAPIType::Vulkan: return Ref<VulkanImage2D>::Create(specification);
		}
		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	Ref<Image2D> Image2D::Create(const ImageSpecification& specification, const void* data)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None: return nullptr;
			case RendererAPIType::Vulkan: return Ref<VulkanImage2D>::Create(specification);
		}
		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	Ref<ImageView> ImageView::Create(const ImageViewSpecification& specification)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None: return nullptr;
			case RendererAPIType::Vulkan: return Ref<VulkanImageView>::Create(specification);
		}
		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}

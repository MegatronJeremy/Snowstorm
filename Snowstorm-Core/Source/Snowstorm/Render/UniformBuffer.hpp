#pragma once

#include <memory>

namespace Snowstorm
{
	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() = default;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
		[[nodiscard]] virtual uint32_t GetRendererID() const = 0;

		static std::shared_ptr<UniformBuffer> Create(uint32_t size, uint32_t binding);
	};
}

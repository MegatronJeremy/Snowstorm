#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

namespace Snowstorm
{
    class RendererAPI
    {
    public:
        RendererAPI() = default;
        virtual ~RendererAPI() = default;

        RendererAPI(const RendererAPI& other) = delete;
        RendererAPI(RendererAPI&& other) = delete;
        RendererAPI& operator=(const RendererAPI& other) = delete;
        RendererAPI& operator=(RendererAPI&& other) = delete;

        enum class API
        {
            None = 0,
            // For headless execution
            OpenGL = 1,
            Vulkan = 2
        };

        virtual void Init() = 0;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
        virtual void SetClearColor(const glm::vec4& color) = 0;
        virtual void Clear() = 0;

        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

        static API GetAPI() { return s_API; }

    private:
        inline static auto s_API = API::OpenGL;
    };
}

#include "pch.h"
#include "Renderer2D.h"

#include <glm/gtc/matrix_transform.hpp>

#include "RenderCommand.h"
#include "Shader.h"
#include "VertexArray.h"

namespace Snowstorm
{
    struct QuadVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float TextureIndex;
        float TilingFactor;
    };

    struct Renderer2DData
    {
        static constexpr uint32_t MaxQuads = 20000;
        static constexpr uint32_t MaxVertices = MaxQuads * 4;
        static constexpr uint32_t MaxIndices = MaxQuads * 6;
        static constexpr uint32_t MaxTextureSlots = 32; // TODO: RenderCaps

        Ref<VertexArray> QuadVertexArray;
        Ref<VertexBuffer> QuadVertexBuffer;
        Ref<Shader> TextureShader;
        Ref<Texture2D> WhiteTexture;

        uint32_t QuadIndexCount = 0;
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr = nullptr;

        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1; // 0 = white texture

        glm::vec4 QuadVertexPositions[4];

        Renderer2D::Statistics Stats;
    };

    namespace
    {
        Renderer2DData s_Data;
    }

    void Renderer2D::Init()
    {
        SS_PROFILE_FUNCTION();

        s_Data.QuadVertexArray = VertexArray::Create();

        s_Data.QuadVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxVertices * sizeof(QuadVertex));
        s_Data.QuadVertexBuffer->SetLayout({
            {ShaderDataType::Float3, "a_Position"},
            {ShaderDataType::Float4, "a_Color"},
            {ShaderDataType::Float2, "a_TexCoord"},
            {ShaderDataType::Float, "a_TextureIndex"},
            {ShaderDataType::Float, "a_TilingFactor"},
        });
        s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

        s_Data.QuadVertexBufferBase = new QuadVertex[Renderer2DData::MaxVertices];

        const auto quadIndices = new uint32_t[Renderer2DData::MaxIndices];

        uint32_t offset = 0;
        for (uint32_t i = 0; i < Renderer2DData::MaxIndices; i += 6)
        {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;

            offset += 4;
        }

        const Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, Renderer2DData::MaxIndices);
        s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
        delete[] quadIndices;

        s_Data.WhiteTexture = Texture2D::Create(1, 1);
        uint32_t whiteTextureData = 0xffffffff;
        s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

        int32_t samplers[Renderer2DData::MaxTextureSlots];
        for (uint32_t i = 0; i < Renderer2DData::MaxTextureSlots; i++)
            samplers[i] = i;

        s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");
        s_Data.TextureShader->Bind();
        s_Data.TextureShader->SetIntArray("u_Textures", samplers, Renderer2DData::MaxTextureSlots);

        s_Data.TextureSlots[0] = s_Data.WhiteTexture;

        s_Data.QuadVertexPositions[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
        s_Data.QuadVertexPositions[1] = {0.5f, -0.5f, 0.0f, 1.0f};
        s_Data.QuadVertexPositions[2] = {0.5f, 0.5f, 0.0f, 1.0f};
        s_Data.QuadVertexPositions[3] = {-0.5f, 0.5f, 0.0f, 1.0f};
    }

    void Renderer2D::Shutdown()
    {
        SS_PROFILE_FUNCTION();
    }

    void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
    {
        SS_PROFILE_FUNCTION();

        const glm::mat4 viewProj = camera.GetProjection() * inverse(transform);

        s_Data.TextureShader->Bind();
        s_Data.TextureShader->SetMat4("u_ViewProjection", viewProj);

        s_Data.QuadIndexCount = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

        s_Data.TextureSlotIndex = 1;
    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera)
    {
        SS_PROFILE_FUNCTION();

        s_Data.TextureShader->Bind();
        s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

        s_Data.QuadIndexCount = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

        s_Data.TextureSlotIndex = 1;
    }

    void Renderer2D::EndScene()
    {
        SS_PROFILE_FUNCTION();

        const uint32_t dataSize = reinterpret_cast<uint8_t*>(s_Data.QuadVertexBufferPtr) - reinterpret_cast<uint8_t*>(
            s_Data.QuadVertexBufferBase);
        s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

        Flush();
    }

    void Renderer2D::Flush()
    {
        // Bind textures
        for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
            s_Data.TextureSlots[i]->Bind(i);

        SS_INFO("Flushing");

        RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);

        s_Data.Stats.DrawCalls++;
    }

    void Renderer2D::FlushAndReset()
    {
        EndScene();

        s_Data.QuadIndexCount = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

        s_Data.TextureSlotIndex = 1;
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
    {
        DrawQuad({position.x, position.y, 0.0f}, size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
    {
        SS_PROFILE_FUNCTION();

        const glm::mat4 transform = translate(glm::mat4(1.0f), position)
            * scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

        DrawQuad(transform, color);
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture,
                              const float tilingFactor, const glm::vec4& tintColor)
    {
        DrawQuad({position.x, position.y, 0.0f}, size, texture, tilingFactor, tintColor);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture,
                              const float tilingFactor, const glm::vec4& tintColor)
    {
        SS_PROFILE_FUNCTION();

        const glm::mat4 transform = translate(glm::mat4(1.0f), position)
            * scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

        DrawQuad(transform, texture, tilingFactor, tintColor);
    }

#pragma region Subtextures
    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<SubTexture2D>& subTexture,
                              const float tilingFactor, const glm::vec4& tintColor)
    {
        DrawQuad({position.x, position.y, 0.0f}, size, subTexture, tilingFactor, tintColor);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<SubTexture2D>& subTexture,
                              const float tilingFactor, const glm::vec4& tintColor)
    {
        SS_PROFILE_FUNCTION();

        constexpr size_t quadVertexCount = 4;
        const glm::vec2* textureCoords = subTexture->GetTexCoords();
        const Ref<Texture2D> texture = subTexture->GetTexture();

        if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
            FlushAndReset();

        float textureIndex = 0.0f;
        for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
        {
            if (*s_Data.TextureSlots[i] == *texture)
            {
                textureIndex = static_cast<float>(i);
                break;
            }
        }

        if (textureIndex == 0.0f)
        {
            textureIndex = static_cast<float>(s_Data.TextureSlotIndex);
            s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
            s_Data.TextureSlotIndex++;
        }

        const glm::mat4 transform = translate(glm::mat4(1.0f), position)
            * scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            s_Data.QuadVertexBufferPtr[i].Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.QuadVertexBufferPtr[i].Color = tintColor;
            s_Data.QuadVertexBufferPtr[i].TexCoord = textureCoords[i];
            s_Data.QuadVertexBufferPtr[i].TextureIndex = textureIndex;
            s_Data.QuadVertexBufferPtr[i].TilingFactor = tilingFactor;
        }

        s_Data.QuadVertexBufferPtr += quadVertexCount;
        s_Data.QuadIndexCount += 6;

        s_Data.Stats.QuadCount++;
    }
#pragma endregion Subtextures

    void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
    {
        constexpr size_t quadVertexCount = 4;
        constexpr glm::vec2 textureCoords[] = {
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f}
        };

        if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
            FlushAndReset();

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            constexpr float textureIndex = 0.0f;
            constexpr float tilingFactor = 1.0f;

            s_Data.QuadVertexBufferPtr[i].Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.QuadVertexBufferPtr[i].Color = color;
            s_Data.QuadVertexBufferPtr[i].TexCoord = textureCoords[i];
            s_Data.QuadVertexBufferPtr[i].TextureIndex = textureIndex;
            s_Data.QuadVertexBufferPtr[i].TilingFactor = tilingFactor;
        }

        s_Data.QuadVertexBufferPtr += quadVertexCount;
        s_Data.QuadIndexCount += 6;

        s_Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor,
                              const glm::vec4& tintColor)
    {
        SS_PROFILE_FUNCTION();

        constexpr size_t quadVertexCount = 4;
        constexpr glm::vec2 textureCoords[] = {
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f}
        };

        if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
            FlushAndReset();

        float textureIndex = 0.0f;
        for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
        {
            if (*s_Data.TextureSlots[i] == *texture)
            {
                textureIndex = static_cast<float>(i);
                break;
            }
        }

        if (textureIndex == 0.0f)
        {
            textureIndex = static_cast<float>(s_Data.TextureSlotIndex);
            s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
            s_Data.TextureSlotIndex++;
        }


        for (size_t i = 0; i < quadVertexCount; i++)
        {
            s_Data.QuadVertexBufferPtr[i].Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.QuadVertexBufferPtr[i].Color = tintColor;
            s_Data.QuadVertexBufferPtr[i].TexCoord = textureCoords[i];
            s_Data.QuadVertexBufferPtr[i].TextureIndex = textureIndex;
            s_Data.QuadVertexBufferPtr[i].TilingFactor = tilingFactor;
        }

        s_Data.QuadVertexBufferPtr += quadVertexCount;
        s_Data.QuadIndexCount += 6;

        s_Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const float rotation,

                                     const glm::vec4& color)
    {
        DrawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, color);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const float rotation,
                                     const glm::vec4& color)
    {
        SS_PROFILE_FUNCTION();

        constexpr size_t quadVertexCount = 4;
        constexpr glm::vec2 textureCoords[] = {
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f}
        };

        if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
            FlushAndReset();

        const glm::mat4 transform = translate(glm::mat4(1.0f), position)
            * rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
            * scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            constexpr float textureIndex = 0.0f;
            constexpr float tilingFactor = 1.0f;

            s_Data.QuadVertexBufferPtr[i].Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.QuadVertexBufferPtr[i].Color = color;
            s_Data.QuadVertexBufferPtr[i].TexCoord = textureCoords[i];
            s_Data.QuadVertexBufferPtr[i].TextureIndex = textureIndex;
            s_Data.QuadVertexBufferPtr[i].TilingFactor = tilingFactor;
        }

        s_Data.QuadVertexBufferPtr += quadVertexCount;
        s_Data.QuadIndexCount += 6;

        s_Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const float rotation,
                                     const Ref<Texture2D>& texture, const float tilingFactor,
                                     const glm::vec4& tintColor)
    {
        DrawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, texture, tilingFactor, tintColor);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const float rotation,
                                     const Ref<Texture2D>& texture, const float tilingFactor,
                                     const glm::vec4& tintColor)
    {
        SS_PROFILE_FUNCTION();

        constexpr size_t quadVertexCount = 4;
        constexpr glm::vec2 textureCoords[] = {
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f}
        };

        if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
            FlushAndReset();

        float textureIndex = 0.0f;

        for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
        {
            if (*s_Data.TextureSlots[i] == *texture)
            {
                textureIndex = static_cast<float>(i);
                break;
            }
        }

        if (textureIndex == 0.0f)
        {
            textureIndex = static_cast<float>(s_Data.TextureSlotIndex);
            s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
            s_Data.TextureSlotIndex++;
        }

        const glm::mat4 transform = translate(glm::mat4(1.0f), position)
            * rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
            * scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            s_Data.QuadVertexBufferPtr[i].Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.QuadVertexBufferPtr[i].Color = tintColor;
            s_Data.QuadVertexBufferPtr[i].TexCoord = textureCoords[i];
            s_Data.QuadVertexBufferPtr[i].TextureIndex = textureIndex;
            s_Data.QuadVertexBufferPtr[i].TilingFactor = tilingFactor;
        }

        s_Data.QuadVertexBufferPtr += quadVertexCount;
        s_Data.QuadIndexCount += 6;

        s_Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, const float rotation,
                                     const Ref<SubTexture2D>& subTexture, const float tilingFactor,
                                     const glm::vec4& tintColor)
    {
        DrawRotatedQuad({position.x, position.y, 0.0f}, size, rotation, subTexture, tilingFactor, tintColor);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, const float rotation,
                                     const Ref<SubTexture2D>& subTexture, const float tilingFactor,
                                     const glm::vec4& tintColor)
    {
        SS_PROFILE_FUNCTION();

        constexpr size_t quadVertexCount = 4;
        const glm::vec2* textureCoords = subTexture->GetTexCoords();
        const Ref<Texture2D> texture = subTexture->GetTexture();

        if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
            FlushAndReset();

        float textureIndex = 0.0f;
        for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
        {
            if (*s_Data.TextureSlots[i] == *texture)
            {
                textureIndex = static_cast<float>(i);
                break;
            }
        }

        if (textureIndex == 0.0f)
        {
            textureIndex = static_cast<float>(s_Data.TextureSlotIndex);
            s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
            s_Data.TextureSlotIndex++;
        }

        const glm::mat4 transform = translate(glm::mat4(1.0f), position)
            * rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f})
            * scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            s_Data.QuadVertexBufferPtr[i].Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.QuadVertexBufferPtr[i].Color = tintColor;
            s_Data.QuadVertexBufferPtr[i].TexCoord = textureCoords[i];
            s_Data.QuadVertexBufferPtr[i].TextureIndex = textureIndex;
            s_Data.QuadVertexBufferPtr[i].TilingFactor = tilingFactor;
        }

        s_Data.QuadVertexBufferPtr += quadVertexCount;
        s_Data.QuadIndexCount += 6;

        s_Data.Stats.QuadCount++;
    }

    void Renderer2D::ResetStats()
    {
        memset(&s_Data.Stats, 0, sizeof(Statistics));
    }

    Renderer2D::Statistics Renderer2D::GetStats()
    {
        return s_Data.Stats;
    }
}

#type vertex
#version 450 core

layout(std140, binding = 0) uniform CameraData
{
    mat4 u_ViewProjection;
};

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in mat4 a_ModelMatrix;
layout(location = 7) in float a_TextureIndex;

out vec2 v_TexCoord;
out float v_TextureIndex;

void main()
{
    v_TexCoord = a_TexCoord;
    v_TextureIndex = a_TextureIndex;

    gl_Position = u_ViewProjection * a_ModelMatrix * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

in vec2 v_TexCoord;
in float v_TextureIndex;

layout(location = 0) out vec4 FragColor;

uniform sampler2D u_Textures[32];
uniform vec4 u_Color;

void main()
{
    FragColor = texture(u_Textures[int(v_TextureIndex)], v_TexCoord) * u_Color;
}

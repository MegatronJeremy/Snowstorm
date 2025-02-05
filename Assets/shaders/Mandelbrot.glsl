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

out vec2 v_TexCoord;

void main()
{
    v_TexCoord = a_TexCoord;
    gl_Position = u_ViewProjection * a_ModelMatrix * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

in vec2 v_TexCoord;
layout(location = 0) out vec4 FragColor;

// Mandelbrot Uniforms
uniform vec2 u_Center;
uniform float u_Zoom;
uniform int u_MaxIterations;

// Converts HSV to RGB for coloring
vec3 hsv_to_rgb(float h, float s, float v)
{
    float p, q, t, f = mod(h * 6.0, 6.0);
    int i = int(f);
    f -= i;
    p = v * (1.0 - s);
    q = v * (1.0 - s * f);
    t = v * (1.0 - s * (1.0 - f));
    
    if (i == 0) return vec3(v, t, p);
    if (i == 1) return vec3(q, v, p);
    if (i == 2) return vec3(p, v, t);
    if (i == 3) return vec3(p, q, v);
    if (i == 4) return vec3(t, p, v);
    return vec3(v, p, q);
}

void main()
{
    // Map fragment coordinates to complex plane
    vec2 c = u_Center + (v_TexCoord - 0.5) * u_Zoom;

    // Mandelbrot iteration
    vec2 z = vec2(0.0);
    int iter;
    for (iter = 0; iter < u_MaxIterations; iter++)
    {
        if (dot(z, z) > 4.0) break;
        z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
    }

    // Color based on iteration count
    float colorFactor = float(iter) / float(u_MaxIterations);
    vec3 color = hsv_to_rgb(colorFactor, 1.0, 1.0);

    // If the point belongs to the Mandelbrot set, make it black
    if (iter == u_MaxIterations)
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    else
        FragColor = vec4(color, 1.0);
}
#version 450 core

layout (std140, binding = 0) uniform UniformBlock {
    mat4 MVPs[256];
} models;

layout (location = 0) in vec2 pos;

uniform int index;

out vec2 v_tex_coords;

void main()
{
    v_tex_coords = pos + 0.5f;
    gl_Position  = models.MVPs[index] * vec4(pos, 1.0f, 1.0f);
}

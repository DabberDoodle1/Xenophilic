#version 450 core

in vec2 v_color;
out vec4 frag_color;

void main()
{
    frag_color = vec4(v_color.x, 0.0, v_color.y, 1.0);
}

#version 450 core

in  vec2 v_tex_coords;
out vec4 frag_color;

uniform bool is_hovering;
uniform sampler2D tex;

void main()
{
    float alpha = texture(tex, vec2(v_tex_coords.x, 1 - v_tex_coords.y)).r;
    float v     = is_hovering ? 1.0f : 0.0f;

    frag_color = vec4(v * 0.5f, 0.0f, v, alpha);

}

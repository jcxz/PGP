#version 330

in vec2 tex_coords;

out vec4 frag_color;

uniform vec4 col;
uniform bool use_texture;
uniform sampler2D tex;


void main(void)
{
  frag_color = use_texture ? texture2D(tex, tex_coords) : col;
}

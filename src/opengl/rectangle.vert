#version 330

out vec2 tex_coords;

uniform mat4 mvp;

const vec2 rect_pos[] = {
  vec2(-1.0f,  1.0f),
  vec2(-1.0f, -1.0f),
  vec2( 1.0f,  1.0f),
  vec2( 1.0f, -1.0f)
};


void main(void)
{
  vec2 pos = rect_pos[gl_VertexID];
  tex_coords = (pos + 1.0f) / 2.0f;
  gl_Position = mvp * vec4(pos, 0.0f, 1.0f);
}

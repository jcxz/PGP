#version 330


uniform mat4 mvp;

const vec2 rect_pos[] = {
  vec2(-1.0f,  1.0f),
  vec2(-1.0f, -1.0f),
  vec2( 1.0f,  1.0f),
  vec2( 1.0f, -1.0f)
};


void main(void)
{
  gl_Position = mvp * vec4(rect_pos[gl_VertexID], 1.0f, 1.0f);
}

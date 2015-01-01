#version 330

layout(location = 0) in vec3 pos;

out vec4 ray_coords;
out vec4 tex_coords;

uniform mat4 mvp;


void main(void)
{
  ray_coords = vec4(pos * 0.5f + 0.5f, 1.0f);
  gl_Position = mvp * vec4(pos, 1.0f);
  tex_coords = gl_Position;
}

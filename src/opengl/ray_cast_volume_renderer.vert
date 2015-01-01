#version 330

layout(location = 0) in vec3 pos;

out vec4 ray_coords;
out vec4 tex_coords;

uniform mat4 mvp;


void main(void)
{
  ray_coords = vec4(pos, 1.0f);
  gl_Position = mvp * ray_coords;
  tex_coords = gl_Position;
}

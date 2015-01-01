#version 330

in vec4 ray_coords;

out vec4 exit_point;


void main(void)
{
  exit_point = ray_coords;
}

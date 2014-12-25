#version 330

uniform vec3 col;  // color of the bounding volume

out vec4 fragColor;


void main(void)
{
  fragColor = vec4(col, 1.0f);
}

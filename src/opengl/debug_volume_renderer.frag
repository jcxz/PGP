#version 330

in vec3 col_frag;

out vec4 fragColor;


void main(void)
{
  fragColor = vec4(col_frag, 1.0f);
  //fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

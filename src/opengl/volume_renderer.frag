#version 330

in vec3 tex_coords;
layout(location = 0) out vec4 fragColor;

uniform sampler3D tex_data;


void main(void)
{
  vec4 c = texture(tex_data, tex_coords);
  fragColor = vec4(c.a * c.rgb, c.a);    // Multiply color by opacity
  //gl_FragColor = c;
}

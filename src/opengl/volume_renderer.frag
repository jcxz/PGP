#version 330

in vec3 tex_coords;

out vec4 fragColor;

uniform sampler3D tex_data;


void main(void)
{
  vec4 c = texture3D(tex_data, tex_coords);
  //gl_FragColor = vec4(c.a * c.rgb, c.a);    // Multiply color by opacity
  //gl_FragColor = c;
  fragColor = c;
  //gl_FragColor = c * vec4(0.5f, 0.5f, 0.5f, 0.5f);// * vec4(0.5f, 0.5f, 0.5f, 1.0f);
  //gl_FragColor = vec4(c.rgb * (c.g + c.r + c.b), c.a);
  //gl_FragColor = vec4(1, 1, 1, 1);

  //if (c.a > 0.05f)
  //  gl_FragColor = vec4(c.rgb, c.a);
  //else
  //  gl_FragColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
}

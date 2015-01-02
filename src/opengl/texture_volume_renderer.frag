#version 330

in vec3 tex_coords;

out vec4 fragColor;

uniform sampler3D tex_data;
uniform sampler1D tex_transfer_func;
uniform bool use_tf;


void main(void)
{
  vec4 c = texture3D(tex_data, tex_coords);
  //fragColor = c;
  //fragColor = vec4(c.a * c.rgb, c.a);
  //fragColor = c * vec4(0.5f, 0.5f, 0.5f, 0.5f); //fragColor = c * vec4(0.5f, 0.5f, 0.5f, 1.0f);
  //fragColor = c * vec4(1.0f, 0.0f, 0.0f, 0.5f);

  //fragColor = vec4(c.a * vec3(10.0f, 10.5f, 10.5f), c.a);
  //fragColor = vec4(c.a * texture1D(tex_transfer_func, c.a).xyz, c.a);

  //fragColor = texture1D(tex_transfer_func, c.a);
  if (use_tf)
    fragColor = texture1D(tex_transfer_func, c.a);
  else
    fragColor = c;

  //fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}














#if 0
#version 330

in vec3 tex_coords;

out vec4 fragColor;

uniform sampler3D tex_data;
uniform sampler1D tex_transfer_func;

const int colors_max = 8;
const vec4 colors[colors_max] = {
  vec4(0.0f, 0.0f, 0.0f, 0.5f),
  vec4(0.0f, 0.0f, 1.0f, 0.5f),
  vec4(0.0f, 1.0f, 0.0f, 0.5f),
  vec4(0.0f, 1.0f, 1.0f, 0.5f),
  vec4(1.0f, 0.0f, 0.0f, 0.5f),
  vec4(1.0f, 0.0f, 1.0f, 0.5f),
  vec4(1.0f, 1.0f, 0.0f, 0.5f),
  vec4(1.0f, 1.0f, 1.0f, 0.5f)
};


void main(void)
{
  vec4 c = texture3D(tex_data, tex_coords);
  //gl_FragColor = vec4(c.a * c.rgb, c.a);    // Multiply color by opacity
  //gl_FragColor = c;

#if 0
  fragColor = c;
#else
#if 0
  if (c.a < 0.0f)
      fragColor = vec4(0.0f, 0.0f, 1.0f, c.a);
  else if (c.a == 0.0f)
      fragColor = vec4(1.0f, 1.0f, 1.0f, c.a);
  else if (c.a < 0.5f)
      //fragColor = vec4(1.0f, 1.0f, 0.0f, c.a);
      //fragColor = vec4(0.0f, 0.0f, 1.0f, c.a);
      fragColor = vec4(texture1D(tex_transfer_func, c.a).xyz, c.a);
      //fragColor = texture1D(tex_transfer_func, c.a);
  else if (c.a < 1.0f)
      fragColor = vec4(0.0f, c.a, 0.0f, c.a);
  else if (c.a > 1.0f)
      fragColor = vec4(1.0f, 0.0f, 0.0f, c.a);
#else
  //fragColor = vec4(colors[int(c.a * 256.0f) % 8].xyz, c.a);
  fragColor = colors[int(c.a * 256.0f) % 8];
#endif
#endif

  //fragColor = vec4(texture1D(tex_transfer_func, c.a).xyz, c.a);
  //fragColor = texture1D(tex_transfer_func, c.a);

  //fragColor = vec4(1.0f, texture1D(tex_transfer_func, c.a).yzw);

  //gl_FragColor = c * vec4(0.5f, 0.5f, 0.5f, 0.5f);// * vec4(0.5f, 0.5f, 0.5f, 1.0f);
  //gl_FragColor = vec4(c.rgb * (c.g + c.r + c.b), c.a);
  //gl_FragColor = vec4(1, 1, 1, 1);

  //if (c.a > 0.05f)
  //  gl_FragColor = vec4(c.rgb, c.a);
  //else
  //  gl_FragColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
}
#endif

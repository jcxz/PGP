#version 330

in vec3 tex_coords;

out vec4 fragColor;

uniform sampler3D tex_data;
uniform sampler1D tex_transfer_func;
uniform bool use_tf;
uniform float num_instances;
uniform float num_instances_inv;


void main(void)
{
  vec4 c = texture3D(tex_data, tex_coords);

  //fragColor = vec4(c.a * c.rgb, c.a);
  //fragColor = c * vec4(0.5f, 0.5f, 0.5f, 0.5f); //fragColor = c * vec4(0.5f, 0.5f, 0.5f, 1.0f);
  //fragColor = c * vec4(1.0f, 0.0f, 0.0f, 0.5f);

  //fragColor = vec4(c.a * vec3(10.0f, 10.5f, 10.5f), c.a);
  //fragColor = vec4(c.a * texture1D(tex_transfer_func, c.a).xyz, c.a);

  if (use_tf)
    fragColor = texture1D(tex_transfer_func, c.a);
  else
  {
    //c.a *= 100000.0f * num_instances_inv * num_instances_inv; //0.5f;
    //c.a *= 1000.0f * num_instances_inv;
    c.a *= 0.4f;
    fragColor = c;
    //fragColor = c * 800.0f * num_instances_inv;
  }
}

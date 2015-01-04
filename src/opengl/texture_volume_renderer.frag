#version 330

//#define TF
#define DIFFUSE


#ifdef DIFFUSE
in vec3 tex_coords;
in vec3 pos_out;

out vec4 frag_color;

uniform sampler3D tex_data;
uniform sampler1D tex_transfer_func;
uniform bool use_tf;
uniform float alpha_correction_factor;
uniform int method;
uniform vec3 La;         // ambientna zlozka svetla
uniform vec3 Ld;         // difuzna zlozka svetla
uniform vec3 light_pos;  // pozicia svetla v scene



vec4 diffuseShading(vec4 data)
{
  vec4 c = texture1D(tex_transfer_func, data.a);

  // vypocet difuzneho osvetlovacieho modelu
  //vec3 L = normalize(light_pos - (2.0f * tex_coords - 1.0f));
  vec3 L = normalize(light_pos - pos_out);
  vec3 N = 2.0f * data.xyz - 1.0f;      // prevod normaly do object space modelu (pretoze suradnice v OpenGL texture su v rozsahu <0, 1>)
  c.rgb = La * c.rgb + Ld * c.rgb * max(dot(N, L), 0.0f);

  // korekcia opacity
  c.a = 1.0f - pow(1.0f - c.a, alpha_correction_factor);

  return c;
}

vec4 transferFunction(vec4 data)
{
  //return texture1D(tex_transfer_func, data.a);
  // korekcia opacity
  vec4 c = texture1D(tex_transfer_func, data.a);
  c.a = 1.0f - pow(1.0f - c.a, alpha_correction_factor);
  return c;
}


vec4 intensity(vec4 data)
{
  data.rgb = data.aaa;
  // korekcia opacity
  data.a = 1.0f - pow(1.0f - data.a, alpha_correction_factor);
  data.a *= 0.4f;
  //return vec4(1.0f, 1.0f, 1.0f, 1.0f);
  return data;
}


void main(void)
{
  vec4 c = texture3D(tex_data, tex_coords);

  if (method == 1)
    frag_color = diffuseShading(c);
  else if (method == 2)
    frag_color = transferFunction(c);
  else
    frag_color = intensity(c);
}
#endif














#if 0
void main(void)
{
  vec4 c = texture3D(tex_data, tex_coords);

  if (use_tf)
    frag_color = texture1D(tex_transfer_func, c.a);
  else
  {
    c.a *= 0.4f;
    frag_color = c;
  }
}
#endif















#ifdef TF
in vec3 tex_coords;

out vec4 fragColor;

uniform sampler3D tex_data;
uniform sampler1D tex_transfer_func;
uniform bool use_tf;
//uniform float num_instances;
//uniform float num_instances_inv;
uniform float alpha_correction_factor;

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
#endif

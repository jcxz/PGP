#version 330

in vec4 ray_coords;
in vec4 tex_coords;

out vec4 frag_color;

uniform sampler3D tex_volume_data;
uniform sampler2D tex_back_faces;
uniform sampler1D tex_transfer_func;
uniform float step;




#if 0
// shader na zobrazenie vstupnych bodov paprsku/vystupnych bodov paprsku/smeru paprsku
void main(void)
{
  vec4 entry_point = ray_coords;
  vec4 exit_point = texture2D(tex_back_faces, (tex_coords.xy / tex_coords.w) * 0.5f + 0.5f);
  vec4 dir = exit_point - entry_point;

  //frag_color = dir;
  frag_color = entry_point;
  //frag_color = exit_point;
}
#else
void main(void)
{
  vec3 entry_point = ray_coords.xyz;
  vec3 exit_point = texture2D(tex_back_faces, (tex_coords.xy / tex_coords.w) * 0.5f + 0.5f).xyz;
  vec3 dir = exit_point - entry_point;

  vec4 accum = vec4(0.0f, 0.0f, 0.0f, 0.0f);

  for (float t = 0.0f; t < 1.0f; t += step)
  {
    vec3 pos = entry_point + t * dir;
    vec4 c = texture3D(tex_volume_data, pos);

#if 0
    //accum.rgb = c.rgb * c.a + (1.0f - c.a) * accum.rgb * accum.a;
    //accum.a = c.a + (1.0f - c.a) * accum.a;

#else
     c.a *= .5f; //reduce the alpha to have a more transparent result
     //Front to back blending
     // dst.rgb = dst.rgb + (1 - dst.a) * src.a * src.rgb
     // dst.a   = dst.a   + (1 - dst.a) * src.a
     c.rgb *= c.a;
     accum = (1.0f - accum.a) * c + accum;

     //break from the loop when alpha gets high enough
     if (accum.a >= .95f) break;
#endif
  }

  frag_color = accum;
}
#endif

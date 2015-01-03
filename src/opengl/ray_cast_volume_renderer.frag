#version 330

#define RAYCASTING
//#ifdef DISPLAY_ENTRY_POINTS
//#ifdef DISPLAY_EXIT_POINTS
//#ifdef DISPLAY_RAY_DIRECTIONS

in vec4 ray_coords;
in vec4 tex_coords;

out vec4 frag_color;

uniform sampler3D tex_volume_data;
uniform sampler2D tex_back_faces;
uniform sampler1D tex_transfer_func;
uniform float step;
uniform float offset;
uniform bool use_tf;



#ifdef RAYCASTING
vec4 rayMarchingTF(vec3 pos, vec3 dir)
{
  vec4 accum = vec4(0.0f, 0.0f, 0.0f, 0.0f);

  for (float t = offset; t < 1.0f; t += step)
  {
    vec4 tex_c = texture3D(tex_volume_data, pos);
    vec4 c = texture1D(tex_transfer_func, tex_c.a);

    c.rgb *= c.a;
    accum = (1.0f - accum.a) * c + accum;

    //break from the loop when alpha gets high enough
    if (accum.a >= .95f) break;

    pos += dir;

    // vyskoc z cyklu ak je niektora zo suradnic uz mimo bounding volume
    if (pos.x > 1.0f || pos.y > 1.0f || pos.z > 1.0f) break;
  }

  return accum;
}


vec4 rayMarching(vec3 pos, vec3 dir)
{
  vec4 accum = vec4(0.0f, 0.0f, 0.0f, 0.0f);

  for (float t = offset; t < 1.0f; t += step)
  {
    vec4 c = texture3D(tex_volume_data, pos);
    c.a *= .5f; //reduce the alpha to have a more transparent result

    //Front to back blending
    // dst.rgb = dst.rgb + (1 - dst.a) * src.a * src.rgb
    // dst.a   = dst.a   + (1 - dst.a) * src.a
    c.rgb *= c.a;
    accum = (1.0f - accum.a) * c + accum;

    //break from the loop when alpha gets high enough
    if (accum.a >= .95f) break;

    pos += dir;

    // vyskoc z cyklu ak je niektora zo suradnic uz mimo bounding volume
    if (pos.x > 1.0f || pos.y > 1.0f || pos.z > 1.0f) break;
  }

  return accum;
}


void main(void)
{
  vec3 pos = ray_coords.xyz;
  vec3 exit_point = texture2D(tex_back_faces, (tex_coords.xy / tex_coords.w) * 0.5f + 0.5f).xyz;
  vec3 dir = (exit_point - pos);

  pos += dir * offset;
  dir *= step;

  if (use_tf)
    frag_color = rayMarchingTF(pos, dir);
  else
    frag_color = rayMarching(pos, dir);
}
#endif


// shader na zobrazenie vstupnych bodov paprskov
#ifdef DISPLAY_ENTRY_POINTS
void main(void) { frag_color = ray_coords; }
#endif


// shader na zobrazenie vystupnych bodov paprskov
#ifdef DISPLAY_EXIT_POINTS
void main(void) { frag_color = texture2D(tex_back_faces, (tex_coords.xy / tex_coords.w) * 0.5f + 0.5f); }
#endif


// shader na zobrazenie smeru paprskov
#ifdef DISPLAY_RAY_DIRECTIONS
void main(void)
{
  vec4 exit_point = texture2D(tex_back_faces, (tex_coords.xy / tex_coords.w) * 0.5f + 0.5f);
  vec4 dir = exit_point - ray_coords;
  frag_color = dir;
}
#endif

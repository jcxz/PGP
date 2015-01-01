#version 330

//#define RAYCASTING_TRANSFER_FUNC
#define RAYCASTING_PEEL_DEPTH


in vec4 ray_coords;
in vec4 tex_coords;

out vec4 frag_color;

uniform sampler3D tex_volume_data;
uniform sampler2D tex_back_faces;
uniform sampler1D tex_transfer_func;
uniform float step;
uniform float offset;



// mierne optimalizovany raycasting s transfer funkciou
#ifdef RAYCASTING_PEEL_DEPTH
void main(void)
{
  vec3 pos = ray_coords.xyz;
  vec3 exit_point = texture2D(tex_back_faces, (tex_coords.xy / tex_coords.w) * 0.5f + 0.5f).xyz;
  vec3 dir = (exit_point - pos);

  pos += dir * offset;
  dir *= step;

  vec4 accum = vec4(0.0f, 0.0f, 0.0f, 0.0f);

  for (float t = offset; t < 1.0f; t += step)
  {
    //vec4 tex_c = texture3D(tex_volume_data, pos);
    //vec4 c = texture1D(tex_transfer_func, tex_c.a);

    vec4 c = texture3D(tex_volume_data, pos);
    c.a *= .5f;

    c.rgb *= c.a;
    accum = (1.0f - accum.a) * c + accum;

    //break from the loop when alpha gets high enough
    if (accum.a >= .95f) break;

    pos += dir;

    // vyskoc z cyklu ak je niektora zo suradnic uz mimo bounding volume
    if (pos.x > 1.0f || pos.y > 1.0f || pos.z > 1.0f) break;
  }

  frag_color = accum;
}
#endif



#ifdef RAYCASTING_TRANSFER_FUNC
void main(void)
{
  vec3 pos = ray_coords.xyz;
  vec3 exit_point = texture2D(tex_back_faces, (tex_coords.xy / tex_coords.w) * 0.5f + 0.5f).xyz;
  vec3 dir = (exit_point - pos) * step;

  vec4 accum = vec4(0.0f, 0.0f, 0.0f, 0.0f);

  for (float t = 0.0f; t < 1.0f; t += step)
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

  frag_color = accum;
}
#endif


// mierne optimalizovany raycasting
#ifdef RAYCASTING2
void main(void)
{
  vec3 pos = ray_coords.xyz;
  vec3 exit_point = texture2D(tex_back_faces, (tex_coords.xy / tex_coords.w) * 0.5f + 0.5f).xyz;
  vec3 dir = (exit_point - pos) * step;

  vec4 accum = vec4(0.0f, 0.0f, 0.0f, 0.0f);

  for (float t = 0.0f; t < 1.0f; t += step)
  {
    vec4 c = texture3D(tex_volume_data, pos);

    c.a *= .5f;
    c.rgb *= c.a;
    accum = (1.0f - accum.a) * c + accum;

    //break from the loop when alpha gets high enough
    if (accum.a >= .95f) break;

    pos += dir;

    // vyskoc z cyklu ak je niektora zo suradnic uz mimo bounding volume
    if (pos.x > 1.0f || pos.y > 1.0f || pos.z > 1.0f) break;
  }

  frag_color = accum;
}
#endif


// raycasting
#ifdef RAYCASTING1
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

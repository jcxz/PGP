#version 330

layout(location = 0) in vec3 pos_vert;
layout(location = 1) in vec3 col_vert;

out vec3 col_frag;

uniform mat4 mvp;

const vec3 colors[] = {
  vec3(1.0f, 0.0f, 0.0f),
  vec3(0.0f, 1.0f, 0.0f),
  vec3(0.0f, 0.0f, 1.0f),
  vec3(1.0f, 1.0f, 0.0f),
  vec3(0.0f, 1.0f, 1.0f),
  vec3(1.0f, 0.0f, 1.0f),
};


vec3 getColor(int id)
{
  if (id < 6) return vec3(1.0f, 0.0f, 0.0f);
  else if (id < 12) return vec3(0.0f, 1.0f, 0.0f);
  else if (id < 18) return vec3(0.0f, 0.0f, 1.0f);
  else if (id < 24) return vec3(1.0f, 1.0f, 0.0f);
  else if (id < 30) return vec3(0.0f, 1.0f, 1.0f);
  else if (id < 36) return vec3(1.0f, 0.0f, 1.0f);
  else return vec3(0.0f, 0.0f, 0.0f);
}


void main(void)
{
  col_frag = col_vert;
  //col_frag = colors[gl_VertexID % 6];
  //col_frag = getColor(gl_VertexID);
  gl_Position = mvp * vec4(pos_vert, 1.0f);
  //gl_Position = vec4(pos_vert, 1.0f);
}

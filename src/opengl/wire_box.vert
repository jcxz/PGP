#version 330


uniform mat4 mv;    // model-view matrix for vertices
uniform mat4 proj;  // projection matrix

uniform vec3 dimensions;   // bounding volume dimensions


vec4 genCubeVertex(uint index, vec3 extent)
{
  uint xyz = 0x2ef0298 >> ((index >> 1) + (index & (uint(1))) * 14);
  uvec3 vert = uvec3(xyz >> 2, xyz >> 1, xyz >> 0);
  vert &= (uint(1));  // isolate the bit determining the correspong coordinate
  vert <<= 1;         // multiply by 2

  return vec4((vec3(vert) - 1.0f) * extent, 1.0f);
}

void main(void)
{
  gl_Position = proj * mv * genCubeVertex(gl_VertexID, dimensions);
}

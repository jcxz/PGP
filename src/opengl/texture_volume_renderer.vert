#version 330

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tex_coords_in;

out vec3 tex_coords;
out vec3 pos_out;

uniform mat4 mvp_matrix;
uniform mat4 tex_matrix;
uniform float num_instances;
uniform float num_instances_inv;


void main(void)
{
  /* Verzia, ked sa transformacie aplikuju na proxy geometriu */

  /* od prednej steny k zadnej */
  //float t = float(num_instances - gl_InstanceID) * float(num_instances_inv);
  //gl_Position = vec4(pos, mix(-1.0f, 1.0f, t), 1.0f);
  //tex_coords = vec3(tex_coords_in, mix(0.0f, 1.0f, t));

  /* od zadnej steny k prednej */
  //float t = float(num_instances - gl_InstanceID) * float(num_instances_inv);
  //gl_Position = vec4(pos, mix(1.0f, -1.0f, t), 1.0f);
  //tex_coords = vec3(tex_coords_in, mix(1.0f, 0.0f, t));


  /* Verzia, ked sa transformacie aplikuju na texturovacie suradnice */

  /* od prednej steny k zadnej */
  //float t = float(num_instances - gl_InstanceID) * float(num_instances_inv);
  //gl_Position = mvp_matrix * vec4(pos, mix(-1.0f, 1.0f, t), 1.0f);
  //tex_coords = (tex_matrix * vec4(tex_coords_in, mix(0.0f, 1.0f, t), 1.0f)).xyz;

  /* od zadnej steny k prednej */
  float t = float(num_instances - gl_InstanceID) * float(num_instances_inv);
  gl_Position = mvp_matrix * vec4(pos, mix(1.0f, -1.0f, t), 1.0f);
  tex_coords = (tex_matrix * vec4(tex_coords_in, mix(1.0f, 0.0f, t), 1.0f)).xyz;
  pos_out = vec3(pos, mix(1.0f, -1.0f, t));
}

#version 330

layout(location = 0) in vec3 vp;
layout(location = 1) in float co;

out float colo;

void main () 
{
  gl_Position = vec4 (vp, 1.0);
  colo = co;
}

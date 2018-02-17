#version 430

layout(location = 0) in vec3 position;
layout(location = 1) uniform mat4 world;
layout(location = 2) uniform mat4 view; // of the light
layout(location = 3) uniform mat4 proj; // of the light

out vec3 vsPos;

void main(void)
{
  vec4 vsPos4 = view * world * vec4(position, 1.0);
  gl_Position = proj * vsPos4;

  vsPos = vsPos4.xyz;
}

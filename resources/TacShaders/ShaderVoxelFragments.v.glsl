// Nathan Park
#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) uniform mat4 world;
layout(location = 3) uniform mat3 worldNormal;

out VertexData {
  vec3 wsPos; // worldspace
  vec3 wsNor; // worldspace
  // vec2 uv; color is handled by a uniform right now
} Output;


void main(void)
{
	Output.wsPos = (world * vec4(position, 1)).xyz;
  // suppress warning of unused normal (used to determine projection face)
  Output.wsNor = worldNormal * normal;
	// vOut.normal = worldNormal * normal;
	// note: gl_Position handled by geometry shader
}

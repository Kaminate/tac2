#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) uniform mat4 world;
layout(location = 3) uniform mat4 invTransWorld;

out vec3 wsNormal;
out vec3 wsPos;

void main(void)
{
	wsNormal = normalize((
    invTransWorld * vec4(normalize(normal), 1.0)).xyz);
	wsPos = (world * vec4(position, 1.0)).xyz;
}

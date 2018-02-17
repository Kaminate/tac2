#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) uniform mat4 world;
layout(location = 4) uniform mat4 view;
layout(location = 5) uniform mat4 proj;

out vec3 wsNormal;
out vec4 wsPos;

void main(void)
{
	gl_Position = proj * view * world * vec4(position, 1.0);
	wsNormal = normalize((world* vec4(normalize(normal), 0.0)).xyz);
	wsPos = world * vec4(position, 1.0);
}

#version 430

layout(location = 0) in vec3 position;
layout(location = 1) uniform mat4 world;
layout(location = 2) uniform mat4 view;
layout(location = 3) uniform mat4 proj;

void main(void)
{
	gl_Position = proj * view * world * vec4(position, 1.0);
}

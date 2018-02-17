#version 430

layout(location = 0) in vec3 position; // in worldspace
layout(location = 1) in vec3 color;
layout(location = 3) uniform mat4 view;
layout(location = 4) uniform mat4 proj;

out vec3 vCol;

void main(void)
{
	gl_Position = proj * view * vec4(position, 1.0);
  vCol = color;
}

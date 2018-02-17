// Nathan Park
#version 430

layout(location = 0) in vec3 position; // worldspace
// layout(location = 1) in vec3 normal; // worldspace
layout(location = 2) in vec4 color;
layout(location = 3) uniform mat4 view;
layout(location = 4) uniform mat4 proj;

out vec4 vColor;
// out vec3 vNormal;

void main()
{
	gl_Position = proj * view * vec4(position, 1);
	vColor = color;
	// nNormal = normal;
}

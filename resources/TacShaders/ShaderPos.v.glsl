#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) uniform mat4 world;
layout(location = 3) uniform mat4 view;
layout(location = 4) uniform mat4 proj;
layout(location = 5) uniform mat4 invTransWorld;

out vec3 wsNormal;
out vec4 wsPos;

void main(void)
{
	gl_Position = proj * view * world * vec4(position, 1.0);
  // if the inverseTranspose has a scale in it (likely), 
  // we must renormalize
	wsNormal = normalize
  (
    // if they gave me non-normalized normals(unlikely), 
    // we must renormalize
    (invTransWorld * vec4(normalize(normal), 1.0)).xyz
  );
	wsPos = world * vec4(position, 1.0);
}

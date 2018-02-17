#version 430

layout(points) in;
layout(line_strip, max_vertices = 2) out; // change this if we want more

layout(location = 4) uniform mat4 view;
layout(location = 5) uniform mat4 proj;
layout(location = 6) uniform float arrowLen;
layout(location = 7) uniform float arrowDistFromSurface;

in vec3 wsPos[];
in vec3 wsNormal[];

// out vec3 something? perhaps normal so we can shade it?

void AddPoint(float dist)
{
  vec3 pos = wsPos[0] + wsNormal[0] * dist;
  gl_Position = proj * view * vec4(pos, 1.0);
  EmitVertex();
}

void main()
{
  AddPoint(arrowDistFromSurface);
  AddPoint(arrowDistFromSurface + arrowLen);

  EndPrimitive();
}

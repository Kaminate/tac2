#version 430

layout(location = 3) uniform sampler2D textureMap;
in vec2 fTexcoord;
out vec4 color;

void main()
{
  float zLightDepthNormalized = texture(textureMap, fTexcoord);
  color = vec4(zLightDepthNormalized);
}

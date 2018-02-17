#version 430

layout(location = 3) uniform sampler2D textureMap;
layout(location = 4) uniform float lightNear;
layout(location = 5) uniform float lightFar;
in vec2 fTexcoord;
out vec4 color;

void main()
{
  // maps [-n,-f] to [-1,1]
  float A = (-lightNear-lightFar) / (lightFar - lightNear);
  float B = -2 * lightNear* lightFar/ (lightFar- lightNear);   

  // nonlinear depth
  float zLightDepth = texture(textureMap, fTexcoord); // [0,1]

  float zLightNDC = zLightDepth * 2.0 - 1.0;          // [-1,1]

  // linear depth
  float zLightView = -B / (zLightNDC + A);            // [-n,-f]

  // linear depth normalized, maps [n,f] to [0,1]
  float zLightViewNormalized                          // [0,1]
    = (-zLightView - lightNear) 
    / (lightFar - lightNear);   

  color.r = zLightViewNormalized;
  color.g = zLightViewNormalized;
  color.b = zLightViewNormalized;

  // float zLightNormalized2 
    // = (zLightDepth - lightNear) 
    // / (lightFar - lightNear);
  // color.r = zLightNormalized2;
  // color.g = zLightNormalized2;
  // color.b = zLightNormalized2;

  // color.r = zLightDepth;
  // color.g = (lightNear + lightFar) / 10000;
  // color.b = zLightDepth;

}

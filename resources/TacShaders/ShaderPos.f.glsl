#version 430

layout(location = 6) uniform mat4 lightView;
layout(location = 7) uniform mat4 lightProj;
layout(location = 8) uniform vec4 color;
layout(location = 9) uniform vec3 wsLightDir;
layout(location = 10) uniform vec4 lightCol;
layout(location = 11) uniform sampler2D shadowMap;
layout(location = 13) uniform float lightNear;
layout(location = 14) uniform float lightFar;

in vec3 wsNormal;
in vec4 wsPos;

out vec4 outColor;
void main()
{
  // take the current pixel into lightspace
  vec4 posLightView = lightView * wsPos;
  vec4 posLightClip = lightProj * posLightView;
  vec3 posLightNDC = posLightClip.xyz / posLightClip.w;
  vec2 posLightUV = (posLightNDC.xy + 1.0) / 2.0;

  // maps [-n,-f] to [-1,1]
  // float A = ( -lightNear - lightFar ) / ( lightFar - lightNear );
  // float B = -2 * lightNear* lightFar / ( lightFar - lightNear );   

  float zLightDepth = texture( shadowMap, posLightUV );  // [0,1]
  float zLightView = -(zLightDepth * (lightFar - lightNear) + lightNear);

  float visibility = 1.0;
  if (-posLightView.z > -zLightView + 0.3)
    visibility = 0.5; // occluded

  // subsurface scattering
  // float vsSurfaceDepth = abs(zLightView - posLightView.z);
  // float blackDist = 3.5f;
  // vec4 black = vec4(0,0,0,1);
  // float percentage = (vsSurfaceDepth * vsSurfaceDepth) / (blackDist * blackDist);
  // vec4 diffuse = mix(color, black, percentage);


  // TEMP: NORMAL IS FLIPPED
  float nDotl = max( 0.2, dot( wsNormal, -wsLightDir ) );
  outColor = visibility * color * lightCol * nDotl;
  // outColor /= 1000.0;
  // outColor += diffuse;
}

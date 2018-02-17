// modified TekF's shader at https://www.shadertoy.com/view/Xdl3D2
#version 430

layout(location = 2) uniform float time;
layout(location = 3) uniform vec2 resolution;
layout(location = 4) uniform sampler2D noise;

in vec2 vTexcoord;

out vec4 outColor;

const float tau = 6.28318530717958647692;

#define GAMMA (2.2)

vec3 ToGamma( in vec3 col )
{
  return pow( col, vec3(1.0/GAMMA) );
}

float Noise( in ivec2 x )
{
  return texture2D(noise, (vec2(x) + 0.5) / 64., -32. ).x;
}

void main(void)
{
  vec2 ndc = vTexcoord * 2 - 1;
  float aspect = resolution.x / resolution.y;
  vec2 p = ndc;
  p.x *= aspect;

  vec3 ray = vec3(p, 1);

  float offset = time * 0.1;
  float speed = 0.7;
  vec3 col = vec3(0);
  
  vec3 stp = ray/max(abs(ray.x),abs(ray.y));
  
  vec3 pos = 2.0*stp+.5;
  for ( int i=0; i < 5; i++ )
  {
    float z = Noise(ivec2(pos.xy));
    z = fract(z-offset);
    float d = 50.0*z-pos.z;
    float w = pow(max(0.0,1.0-8.0*length(fract(pos.xy)-.5)),2.0);
    vec3 c = max(vec3(0),vec3(1.0-abs(d+speed*.5)/speed,1.0-abs(d)/speed,1.0-abs(d-speed*.5)/speed));
    col += 1.5*(1.0-z)*c*w;
    pos += stp;
  }
  
  outColor = vec4(ToGamma(col),1.0);
}

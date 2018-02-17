// Nathan Park
#version 430

layout(location = 4) uniform vec3 uvw;
out vec4 outColor;

void main()
{
	// outColor = vec4( pow(uvw, vec3(1.0 / 2.2) ), 1.0 );
  outColor = vec4( uvw, 1.0 );
}

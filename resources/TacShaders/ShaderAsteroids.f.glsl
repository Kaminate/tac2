#version 430

layout(location = 6) uniform vec4 color;

in vec3 wsNormal;
in vec4 wsPos;

out vec4 outColor;

void main()
{
	vec3 wsLightDir = normalize(vec3(1,1,-1));

	float nDotl = max(0, dot(wsNormal, -wsLightDir));

	outColor = color * nDotl;
}

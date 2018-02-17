#version 430

layout(location = 3) uniform sampler2D textureMap;
in vec2 fTexcoord;
out vec4 color;

void main()
{
	// float val = texture(textureMap, fTexcoord);
	//vec3 color = vec3(val);
	// gl_FragColor = vec4(vec3(val), 1.0);

  // gl_FragColor = vec4(texture(textureMap).xyz, 1.0);
  color = texture(textureMap, fTexcoord);
}

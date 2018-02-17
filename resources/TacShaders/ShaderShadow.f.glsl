#version 430

in vec3 vsPos;
layout(location = 4) uniform float lightNear;
layout(location = 5) uniform float lightFar;
out vec4 depth4;

// NOTE: The depth buffer opengl generates is in 
//       NONLINEAR SPACE
//
//       The color buffer this shader generates is in
//       NORMALIZED LINEAR VIEW SPACE

void main()
{
  float depth = -vsPos.z;
  float normalizedDepth = (depth - lightNear) / (lightFar - lightNear);
  depth4 = vec4(normalizedDepth, normalizedDepth, normalizedDepth, 1.0);

  // gl_FragData[0] = -vsPos.z / 30.0; should be ok
  // normalizedDepth = vec4(0.5,0,0,1.0);
  // gl_FragData[0] = abs(vsPos.z) / 30.0f; // todo: replace w/ above
  // GLFRAGDATA is DEPRECATED

	// uhh.. don't need to output anything
	// because its written to depth buffer automatically

	// will be dropped automatically, but may need to be here
	// gl_FragColor = vec4(1.0);
}

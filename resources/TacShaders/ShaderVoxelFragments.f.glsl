#version 430

// todo: fix everything that references this struct
struct FragmentData 
{
  vec3 fragPos; // worldspace
  uint padding;
  vec4 fragColor; // must be aligned on 
};

layout(std140, binding = 0) buffer Fragments
{
  FragmentData fragments[];
  // i would like to put the fragmentCounter in here too
  // but i cant fiugre out how
};

// what is offset?
layout(binding = 1, offset = 0) uniform atomic_uint fragmentCounter;
layout(location = 6) uniform vec4 color;
layout(location = 7) uniform uint maxFrags;

// todo: fix the fuck outta my locations
// (since my shader.geom and shader.frag both use view, proj,
// (they need the same location)
// layout(location = 0) uniform mat4 view;
// layout(location = 1) uniform mat4 proj;

in VoxelData {
    vec3 wsPos;
    // vec3 normal;
    // vec2 uv;
} In;



out vec4 outColor; // temp
void main()
{
	uint priorFragmentNumber = atomicCounterIncrement(fragmentCounter);
	if (priorFragmentNumber < maxFrags)
	{
		// vec4 pos = proj * view * vec4(wsPos, 1.0);
		// pos /= pos.w;

		fragments[priorFragmentNumber].fragPos = In.wsPos;
		// fragments[priorFragmentNumber].fragPos = ivec2(gl_FragCoord.xy);
		fragments[priorFragmentNumber].fragColor = color;
		fragments[priorFragmentNumber].padding = 0; // temp
	}

	outColor = color;
	// if (maxFrags == 10000)
	// outColor.r = 1.0;
	// outColor.r += fragmentCounter;// * 0.00000001;
}

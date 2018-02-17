// Nathan Park
#version 430

layout(triangles) in;
// can output only points, line_strip, or triangle_strip
layout(triangle_strip, max_vertices = 3) out;

in VertexData
{
	vec3 wsPos;
	vec3 wsNor;
	// vec2 tex;
} Input[3];

out VoxelData
{
	///////////////////////////////////
	// todo // replace with TexSpace //
	///////////////////////////////////
	//
	// Reason: 
	//   We want to do the heavy work in the shader with less things
	//   (less threads in geom shader than frag shader)
	//   So ideally, we would compute the entire voxel index in the 
	//   geom shader. However, we can't interpolate unsigned pos
	//   (or it wouldnt make sense if we could), so the BEST WE CAN DO
	//   is compute the voxel space position here, and compute the 
	//   index in the fragment shader.
	//   
	//   NOW WE HAVE 2 CHOICES:
	// 
	//     1. Save position in TexSpace [0,1][0,1][0,1]
	//     2. Save position in VoxSpace [0,1024][0,1024][0,1024]
	//
	//   I assume that TexSpace is better, because precision of float
	//   is greater near the center, allowing for better interpolation,
	//   but honestly idk
	//   <-- addendum: We're calling this struct VoxelData.
	//    (not only because dominik does it, but also because it makes
	//		sense) With the name in mind, it only makes sense to send the
  //    pos in TexSpace; The frag shader shouldnt have to know how to 
  //    convert ws into TexSpace, only how to convert TexSpace into 
  //    indexes.
	//   
	//   ON THE BRIGHT SIDE
	//     I don't have to worry about that, because we're just trying to 
	//     get this to work, so wsPos it is!
	// 
	///////////////////////////////////////////////////////////
	//  tl;dr: position should be in TexSpace[0,1][0,1][0,1] //
	///////////////////////////////////////////////////////////
	vec3 wsPos;
	// vec3 wsNor;
	// vec2 tex;
} Output;

layout(location = 4) uniform mat4 view;
layout(location = 5) uniform mat4 proj;
uniform mat4 viewProjs[3]; // who gives a shit about the layout?

int GetProjectedAxis(vec3 normal)
{
	// x
  int axisIndex = 0;
  float greatestDot = abs(dot(vec3(1,0,0), normal));

  // y
  float axisDot = abs(dot(vec3(0,1,0), normal));
  if(axisDot > greatestDot)
  {
  	greatestDot = axisDot;
  	axisIndex = 1;
  }

  // z
  axisDot = abs(dot(vec3(0,0,1), normal));
  if(axisDot > greatestDot)
  {
  	greatestDot = axisDot;
  	axisIndex = 2;
  }
  return axisIndex;
}

void UseMat4(mat4 mat)
{
	if(mat[0][0] == 123.456)
	{
		gl_Position.x = 123.456;
	}
}
void main()
{
	/////////////////////////////////////////
	// todo: // reference: dominik lazarek //
	/////////////////////////////////////////
	//
	// - store voxel grid space points to interpolate in the frag shader
	// - project the points onto the plane
	// - but the vertex we are emitting is done in 2 steps
	//   - project the points on the dominant axis
	//   - ONCE PROJECTED, add 1/2 of a voxel width to the points
	//   - remember that when we emit a point, gl_Position is in 
	//     clipspace of the dominant axis. 
  //
  //   For which space Output.pos should be in, refer to the comments in 
  //   



	for (uint i = 0; i < gl_in.length(); ++i)
	{
		int dominantAxis = GetProjectedAxis( Input[ i ].wsNor );
		gl_Position = viewProjs[ dominantAxis ] * vec4(Input[ i ].wsPos, 1.0);

		// todo 
		UseMat4(view);
		UseMat4(proj);


		// gl_Position = proj * view * vec4(Input[i].wsPos, 1.0);

		// UseMat4(viewProjs[0]);

		Output.wsPos = Input[i].wsPos;
		EmitVertex();
	}
	EndPrimitive();
}

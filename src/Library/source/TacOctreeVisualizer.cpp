
#include "TacOctreeVisualizer.h"
#include <typeinfo>
#include "TacResourceManager.h"
#include "TacShaderResource.h"

namespace Tac
{
  //////////////////////
  // OctreeVis::Scope //
  //////////////////////
  OctreeVis::Scope::Scope(
    const Vector3& octreePos, 
    const Vector3& octreeHalfDim,
    const Params& params ,
    const Matrix4& viewMatrix,
    const Matrix4& projMatrix,
    const Geometry& cubeOfHalfwidth1)
    : mOctreePos(octreePos) 
    , mOctreeHalfDim(octreeHalfDim)
    , mParams( params )
    , mCubeOfHalfwidth1( cubeOfHalfwidth1 )
  {
    const Shader & s = mParams.GetShader();
    s.Activate();

    mCubeOfHalfwidth1.SendAttribute(Attribute::ePos, mParams.mPos);
    glUniformMatrix4fv(mParams.mView, 1, GL_TRUE, viewMatrix.v);
    glUniformMatrix4fv(mParams.mProj, 1, GL_TRUE, projMatrix.v);

  };

  OctreeVis::Scope::~Scope()
  {
    const Shader & s = mParams.GetShader();
    s.Deactivate();
  }

  void OctreeVis::Scope::RenderNode(
    const Vector3 & nodePos,
    const Vector3 & nodeHalfDim)
  {
    Matrix4 world(
      nodeHalfDim.x,0,0,nodePos.x,
      0,nodeHalfDim.y,0,nodePos.y,
      0,0,nodeHalfDim.z,nodePos.z,
      0,0,0,1);
    glUniformMatrix4fv(mParams.mWorl, 1, GL_TRUE, world.v);

    // position relative to octree
    Vector3 mini = mOctreePos - mOctreeHalfDim;
    Vector3 maxi = mOctreePos + mOctreeHalfDim;
    Vector3 n = nodePos - mOctreePos + mOctreeHalfDim; // numerator
    Vector3 d = mOctreeHalfDim * 2.0f; // denominator
    glUniform3f( mParams.mUVW, n.x / d.x, n.y / d.y, n.z / d.z );

    mCubeOfHalfwidth1.Draw();
  }

  ///////////////////////
  // OctreeVis::Params //
  ///////////////////////
  void OctreeVis::Params::OnReload()
  {
    const Shader & s = GetShader();

    mPos = s.GetAttrib("position");

    mWorl = s.GetUnifor("world");
    mView = s.GetUnifor("view");
    mProj = s.GetUnifor("proj");
    mUVW = s.GetUnifor("uvw");
  }

  ///////////////
  // OctreeVis //
  ///////////////
  OctreeVis::OctreeVis(
    const Vector3& octreePos, const Vector3& octreeHalfDim )
    : mOctreePos(octreePos)
    , mOctreeHalfDim(octreeHalfDim)
    , mCubeOfHalfwidth1( 
    
    // uhh dont look at this
    std::shared_ptr<GeometryData>(&((new GeometryData())->LoadCube(1)))
    
    //
    //GeometryData().LoadCube(1)
    //
    
    
    )
  {

  }

  void OctreeVis::BindToShader( class ShaderResource * resource )
  {
    mShaderParams.BindToResource(resource);
  }

  OctreeVis::Scope OctreeVis::OpenScope(
    const Matrix4 & view, 
    const Matrix4& proj ) const
  {
    return Scope(
      mOctreePos,
      mOctreeHalfDim,
      mShaderParams, view, proj, mCubeOfHalfwidth1);
  }

} // Tac


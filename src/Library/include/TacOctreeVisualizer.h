#ifndef __TAC_OCTREE_VISUALIZER_H_
#define __TAC_OCTREE_VISUALIZER_H_

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#include "TacVector3.h"
#include "TacMatrix4.h"
#include "TacShaderView.h"
#include "TacGeometry.h"

#define octreevisualizer_YELLx

#ifdef octreevisualizer_YELL
#include "TacYeller.h" //temp
#endif

namespace Tac
{

  class OctreeVis
  {
  public:
    class Params : public ShaderView
    {
    public:
      virtual void OnReload() override;
      // attribs
      GLuint mPos;
      // uniforms
      GLuint mWorl;
      GLuint mView;
      GLuint mProj;
      GLuint mUVW;
    };

    class Scope // scoped
#ifdef octreevisualizer_YELL
      : public Yeller<Scope>
#endif
    {
    public:
      // not defined
      TacDLLAPI Scope& operator = (const Scope& rhs);
      TacDLLAPI ~Scope();
      TacDLLAPI void RenderNode(
        const Vector3 & nodePos, 
        const Vector3 & nodeHalfWidth);

    private:
      friend class OctreeVis;
      Scope(
        const Vector3& octreePos, 
        const Vector3& octreeHalfDim,
        const Params& shaderParams,
        const Matrix4& viewMatrix,
        const Matrix4& projMatrix,
        const Geometry& cubeOfHalfwidth1);

      const Vector3& mOctreePos;
      const Vector3& mOctreeHalfDim;
      const Params& mParams;
      const Geometry& mCubeOfHalfwidth1;
    };

    TacDLLAPI OctreeVis(
      const Vector3& octreePos,
      const Vector3& octreeHalfDim);
    // fk it. this is an init function
    TacDLLAPI void BindToShader(class ShaderResource * resource);
    TacDLLAPI Scope OpenScope(
      const Matrix4 & view,
      const Matrix4& proj) const;

  private:
    Params mShaderParams;
    Geometry mCubeOfHalfwidth1;
    Vector3 mOctreePos;
    Vector3 mOctreeHalfDim;
  };

} // Tac

#endif

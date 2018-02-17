/*
\author Nathan Park
*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once
#include "TacVector3.h"
#include "TacVector4.h"
#include "TacMatrix4.h"

namespace Tac
{
  class Frustrum
  {
  public:
    enum Planes
    {
      eTop, eBottom, eLeft, eRight, eNear, eFar, eCount
    };
  private:
    // ax + by + cz + d = 0
    // normal faces outwards
    Vector4 mPlanes[Planes::eCount]; 
  };

  class Camera
  {
    public:
      TacDLLAPI Camera();

      TacDLLAPI Matrix4 GetViewMatrix()const;
      TacDLLAPI Matrix4 GetInverseViewMatrix()const;
      TacDLLAPI Matrix4 GetPerspectiveProjMatrix()const;
      TacDLLAPI Matrix4 GetInversePerspectiveProjMatrix()const;
      TacDLLAPI Matrix4 GetOrthographicProjMatrix()const;

      TacDLLAPI void GetCamDirections(Vector3 & right, Vector3 & up) const;

      float mNear;
      float mFar;
      float mFieldOfViewYRad;
      float mAspectRatio;
      float mWidth;  // for orthographic proj
      float mHeight; // for orthographic proj
    
      // pulled from the transformcomponent during graphics update
      Vector3 mPosition;
      Vector3 mWorldSpaceUp;
      Vector3 mViewDirection;
  };
}

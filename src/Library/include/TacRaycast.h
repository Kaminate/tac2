#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif


#pragma once

#include "TacVector3.h"

namespace Tac
{

  // Find out if your ray is inside the sphere before calling this fn
  TacDLLAPI bool RaycastSphere(
    const Vector3 & spherePos, 
    float sphereRadius, 
    const Vector3 & rayPos,
    const Vector3 & normalizedRayDir,
    float & dist);

  // ignores the triangle normal
  TacDLLAPI bool RaycastTriangle(
    const Vector3& p0,
    const Vector3& p1,
    const Vector3& p2,
    const Vector3 & rayPos,
    const Vector3 & normalizedRayDir,
    float & dist);

  // returns true on success
  TacDLLAPI bool UnitTest_RaycastTriangle();

}

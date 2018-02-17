/*!
\author Nathan Park
*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once

#include "TacVector3.h"

namespace Tac
{
  class GeometryData;
  class BoundingSphere
  {
  public:
    TacDLLAPI BoundingSphere();
    TacDLLAPI BoundingSphere(float rad, Vector3 center);
    TacDLLAPI bool Load(const GeometryData & data);
    TacDLLAPI void AddSphere (const BoundingSphere & rhs);
    TacDLLAPI void AddPoint (const Vector3 & rhs);

    float mRadius;
    Vector3 mPos;
  };

} // Tac


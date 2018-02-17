#include "TacRaycast.h"

namespace Tac
{
  bool RaycastSphere(
    const Vector3 & spherePos, 
    float sphereRadius, 
    const Vector3 & rayPos,
    const Vector3 & rayDir,
    float & dist)
  {
    // [xyz] = rayPos + t * rayDir

    // (x-xC)^2 + (y-yC)^2 + (z - zC)^2 = r^2
    
    Vector3 circleToRay = rayPos - spherePos;
    //float a = 1;
    float b = 2 * rayDir.Dot(circleToRay);
    float c = circleToRay.Dot(circleToRay) - sphereRadius * sphereRadius;
    // d = (-b +/- sqrt(b^2 - 4 a c) ) / 2a
    float discriminant =  b * b - 4 /*a*/ * c;
    if (discriminant < 0)
      return false;
    else
    {
      float sqrtDiscriminant = sqrt(discriminant);
      float closerDist = (-b - sqrtDiscriminant) / (2 /* a*/);
      
      if (closerDist < 0)
        return false;

      dist = closerDist;
      return true;
    }
  }

  bool RaycastTriangle( 
    const Vector3& p0, 
    const Vector3& p1, 
    const Vector3& p2, 
    const Vector3 & rayPos, 
    const Vector3 & normalizedRayDir, 
    float & dist )
  {
    Vector3 v2 = p2 - p0;
    Vector3 v1 = p1 - p0;
    Vector3 b = rayPos - p0;
    Vector3 p = normalizedRayDir.Cross(v2);
    Vector3 q = b.Cross(v1);
    float pdotv1 = p.Dot(v1);

    float t = q.Dot(v2) / pdotv1;
    float u = p.Dot(b) / pdotv1;
    float v = q.Dot(normalizedRayDir) / pdotv1;

    if (t > 0 && u >= 0 && v >= 0 && u + v <= 1)
    {
      dist = t;
      return true;
    }
    return false;
  }

  bool UnitTest_RaycastTriangle()
  {
    bool success = true;

    Vector3 p0(0,0,0);
    Vector3 p1(10,0,0);
    Vector3 p2(0,10,0);

    Vector3 center = (p0 + p1 + p2) / 3;

    Vector3 origin(1,1,10);
    Vector3 dir(0,0,-1);

    float dist;

    success &= RaycastTriangle(p0,p1,p2,origin,dir,dist);
    if (dist != 10)
      success = false;


    return success;

  }



}

#include "TacBoundingSphere.h"
#include "TacGeometryData.h"
#include "TacErrorIf.h"
#include <algorithm> // std::max

namespace Tac
{
  TacDLLAPI BoundingSphere::BoundingSphere()
    : mPos(0,0,0)
    , mRadius(0)
  {

  }

  TacDLLAPI BoundingSphere::BoundingSphere( float rad, Vector3 center )
    : mPos(center), mRadius(rad)
  {

  }

  void BoundingSphere::AddSphere(const BoundingSphere & rhs )
  {
    // get the axis 
    Vector3 toRhs = rhs.mPos - mPos;
    float distToRhs = toRhs.Length();

    if (distToRhs < abs(mRadius - rhs.mRadius))
    {
      // one circle is inside another
      // this case also handles the divide by 0 during normalize
      if (rhs.mRadius > mRadius)
      {
        *this = rhs;
        return;
      }
    }

    Vector3 axis = toRhs;
    axis.Normalize();

    Vector3 point1 = mPos - axis * mRadius;

    float radius2 = std::max(mRadius, rhs.mRadius + distToRhs);
    Vector3 point2 = mPos + axis * radius2;
    
    mPos = (point1 + point2) / 2.0f;
    mRadius = point1.Dist(point2) / 2.0f;

    return;
  }

  void BoundingSphere::AddPoint( const Vector3 & rhs )
  {
    //float dist = mPos.Dist(rhs);
    //float distFromRing = dist - mRadius;
    //if (distFromRing > 0)
    Vector3 topoint = rhs - mPos;
    float distSq = topoint.LengthSq();
    if(distSq > mRadius * mRadius)
    {
      float newRadius = (sqrt(distSq) + mRadius) / 2.0f;
      topoint.Normalize();
      topoint *= newRadius - mRadius;
      mPos += topoint;
      mRadius = newRadius;
      
      
      // they are outside
      //Vector3 toNewPoint = rhs - mPos;

      //float halfWay = distFromRing / 2.0f;
      //// move half the way towards them
      //toNewPoint.Normalize();
      //toNewPoint *= halfWay;
      //mPos += toNewPoint;

      //// expand our radius by that much on each side
      //mRadius += halfWay;
    }
    else
    {
      // do nothing, we already bound them
    }
  }

  TacDLLAPI bool BoundingSphere::Load( const GeometryData & data )
  {
    unsigned numVerts = data.mCounts[Attribute::ePos];
    if (numVerts == 0)
      return false;
    mPos = data.mPos[0];
    mRadius = 0;
    for (unsigned i = 1; i < numVerts; ++i)
    {
      Vector3 & iVert = data.mPos[i];
      AddPoint(iVert);
    }

    return true;
  }

} // Tac
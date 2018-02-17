/*!
\author Nathan Park
*/
#include "TacCamera.h"
#include "TacErrorIf.h"
#include "TacMathUtil.h"

namespace Tac
{
  Camera::Camera()
    : mNear(0.1f)
    , mFar(1000.0f)
    , mFieldOfViewYRad(ToRadians(90))
    , mViewDirection(0,1,0)
    , mAspectRatio(4/3.0f)
    , mWorldSpaceUp(0,0,1)
    , mPosition(0,0,0)
  {
  }

  Matrix4 Camera::GetViewMatrix()const
  {
    Vector3 camR(0,0,0);
    Vector3 camU(0,0,0);
    Vector3 negZ = -mViewDirection;
    GetCamDirections(camR, camU);

    Matrix4 worldToCamRot(
      camR.x, camR.y, camR.z, 0,
      camU.x, camU.y, camU.z, 0,
      negZ.x, negZ.y, negZ.z, 0,
      0,0,0,1);

    Matrix4 worldToCamTra(
      1,0,0, -mPosition.x,
      0,1,0, -mPosition.y,
      0,0,1, -mPosition.z,
      0,0,0,1);

    return worldToCamRot * worldToCamTra;
  }

  Matrix4 Camera::GetPerspectiveProjMatrix()const
  {
    //                                       /----- [x,y,z]
    //                          [x'y'z']/----
    //                          /----|
    //                    /----/     |
    //               /---/           | projHeight / 2
    //           /--/   theta        |
    //    camera/--------------------|
    //                     d

    // map the x from (-1, 1) by divinding by projWidth

    float theta = mFieldOfViewYRad / 2.0f;

    // note:           
    // we ignored sX, sY, A, B for simplicity
    // P' (PROJ)  PROJ                         P( CAM )
    // [-dx/z] = [-dx/z] / w = [xd] = [d 0 0 0][x]
    // [-dy/z] = [-dy/z] / w = [yd] = [0 d 0 0][y]
    // [-d]    = [-dz/z] / w = [zd] = [0 0 d 0][Z]
    // [1]     = [1]     / w = [-z] = [0 0-1 0][1]

    // I believe that projection is 4D, 
    // Once you divide by w, you get NDC (3D).    

    float cotTheta = 1.0f / tan(theta);
    // sX = d / (pW / 2) = cot(theta) / aspectRatio
    float sX  =  cotTheta / mAspectRatio; // maps x to -1, 1

    // sY = d / (pH / 2)
    float sY = cotTheta; // maps y to -1, 1

    float A = (-mNear -mFar) / (mFar - mNear);    // maps -n,-f to -1,1
    float B = -2 * mNear * mFar / (mFar - mNear); // maps -n,-f to -1,1

    Matrix4 perspective (
      sX,0,0,0,  
      0,sY,0,0,  
      0,0,A,B,   
      0,0,-1,0); 

    return perspective;
  }

  Tac::Matrix4 Camera::GetInverseViewMatrix()const
  {
    Vector3 camR(0,0,0);
    Vector3 camU(0,0,0);
    Vector3 negZ = -mViewDirection;
    GetCamDirections(camR, camU);

    Matrix4 camToWorRot(
      camR.x, camU.x, negZ.x, 0,
      camR.y, camU.y, negZ.y, 0,
      camR.z, camU.z, negZ.z, 0,
      0,0,0,1);

    Matrix4 camToWorTra(
      1,0,0, mPosition.x,
      0,1,0, mPosition.y,
      0,0,1, mPosition.z,
      0,0,0,1);

    return camToWorTra * camToWorRot;
  }

  void Camera::GetCamDirections( Vector3 & right, Vector3 & up ) const
  {
    if (mViewDirection.IsAlignedWith(mWorldSpaceUp))
    {
      Vector3 arbitrayVector(1,0,0);
      if (mViewDirection.IsAlignedWith(arbitrayVector))
        arbitrayVector = Vector3(0,1,0);

      right = mViewDirection.Cross(arbitrayVector);
    }
    else
      right = mViewDirection.Cross(mWorldSpaceUp);

    right.Normalize(); // oops. this is important
    up = right.Cross(mViewDirection);
  }

  Tac::Matrix4 Camera::GetOrthographicProjMatrix()const
  {
    float a = -2/(mFar - mNear);
    float b = -(mNear + mFar) / (mFar - mNear);

    /*
     ___________
    |           |^
    |           ||
    |     +     |h
    |           ||
    |___________|v
    <-----w----->

    x: aligns -w/2 to -1, w/2 to 1
    y: aligns -h/2 to -1, h/2 to 1
    z: aligns -near to -1, -far to 1

    */

    return Matrix4(
      2/mWidth, 0, 0, 0,
      0, 2/mHeight, 0, 0,
      0, 0, a, b,
      0, 0, 0, 1);
  }



  // http://allenchou.net/2014/02/game-math-how-to-eyeball-the-inverse-of-a-matrix/
  Matrix4 Camera::GetInversePerspectiveProjMatrix() const
  {
    float theta = mFieldOfViewYRad / 2.0f;
    float cotTheta = 1.0f / tan(theta);
    float sX  =  cotTheta / mAspectRatio; // maps x to -1, 1
    float sY = cotTheta; // maps y to -1, 1
    float A = (-mNear -mFar) / (mFar - mNear);    // maps -n,-f to -1,1
    float B = -2 * mNear * mFar / (mFar - mNear); // maps -n,-f to -1,1

    Matrix4 invPerspective (
      1/sX,0,0,0,
      0,1/sY,0,0,
      0,0,0,-1,
      0,0,1/B,A/B);

    return invPerspective;
  }

} // Tac

/*
Nathan Park
*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once
#include <iostream> // output operator
#include "TacVector4.h"

namespace Tac
{
  // this matrix can be thought of as an array of 4 row vectors.
  // Since the default OpenGL matrix is column major, when you pass the matrix
  // ie: using glUniformMatrix4f(), pass GL_TRUE to the transpose argument
  struct Vector3;
  struct Matrix4
  {
    union
    {
      struct  
      {
        float 
          m00, m01, m02, m03,
          m10, m11, m12, m13,
          m20, m21, m22, m23,
          m30, m31, m32, m33;
      };

      float m[4][4];
      float v[16];
    };
    TacDLLAPI Matrix4(); // identity
    TacDLLAPI Matrix4(
      float mm00, float mm01, float mm02, float mm03,
      float mm10, float mm11, float mm12, float mm13,
      float mm20, float mm21, float mm22, float mm23,
      float mm30, float mm31, float mm32, float mm33);

    TacDLLAPI Vector4 operator*(const Vector4& rhs) const;

    TacDLLAPI Matrix4 operator+(const Matrix4& rhs) const;
    TacDLLAPI Matrix4 operator*(const Matrix4& rhs) const;
    TacDLLAPI Matrix4 operator*(float val) const;

    TacDLLAPI Matrix4& operator*=(const Matrix4& rhs);
    TacDLLAPI Matrix4& operator*=(float val);

    TacDLLAPI float * operator [](unsigned row);
    TacDLLAPI const float * operator [] (unsigned row)const;

    TacDLLAPI void Zero(void);
    TacDLLAPI void Identity(void);
    TacDLLAPI void Transpose(void);
    TacDLLAPI static Matrix4 Transform(
      const Vector3 & scale,
      const Vector3 & rotate,
      const Vector3 & translate);
    TacDLLAPI static Matrix4 TransformInv(
      const Vector3 & nonInversedScale,
      const Vector3 & nonInversedRotate,
      const Vector3 & nonInversedTranslate);

    //Tensor Product
    TacDLLAPI static Matrix4 Tensor(const Vector4& vec);

    //Tilda over n
    TacDLLAPI static Matrix4 Tilda(const Vector4& vec);

    TacDLLAPI void Invert();
    TacDLLAPI void InvertAffine();

    // returns the inverse of the projection matrix
    TacDLLAPI static Matrix4 InvertProjectionMatrix(const Matrix4 & projMatrix);

    //Rotates a vector to an angle
    TacDLLAPI static Matrix4 VectorAlignment(
      const Vector4& vector,
      float rads);
  };

  TacDLLAPI std::ostream & operator << (std::ostream & lhs, const Matrix4 & mat);
}


/*
Nathan Park
*/

#pragma once
#pragma warning ( disable : 4201 ) // nameless struct/union

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#include <iostream> // output operator
#include "TacVector3.h"
namespace Tac
{
  // this matrix can be thought of as an array of 3 row vectors.
  // Since the default OpenGL matrix is column major, when you pass the matrix
  // ie: using glUniformMatrix3f(), pass GL_TRUE to the transpose argument

  struct Matrix3
  {
    union
    {
      struct  
      {
        float 
          m00, m01, m02,
          m10, m11, m12,
          m20, m21, m22;
      };
      float v[9];
      float m[3][3];
    };
    // identity
    TacDLLAPI Matrix3(); // identity
    TacDLLAPI Matrix3(
      float mm00, float mm01 , float mm02 ,
      float mm10, float mm11 , float mm12 ,
      float mm20, float mm21 , float mm22 );
    TacDLLAPI Matrix3 operator+(const Matrix3 & rhs) const;
    TacDLLAPI Vector3 operator*(const Vector3& rhs) const;
    TacDLLAPI Matrix3 operator*(const Matrix3& rhs) const;
    TacDLLAPI Matrix3 operator*( float val ) const;
    TacDLLAPI Matrix3& operator*=(const Matrix3& rhs);

    TacDLLAPI void Zero(void);
    TacDLLAPI void Identity(void);
    TacDLLAPI void Transpose(void);
    TacDLLAPI void Invert();

    // returns a row vector[3]
    TacDLLAPI float * operator [](unsigned row);
    TacDLLAPI const float * operator [](unsigned row) const;

    TacDLLAPI static Matrix3 RotRadX(const float rotRad);
    TacDLLAPI static Matrix3 RotRadY(const float rotRad);
    TacDLLAPI static Matrix3 RotRadZ(const float rotRad);
    TacDLLAPI static Matrix3 FromColVectors(
      const Vector3 & v0,
      const Vector3 & v1,
      const Vector3 & v2);

    //Tensor Product (cross product matrix)
    //   vec3 a, b;
    //   Cross(a ,b) == Tensor(a) * b
    TacDLLAPI static Matrix3 Tensor(const Vector3& vec);
    //Tilda over n
    TacDLLAPI static Matrix3 Tilda(const Vector3& vec);
    //Rotates a vector to an angle
    TacDLLAPI static Matrix3 VectorAlignment(
      const Vector3& vector,
      float rad);
  };

  TacDLLAPI std::ostream & operator << (std::ostream & lhs, const Matrix3 & mat);
}


/*
Nathan Park
*/

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once
#pragma warning ( disable : 4201 ) // nameless struct/union

#include <iostream> // ostream
#include "TacVector3.h"

namespace Tac
{
  struct Vector4
  {
    float x, y, z, w;
    TacDLLAPI Vector4(); // does not initialize
    TacDLLAPI Vector4(float xx, float yy, float zz, float ww);
    TacDLLAPI Vector4(const Vector3 & vec, float ww );
    TacDLLAPI Vector4 operator-(void) const;  
    TacDLLAPI Vector4 operator+(const Vector4& rhs) const;
    TacDLLAPI Vector4 operator-(const Vector4& rhs) const;
    TacDLLAPI Vector4 operator*(const float rhs) const;
    TacDLLAPI Vector4 operator/(const float rhs) const;
    TacDLLAPI Vector4& operator+=(const Vector4& rhs);
    TacDLLAPI Vector4& operator-=(const Vector4& rhs);
    TacDLLAPI Vector4& operator*=(const float rhs);
    TacDLLAPI Vector4& operator/=(const float rhs);
    TacDLLAPI float & operator[](unsigned index);
    TacDLLAPI const float & operator [] (unsigned index) const;
    TacDLLAPI bool operator==(const Vector4& rhs) const;
    TacDLLAPI bool operator!=(const Vector4& rhs) const;
    TacDLLAPI void Zero(void);
    TacDLLAPI Vector3 XYZ();
  };


  TacDLLAPI std::ostream & operator << (std::ostream & lhs, const Vector4 & vec);
  TacDLLAPI std::istream & operator >> (std::istream & lhs, Vector4 & vec);

}

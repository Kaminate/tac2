/*
Nathan Park
*/

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once

#include <iostream> // istream, ostream

namespace Tac
{
  struct Vector2;
  struct Vector3
  {
    float x, y, z;

    // default ctor, does not initialze data
    TacDLLAPI Vector3(); 
    TacDLLAPI Vector3(float xx, float yy, float zz);
    TacDLLAPI Vector3(const Vector2 & vec, float zval);
    TacDLLAPI Vector3(float xx, const Vector2 & yz);
    TacDLLAPI Vector3 operator-(void) const;  
    TacDLLAPI Vector3 operator+(const Vector3& rhs) const;
    TacDLLAPI Vector3 operator-(const Vector3& rhs) const;
    TacDLLAPI Vector3 operator*(const float rhs) const;
    TacDLLAPI Vector3 operator/(const float rhs) const;
    TacDLLAPI Vector3& operator+=(const Vector3& rhs);
    TacDLLAPI Vector3& operator-=(const Vector3& rhs);
    TacDLLAPI Vector3& operator*=(const float rhs);
    TacDLLAPI Vector3& operator/=(const float rhs);
    TacDLLAPI bool operator==(const Vector3& rhs) const;
    TacDLLAPI bool operator!=(const Vector3& rhs) const;
    TacDLLAPI float & operator[](unsigned index);
    TacDLLAPI const float & operator[](unsigned index) const;
    TacDLLAPI float Dot(const Vector3& rhs) const;
    TacDLLAPI Vector3 Cross(const Vector3& rhs) const;
    TacDLLAPI float Length(void) const;
    TacDLLAPI float LengthSq(void) const;
    TacDLLAPI float Dist(const Vector3 & rhs) const;
    TacDLLAPI float DistSq(const Vector3 & rhs) const;
    TacDLLAPI void Normalize(void);
    TacDLLAPI void Zero(void);
    TacDLLAPI bool IsAlignedWith(const Vector3 & rhs) const;
  };
  TacDLLAPI std::ostream & operator << (std::ostream & lhs, const Vector3 & vec);
  TacDLLAPI std::istream & operator >> (std::istream & lhs, Vector3 & vec);
  TacDLLAPI Vector3 ComponentwiseMult(const Vector3 & lhs, const Vector3 & rhs);
} //Tac

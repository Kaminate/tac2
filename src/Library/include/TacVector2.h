// Nathan Park

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once
#include <iostream> // ostream, istream

namespace Tac
{
  struct Vector2
  {
    float x;
    float y;

    TacDLLAPI Vector2(float xx = 0.0f, float yy = 0.0f);
    TacDLLAPI Vector2 operator-(void) const;  
    TacDLLAPI Vector2 operator+(const Vector2& rhs) const;
    TacDLLAPI Vector2 operator-(const Vector2& rhs) const;
    TacDLLAPI Vector2 operator*(const float rhs) const;
    TacDLLAPI Vector2 operator/(const float rhs) const;
    TacDLLAPI Vector2& operator+=(const Vector2& rhs);
    TacDLLAPI Vector2& operator-=(const Vector2& rhs);
    TacDLLAPI Vector2& operator*=(const float rhs);
    TacDLLAPI Vector2& operator/=(const float rhs);
    TacDLLAPI bool operator==(const Vector2& rhs) const;
    TacDLLAPI bool operator!=(const Vector2& rhs) const;
    TacDLLAPI float Dot(const Vector2& rhs) const;
    TacDLLAPI float Dist(const Vector2 & rhs) const;
    TacDLLAPI float DistSq(const Vector2 & rhs) const;
    TacDLLAPI float Length(void) const;
    TacDLLAPI float LengthSq(void) const;
    TacDLLAPI void Normalize(void);
    TacDLLAPI void Zero(void);
    TacDLLAPI float AngleRad(const Vector2 & rhs) const;
  };
  TacDLLAPI std::ostream & operator << (std::ostream & is, const Vector2 & vec);
  TacDLLAPI std::istream & operator >> (std::istream & lhs, Vector2 & vec);
}


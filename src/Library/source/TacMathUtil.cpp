// Nathan Park
#include "TacMathUtil.h"
#include <cmath> // overload of abs(float)
#include <stdlib.h>

namespace Tac
{
  bool IsAboutZero(float f) 
  { 
    return abs(f) < EPSILON; 
  }
  bool IsAboutOne(float f) 
  {
    return abs(f - 1.0f) < EPSILON;
  }
  float ToRadians(float deg)
  {
    return deg * 0.0174532925f;
  }
  float ToDegrees(float rad)
  {
    return rad * 57.2957795f;
  }
  int RandInt(int mini, int maxi)
  {
    return mini + rand() % (maxi - mini);
  }
  float RandFloat(float mini, float maxi)
  {
    float percentage = rand() / (float)RAND_MAX;
    return mini + percentage * (maxi - mini);
  }
  
}


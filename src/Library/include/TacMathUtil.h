// Nathan Park
#pragma once

#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

namespace Tac
{
	const float EPSILON = 0.001f;
  const float PI = 3.14159f;
  const float TWO_PI = 6.283185307f;

  template<typename T>
  void Clamp( T & val, const T & mini, const T & maxi )
  {
    if (val < mini) 
      val = mini;
    else if (val > maxi )
      val = maxi;
  }

  TacDLLAPI bool IsAboutZero(float f);
  TacDLLAPI bool IsAboutOne(float f);
  TacDLLAPI float ToRadians(float deg);
  TacDLLAPI float ToDegrees(float rad);
  TacDLLAPI float RandFloat(float mini = 0, float maxi = 1);
  TacDLLAPI int RandInt(int mini, int maxi);

  template <typename T>
  T Lerp(const T & begin, const T & end, float percent)
  {
    return begin * (1 - percent) + end * percent;
  }



}


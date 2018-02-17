/*!
\author Nathan Park
*/
#pragma once

#include "TacVector3.h"
namespace Tac
{

  // hue (0,2PI)
  // saturation (0,1)
  // value (0,1)
  // http://www.cs.rit.edu/~ncs/color/t_convert.html
  void ToHSV(
    float r, 
    float g, 
    float b,
    float & h,
    float & s,
    float & v);
  void ToRGB(
    float h, 
    float s, 
    float v,
    float & r,
    float & g, 
    float & b);

  Vector3 ToHSV(const Vector3 & rgb);
  Vector3 ToRGB(const Vector3 & hsv);

  
} // Tac


/*!
\author Nathan Park
*/
#include "TacHSV.h"
#include "TacMathUtil.h"
#include <algorithm>
namespace Tac
{

  // haven't tested this function at all
  // http://www.cs.rit.edu/~ncs/color/t_convert.html
  void ToHSV(
    float r, 
    float g, 
    float b,
    float & h,
    float & s,
    float & v)
  {
    float mini = std::min(r, std::min(g, b));
    float maxi = std::max(r, std::max(g, b));
    v = maxi;
    float delta = maxi - mini;
    if (maxi != 0)
      s = delta / maxi;
    else
    {
      s = 0;
      h = -1;
      return;
    }
    if (r == maxi)
      h = (g - b) / delta;
    else if (g == maxi)
      h = 2 + (b - r) / delta;
    else
      h = 4 + (4 - g) / delta;
    h *= PI / 3;
    if (h < 0)
      h += 2 * PI;
  }

  // http://en.wikipedia.org/wiki/HSV_color_space#Converting_to_RGB
  void ToRGB(
    float h, 
    float s, 
    float v,
    float & r,
    float & g, 
    float & b)
  {
    if (h < 0)
    {
      h = TWO_PI - fmod(-h, TWO_PI);
      //float temp = -h;
      //temp = fmod(temp, TWO_PI)
      //h = TWO_PI - temp;
    }

    float c = v * s; // chroma
    float hp = fmod(h, TWO_PI) / (PI / 3);

    float x = c * (1.0f - (float) fabs((float)fmod(hp,2) - 1.0));
    switch((unsigned) hp)
    {
      case 5: r = c; g = 0; b = x; break;
      case 4: r = x; g = 0; b = c; break;
      case 3: r = 0; g = x; b = c; break;
      case 2: r = 0; g = c; b = x; break;
      case 1: r = x; g = c; b = 0; break;
      case 0: r = c; g = x; b = 0; break;
      default: r = g = b = 0;
    }
    
    float mini = v - c;
    r +=  mini;
    g +=  mini;
    b +=  mini;
  }

  Tac::Vector3 ToRGB( const Vector3 & hsv )
  {
    Vector3 rgb(0,0,0);
    ToRGB(hsv.x, hsv.y, hsv.z, rgb.x, rgb.y, rgb.z);
    return rgb;
  }


  Tac::Vector3 ToHSV( const Vector3 & rgb )
  {
    Vector3 hsv(0,0,0);
    ToHSV(rgb.x, rgb.y, rgb.z, hsv.x, hsv.y, hsv.z);
    return rgb;
  }

}

/*!
\author Nathan Park
*/
#include "TacVector2.h"
#include <TacMathUtil.h>
#include <iomanip> // setw
namespace Tac
{
	Vector2::Vector2(float xx, float yy) : x(xx),y(yy)
  {

  }
	Vector2 Vector2::operator-(void) const 
  {
    return Vector2(-x, -y); 
  }
	Vector2 Vector2::operator+(const Vector2& rhs) const
	{
    return Vector2(x + rhs.x, y + rhs.y);
	}
	Vector2 Vector2::operator-(const Vector2& rhs) const
	{
    return Vector2(x - rhs.x, y - rhs.y); 
	}
	Vector2 Vector2::operator*(const float rhs) const
	{
    return Vector2(x * rhs, y * rhs);
	}

	Vector2 Vector2::operator/(const float rhs) const
	{
    return Vector2(x / rhs, y / rhs);
	}
	Vector2& Vector2::operator+=(const Vector2& rhs)
	{
    x += rhs.x;
    y += rhs.y;
	  return *this;
	}
	Vector2& Vector2::operator-=(const Vector2& rhs)
	{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
	}
	Vector2& Vector2::operator*=(const float rhs)
	{
    x *= rhs;
    y *= rhs;
    return *this;
	}
	Vector2& Vector2::operator/=(const float rhs)
	{
    x /= rhs;
    y /= rhs;
    return *this;
	}
	bool Vector2::operator==(const Vector2& rhs) const
	{
    if (abs(x - rhs.x) > EPSILON)
      return false;
    if (abs(y - rhs.y) > EPSILON)
      return false;
	  return true;
	}

	bool Vector2::operator!=(const Vector2& rhs) const
	{
    return !(*this == rhs);
	}

	float Vector2::Dot(const Vector2& rhs) const
	{
    return x * rhs.x + y * rhs.y;
	}
	float Vector2::Length(void) const
	{
	  return sqrt(x * x + y * y); 
	}
	float Vector2::LengthSq(void) const
	{
	  return x * x + y * y; 
	}

	void Vector2::Normalize(void)
	{
    *this /= Length();
	}

	void Vector2::Zero(void)
	{
	  x = 0;
	  y = 0;
	}

  float Vector2::Dist( const Vector2 & rhs ) const
  {
    return sqrt(DistSq(rhs));
  }

  float Vector2::DistSq( const Vector2 & rhs ) const
  {
    float dx = x - rhs.x;
    float dy = y - rhs.y;
    return dx * dx + dy * dy;
  }

  float Vector2::AngleRad( const Vector2 & rhs ) const
  {
    //A.b == |A||B|cos0;
    float aDotb = Dot(rhs);
    float cosTheta = aDotb / (Length() * rhs.Length());
    return acos(cosTheta);
  }

  std::ostream & operator<<( std::ostream & is, const Vector2 & vec )
  {
    return is 
      << "(" << std::setw(3) << std::setprecision(2) 
      << vec.x << ", " 
      << vec.y << ")";
  }
  std::istream & operator >> (std::istream & lhs, Vector2 & vec)
  {
    return lhs >> vec.x >> vec.y;
  }
}

/*
Nathan Park
*/
#include "TacVector3.h"
#include "TacVector2.h"
#include <TacMathUtil.h>
#include <iomanip> // setw
#include <algorithm> // min
namespace Tac
{
  //Vector3::Vector3(float val) : x(val),y(val),z(val)
  //{
  //}
  Vector3::Vector3(float xx, float yy, float zz) : x(xx),y(yy),z(zz)
  {
  }
  Vector3::Vector3(const Vector2 & vec, float zval ) 
    : x(vec.x), y(vec.y), z(zval)
  {
  }
  Vector3::Vector3(float xx, const Vector2 & yz) : x(xx), y(yz.x), z(yz.y)
  {
  }

  Vector3::Vector3()
  {

  }

  Vector3 Vector3::operator-(void) const
  {
    return Vector3(-x, -y, -z);
  }
  Vector3 Vector3::operator+(const Vector3& rhs) const
  {
    return Vector3 (
      x + rhs.x,
      y + rhs.y,
      z + rhs.z);
  }

  Vector3 Vector3::operator-(const Vector3& rhs) const
  {
    return Vector3 (
      x - rhs.x,
      y - rhs.y,
      z - rhs.z);
  }
  Vector3 Vector3::operator*(const float rhs) const
  {
    return Vector3 (
      x * rhs,
      y * rhs,
      z * rhs);
  }

  Vector3 Vector3::operator/(const float rhs) const
  {
    return Vector3 (
      x / rhs,
      y / rhs,
      z / rhs);
  }
  Vector3& Vector3::operator+=(const Vector3& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
  }
  Vector3& Vector3::operator-=(const Vector3& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
  }
  Vector3& Vector3::operator*=(const float rhs)
  {
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
  }
  Vector3& Vector3::operator/=(const float rhs)
  {
    x /= rhs;
    y /= rhs;
    z /= rhs;
    return *this;
  }
  bool Vector3::operator==(const Vector3& rhs) const
  {
    if (abs(x - rhs.x) > EPSILON)
      return false;
    if (abs(y - rhs.y) > EPSILON)
      return false;
    if (abs(z - rhs.z) > EPSILON)
      return false;
    return true;
  }
  bool Vector3::operator!=(const Vector3& rhs) const
  {
    return !(*this == rhs);
  }
  float Vector3::Dot(const Vector3& rhs) const
  {
    return 
      x * rhs.x+
      y * rhs.y+
      z * rhs.z;
  }
  Vector3 Vector3::Cross(const Vector3& rhs) const
  {
    return Vector3(
    y * rhs.z - rhs.y * z,
    z * rhs.x - rhs.z * x,
    x * rhs.y - rhs.x * y);
  }
  float Vector3::Length(void) const
  {
    return sqrt(x * x + y * y + z * z); 
  }

  float Vector3::LengthSq(void) const
  {
    return x * x + y * y + z * z; 
  }
  void Vector3::Normalize(void)
  {
    *this /= Length();
  }
  void Vector3::Zero(void)
  {
    x = 0;
    y = 0;
    z = 0;
  }

  float & Vector3::operator[]( unsigned index )
  {
    return *(&x + index);
  }

  const float & Vector3::operator[]( unsigned index ) const
  {
    return *(&x + index);
  }

  bool Vector3::IsAlignedWith( const Vector3 & rhs ) const
  {
    return std::abs(Dot(rhs)) > 1.0f - EPSILON;
  }

  float Vector3::Dist( const Vector3 & rhs ) const
  {
    float dx = x - rhs.x;
    float dy = y - rhs.y;
    float dz = z - rhs.z;
    return sqrt(dx * dx + dy * dy + dz * dz);
  }

  float Vector3::DistSq( const Vector3 & rhs ) const
  {
    float dx = x - rhs.x;
    float dy = y - rhs.y;
    float dz = z - rhs.z;
    return dx * dx + dy * dy + dz * dz;
  }

  std::ostream & operator<<( std::ostream & lhs, const Vector3 & vec )
  {
    return lhs
      << vec.x 
      << " "
      << vec.y 
      << " "
      << vec.z;
  }
  std::istream & operator >> (std::istream & lhs, Vector3 & vec)
  {
    return lhs >> vec.x >> vec.y >> vec.z;
  }

  Vector3 ComponentwiseMult( const Vector3 & lhs, const Vector3 & rhs )
  {
    return Vector3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
  }

}
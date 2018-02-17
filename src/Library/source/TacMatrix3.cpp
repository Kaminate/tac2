/*
Nathan Park
*/
#include "TacMatrix3.h"
#include <TacErrorIf.h>
#include <TacMathUtil.h>
#include <iomanip>
namespace Tac
{
  Matrix3::Matrix3()
    : m00(1), m01(0), m02(0)
    , m10(0), m11(1), m12(0)
    , m20(0), m21(0), m22(1)
  {
  }
  Matrix3::Matrix3(
    float mm00, float mm01, float mm02,
    float mm10, float mm11, float mm12,
    float mm20, float mm21, float mm22)
    : m00(mm00), m01(mm01), m02(mm02)
    , m10(mm10), m11(mm11), m12(mm12)
    , m20(mm20), m21(mm21), m22(mm22)
  {
  }
  Vector3 Matrix3::operator*(const Vector3& rhs) const
  {
    return Vector3(
      m00 * rhs.x + m01 * rhs.y + m02 * rhs.z,
      m10 * rhs.x + m11 * rhs.y + m12 * rhs.z,
      m20 * rhs.x + m21 * rhs.y + m22 * rhs.z);
  }
  Matrix3 Matrix3::operator*(const Matrix3& rhs) const
  {
    Matrix3 temp(0,0,0,0,0,0,0,0,0);
    for(int i = 0; i < 3; ++i)
    {
      for(int j = 0; j < 3; ++j)
      {
        for(int k = 0; k < 3; ++k)
        {
          temp[i][j] += (*this)[i][k] * rhs[k][j];
        }
      }
    }
    return temp;
  }

  Matrix3 Matrix3::operator*( float val ) const
  {
    return Matrix3(
      m00 * val, m01 * val, m02 * val,
      m10 * val, m11 * val, m12 * val,
      m20 * val, m21 * val, m22 * val);
  }

  Matrix3& Matrix3::operator*=(const Matrix3& rhs)
  {
    Matrix3 temp(0,0,0,0,0,0,0,0,0);
    for(int i = 0; i < 3; ++i)
    {
      for(int j = 0; j < 3; ++j)
      {
        for(int k = 0; k < 3; ++k)
        {
          temp[i][j] += (*this)[i][k] * rhs[k][j];
        }
      }
    }
    *this = temp;
    return *this;
  }
  void Matrix3::Zero(void)
  {
    m00 = 0;
    m01 = 0;
    m02 = 0;
    m10 = 0;
    m11 = 0;
    m12 = 0;
    m20 = 0;
    m21 = 0;
    m22 = 0;
  }
  void Matrix3::Identity(void)
  {
    m00 = 1;
    m01 = 0;
    m02 = 0;
    m10 = 0;
    m11 = 1;
    m12 = 0;
    m20 = 0;
    m21 = 0;
    m22 = 1;
  }
  void Matrix3::Transpose(void)
  {
    Matrix3 temp(
      m00,m10,m20,
      m01,m11,m21,
      m02,m12,m22);
    *this = temp;
  }

  Matrix3 Matrix3::Tensor(const Vector3& vec)
  {
    return Matrix3 (
      vec[0] * vec[0], vec[0] * vec[1], vec[0] * vec[2],
      vec[1] * vec[0], vec[1] * vec[1], vec[1] * vec[2],
      vec[2] * vec[0], vec[2] * vec[1], vec[2] * vec[2]);
  }

  Matrix3 Matrix3::Tilda(const Vector3& vec)
  {
    return Matrix3(
      0, -vec.z,  vec.y,
      vec.z, 0, -vec.x,
      -vec.y, vec.x, 0);
  }
  Matrix3 Matrix3::VectorAlignment(const Vector3& vector,float rad)
  {
    float cosTheta = cos(rad);


    //Matrix3 CosI;
    //CosI.Identity();
    //CosI *= cos(rad);

    //Matrix3 NxN;
    //NxN = Tensor(vector);
    //NxN *= (1 - cos(rad));

    //Matrix3 SquiglyN;
    //SquiglyN = Tilda(vector);
    //SquiglyN *= sin(rad);

    return Matrix3() * cosTheta + Tensor(vector)* (1 - cosTheta) + Tilda(vector)* sin(rad);
  }
  Matrix3 Matrix3::RotRadX( const float rotRad )
  {
    float s = sin(rotRad);
    float c = cos(rotRad);
    return Matrix3(
      1,0,0,
      0,c,-s,
      0,s,c);
  }
  Matrix3 Matrix3::RotRadY( const float rotRad )
  {
    float s = sin(rotRad);
    float c = cos(rotRad);
    return Matrix3(
      c,0,s,
      0,1,0,
      -s,0,c);
  }
  Matrix3 Matrix3::RotRadZ( const float rotRad )
  {
    float s = sin(rotRad);
    float c = cos(rotRad);
    return Matrix3(
      c,-s,0,
      s,c,0,
      0,0,1);
  }
  void Matrix3::Invert()
  {
    float determinant, invDeterminant;
    float tmp[9];

    tmp[0] = v[4] * v[8] - v[5] * v[7];
    tmp[1] = v[2] * v[7] - v[1] * v[8];
    tmp[2] = v[1] * v[5] - v[2] * v[4];
    tmp[3] = v[5] * v[6] - v[3] * v[8];
    tmp[4] = v[0] * v[8] - v[2] * v[6];
    tmp[5] = v[2] * v[3] - v[0] * v[5];
    tmp[6] = v[3] * v[7] - v[4] * v[6];
    tmp[7] = v[1] * v[6] - v[0] * v[7];
    tmp[8] = v[0] * v[4] - v[1] * v[3];

    // check determinant if it is 0
    determinant = v[0] * tmp[0] + v[1] * tmp[3] + v[2] * tmp[6];

    if(fabs(determinant) <= EPSILON)
    {
      //DebugError("No inverse exists");
      //Identity();
      return;
    }

    // divide by the determinant
    invDeterminant = 1.0f / determinant;
    v[0] = invDeterminant * tmp[0];
    v[1] = invDeterminant * tmp[1];
    v[2] = invDeterminant * tmp[2];
    v[3] = invDeterminant * tmp[3];
    v[4] = invDeterminant * tmp[4];
    v[5] = invDeterminant * tmp[5];
    v[6] = invDeterminant * tmp[6];
    v[7] = invDeterminant * tmp[7];
    v[8] = invDeterminant * tmp[8];

  }

  Tac::Matrix3 Matrix3::operator+( const Matrix3 & rhs ) const
  {
    return Matrix3(
      m00 + rhs.m00,
      m01 + rhs.m01,
      m02 + rhs.m02,
      m10 + rhs.m10,
      m11 + rhs.m11,
      m12 + rhs.m12,
      m20 + rhs.m20,
      m21 + rhs.m21,
      m22 + rhs.m22);
  }

  float * Matrix3::operator[]( unsigned row )
  {
    return m[row];
  }

  const float * Matrix3::operator[]( unsigned row ) const
  {
    return m[row];
  }

  Tac::Matrix3 Matrix3::FromColVectors( 
    const Vector3 & v0, 
    const Vector3 & v1,
    const Vector3 & v2 )
  {
    return Matrix3(
      v0.x, v1.x, v2.x,
      v0.y, v1.y, v2.y,
      v0.z, v1.z, v2.z);
  }

  std::ostream & operator<<( std::ostream & lhs, const Matrix3 & mat )
  {
    for (unsigned r = 0; r < 3; ++r)
    {
      for (unsigned c = 0; c < 3; ++c)
      {
        float val = mat[r][c];
        if (IsAboutZero(val)) val = 0;
        lhs << std::setw(3) << std::setprecision(2) << val << " ";
      }
      if (r != 3)
        lhs << std::endl;
    }
    return lhs; 
  }
} // Tac

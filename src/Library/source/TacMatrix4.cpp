/*
Nathan Park
*/
#include "TacMatrix4.h"
#include <TacVector3.h>
#include <iomanip>
#include <TacMathUtil.h>
#include <TacErrorIf.h>
#include <TacMatrix3.h>
namespace Tac
{
  Matrix4::Matrix4(
    float mm00, float mm01, float mm02, float mm03,
    float mm10, float mm11, float mm12, float mm13,
    float mm20, float mm21, float mm22, float mm23,
    float mm30, float mm31, float mm32, float mm33)
    : m00(mm00), m01(mm01), m02(mm02), m03(mm03)
    , m10(mm10), m11(mm11), m12(mm12), m13(mm13)
    , m20(mm20), m21(mm21), m22(mm22), m23(mm23)
    , m30(mm30), m31(mm31), m32(mm32), m33(mm33)
  {
  }

  Matrix4::Matrix4()
    : m00(1), m01(0), m02(0), m03(0)
    , m10(0), m11(1), m12(0), m13(0)
    , m20(0), m21(0), m22(1), m23(0)
    , m30(0), m31(0), m32(0), m33(1)
  {
  }

  Vector4 Matrix4::operator*(const Vector4& rhs) const
  {
    return Vector4(
      m00 * rhs.x + m01 * rhs.y + m02 * rhs.z + m03 * rhs.w,
      m10 * rhs.x + m11 * rhs.y + m12 * rhs.z + m13 * rhs.w,
      m20 * rhs.x + m21 * rhs.y + m22 * rhs.z + m23 * rhs.w,
      m30 * rhs.x + m31 * rhs.y + m32 * rhs.z + m33 * rhs.w);
  }
  Matrix4 Matrix4::operator+(const Matrix4& rhs) const
  {
    return Matrix4(
      m00 + rhs.m00,
      m01 + rhs.m01,
      m02 + rhs.m02,
      m03 + rhs.m03,
      m10 + rhs.m10,
      m11 + rhs.m11,
      m12 + rhs.m12,
      m13 + rhs.m13,
      m20 + rhs.m20,
      m21 + rhs.m21,
      m22 + rhs.m22,
      m23 + rhs.m23,
      m30 + rhs.m30,
      m31 + rhs.m31,
      m32 + rhs.m32,
      m33 + rhs.m33);
  }
  Matrix4 Matrix4::operator*(const Matrix4& rhs) const
  {
    Matrix4 temp;
    temp.Zero(); 

    for(int i = 0; i < 4; ++i)
    {
      for(int j = 0; j < 4; ++j)
      {
        for(int k = 0; k < 4; ++k)
        {
          temp[i][j] += m[i][k] * rhs[k][j];
        }
      }
    }

    return temp;
  }

  Matrix4 Matrix4::operator*( float val ) const
  {
    return Matrix4(
      m00 * val, m01 * val, m02 * val, m03 * val,
      m10 * val, m11 * val, m12 * val, m13 * val,
      m20 * val, m21 * val, m22 * val, m23 * val,
      m30 * val, m31 * val, m32 * val, m33 * val);
  }

  Matrix4& Matrix4::operator*=(const Matrix4& rhs)
  {
    Matrix4 temp;

    temp.Zero(); 

    for(int i = 0; i < 4; ++i)
    {
      for(int j = 0; j < 4; ++j)
      {
        for(int k = 0; k < 4; ++k)
        {
          temp[i][j] += m[i][k] * rhs[k][j];
        }
      }
    }

    *this = temp;

    return *this;
  }

  Matrix4& Matrix4::operator*=( float val )
  {
    for(float & f : v) f *= val;
    return *this;
  }

  void Matrix4::Zero(void)
  {
    for (unsigned i = 0; i < 16; ++i)
      v[i] = 0;
  }

  void Matrix4::Identity(void)
  {
    Zero();
    m00 = 1;
    m11 = 1;
    m22 = 1;
    m33 = 1;
  }
  void Matrix4::Transpose(void)
  {
    Matrix4 temp(
      m00,m10,m20,m30,
      m01,m11,m21,m31,
      m02,m12,m22,m32,
      m03,m13,m23,m33);
    *this = temp;
  }

  Matrix4 Matrix4::Tensor(const Vector4& vec)
  {
    return Matrix4 (
      vec[0] * vec[0], vec[0] * vec[1], vec[0] * vec[2], 0,
      vec[1] * vec[0], vec[1] * vec[1], vec[1] * vec[2], 0,
      vec[2] * vec[0], vec[2] * vec[1], vec[2] * vec[2], 0,
      0,0,0,1);
  }
  Matrix4 Matrix4::Tilda(const Vector4& vec)
  {
    Matrix4 temp(
      0, -vec.z,  vec.y, 0,
      vec.z, 0, -vec.x, 0,
      -vec.y,  vec.x, 0, 0,
      0, 0, 0, 1);

    return temp;
  }
  Matrix4 Matrix4::VectorAlignment(const Vector4& vector,float rads)
  {
    // this is pheonix's funciton, it may not work
    Matrix4 CosI;
    CosI.Identity();
    CosI *= cos(rads);
    CosI.m33 = 1;

    Matrix4 NxN;
    NxN = Tensor(vector);
    NxN *= (1 - cos(rads));
    NxN.m33 = 1;

    Matrix4 SquiglyN;
    SquiglyN = Tilda(vector);
    SquiglyN *= sin(rads);
    SquiglyN.m33 = 1;

    return CosI + NxN + SquiglyN;
  }

  void Matrix4::Invert()
  {
    if(  IsAboutZero(v[12])
      && IsAboutZero(v[13])
      && IsAboutZero(v[14])
      && IsAboutOne (v[15]))
      InvertAffine();

    else
      DebugError("Write your own invert function");
  }
  void Matrix4::InvertAffine()
  {
    Matrix3 r(v[0],v[1],v[2], v[4],v[5],v[6], v[8],v[9],v[10]);
    r.Invert();
    v[0] = r.v[0];  v[1] = r.v[1];  v[2] = r.v[2];
    v[4] = r.v[3];  v[5] = r.v[4];  v[6] = r.v[5];
    v[8] = r.v[6];  v[9] = r.v[7];  v[10]= r.v[8];

    // -R^-1 * T
    float x = v[3];
    float y = v[7];
    float z = v[11];
    v[3]  = -(r.v[0] * x + r.v[1] * y + r.v[2] * z);
    v[7]  = -(r.v[3] * x + r.v[4] * y + r.v[5] * z);
    v[11] = -(r.v[6] * x + r.v[7] * y + r.v[8] * z);

  }
  float * Matrix4::operator[]( unsigned row )
  {
    return m[row];
  }

  const float * Matrix4::operator[]( unsigned row ) const
  {
    return m[row];
  }
  Tac::Matrix4 Matrix4::InvertProjectionMatrix( const Matrix4 & projMatrix )
  {
    return Matrix4(
      1/projMatrix.m00, 0, 0, 0,
      0, 1/projMatrix.m11, 0, 0,
      0,0,0,-1,
      0,0,1/projMatrix.m23, projMatrix.m22/projMatrix.m23);
  }

  Tac::Matrix4 Matrix4::Transform( 
    const Vector3 & scale, 
    const Vector3 & rotate, 
    const Vector3 & translate )
  {
    Matrix3 rot;
    if (rotate != Vector3(0,0,0))
    {
      rot 
        = Matrix3::RotRadZ(rotate.z)
        * Matrix3::RotRadY(rotate.y)
        * Matrix3::RotRadX(rotate.x);
    }
    return Matrix4(
      scale.x * rot.m00, scale.y * rot.m01, scale.z * rot.m02, translate.x,
      scale.x * rot.m10, scale.y * rot.m11, scale.z * rot.m12, translate.y,
      scale.x * rot.m20, scale.y * rot.m21, scale.z * rot.m22, translate.z,
      0, 0, 0, 1);
  }

  Tac::Matrix4 Matrix4::TransformInv( 
    const Vector3 & nonInversedScale,
    const Vector3 & nonInversedRotate,
    const Vector3 & nonInversedTranslate )
  {
    Vector3 s(
      1/nonInversedScale.x, 
      1/nonInversedScale.y, 
      1/nonInversedScale.z);
    // NOTE( N8 ): this must be in opposite order (xyz, zyx) of 
    //             Matrix4::Transform
    Matrix3 r
      = Matrix3::RotRadX(-nonInversedRotate.x)
      * Matrix3::RotRadY(-nonInversedRotate.y)
      * Matrix3::RotRadZ(-nonInversedRotate.z);
      
    Vector3 t(
      -nonInversedTranslate.x,
      -nonInversedTranslate.y,
      -nonInversedTranslate.z);

    float m03 = s.x * ( t.x * r.m00 + t.y * r.m01 + t.z * r.m02 );
    float m13 = s.y * ( t.x * r.m10 + t.y * r.m11 + t.z * r.m12 );
    float m23 = s.z * ( t.x * r.m20 + t.y * r.m21 + t.z * r.m22 );

    return Matrix4(
      s.x * r.m00, s.x * r.m01, s.x * r.m02, m03,
      s.y * r.m10, s.y * r.m11, s.y * r.m12, m13,
      s.z * r.m20, s.z * r.m21, s.z * r.m22, m23,
      0,0,0,1);
  }

  std::ostream & operator<<( std::ostream & lhs, const Matrix4 & mat )
  {
    for (unsigned r = 0; r < 4; ++r)
    {
      for (unsigned c = 0; c < 4; ++c)
      {
        float val = mat[r][c];
        if (abs(val) < 0.001f) val = 0;
        lhs << std::setw(4) << std::setprecision(2) << val << " ";
      }
      if (r != 3)
        lhs << std::endl;
    }
    return lhs; 
  }
} // Tac

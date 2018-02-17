#pragma once
#include <math.h> // sqrt
#include <string>
#include <vector>
#include "GL/glew.h"
#include "GL/GL.h"
namespace Tacchic
{
  template< unsigned N >
  class Vec
  {
  public:
    float mValues[N];
    Vec()
    {
    }
    Vec(const Vec & rhs)
    {
      for(unsigned i = 0; i < N; ++i)
        mValues[i] = rhs[i];
    }
    Vec & operator += (const Vec & rhs)
    {
      for(unsigned i = 0; i < N; ++i)
        mValues[i] += rhs[i];
      return *this;
    }
    Vec & operator -= (const Vec & rhs)
    {
      return *this += -rhs;
    }
    Vec & operator *= (float rhs)
    {
      for (unsigned i =0;i<N;++i)
        mValues[i]*=rhs[i];
    }
    Vec & operator /= (float rhs)
    {
      return *this *= (1.0f / rhs);
    }
    const Vec operator * (float rhs) const
    {
      Vec2 toReturn;
      for(unsigned i = 0; i < N; ++i)
        toReturn = mValues[i] * rhs;
    }
    const Vec operator / (float rhs) const
    {
      return *this * (1.0f / rhs);
    }
    const Vec operator + (const Vec & rhs) const
    {
      return Vec3(*this) += rhs;
    }
    const Vec operator - (const Vec & rhs) const
    {
      return *this + -rhs;
    }
    const Vec operator - () const 
    {
      Vec toReturn;
      for(int i = 0; i < N; ++i)
        toReturn[i] = -mValues[i];
      return toReturn;
    }
    const Vec Cross(const Vec & rhs) const
    {
      static_assert(N == 3, "Can only cross Vec3");

      Vec3 toReturn;
      toReturn[0] = mValues[1] * rhs[2] - mValues[2] * rhs[1];
      toReturn[1] = mValues[0] * rhs[2] - mValues[2] * rhs[0];
      toReturn[2] = mValues[0] * rhs[1] - mValues[1] * rhs[0];
      return toReturn;
    }

    bool operator == (const Vec & rhs) const
    {
      for(unsigned i = 0; i < N; ++i)
        if(mValues[i] != rhs[i])
          return false;
      return true;
    }
    bool operator != (const Vec & rhs) const
    {
      for(unsigned i = 0; i < N; ++i)
        if(mValues[i] != rhs[i])
          return true;
      return false;
    }

    float Dot(const Vec & rhs) const
    {
      float total = 0;
      for(unsigned i = 0; i < N; ++i)
        total += mValues[i] * rhs[i];
      return total;
    }
    float LengthSq() const
    {
      return Dot(*this);
    }
    float Length() const
    {
      return sqrt(LengthSq());
    }
    float DistSq(const Vec & rhs) const
    {
      Vec toRhs = rhs - *this;
      return toRhs.LengthSq();
    }
    float Dist(const Vec & rhs) const
    {
      return sqrt(DistSq(rhs));
    }
    void Normalize()
    {
      float len = Length();
      for(unsigned i = 0; i < N; ++i)
        mValues[i] /= len;
    }
    void Zero()
    {
      for(unsigned i = 0; i < N; ++i)
        mValues[i] = 0;
    }
    const float & operator [] (unsigned index)const
    {
      return mValues[index];
    }
    float & operator[](unsigned index)
    {
      return mValues[index];
    }
  };
  typedef Vec< 2 > Vec2;
  typedef Vec< 3 > Vec3;
  typedef Vec< 4 > Vec4;

  template< unsigned N >
  class Mat
  {
  public:
    Vec mRows[ N ];
  };
  typedef Mat< 2 > Mat2;
  typedef Mat< 3 > Mat3;
  typedef Mat< 4 > Mat4;

  class ShaderData
  {
  public:
    ShaderData(
      const GLenum type,
      const GLuint size,
      const std::string & name,
      void * data) : mtype(type), msize(size), mname(name), mdata(data){}
    ShaderData()
      :mtype(GL_NONE)
      ,msize(1)
      ,mname("Undefined")
      ,mdata(nullptr){}
    GLenum mtype; // e.g. GL_FLOAT_VEC3
    // num components in units of type (always 1 for non-array uniforms)
    GLuint msize;      
    std::string mname;
    void* mdata;
  };

  enum Attribute
  {
    ePos,
    eCol, 
    eUvs, 
    eNor, 
    eBoneId, 
    eBoneWeight, 
    eCount
  };

  class Geometry
  {
  public:
    struct RawData
    {
    public:
      void Clear();
      bool Load( const std::string & fileName );
      bool Save( const std::string & fileName );
      std::vector< Vec3 > mPos;
      std::vector< Vec3 > mCol;
      std::vector< Vec2 > mUvs;
      std::vector< Vec3 > mNor;
      std::vector< unsigned > mBoneIndexes;
      std::vector< float > mBoneWeights;
    } mRawData;
    struct GLData
    {
      // GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, 
      // GL_UNSIGNED_INT, GL_HALF_FLOAT, GL_FLOAT, GL_FIXED, 
      // GL_INT_2_10_10_10_REV, and GL_UNSIGNED_INT_2_10_10_10_REV
      GLenum componentType;
      unsigned componentsPerVert;
      unsigned stride; // byte-offset between two successive elements
      GLuint mBuffer;
    };
    GLData mGLData[Attribute::eCount];
    void LoadGLDataFromRawData();
    std::string mName;
  };

  class Material
  {
  public:
    std::string mName;

    // std::string mHLSL;
    // fk it ubershader

    Vec3 mColor;
    ShaderData mDataColor;
  };

  class ResourceManager
  {
  public:
    std::vector< Geometry* > mGeometries; // owned
    std::vector< Material* > mMaterials; // owned
  };

  class Model
  {
  public:
    Geometry * mGeometry; // owned my resource manager
    Material * mMaterial; // owned my resource manager
    // animation

    Vec3 mPos;
    Vec3 mScale;
    Vec3 mRot; // xyz, radians
  private:
  };
  class Frustrum
  {
  public:
    enum Planes
    {
      eTop, eBottom, eLeft, eRight, eNear, eFar, eCount
    };
  private:
    // ax + by + cz + d = 0
    // normal faces outwards
    Vec4 mPlanes[Planes::eCount]; 
  };

  class Camera
  {
    public:
      Camera();

      Mat4 GetView()const;
      Mat4 GetViewInverse()const;
      Mat4 GetProjPerspective()const;
      Mat4 GetProjPerspectiveInverse()const;
      Mat4 GetProjOrthographic()const;

      void GetCamDirections(Vec3 & right, Vec3 & up) const;

      float mNear;
      float mFar;
      float mFieldOfViewYRad;
      float mAspectRatio;
      float mWidth;  // for orthographic proj
      float mHeight; // for orthographic proj
    
      Vec3 mPosition;
      Vec3 mWorldSpaceUp;
      Vec3 mViewDirection;
  };
  class AccelerationStructure
  {
  public:
    virtual void GetModels(
      std::vector<Model*> & models,
      const Frustrum & cameraFrustrum) = 0;
  };

  class NoAcceleration : public AccelerationStructure
  {
  public:
    void GetModels(
      std::vector<Model*> & models,
      const Frustrum & cameraFrustrum) override;
  private:
    std::vector<Model*> mModels;
  };

  class Renderer
  {
  public:
    void Frame();
  };
}

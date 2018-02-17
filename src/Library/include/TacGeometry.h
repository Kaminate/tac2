/*!
\author Nathan Park
Holds the vertex buffer objects for the mesh geometry
*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once

#include "TacGLFWIncludes.h"
#include "TacVector3.h"
#include "TacVector2.h"
#include "TacSharedPtr.h"
#include "TacBoundingSphere.h"
#include "TacGeometryData.h"

#include "TacGraphics\TacShaderData.h"

#include <map>

namespace Tac
{
  class Geometry
  {    
  public:
    struct AttributeData
    {
      AttributeData();
      // GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, 
      // GL_UNSIGNED_INT, GL_HALF_FLOAT, GL_FLOAT, GL_FIXED, 
      // GL_INT_2_10_10_10_REV, and GL_UNSIGNED_INT_2_10_10_10_REV
      GLenum componentType;

      // 1-4. pos(vec3) = 3. boneIndexes(vec3i) = 4
      unsigned componentsPerVert;
      unsigned stride;               // byte-offset between two successive elements
      void* data;
    };

    TacDLLAPI Geometry(std::shared_ptr<GeometryData> /*const GeometryData &*/ data);
    TacDLLAPI ~Geometry();

    TacDLLAPI bool HasAttrib(Attribute attrib) const;
    TacDLLAPI GLuint GetAttribute(Attribute attrib) const;
    TacDLLAPI void SendAttribute(Attribute attrib, GLuint location) const;
    TacDLLAPI void Draw() const;
    TacDLLAPI unsigned GetVertexCount() const;

    TacDLLAPI const static int BonePerVert = 4;
    TacDLLAPI const BoundingSphere & GetBoundingSphere() const;
    TacDLLAPI const GeometryData & GetGeometryData() const;

    const AttributeData & GetAttributeData(Attribute attrib)const;
  private:
    BoundingSphere mBoundingSphere;
    unsigned mVertexCount;
    std::map<Attribute, GLuint> mBufferObjects; 

    void LoadVertexAttrib(
      Attribute attrib,
      unsigned entireBufferSize,
      void * data);

    std::shared_ptr< GeometryData > mGeomData;

    AttributeData mShaderData[Attribute::eCount];
  };
  DefineSharedPtr(Geometry);
} // Tac


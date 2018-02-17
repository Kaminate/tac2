/*!
\author Nathan Park
Holds the vertex buffer objects for the mesh geometry
*/
#include "TacGeometry.h"
#include "TacGraphicsErrors.h"
#include "TacErrorIf.h"

namespace Tac
{
  Geometry::Geometry(std::shared_ptr< GeometryData > /*const GeometryData & */data)
    : mVertexCount ( data->mCounts[Attribute::ePos])
    , mBoundingSphere(data->mBoundingSphere)
    , mGeomData( data )
  {
    // todo: replace mBufferObjects with mShaderData
    LoadVertexAttrib(
      Attribute::ePos,
      sizeof(Vector3) * data->mCounts[Attribute::ePos],
      data->mPos);
    AttributeData & posData = mShaderData[Attribute::ePos];
    posData.componentType = GL_FLOAT;
    posData.componentsPerVert = 3;
    posData.data = data->mPos;
    posData.stride = 0;

    LoadVertexAttrib(
      Attribute::eCol,
      sizeof(Vector3) * data->mCounts[Attribute::eCol],
      data->mCol);
    AttributeData & posCol = mShaderData[Attribute::eCol];
    posCol.componentType = GL_FLOAT;
    posCol.componentsPerVert = 3;
    posCol.data = data->mCol;
    posCol.stride = 0;

    LoadVertexAttrib(
      Attribute::eUvs,
      sizeof(Vector2) * data->mCounts[Attribute::eUvs],
      data->mUVs);
    AttributeData & posUVS = mShaderData[Attribute::eUvs];
    posUVS.componentType = GL_FLOAT;
    posUVS.componentsPerVert = 2;
    posUVS.data = data->mUVs;
    posUVS.stride = 0;


    
    LoadVertexAttrib(
      Attribute::eNor,
      sizeof(Vector3) * data->mCounts[Attribute::eNor],
      data->mNor);
    AttributeData & posNor = mShaderData[Attribute::eNor];
    posNor.componentType = GL_FLOAT;
    posNor.componentsPerVert = 3;
    posNor.data = data->mNor;
    posNor.stride = 0;


    LoadVertexAttrib(
     Attribute::eBoneId,
     sizeof(unsigned) * data->mCounts[Attribute::eBoneId],
     data->mBoneIds);
    AttributeData & posBoneId = mShaderData[Attribute::eBoneId];
    posBoneId.componentType = GL_UNSIGNED_INT;
    posBoneId.componentsPerVert = 4;
    posBoneId.data = data->mBoneIds;
    posBoneId.stride = 0;


    LoadVertexAttrib(
      Attribute::eBoneWeight, 
      sizeof(float) * data->mCounts[Attribute::eBoneWeight], 
      data->mBoneWeights);
    AttributeData & boneWeightData = mShaderData[Attribute::eBoneId];
    boneWeightData.componentType = GL_FLOAT;
    boneWeightData.componentsPerVert = 4;
    boneWeightData.data = data->mBoneWeights;
    boneWeightData.stride = 0;
  }

  Geometry::~Geometry()
  {
    for (auto pair : mBufferObjects)
    {
      GLuint buffer = pair.second;
      glDeleteBuffers(1, &buffer);
    }
  }

  bool Geometry::HasAttrib(Attribute attrib) const
  {
    return mBufferObjects.find(attrib) != mBufferObjects.end();
  }

  GLuint Geometry::GetAttribute( Attribute attrib ) const
  {
    ErrorIf(!HasAttrib(attrib), "Please call HasAttribute() first");
    return mBufferObjects.at(attrib);
  }

  void Geometry::SendAttribute( Attribute attrib, GLuint loc ) const
  {
    glBindBuffer(GL_ARRAY_BUFFER, GetAttribute(attrib));

    switch (attrib)
    {
    case Attribute::ePos:
      glVertexAttribPointer(loc,3,GL_FLOAT, GL_FALSE, 0, 0);
      break;
    case Attribute::eCol:
      glVertexAttribPointer(loc,3,GL_FLOAT, GL_FALSE, 0, 0);
      break;
    case Attribute::eUvs:
      glVertexAttribPointer(loc,2,GL_FLOAT, GL_FALSE, 0, 0);
      break;
    case Attribute::eNor:
      glVertexAttribPointer(loc,3,GL_FLOAT, GL_FALSE, 0, 0);
      break;
    case Attribute::eBoneId:
      glVertexAttribIPointer(loc, BonePerVert,GL_UNSIGNED_INT,0,0);
      break;
    case Attribute::eBoneWeight:
      glVertexAttribPointer(loc,BonePerVert,GL_FLOAT,GL_FALSE,0,0);
      break;
    default:
      DebugError("Don't know how to send this attribute");
      break;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  void Geometry::Draw() const
  {
    glDrawArrays(GL_TRIANGLES, 0, mVertexCount);
  }

  void Geometry::LoadVertexAttrib( 
    Attribute attrib, 
    unsigned entireBufferSize,
    void * data )
  {
    if (entireBufferSize == 0)
      return;

    ErrorIf(HasAttrib(attrib), "This attribute has already been loaded");

    GLuint buffer;

    // it will crash here if glew is uninitialized
    glGenBuffers(1, &buffer); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glBufferData(
      GL_ELEMENT_ARRAY_BUFFER,
      entireBufferSize,
      data,
      GL_STATIC_DRAW);

    mBufferObjects[attrib] = buffer;
  }

  const BoundingSphere & Geometry::GetBoundingSphere() const
  {
    return mBoundingSphere;
  }

  const GeometryData & Geometry::GetGeometryData() const
  {
    return *mGeomData;
  }

  unsigned Geometry::GetVertexCount() const
  {
    return mVertexCount;

  }

  const Geometry::AttributeData & Geometry::GetAttributeData( 
    Attribute attrib ) const
  {
    return mShaderData[attrib];
  }


  Geometry::AttributeData::AttributeData() :
    //name("Undefined"),
    //type(0),
    componentType(0),
    //numValues(0),
    componentsPerVert(0),
    //numComponents(0),
    //byteSize(0),
    stride(0),
    data(nullptr)
  {

  }

} // Tac

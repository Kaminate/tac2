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

#include "TacVector3.h"
#include "TacVector2.h"
#include "TacBoundingSphere.h"

#include <string>
#include <fstream>

namespace Tac
{

  /* example creation of geometry data:
  GeometryData data;
  data.mCounts[Geometry::Attribute::Position] = 3;
  data.mPositions = new Vector3[3];
  data.mPositions[0] = Vector3(1,0,0);
  data.mPositions[1] = Vector3(0,0,1);
  data.mPositions[2] = Vector3(-1,0,0);
  data.Save("resources/tri1.tacmodel");
  data.Load("resources/tri1.tacmodel");
  */
  enum Attribute
  {
    ePos = 0,
    eCol, 
    eUvs, 
    eNor, 
    eBoneId, 
    eBoneWeight, 
    eCount
  };



  class GeometryData
  {
  public:
    TacDLLAPI GeometryData();
    TacDLLAPI GeometryData(const GeometryData & rhs);
    TacDLLAPI ~GeometryData();
    TacDLLAPI void Clear();
    TacDLLAPI bool Load(const std::string & fileName);
    TacDLLAPI GeometryData & LoadCube(float halfWidth);
    TacDLLAPI GeometryData & LoadTriange(float length);
    TacDLLAPI GeometryData & LoadNDCQuad();
    TacDLLAPI bool Save(const std::string & fileName);

    unsigned mCounts[ Attribute::eCount ];
    // todo: make these std::vectors
    Vector3 * mPos;
    Vector3 * mCol;
    Vector2 * mUVs;
    Vector3 * mNor;
    unsigned * mBoneIds;
    float * mBoneWeights;
    BoundingSphere mBoundingSphere;

    template<typename T>
    void Save(std::ofstream & ofs, unsigned count, const T* data)
    {
      if (count == 0)
        return;
      ofs.write(reinterpret_cast<const char*>(data), count * sizeof(T));
    }

    template<typename T>
    void Load(std::ifstream & ifs, unsigned count, T * & data)
    {
      if (count == 0)
        return;
      data = new T [count];
      ifs.read(reinterpret_cast<char*>(data), count * sizeof(T));
    }
  };
} // Tac


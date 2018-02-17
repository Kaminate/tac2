/*!
\file OBJInfo.cpp
\brief
\author Nathan Park
\par
All content (c) 2013-2014 DigiPen (USA) Corporation, all rights reserved. 
Reproduction or disclosure o this file or this contents without the 
prior written consent of DigiPen Institute of technology is prohibited.
*/

#include "TacOBJInfo.h"

//#include "ErrorIf.h"

#include <fstream> 
//#include <sstream>
#include <algorithm> // count
#include <numeric> // accumulate
#include "TacErrorIf.h"
#include <limits>
#include "TacGeometryData.h"

namespace Tac
{
  /*
  struct VecAboutEqual {
  Vector3 mV;
  VecAboutEqual(const Vector3 & v) : mV(v) {}
  bool operator () (const Vector3 & v) { return v.Dot(mV) > 1 - 0.001f; } };
  */

  // all normals are interpolated. No face normals (cube impossible)
  /*

  TriMesh::TriMesh(OBJData & data)
  {
  unsigned reserveCount = data.mPositions.size();
  mFinalTexCoords.reserve(reserveCount);

  std::vector<Vector3> computedFaceNormal;
  data.FillVectorWithFaceNormals(computedFaceNormal);

  // each vertex can be connected to infinite faces, and thus have
  // infinite normals
  // accumulator or normals
  std::vector< std::vector<Vector3> > vertexNormalAccumulators;
  vertexNormalAccumulators.resize(data.mPositions.size());

  // 1. accumulate normals
  for (unsigned iface = 0; iface < data.mFaces.size(); ++iface)
  {
  const std::array<unsigned,3> & face = data.mFaces[iface];

  for (unsigned ivert = 0; ivert < 3; ++ivert)
  {
  const unsigned & vert = face[ivert];

  const Vector3 & faceNormal = computedFaceNormal[iface];

  std::vector<Vector3> & currVertNormalAccumulator 
  = vertexNormalAccumulators[vert - 1];

  // add if unique
  bool unique = currVertNormalAccumulator.end() == std::find_if(
  currVertNormalAccumulator.begin(), 
  currVertNormalAccumulator.end(), VecAboutEqual(faceNormal));

  if (unique)
  currVertNormalAccumulator.push_back(faceNormal);
  }
  }

  // 2. Average
  mFinalNormals.reserve(reserveCount);
  for (
  auto it = vertexNormalAccumulators.begin();
  it != vertexNormalAccumulators.end(); 
  ++it)
  {
  std::vector<Vector3> & accumulator = *it;
  Vector3 total 
  = std::accumulate(accumulator.begin(), accumulator.end(), Vector3(0,0,0));
  Vector3 average = total / (float) accumulator.size();
  average.Normalize();
  mFinalNormals.push_back(average);
  }


  mFinalPositions = data.mPositions;

  // find the min/max
  for (auto it = mFinalPositions.begin(); it != mFinalPositions.end(); ++it)
  {
  const Vector3 & pos  = *it;

  for (unsigned i = 0; i < 3; ++i)
  {
  if (pos[i] < mMin[i])
  mMin[i] = pos[i];
  else if (pos[i] > mMax[i])
  mMax[i] = pos[i];
  }
  }

  Vector3 center = (mMin + mMax) / 2;

  // compute tex coords
  mFinalTexCoords.reserve(reserveCount);
  for (auto it = mFinalPositions.begin(); it != mFinalPositions.end(); ++it)
  {

  Vector3 v = *it;

  // transform rel to center
  v -= center;
  float r = v.Length();

  float halfAngle  = acos(v.z / r); // 0 to pi?
  float wholeAngle = atan2(v.y, v.x) + PI;
  mFinalTexCoords.push_back(Vector2(
  halfAngle / PI, // 0, 1
  wholeAngle / TWO_PI)); // 0, 1
  }

  // faces
  mFaces.reserve(data.mFaces.size());
  for (auto it = data.mFaces.begin(); it != data.mFaces.end(); ++it)
  {
  const std::array<unsigned, 3> & face = *it;
  std::array<unsigned short,3> foo;
  foo[0] = face[0] - 1;
  foo[1] = face[1] - 1;
  foo[2] = face[2] - 1;
  mFaces.push_back(foo);
  }
  }

  */

  /*
  void TriMesh::DebugDrawNormals(
  const Vector3 & s, 
  const Matrix3 & r, 
  const Vector3 & t, 
  float len)
  {
  Vector4 colorBegin(0.8f, 0, 0.45f, 1);
  Vector4 colorEnd(1, 0.5f, 1, 0);

  Graphics * graphicsSystem = Core::Instance().GetSystem<Graphics>();
  DebugDrawLayer * layer = graphicsSystem->GetDebugLayer();
  DebugDrawer * drawer = layer->RequestDebugDrawer();
  if (!drawer) return;
  for (unsigned i = 0; i < mFinalPositions.size(); ++i)
  {
  Matrix4 transform = Matrix4::Transform(s, r, t);
  Vector4 pos4 = Vector4(mFinalPositions[i], 1);
  pos4 = transform * pos4;
  Vector3 pos3 = pos4.xyz();
  Vector4 nor4 = Vector4(mFinalNormals[i], 0);
  nor4 = transform * nor4;
  nor4.Normalize();
  nor4 *= len;
  Vector3 nor3 = nor4.xyz();
  drawer->DrawLine(pos3, pos3 + nor3, colorBegin, colorEnd);
  }
  }

  */
  // data

  /*
  OBJData::OBJData()
  {
  mPositions.reserve(65000);
  mFaces.reserve(65000);
  }
  void OBJData::FillVectorWithFaceNormals(std::vector<Vector3>& normals) const
  {
  normals.clear();
  normals.reserve(mFaces.size());
  for (auto it = mFaces.begin(); it != mFaces.end(); ++it)
  {
  const std::array<unsigned,3> & face = *it;
  const Vector3 & v0 = mPositions[face[0] - 1];
  const Vector3 & v1 = mPositions[face[1] - 1];
  const Vector3 & v2 = mPositions[face[2] - 1];
  Vector3 v0to1 = v1 - v0;
  Vector3 v0to2 = v2 - v0;
  Vector3 faceNormal = v0to1.Cross(v0to2);
  faceNormal.Normalize();
  normals.push_back(faceNormal);
  }
  }

  void OBJData::Load(const std::string & entirePath)
  {
  std::ifstream ifs(entirePath.c_str(), std::ifstream::in);

  ErrorIf(!ifs.is_open(), "Failed to open file %s", entirePath.c_str());


  char c;
  Vector3 pos;
  std::string line;
  std::array<unsigned,3> face;

  while (ifs.good())
  {
  char peeked = ifs.peek();
  switch (peeked)
  {
  case 'v':
  ifs >> c >> pos.x >> pos.y >> pos.z;
  mPositions.push_back(pos);
  break;
  case 'f':
  ifs >> c;
  ifs >> face[0] >> face[1] >> face[2];
  mFaces.push_back(face);
  break;
  case '#': // fallthrough
  default:
  break;
  }
  std::getline(ifs, line);
  }
  }

  */

  ObjDataV2::ObjDataV2( const std::string & entirePath )
  {
    // holds a bunny or a horse
    mPositions.reserve(50000); 
    mFaces.reserve(100000);

    std::ifstream ifs(entirePath.c_str(), std::ifstream::in);
    if(!ifs.is_open())
    {
      mErrorMsg = "Failed to open file " + entirePath;
      return;
    }

    char c;
    Vector3 pos(0,0,0);
    Tri face;


    while (ifs.good())
    {
      char peeked = ifs.peek();
      switch (peeked)
      {
      case 'v':
        ifs >> c >> pos.x >> pos.y >> pos.z;
        mPositions.push_back(pos);
        break;
      case 'f':
        ifs >> c;
        ifs >> face.mPosIndex[0] >> face.mPosIndex[1] >> face.mPosIndex[2];
        // objs start at index 1. Change it to index 0
        face.mPosIndex[0]--;
        face.mPosIndex[1]--;
        face.mPosIndex[2]--;
        mFaces.push_back(face);
        break;
        //case '#': // fallthrough
        //default:
        //break;
      }
      ifs.ignore( std::numeric_limits<std::streamsize>::max(), '\n');
    }

    ComputeNormals();
  }


  void ObjDataV2::ComputeNormals()
  {
    ErrorIf(!Ok(), "not ok");
    ErrorIf(!mFaceNormals.empty(), "already computed");
    //ErrorIf(!mPerVertNormals.empty(), "already computed");

    unsigned numFaces = mFaces.size();
    mFaceNormals.resize(numFaces);
    mPerVertNormals.resize(mPositions.size(), Vector3(0,0,0));


    //std::vector<std::vector<Vector3> > perVertUniqueNormals;
    //perVertUniqueNormals.resize(mPositions.size());
    //auto insertNormal = [&perVertUniqueNormals](
    //  unsigned vertexIndex,
    //  const Vector3 & normal)
    //{
    //  std::vector<Vector3> & uniqueNormals 
    //    = perVertUniqueNormals[vertexIndex];

    //  bool unique = true;
    //  for(const Vector3 & uniqueNormal : uniqueNormals)
    //  {
    //    if(normal.Dot(uniqueNormal) > 0.9f)
    //    {
    //      unique = false;
    //      break;
    //    }
    //  }

    //  if(unique)
    //    uniqueNormals.push_back(normal);
    //};

    for(unsigned i = 0; i < numFaces; ++i)
    {
      const Tri & curFace = mFaces[i];
      const Vector3 & v0 = mPositions[ curFace.mPosIndex[0] ];
      const Vector3 & v1 = mPositions[ curFace.mPosIndex[1] ];
      const Vector3 & v2 = mPositions[ curFace.mPosIndex[2] ];

      Vector3 normal = (v1 - v0).Cross(v2 - v0);
      normal.Normalize();
      mFaceNormals[i] = normal;
      //insertNormal(curFace.mPosIndex[0], normal);
      //insertNormal(curFace.mPosIndex[1], normal);
      //insertNormal(curFace.mPosIndex[2], normal);
      mPerVertNormals[curFace.mPosIndex[0]] += normal;
      mPerVertNormals[curFace.mPosIndex[1]] += normal;
      mPerVertNormals[curFace.mPosIndex[2]] += normal;
    }

    unsigned numVertexes = mPositions.size();
    for(unsigned i = 0; i < numVertexes; ++i)
    {
      Vector3 & perVertexNormal = mPerVertNormals[i];
      //std::vector<Vector3>& normals = perVertUniqueNormals[i];
      //for(Vector3 & normal : normals)
        //perVertexNormal += normal;
      perVertexNormal.Normalize();
    }
  }

  void ObjDataV2::Fill( class GeometryData & geomData, NormalType normType )
  {
    ErrorIf(!Ok(), "not ok");

    unsigned numFaces = mFaces.size();
    unsigned triCount = numFaces * 3;

    // positions
    geomData.mCounts[Attribute::ePos] = triCount;
    geomData.mPos = new Vector3[triCount];
    unsigned triIndex = 0;
    for(Tri & face : mFaces) 
    {
      geomData.mPos[triIndex++] = mPositions[ face.mPosIndex[0] ];
      geomData.mPos[triIndex++] = mPositions[ face.mPosIndex[1] ];
      geomData.mPos[triIndex++] = mPositions[ face.mPosIndex[2] ];
    }

    // normals
    geomData.mCounts[Attribute::eNor] = triCount;
    geomData.mNor = new Vector3[triCount];
    switch (normType)
    {
    case Tac::ObjDataV2::eFace:
      triIndex = 0;
      for(Vector3 & faceNorm : mFaceNormals)
      {
        geomData.mNor[triIndex++] = faceNorm;
        geomData.mNor[triIndex++] = faceNorm;
        geomData.mNor[triIndex++] = faceNorm;
      }
      break;
    case Tac::ObjDataV2::eAveraged:
      triIndex = 0;
      for(Tri & face : mFaces) 
      {
        geomData.mNor[triIndex++] = mPerVertNormals[ face.mPosIndex[0] ];
        geomData.mNor[triIndex++] = mPerVertNormals[ face.mPosIndex[1] ];
        geomData.mNor[triIndex++] = mPerVertNormals[ face.mPosIndex[2] ];
      }
      break;
    default:
      break;
    }
  }

  bool ObjDataV2::Ok()
  {
    return mErrorMsg.empty();
  }

  const std::string & ObjDataV2::GetErrorMsg() const
  {
    return mErrorMsg;
  }


}; // Tac


/*!
\file OBJInfo.h
\brief
\author Nathan Park
\par
All content (c) 2013-2014 DigiPen (USA) Corporation, all rights reserved. 
Reproduction or disclosure o this file or this contents without the 
prior written consent of DigiPen Institute of technology is prohibited.
*/

#pragma once

#include <vector>
#include <string>
#include <array>

#include "TacVector2.h"
#include "TacVector3.h"
#include "TacMatrix3.h"

namespace Tac
{
  struct ObjDataV2
  {
    struct Tri
    {
      // starts at index 0
      unsigned mPosIndex[ 3 ];
    };
    ObjDataV2(const std::string & entirePath);

    std::vector<Vector3> mPositions;
    std::vector<Vector3> mPerVertNormals;

    std::vector<Tri> mFaces;
    std::vector<Vector3> mFaceNormals;

    enum NormalType{eFace, eAveraged};

    void Fill(class GeometryData & geomData, NormalType normType);
    bool Ok();
    const std::string & GetErrorMsg() const;

  private:
    void ComputeNormals();
    std::string mErrorMsg;
  };

} // Tac


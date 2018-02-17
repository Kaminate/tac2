/*!
   DebugDraw provides 2 ways to debug draw to the screen.
    1. The graphics system has a DebugDrawer. Call it's draw functions.
    2. Add your own DebugDrawLayer to your system, then 
       register it getting the graphics system's debugdrawer, then 
       DebugDrawer->AddLayer( myLayer );
*/
#pragma once

#include <GL/glew.h>
#include <GL/GL.h>

#include <string>
#include <set>
#include <vector>

#include "TacVector3.h"
#include "TacVector4.h"

#include "TacMatrix3.h"
#include "TacMatrix4.h"

#include "TacShaderDebugDraw.h"
#include "TacColors.h"
#include "TacGeometry.h"


namespace Tac
{
  class DebugDrawer
  {
  public:
    DebugDrawer(class GraphicsSystem * graphics);
    ~DebugDrawer();
    void Update(float dt);
    
    void DrawLine(
      const Vector3 & begin, 
      const Vector3 & end, 
      const Vector3 & color = Vector3(1,0,0));
    
    void DrawLine(
      const Vector3 & begin, 
      const Vector3 & end, 
      const Vector3 & colorBegin,
      const Vector3 & colorEnd);

    void DrawArrow(
      const Vector3 & pos,
      const Vector3 & dir_and_magnitude,
      const Vector3 & color = Vector3(1,0,0));
  
    void DrawArrow(
      const Vector3 & pos,
      const Vector3 & unitDir,
      float magnitude,
      const Vector3 & color = Vector3(1,0,0));

    void DrawCircle(
      const Vector3 & pos,
      float radius,
      Vector3 normal = Vector3(0,0,1), // +,- doesnt matter, copy by value
      const Vector3 & color = Color::red,
      unsigned numSides = 12);

    //void DrawElipse(
    //  const Vector3 & pos,
    //  float radiusX,
    //  float radiusY,
    //  float radiusZ,
    //  const Vector3 & color = Color::red,
    //  unsigned numSides = 12);

    void DrawCone(
      const Vector3 & coneTipPos,
      float radius,
      float height,
      Vector3 normal,
      const Vector3 & color = Color::red,
      unsigned numSides = 12);

    void DrawCircleFacingCamera(
      const Vector3 & pos,
      float radius,
      const Vector3 & color = Color::red, 
      unsigned numSides = 12);

    void DrawCoordinateAxis(
      float arrowLength,
      const Matrix4 & transform = Matrix4());

    void DrawGrid(
      float entireWidth = 10,
      unsigned numDivisions = 10,
      const Vector3 & color = Vector3(0.3f, 0.3f, 0.3f),
      const Vector3 & center = Vector3(0,0,0));

    int GetDrawableVertexCount() const;

  private:

    bool mNeedsRespecification;
    GLuint mColBuffer;
    GLuint mPosBuffer;


    ShaderDebugDraw mShader;
    
    std::vector<Vector3> mPositions;
    std::vector<Vector3> mColors;
    const static unsigned MAX_NUM_VERTS = 4096; // 4096 points, 2048 lines
    
    void OrphanBuffers();
    void CopyToBuffer();
    class GraphicsSystem * mGraphics;
    Geometry::AttributeData mPosData;
    Geometry::AttributeData mColData;

    int mAccumulatedVertexCount;
    
  };
  
}


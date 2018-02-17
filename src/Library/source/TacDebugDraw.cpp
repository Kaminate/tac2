/*!

*/
#include "TacDebugDraw.h"


#include "TacErrorIf.h"

#include "GL/GL.h"


#include <algorithm>
#include "TacCore.h"
#include "TacCamera.h"
#include "TacGeometryData.h"
#include "TacSystemGraphics.h" // class Glew
#include "TacComponentCamera.h"
#include "TacMathUtil.h"
#include <TacSystem.h>

// render pass
#include "TacFramebuffer.h"
#include "TacGraphics/TacOperations/TacViewportOperation.h"
#include "TacGraphics/TacOperations/TacBindCameraUniform.h"
#include <TacGraphics/TacOperations/TacBindBuffer.h>
#include <TacGraphics/TacOperations/TacBindAttribute.h>
#include <TacGraphics/TacOperations/TacDrawOperation.h>


namespace Tac
{
  /////////////////
  // DebugDrawer //
  /////////////////

  DebugDrawer::DebugDrawer(GraphicsSystem * graphicsSys)
    : mGraphics(graphicsSys)
    , mNeedsRespecification(true)
    , mAccumulatedVertexCount(0)
  {
    mPosData.componentsPerVert = 3;
    mPosData.componentType = GL_FLOAT;
    mPosData.data = nullptr;
    mPosData.stride = 0;
    mColData = mPosData;


    graphicsSys->GetCore()->SetDebugDrawStart(
      [this](){
        //mPositions.clear();
        //mColors.clear();
        //mNeedsRespecification = true;
    });

    mPositions.reserve(MAX_NUM_VERTS);
    mColors.reserve(MAX_NUM_VERTS);

    auto createVec3Buffer = [](GLuint & buf)
    {
      glGenBuffers(1, &buf); // it will crash here if glew is uninitialized
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf);
      glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        MAX_NUM_VERTS * sizeof(Vector3),
        nullptr,
        //GL_STREAM_DRAW);
        GL_DYNAMIC_DRAW);
    };

    createVec3Buffer(mPosBuffer);
    createVec3Buffer(mColBuffer);
    /*
    ShaderResource * resourceDebugDraw = new ShaderResource();
    resourceDebugDraw->AddShader(ShaderResource::eVertexShader,
    "resources/TacShaders/ShaderDebugDraw.v.glsl");
    resourceDebugDraw->AddShader(ShaderResource::eFragmentShader,
    "resources/TacShaders/ShaderDebugDraw.f.glsl");
    resourceDebugDraw->Init();
    graphicsSys->GetCore()->mResourceManager->AddResource(resourceDebugDraw);
    mShader.BindToResource(resourceDebugDraw);






    /*/
    ResourceManager & resources = ResourceManager::Instance();

    FramebufferStage * backBufferStage = new FramebufferStage();
    backBufferStage->SetName("debug draw onto backbuffer");
    resources.AddResource(backBufferStage);
    backBufferStage->GetActiveRef() = false; // temp

    std::vector<GLenum> drawBufs;
    drawBufs.push_back(GL_BACK_LEFT);
    backBufferStage->SetFramebuffer(Framebuffer::GetBackbuffer(), drawBufs);
    graphicsSys->GetRenderer()->AddFramebufferStage(backBufferStage);

    ShaderResource * drawShader = new ShaderResource();
    drawShader->AddShader(ShaderResource::eVertexShader,
      "resources/TacShaders/ShaderDebugDraw.v.glsl");
    drawShader->AddShader(ShaderResource::eFragmentShader,
      "resources/TacShaders/ShaderDebugDraw.f.glsl");
    drawShader->Init();
    resources.AddResource(drawShader);

    ShaderPass *pass = new ShaderPass();
    resources.AddResource(pass);
    backBufferStage->AddShaderPass(pass);
    pass->SetShader(drawShader);

    pass->AddPreRenderOp(new BindViewportOperation(0,0));
    pass->AddPreRenderOp(new BindCameraUniform(
      drawShader->GetUniform("view"),
      BindCameraUniform::eMatrixView));
    pass->AddPreRenderOp(new BindCameraUniform(
      drawShader->GetUniform("proj"),
      BindCameraUniform::eMatrixProjPerspective));

    pass->AddPreRenderOp(
      new BindBuffer(GL_ARRAY_BUFFER, mPosBuffer));
    pass->AddPreRenderOp(
      new BindAttribute(drawShader->GetAttribute("position"),
      Attribute::ePos, &mPosData));
    pass->AddPreRenderOp(
      new BindBuffer(GL_ARRAY_BUFFER, mColBuffer));
    pass->AddPreRenderOp(
      new BindAttribute(drawShader->GetAttribute("color"),
      Attribute::ePos, &mPosData));

    pass->AddPostRenderOperation(new BindBuffer(GL_ARRAY_BUFFER, 0));

    DrawOperationIndependent * draw = new DrawOperationIndependent(
      GL_LINES,
      [this]()->int
    {
      return GetDrawableVertexCount();
    }
    );
    pass->AddPreRenderOp(draw);

    if (mNeedsRespecification)
    {
      mNeedsRespecification = false;
      OrphanBuffers();
      CopyToBuffer();
    }


#ifdef TACTWEAK
    std::string tweakGroup(" group=debugdraw ");
    graphicsSys->mRenderBarIsWunderBar->AddVar(
      "debugdrawactive",
      "label='Active'" + tweakGroup,
      TW_TYPE_BOOLCPP, &backBufferStage->GetActiveRef());
#endif TACTWEAK

  }

  DebugDrawer::~DebugDrawer()
  {

  }

  void DebugDrawer::OrphanBuffers()
  {
    // http://www.opengl.org/wiki/Buffer_Object_Streaming
    auto orphanVec3Buffer = [](GLuint buf){
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf);
      glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        MAX_NUM_VERTS * sizeof(Vector3),
        nullptr, // data (null to orphan the old buffer)
        GL_STREAM_DRAW);
    };

    orphanVec3Buffer(mPosBuffer);
    orphanVec3Buffer(mColBuffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  void DebugDrawer::CopyToBuffer()
  {
    if (mPositions.empty()) 
      return;

    auto copyVec3Buffer = [ ]( std::vector<Vector3> & attribs, GLuint buf)
    {
      glBindBuffer(GL_ARRAY_BUFFER, buf);
      float * data = (float*) glMapBufferRange(
        GL_ARRAY_BUFFER, 
        0,
        MAX_NUM_VERTS * sizeof(Vector3),
        GL_MAP_WRITE_BIT);

      unsigned numToCopy = std::min(attribs.size(), MAX_NUM_VERTS);

      memcpy(data, &attribs[0], numToCopy * sizeof(Vector3));
      glUnmapBuffer(GL_ARRAY_BUFFER);
    };

    copyVec3Buffer(mPositions, mPosBuffer);
    copyVec3Buffer(mColors, mColBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // !
  }

  // public functions
  void DebugDrawer::DrawLine(
    const Vector3 & p0, 
    const Vector3 & p1, 
    const Vector3 & color)
  {

    mPositions.push_back(p0);
    mPositions.push_back(p1);
    mColors.push_back(color);
    mColors.push_back(color);
  }

  void DebugDrawer::DrawLine(
    const Vector3 & begin, 
    const Vector3 & end, 
    const Vector3 & colorBegin,
    const Vector3 & colorEnd)
  {

    mPositions.push_back(begin);
    mPositions.push_back(end);
    mColors.push_back(colorBegin);
    mColors.push_back(colorEnd);
  }

  void DebugDrawer::DrawArrow( 
    const Vector3 & pos, 
    const Vector3 & dir_and_magnitude, 
    const Vector3 & color /*= Vector4(1,0,0,1)*/)
  {

    if (dir_and_magnitude.Length() < 0.001f) return;

    Vector3 head = pos + dir_and_magnitude;
    DrawLine(pos, head, color);

    Vector3 xAxis = -dir_and_magnitude;
    xAxis.Normalize();

    // get an arbitrary perpendicular vector
    Vector3 yAxis = xAxis.Cross(Vector3(1,0,0));
    if (yAxis.Length() < 0.001f) 
      // they were paralell, cross with a differnt vector
        yAxis = xAxis.Cross(Vector3(0,1,0));

    Vector3 zAxis = xAxis.Cross(yAxis);

    // yAxis, zAxis should already be normalized


    // align the x with the -dir
    Matrix3 rotatedToStandard = Matrix3::FromColVectors(
      xAxis, yAxis, zAxis);

    const unsigned numHooks = 3;
    static Vector3 sHooks[numHooks];

    const float rad30 = ToRadians(30);

    for (unsigned i = 0; i < numHooks; ++i)
    {
      //  z
      //  ^           hook (swept around x axis)
      //  |     __.-->
      //  |_----_________x
      //
      Vector3 hook(cos(rad30), 0, sin(rad30));

      float angleRad = 2.0f * PI * i / numHooks;
      Matrix3 rotX = Matrix3::RotRadX(angleRad);
      hook = rotX * hook;
      hook = rotatedToStandard * hook;
      hook *= dir_and_magnitude.Length();
      hook /= 5.0f;

      Vector3 currPoint = head + hook;

      sHooks[i] = currPoint;
      DrawLine(head, currPoint, color);
    }

    for (unsigned i = 0; i < numHooks; ++i)
    {
      DrawLine(sHooks[i], sHooks[(i + 1) % numHooks], color);
    }
  }

  void DebugDrawer::DrawArrow(
    const Vector3 & pos, 
    const Vector3 & unitDir, 
    float magnitude, 
    const Vector3 & color /*= Vector4(1,0,0,1)*/ )
  {
    DrawArrow(pos, unitDir * magnitude, color);
  }

  void DebugDrawer::DrawCircle( 
    const Vector3 & pos,
    float radius, 
    Vector3 normal /* = Vector3(0,0,1)*/, /* +,- doesnt matter */ 
    const Vector3 & color /*= Vector4(1,0,0,1)*/,
    unsigned numSides /* = 8*/)
  {
    Clamp(numSides, 3u, 18u);

    // lets let radius be any number
    if(normal.Length() < 0.001f)
      return;
    //ErrorIf(normal.Length() < 0.001f, "Let the normal not be a a zero vector");

    normal.Normalize(); // this is why we copied by value

    // get a tangent vector
    Vector3 tangent = normal.Cross(Vector3(1,0,0));
    if (tangent.Length() == 0)
      tangent = normal.Cross(Vector3(0,1,0));

    tangent.Normalize();
    // once you normalize the tangent
    // when creating the bitangent, it will already be normalized
    Vector3 bitangent = tangent.Cross(normal);


    // worldspace                                             circlespace
    // [x'] = [circlePos] + [ bitangent.x, tangent.x, normal.x ][x]
    // [y'] = [circlePos] + [ bitangent.y, tangent.y, normal.y ][y]
    // [z'] = [circlePos] + [ bitangent.z, tangent.z, normal.z ][z]



    // draw a circle on the x-y plane (perpendicular to the normal)
    // align normal to z,
    Matrix3 circleRot_to_WorldRot = Matrix3::FromColVectors(
      bitangent, tangent, normal);


    static std::vector<Vector3> sPoints;
    sPoints.clear();
    for (unsigned i = 0; i < numSides; ++i)
    {
      float alphaRad = 2.0f * 3.14f * i / numSides;

      float sinAlpha = radius * sin(alphaRad);
      float cosAlpha = radius * cos(alphaRad);

      // vector on x-y plane
      Vector3 circleSpacePoint(cosAlpha, sinAlpha, 0);

      Vector3 worldSpacePoint 
        = pos + circleRot_to_WorldRot * circleSpacePoint;
      sPoints.push_back(worldSpacePoint);
    }
    if (sPoints.empty())
      return;

    // connect all points
    for (unsigned i = 0; i < sPoints.size(); ++i)
    {
      // precedence of % is greater than +
      unsigned wrapAroundIndex = (i + 1) % sPoints.size();

      const Vector3 & p0 = sPoints[i];
      const Vector3 & p1 = sPoints[wrapAroundIndex];
      DrawLine(p0, p1, color);
    }
  }

  void DebugDrawer::DrawCoordinateAxis( 
    float arrowLength, 
    const Matrix4 & transform)
  {
    Vector3 pos(transform.m03, transform.m13, transform.m23);

    // column 1
    Vector3 x(transform.m00, transform.m10, transform.m20);
    x.Normalize();

    // column 2
    Vector3 y(transform.m01, transform.m11, transform.m21);
    y.Normalize();

    // column 3
    Vector3 z(transform.m02, transform.m12, transform.m22);
    z.Normalize();

    // draw each axis
    DrawArrow(pos, x, arrowLength, Color::red  );
    DrawArrow(pos, y, arrowLength, Color::green);
    DrawArrow(pos, z, arrowLength, Color::blue );
  }

  void DebugDrawer::DrawGrid(
    float entireWidth,
    unsigned numDivisions,
    const Vector3 & color,
    const Vector3 & center)
  {
    ErrorIf(numDivisions > 100, "too many grid");

    float cellWidth = entireWidth / numDivisions;
    float halfEntireWidth = entireWidth / 2;
    for (unsigned i = 0; i <= numDivisions; ++i)
    {
      float val = i * cellWidth - halfEntireWidth;
      float mini = -halfEntireWidth;
      float maxi = halfEntireWidth;
      DrawLine(
        center + Vector3(val, mini, 0), 
        center + Vector3(val, maxi, 0), 
        color, color);
      DrawLine(
        center + Vector3(mini, val, 0), 
        center + Vector3(maxi, val, 0), 
        color, color);
    }
  }

  void DebugDrawer::DrawCircleFacingCamera(
    const Vector3 & pos,
    float radius, 
    const Vector3 & color /*= Vector4(1,0,0,1)*/,
    unsigned numSides /*= 17*/ )
  {
    CameraComponent * camComp = mGraphics->GetCurrentCamera();
    Camera & cam = camComp->mCamera;

    DrawCircle(pos, radius, cam.mViewDirection, color, numSides);
  }

  void DebugDrawer::DrawCone( 
    const Vector3 & coneTipPos,
    float radius,
    float height,
    Vector3 normal, 
    const Vector3 & color /*= Vector4(1,0,0,1)*/,
    unsigned numSides )
  {
    Clamp(numSides, 3u, 20u);

    // lets let radius be any number
    ErrorIf(normal.Length() < 0.001f, "Let the normal not be a a zero vector");

    normal.Normalize(); // this is why we copied by value

    // get a tangent vector
    Vector3 tangent = normal.Cross(Vector3(1,0,0));
    if (tangent.Length() == 0)
      tangent = normal.Cross(Vector3(0,1,0));

    tangent.Normalize();
    // once you normalize the tangent
    // when creating the bitangent, it will already be normalized
    Vector3 bitangent = tangent.Cross(normal);


    // worldspace                                             circlespace
    // [x'] = [circlePos] + [ bitangent.x, tangent.x, normal.x ][x]
    // [y'] = [circlePos] + [ bitangent.y, tangent.y, normal.y ][y]
    // [z'] = [circlePos] + [ bitangent.z, tangent.z, normal.z ][z]



    // draw a circle on the x-y plane (perpendicular to the normal)
    // align normal to z,
    Matrix3 circleRot_to_WorldRot = Matrix3::FromColVectors(
      bitangent, tangent, normal);


    static std::vector<Vector3> sPoints;
    sPoints.clear();
    for (unsigned i = 0; i < numSides; ++i)
    {
      float alphaRad = 2.0f * 3.14f * i / numSides;

      float sinAlpha = radius * sin(alphaRad);
      float cosAlpha = radius * cos(alphaRad);

      // vector on x-y plane
      Vector3 circleSpacePoint(cosAlpha, sinAlpha, 0);

      Vector3 worldSpacePoint 
        = coneTipPos + normal * height + circleRot_to_WorldRot * circleSpacePoint;
      sPoints.push_back(worldSpacePoint);
    }
    if (sPoints.empty())
      return;

    // connect all points
    for (unsigned i = 0; i < sPoints.size(); ++i)
    {
      // precedence of % is greater than +
      unsigned wrapAroundIndex = (i + 1) % sPoints.size();

      const Vector3 & p0 = sPoints[i];
      const Vector3 & p1 = sPoints[wrapAroundIndex];
      DrawLine(p0, p1, color);
      DrawLine(p0, coneTipPos, color);
    }
  }

  void DebugDrawer::Update( float dt )
  {
    dt;
    mAccumulatedVertexCount = mPositions.size();
    CopyToBuffer();
    mPositions.clear();
    mColors.clear(); 
  }

  int DebugDrawer::GetDrawableVertexCount() const
  {
    if (mAccumulatedVertexCount > MAX_NUM_VERTS)
    {
      std::cout << "Debug drawing too many lines" << std::endl;
    }
    return std::min(mAccumulatedVertexCount, (int)MAX_NUM_VERTS);
  }

} // Tac

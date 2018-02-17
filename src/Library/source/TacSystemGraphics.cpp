/*!
\author Nathan Park
\par Project: Tac Engine
\brief
*/
#include "TacCore.h"
#include "TacErrorIf.h"
#include "TacGLFWIncludes.h"
#include "TacSystemGraphics.h"
#include "TacGraphicsErrors.h"
#include "TacComponentModel.h"
#include "TacComponentLight.h"
#include "TacComponentCamera.h"
#include "TacComponentTransform.h"
#include <TacBoundingSphere.h>
#include "TacGeometryData.h"
#include "TacMathUtil.h"
#include "TacShader.h"


#include <SOIL.h>
#include <TacException.h>
#include <sstream>
#include <iomanip>
#include <queue>
#include <TacGraphics\TacOperations\TacBindFramebuffer.h>
#include "TacTexture.h"
#include <TacFramebuffer.h>
#include <TacGraphics\TacOperations\TacViewportOperation.h>
#include <TacGraphics\TacOperations\TacDepthEnableOp.h>
#include <TacGraphics\TacOperations\TacDepthFuncOp.h>
#include <TacGraphics\TacOperations\TacDepthMaskOp.h>
#include <TacGraphics\TacOperations\TacClearDepthOp.h>
#include <TacGraphics\TacOperations\TacClearOp.h>
#include <TacGraphics\TacOperations\TacBindCameraUniform.h>
#include <TacGraphics\TacOperations\TacBindUniform.h>

namespace Tac
{

  // https://www.opengl.org/archives/resources/features/OGLextensions/
  int isExtensionSupported(const char *extension)

  {

    const GLubyte *extensions = NULL;

    const GLubyte *start;

    GLubyte *where, *terminator;
    /* Extension names should not have spaces. */

    where = (GLubyte *) strchr(extension, ' ');

    if (where || *extension == '\0')

      return 0;

    extensions = glGetString(GL_EXTENSIONS);

    /* It takes a bit of care to be fool-proof about parsing the

    OpenGL extensions string. Don't be fooled by sub-strings,

    etc. */

    start = extensions;

    for (;;) {

      where = (GLubyte *) strstr((const char *) start, extension);

      if (!where)

        break;

      terminator = where + strlen(extension);

      if (where == start || *(where - 1) == ' ')

        if (*terminator == ' ' || *terminator == '\0')

          return 1;

      start = terminator;

    }

    return 0;

  }

  class RenderCallback : public Callback
  {
    GraphicsSystem * mGraphics;
  public:
    RenderCallback::RenderCallback( 
      GraphicsSystem * graphicsSys ) 
      : mGraphics(graphicsSys)
    {

    }
    virtual void RenderCallback::Execute()
    {
      mGraphics->Render();
    }
  };

  void APIENTRY GraphicsSystem::GLErrorCallback(
    GLenum source, 
    GLenum type, 
    GLuint id, 
    GLenum severity, 
    GLsizei length, 
    const GLchar* message, 
    GLvoid* userParam)
  {
    if(severity == GL_DEBUG_SEVERITY_NOTIFICATION)
      return; 

    GraphicsSystem * system = (GraphicsSystem *) userParam;
    system;
    UnusedParameter(length);

    std::set<unsigned> ignored;
    // i forget what this is
    ignored.insert(131218);
    // buffer performance warning copied from vram to dma cached
    ignored.insert(131186); 
    if(ignored.find(id) != ignored.end())
      return;

    std::string strCur;
#define SetAndBreak(glenum) case glenum: strCur = #glenum; break;
#define SetDefault default: strCur = "ERROR[ Unknown ];"; break;

    switch (source)
    {
      SetAndBreak(GL_DEBUG_SOURCE_API);
      SetAndBreak(GL_DEBUG_SOURCE_WINDOW_SYSTEM);
      SetAndBreak(GL_DEBUG_SOURCE_SHADER_COMPILER);
      SetAndBreak(GL_DEBUG_SOURCE_THIRD_PARTY);
      SetAndBreak(GL_DEBUG_SOURCE_APPLICATION);
      SetAndBreak(GL_DEBUG_SOURCE_OTHER);
      SetDefault
    }
    std::string strSource = strCur;

    switch (type)
    {
      SetAndBreak(GL_DEBUG_TYPE_ERROR);
      SetAndBreak(GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR);
      SetAndBreak(GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR);
      SetAndBreak(GL_DEBUG_TYPE_PORTABILITY);
      SetAndBreak(GL_DEBUG_TYPE_PERFORMANCE);
      SetAndBreak(GL_DEBUG_TYPE_OTHER);
      SetDefault
    }
    std::string strType = strCur;

    switch (severity)
    {
      SetAndBreak(GL_DEBUG_SEVERITY_HIGH);
      SetAndBreak(GL_DEBUG_SEVERITY_MEDIUM);
      SetAndBreak(GL_DEBUG_SEVERITY_LOW);
      SetAndBreak(GL_DEBUG_SEVERITY_NOTIFICATION);
      SetDefault;
    }
    std::string strSeverity = strCur;

    using std::cout; using std::endl; using std::setw;

    static int ErrorCount = 0;
    cout << "<ERROR> " + std::to_string(++ErrorCount)<< endl;
    if ( message )
    {
      cout << "\t" << setw(10) << "Desc: "     << message     << endl;
      cout << "\t" << setw(10) << "Source: "   << strSource   << endl;
      cout << "\t" << setw(10) << "Type: "     << strType     << endl;
      cout << "\t" << setw(10) << "Severity: " << strSeverity << endl;
      cout << "</ERROR>" << endl;
    }
    __debugbreak();
  }

  Glew::Glew()
  {
    GLenum err = glewInit();
    if(err != GLEW_OK)
    {
      std::string errorString("glew init failed: ");
      std::stringstream glewSS;
      glewSS << glewGetErrorString(err);
      throw Exception("glw init failed: " + glewSS.str());
    }
  }

  std::string GetGlString(GLenum toGet)
  {
    std::stringstream ss;
    ss << glGetString(toGet);
    return ss.str();
  }

#ifdef TACTWEAK
  void TW_CALL TweakCallbackRevoxelize(void * data)
  {
    GraphicsSystem * g = (GraphicsSystem*)data;
    g->revoxelize();
  }
#endif

#ifdef TEMPVoxelPoint
  void TW_CALL TweakCallbackChangePoint(void * data)
  {
    GraphicsSystem * sys = (GraphicsSystem*)data;

    std::shared_ptr< GeometryData > testData( new GeometryData() );
    testData->mPos = new Vector3[ 3 ];
    testData->mNor = new Vector3[ 3 ];
    testData->mCounts[ Attribute::ePos ] = 3;
    testData->mCounts[ Attribute::eNor ] = 3;
    for(int i = 0; i < 3; ++i)
    {
      testData->mPos[ i ] = sys->mTEMPpos[ i ];
      testData->mNor[ i ] = sys->mTEMPnor;
    }
    sys->mTEMPDELETEME = std::shared_ptr<Geometry>(new Geometry(testData));
  }
#endif

  GraphicsSystem * GraphicsSystem::gInstance = nullptr;
  GraphicsSystem::GraphicsSystem()
    : mCurrentCamera(nullptr)
    , mCurrentLight(nullptr)
    , mGLVersion(GetGlString(GL_VERSION))
    , mGLSLVersion(GetGlString(GL_SHADING_LANGUAGE_VERSION))
    , mDebugDrawer(nullptr)
    , mOctree(7 // levels
    , Vector3(0,0,0) // center
    , Vector3(10,10,10)) // halfDim
    , mOctreeVisualizer( mOctree.GetPos(), mOctree.GetHalfDim() )
    , mLayerToRender(-1)
    , mClearcolor(  13 / 255.0f,  65 / 255.0f,  37 / 255.0f,  1  )

    , mRenderObjects(true)
    , mRenderDepthBuffer(false)

    , mRenderer(new Renderer())


  {}

  GraphicsSystem::~GraphicsSystem()
  {
    delete mDebugDrawer;
    gInstance = nullptr;
    //for(FramebufferStage * stage : mFramebufferStages)
    //delete stage;
    //mFramebufferStages.clear();
  }


  void GraphicsSystem::Update( float dt )
  {
    UnusedParameter(dt);
    mDebugDrawer->DrawGrid(20, 20);
    mDebugDrawer->DrawCoordinateAxis(20,Matrix4(
      1,0,0,0,
      0,1,0,0,
      0,0,1,0.05f, // slightly above ground
      0,0,0,1
      ));
    mDebugDrawer->Update(dt);

    ComponentSet & cameras = GetComponentSet(Comp::eCompCamera); 
    for(SPComp curSPComp : cameras)
    {
      CameraComponent * curCamera = (CameraComponent *) curSPComp.get();
      TransformComp * cameraTransform;
      GET_SIBL(cameraTransform, curCamera, Comp::eCompTransform);

      curCamera->mCamera.mPosition = cameraTransform->mPos;

    }

    ComponentSet & lights = GetComponentSet(Comp::eCompLight); 
    for(SPComp spLight : lights)
    {
      LightComponent * curLight = (LightComponent*)spLight.get();
      curLight->mCamera.mViewDirection = curLight->mDirection;

      if(curLight->HasSibling(Comp::eCompTransform))
      {
        TransformComp * lightCamTrans;
        GET_SIBL(lightCamTrans, curLight, Comp::eCompTransform);
        curLight->mCamera.mPosition = lightCamTrans->mPos;
      }

      curLight->mView = curLight->mCamera.GetViewMatrix();
      curLight->mProj = curLight->mCamera.GetPerspectiveProjMatrix();

      float fardist = curLight->mCamera.mFar;
      float farW = fardist * tan(curLight->mCamera.mFieldOfViewYRad / 2.0f);
      float drawH = 1.414f * farW;
      mDebugDrawer->DrawCone(curLight->mCamera.mPosition,
        drawH,
        fardist,
        curLight->mDirection, curLight->mColor.XYZ());
    }

    ComponentSet & models = GetComponentSet(Comp::eCompModel); 
    if (models.empty())
    {
      mSceneBounds.mRadius = 0;
      mSceneBounds.mPos = Vector3(0,0,0);
    }
    else
    {
      mSceneBounds = GetSceneBoundingSphere();
      for(SPComp comp : models)
      {
        ModelComponent * curModel = (ModelComponent*)comp.get();
        TransformComp * curTrans;
        GET_SIBL(curTrans, curModel, Comp::eCompTransform);
        const BoundingSphere & bounds 
          = curModel->GetGeometry()->GetBoundingSphere();

        Vector4 toCenter(bounds.mPos, 0.0f);
        toCenter = curTrans->mFakeTransform * toCenter;

        float radius 
          = bounds.mRadius 
          * std::max(std::max(
          curTrans->mSca.x,
          curTrans->mSca.y),
          curTrans->mSca.z);
        mDebugDrawer->DrawCircleFacingCamera(
          curTrans->mPos + toCenter.XYZ(),
          radius, curModel->mColor.XYZ(), 25);
      }
    }
  }

  void GraphicsSystem::UpdateFakeTime( float dt )
  {
    UnusedParameter(dt);
  }

  void GraphicsSystem::Render()
  {

    CheckOpenGLErrors();
    glClearColor(
      mClearcolor.x, mClearcolor.y, mClearcolor.z, mClearcolor.w);
    glClear(
      GL_COLOR_BUFFER_BIT | 
      GL_DEPTH_BUFFER_BIT | 
      GL_STENCIL_BUFFER_BIT);

    CheckOpenGLErrors();
    if(mRenderObjects)
      RenderObjectsShadowed();

    CheckOpenGLErrors();
    if(mRenderDepthBuffer)
      RenderDepthBufferToScreen();

    CheckOpenGLErrors();
    TestOctreeDebugDraw();

    CheckOpenGLErrors();

    mRenderer->Render(GetComponentSet(Comp::eCompModel));

    CheckOpenGLErrors();
  }

  void GraphicsSystem::SetCurrentCamera( 
    CameraComponent * cameraComp )
  {
    mCurrentCamera = cameraComp;
  }

  void GraphicsSystem::SetCurrentLight( 
    LightComponent * lightComp )
  {
    mCurrentLight = lightComp;
  }

  BoundingSphere GraphicsSystem::GetBoundingSphere(
    ModelComponent * iModel) const
  {
    TransformComp * trans 
      = (TransformComp*) iModel->GetSibling(Comp::Type::eCompTransform);
    ErrorIf(!trans, "modelcomp has no transform sibl");
    const Geometry * geom = iModel->GetGeometry();

    if (!geom)
    {
      return BoundingSphere(0, trans->mPos);
    }

    BoundingSphere sphere = geom->GetBoundingSphere();
    if (trans)
    {
      sphere.mPos += trans->mFakePos;
      const Vector3 & scale = trans->mSca;

      using std::max;
      float maxScale = max(max(scale.x, scale.y), scale.z);
      sphere.mRadius *= maxScale;
    }
    return sphere;
  }

  // there must be objects in the scene
  BoundingSphere GraphicsSystem::GetSceneBoundingSphere() const
  {
    const ComponentSet & models = GetComponentSet(Comp::eCompModel);
    ErrorIf(models.empty(), "cannot get bounding sphere of no models");

    auto it = models.begin();
    SPComp comp = *it;
    BoundingSphere sceneBounds = GetBoundingSphere((ModelComponent*) comp.get());
    for (++it; it != models.end(); ++it)
    {
      comp = *it;
      sceneBounds.AddSphere(GetBoundingSphere((ModelComponent*) comp.get()));
    }
    return sceneBounds;
  }


  void GraphicsSystem::RenderObjectsShadowed() const
  {
    if (!mCurrentCamera)
      return;

    if (!mCurrentLight)
      return;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GLuint bufs[] = {GL_BACK_LEFT};
    glDrawBuffers(1, bufs);
    Camera & cam = mCurrentCamera->mCamera;
    Camera & lightCam = mCurrentLight->mCamera;
    Matrix4 lightview = lightCam.GetViewMatrix();
    //Matrix4 lightproj = lightCam.GetOrthographicProjMatrix();
    Matrix4 lightproj = lightCam.GetPerspectiveProjMatrix();

    glClearColor(
      mClearcolor.x, mClearcolor.y, mClearcolor.z, mClearcolor.w);
    glClear(
      GL_DEPTH_BUFFER_BIT | 
      GL_COLOR_BUFFER_BIT | 
      GL_STENCIL_BUFFER_BIT);
    Window & window = GetCore()->GetWindow();
    glViewport(0,0,window.GetWidth(), window.GetHeight());

    mShaderPos.mResource->GetShader().Activate();
    Matrix4 view = cam.GetViewMatrix();
    Matrix4 proj = cam.GetPerspectiveProjMatrix();
    glUniformMatrix4fv(mShaderPos.mView, 1, GL_TRUE, view.v);
    glUniformMatrix4fv(mShaderPos.mProj, 1, GL_TRUE, proj.v);
    glUniform1f(mShaderPos.mLightNear, mCurrentLight->mCamera.mNear);
    glUniform1f(mShaderPos.mLightFar, mCurrentLight->mCamera.mFar);

    const Vector4 & c = mCurrentLight->mColor;
    glUniform4f(mShaderPos.mLightCol, c.x, c.y, c.z, c.w);
    const Vector3 & d = mCurrentLight->mDirection;
    glUniform3f(mShaderPos.mLightDir, d.x, d.y, d.z);

    glUniformMatrix4fv(mShaderPos.mLightView, 1, GL_TRUE, lightview.v);
    glUniformMatrix4fv(mShaderPos.mLightProj, 1, GL_TRUE, lightproj.v);

    // sampler 0 to texture 0
    glUniform1i(mShaderPos.mShadowMap, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    Framebuffer* lightfbo = mCurrentLight->GetFBO();
    Texture * linearDepth = lightfbo->GetTexture(GL_COLOR_ATTACHMENT0);
    glBindTexture(GL_TEXTURE_2D, linearDepth->GetHandle());

    for (SPComp comp: GetComponentSet(Comp::eCompModel))
    {
      ModelComponent * iModel = (ModelComponent*) comp.get();

      // color
      const Vector4 & c = iModel->mColor;
      glUniform4f(mShaderPos.mCol, c.x, c.y, c.z, c.w);

      // world matrix
      Matrix4 world;
      TransformComp * iTransform;
      GET_SIBL(iTransform, iModel, Comp::eCompTransform);
      world = iTransform->mFakeTransform;
      glUniformMatrix4fv(mShaderPos.mWorl, 1, GL_TRUE, world.v);

      // normal matrix
      Matrix4 invtransWorld = world;
      invtransWorld.Invert();
      invtransWorld.Transpose();
      glUniformMatrix4fv(
        mShaderPos.mInvTransWorld, 1, GL_TRUE, invtransWorld.v);

      if(const Geometry * geom = iModel->GetGeometry())
      {
        geom->SendAttribute(Attribute::ePos, mShaderPos.mPos);
        geom->SendAttribute(Attribute::eNor, mShaderPos.mNor);
        geom->Draw();
      }
    }
    mShaderPos.mResource->GetShader().Deactivate();
  }

  void GraphicsSystem::RenderDepthBufferToScreen() const
  {
    if (!mCurrentLight)
      return;

    Window & window = GetCore()->GetWindow();
    int depthW = 200;
    int buf = 10;
    int depthH = (int)((float)depthW / window.GetAspect());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GLuint bufs[] = {GL_BACK_LEFT};
    glDrawBuffers(1, bufs);

    // render the light fbo's linear color texture
    mShaderLinearDepth.mResource->GetShader().Activate();
    glUniform1i(mShaderLinearDepth.mTexture, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, 
      mCurrentLight->GetFBO()->GetTexture(GL_COLOR_ATTACHMENT0)->GetHandle());
    mNDCQuad->SendAttribute(Attribute::ePos, mShaderLinearDepth.mPos);
    mNDCQuad->SendAttribute(Attribute::eUvs, mShaderLinearDepth.mTexCoords);
    glViewport(buf,buf, depthW, depthH);
    mNDCQuad->Draw();
    mShaderLinearDepth.mResource->GetShader().Deactivate();

    // render the light fbo's nonlinear depth texture
    mShaderNonlinearDepth.mResource->GetShader().Activate();
    glUniform1f(mShaderNonlinearDepth.mLightNear, 
      mCurrentLight->mCamera.mNear);
    glUniform1f(mShaderNonlinearDepth.mLightFar, 
      mCurrentLight->mCamera.mFar);
    glUniform1i(mShaderNonlinearDepth.mTexture, 0);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, 
      mCurrentLight->GetFBO()->GetTexture(GL_DEPTH_ATTACHMENT)->GetHandle());
    mNDCQuad->SendAttribute(Attribute::ePos, mShaderLinearDepth.mPos);
    mNDCQuad->SendAttribute(Attribute::eUvs, mShaderLinearDepth.mTexCoords);
    glViewport(buf,depthH + buf + buf, depthW, depthH);
    mNDCQuad->Draw();
    mShaderNonlinearDepth.mResource->GetShader().Deactivate();
  }

  static Matrix3 InverseTranspose(
    const Vector3& scale,
    const Vector3& eulerRadians)
  {
    // (trs)-1
    // (rs)-1
    // (rz ry rx s)-1
    // s-1 rx-1 ry-1 rz-1

    Matrix3 invScale = Matrix3(
      1.0f / scale.x, 0, 0,
      0, 1.0f / scale.y, 0,
      0, 0, 1.0f / scale.z);

    Matrix3 inverseTranspose = invScale 
      * Matrix3::RotRadX(-eulerRadians.x)
      * Matrix3::RotRadY(-eulerRadians.y)
      * Matrix3::RotRadZ(-eulerRadians.z);
    inverseTranspose.Transpose();
    return inverseTranspose;
  }

  CameraComponent * GraphicsSystem::GetCurrentCamera() const
  {
    return mCurrentCamera;
  }

  void GraphicsSystem::TestOctreeDebugDraw() const
  {
    if (!mCurrentCamera 
      || mLayerToRender < 0 
      || mLayerToRender >= (int)mOctree.GetNumLevels() )
      return;

    const Camera & cam = mCurrentCamera->mCamera;

    Window & wind =  GetCore()->GetWindow();
    glViewport(0, 0, wind.GetWidth(), wind.GetHeight());

    Matrix4 viewMat = cam.GetViewMatrix();
    Matrix4 projmat = cam.GetPerspectiveProjMatrix();
    OctreeVis::Scope scope = mOctreeVisualizer.OpenScope( 
      viewMat, 
      projmat );


    std::queue<unsigned> layerIndexes;
    std::queue<Vector3> nodePositions;
    std::queue<Vector3> nodeHalfDims;

    layerIndexes.push( 0 );
    nodePositions.push( mOctree.GetPos() );
    nodeHalfDims.push( mOctree.GetHalfDim() );


    for(int curLayer = 0; curLayer < mLayerToRender; ++curLayer)
    {
      unsigned nodesInLayer = layerIndexes.size();
      while(nodesInLayer--)
      {

        unsigned nodeIndex = layerIndexes.front();
        layerIndexes.pop();

        Vector3 nodePos = nodePositions.front();
        nodePositions.pop();

        Vector3 nodeHalfDim = nodeHalfDims.front();
        nodeHalfDims.pop();


        const OctreeNode * node = mOctree.GetLevel(curLayer) + nodeIndex;

        ErrorIf( node->childTileIndex == OctreeNode::NULL_CHILD, 
          "Every non-leaf node must have a child. ");

        const OctreeNode * childOctet 
          = mOctree.GetLevel(curLayer + 1) + node->childTileIndex;

        Vector3 childHalfDim = nodeHalfDim / 2.0f;

        for(unsigned octetIndex = 0; octetIndex < 8; ++octetIndex)
        {
          const OctreeNode * child = childOctet + octetIndex;
          if( !child->used )
            continue;

          Vector3 dir = Octree::Direction(octetIndex);
          Vector3 childPos = nodePos; 
          for( int axis = 0; axis < 3; ++axis)
            childPos[axis] += dir[axis] * childHalfDim[axis];

          layerIndexes.push(node->childTileIndex + octetIndex);
          nodePositions.push(childPos);
          nodeHalfDims.push(childHalfDim);
        }
      }
    }

    int nodesToRender = nodePositions.size();
    while(nodesToRender--)
    {
      Vector3 nodePos = nodePositions.front();
      nodePositions.pop();

      Vector3 nodeHalfDim = nodeHalfDims.front();
      nodeHalfDims.pop();

      scope.RenderNode(nodePos, nodeHalfDim);
    }
  }

  void GraphicsSystem::revoxelize()
  {
    mOctree.Clear();
    auto compSet = GetComponentSet(Comp::eCompModel);

    if (!mCurrentCamera)
      return;

    GLsizei dim = mOctree.GetDimension();
    glViewport( 0, 0, dim, dim );

    mShaderVoxelFrags.mResource->GetShader().Activate();
    glDrawBuffer(GL_NONE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    GLuint zero = 0;
    glBufferSubData(
      GL_ATOMIC_COUNTER_BUFFER, 
      0,
      sizeof(GLuint), 
      &zero);
    glUniform1ui(mShaderVoxelFrags.mMaxFrags, mMaxNumFragments);
    Matrix4 view = mCurrentCamera->mCamera.GetViewMatrix();
    Matrix4 proj = mCurrentCamera->mCamera.GetPerspectiveProjMatrix();
    glUniformMatrix4fv(mShaderVoxelFrags.mView, 1, GL_TRUE, view.v);
    glUniformMatrix4fv(mShaderVoxelFrags.mProj, 1, GL_TRUE, proj.v);

    const Vector3 & halfDim = mOctree.GetHalfDim();

    Matrix4 viewProjs[3];

    Matrix4 worldToNDC = Matrix4( // project on x axis
      1.0f / halfDim.x, 0, 0, 0,
      0, 1.0f / halfDim.y, 0, 0,
      0, 0, 1.0f / halfDim.z, 0,
      0, 0, 0, 1);

    // [ y z x 1 ]
    viewProjs[ 0 ] = worldToNDC * Matrix4(
      0, 1, 0, 0,
      0, 0, 1, 0,
      1, 0, 0, 0,
      0, 0, 0, 1 );
    // [ x z y 1 ]
    viewProjs[ 1 ] = worldToNDC * Matrix4(
      1, 0, 0, 0,
      0, 0, 1, 0,
      0, 1, 0, 0,
      0, 0, 0, 1 );
    // [ x y z 1 ]
    viewProjs[ 2 ] = worldToNDC;

    glUniformMatrix4fv(
      mShaderVoxelFrags.mViewProjs, 3, GL_TRUE, ( float* )viewProjs );

    for (SPComp comp: compSet)
    {
      ModelComponent * iModel = (ModelComponent*) comp.get();
      Matrix4 world;
      TransformComp * iTransform
        = (TransformComp*) comp->GetSibling(Comp::Type::eCompTransform);
      if (iTransform)
        world = iTransform->mFakeTransform;
      glUniformMatrix4fv(mShaderVoxelFrags.mWorl, 1, GL_TRUE, world.v);

      Matrix3 worldNormal = InverseTranspose(
        iTransform->mSca,
        iTransform->mRot);

      glUniformMatrix3fv(
        mShaderVoxelFrags.mWorlNormal, 1, GL_TRUE, worldNormal.v);

      // color
      const Vector4 & c = iModel->mColor;
      glUniform4f(mShaderVoxelFrags.mCol, c.x, c.y, c.z, c.w);

      if(const Geometry * geom = iModel->GetGeometry())
      {
        geom->SendAttribute(Attribute::ePos, mShaderVoxelFrags.mPos);
        geom->SendAttribute(Attribute::eNor, mShaderVoxelFrags.mNor);
        geom->Draw();
      }
    }

#ifdef TEMPVoxelPoint
    if( mTEMPDELETEME)
    {
      glUniform4f(mShaderVoxelFrags.mCol, 1, 1, 1, 1);
      Matrix4 identity4;
      glUniformMatrix4fv(mShaderVoxelFrags.mWorl, 1, GL_TRUE, identity4.v );
      Matrix3 identity3;
      glUniformMatrix3fv(mShaderVoxelFrags.mWorlNormal, 1, GL_TRUE, identity3.v );
      mTEMPDELETEME->SendAttribute(Attribute::ePos, mShaderVoxelFrags.mPos);
      mTEMPDELETEME->SendAttribute(Attribute::eNor, mShaderVoxelFrags.mNor);
      mTEMPDELETEME->Draw();
    }
#endif


    mShaderVoxelFrags.mResource->GetShader().Deactivate();

    glDrawBuffer(GL_BACK_LEFT);
    glClear(GL_DEPTH_BUFFER_BIT);

    glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
    void * ptr = glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_READ_ONLY);
    GLuint numIncrements = *reinterpret_cast<GLuint*>(ptr);

    unsigned numPixelsToDraw = std::min(numIncrements, mMaxNumFragments);

    // just inspectin
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    void * pVoid = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    Frag * fragPtr = reinterpret_cast<Frag*>(pVoid);

    for(
      unsigned levelIndexToMark = 1; 
      levelIndexToMark < mOctree.GetNumLevels();
    ++levelIndexToMark )
    {
      std::cout 
        << "voxelizing level: " 
        << levelIndexToMark 
        << " of " 
        << mOctree.GetNumLevels() - 1
        << std::endl;
      // lets build our octree here
      for( Frag * frag = fragPtr; frag < fragPtr + numPixelsToDraw; ++frag)
      {
        mOctree.MarkCellToAdd(frag->fragPos, levelIndexToMark);
      }

      // the last level are leaf nodes, dont allocate children
      unsigned leafNodeLevelIndex = mOctree.GetNumLevels() - 1;
      if( levelIndexToMark < leafNodeLevelIndex)
      {
        mOctree.AllocateMarkedNodes(levelIndexToMark);
      }
      //mOctree.Print();
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
    static int num = 0;
    std::cout << "voxelizing done! ( times voxelized: " << ++num 
      << " )" << std::endl << std::endl;
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
  }

  Renderer * GraphicsSystem::GetRenderer()
  {
    return mRenderer.get();
  }

  DebugDrawer & GraphicsSystem::GetDebugDrawer()
  {
    return *mDebugDrawer;
  }

  void GraphicsSystem::Init()
  {

    CheckOpenGLErrors();
    ErrorIf(gInstance, "already exists");
    gInstance = this;

#ifdef TACTWEAK
    mRenderBarIsWunderBar = GetCore()->GetTweak().Create("TacGraphics");
    mRenderBarIsWunderBar->SetPosition(25,25);
    mRenderBarIsWunderBar->SetSize(250,300);
    mRenderBarIsWunderBar->SetIconified(true);
    mRenderBarIsWunderBar->AddVar(
      "ClearColor", "label='Clear Color'", TW_TYPE_COLOR4F, &mClearcolor);
    mRenderBarIsWunderBar->AddVar(
      "renderObjects", 
      "label='render objects'", TW_TYPE_BOOLCPP, &mRenderObjects);
    mRenderBarIsWunderBar->AddVar(
      "renderDepthBuffer", 
      "label='render depth buffer'", TW_TYPE_BOOLCPP, &mRenderDepthBuffer);

    std::string params("label='Octree Level Index' min=-1 max=");
    params += std::to_string(mOctree.GetNumLevels() - 1);
    mOctreeVisBar = GetCore()->GetTweak().Create("OctreeVis");
    int width = 270;
    int buffer = 10;
    int pos = GetCore()->GetWindow().GetWidth() - buffer - width;
    mOctreeVisBar->SetPosition(pos,buffer);
    mOctreeVisBar->SetSize(width,200);
    mOctreeVisBar->AddVar("OctreeLevel", params, 
      TW_TYPE_INT32, &mLayerToRender);
    mOctreeVisBar->AddButton(
      "revis", "Revoxelize", TweakCallbackRevoxelize, this);
    mOctreeVisBar->SetIconified( false );
#endif

    CheckOpenGLErrors();
    // set up system callback
    GetCore()->SetRenderCallback(SPCallback(new RenderCallback(this)));
    ResourceManager & manager = *GetCore()->mResourceManager;

    std::cout << "OpenGL version: " << mGLVersion   << std::endl;
    std::cout << "GLSL   version: " << mGLSLVersion << std::endl;
    const char * errmsg = "Your graphics card sucks";
    ErrorIf(mGLVersion  [0] < '4', errmsg);
    ErrorIf(mGLSLVersion[0] < '4', errmsg);


    ShaderResource * resourceFSTex = new ShaderResource();
    resourceFSTex->AddShader(ShaderResource::eVertexShader,
      "resources/TacShaders/ShaderFullscreenTexture.v.glsl");
    resourceFSTex->AddShader(ShaderResource::eFragmentShader,
      "resources/TacShaders/ShaderFullscreenTexture.f.glsl");
    resourceFSTex->Init();
    manager.AddResource(resourceFSTex);
    mShaderFSTex.BindToResource(resourceFSTex);

    ShaderResource * resourcePixels = new ShaderResource();
    resourcePixels->AddShader(ShaderResource::eVertexShader,
      "resources/TacShaders/ShaderPixels.v.glsl");
    resourcePixels->AddShader(ShaderResource::eFragmentShader,
      "resources/TacShaders/ShaderPixels.f.glsl");
    resourcePixels->Init();
    manager.AddResource(resourcePixels);
    mShaderPixels.BindToResource(resourcePixels);

    ShaderResource * resourcePos = new ShaderResource();
    resourcePos->AddShader(ShaderResource::eVertexShader,
      "resources/TacShaders/ShaderPos.v.glsl");
    resourcePos->AddShader(ShaderResource::eFragmentShader,
      "resources/TacShaders/ShaderPos.f.glsl");
    resourcePos->Init();
    manager.AddResource(resourcePos);
    mShaderPos.BindToResource(resourcePos);

    ShaderResource * resourceVoxFrags = new ShaderResource();
    resourceVoxFrags->AddShader(ShaderResource::eVertexShader,
      "resources/TacShaders/ShaderVoxelFragments.v.glsl");
    resourceVoxFrags->AddShader(ShaderResource::eGeometryShader,
      "resources/TacShaders/ShaderVoxelFragments.g.glsl");
    resourceVoxFrags->AddShader(ShaderResource::eFragmentShader,
      "resources/TacShaders/ShaderVoxelFragments.f.glsl");
    resourceVoxFrags->Init();
    manager.AddResource(resourceVoxFrags);
    mShaderVoxelFrags.BindToResource(resourceVoxFrags);

    ShaderResource * resourceNonlinearDepth = new ShaderResource();
    resourceNonlinearDepth->AddShader(ShaderResource::eVertexShader,
      "resources/TacShaders/ShaderFullscreenTexture.v.glsl");
    resourceNonlinearDepth->AddShader(ShaderResource::eFragmentShader,
      "resources/TacShaders/ShaderNonlinearDepth.f.glsl");
    resourceNonlinearDepth->Init();
    manager.AddResource(resourceNonlinearDepth);
    mShaderNonlinearDepth.BindToResource(resourceNonlinearDepth);

    ShaderResource * resourceLinearDepth = new ShaderResource();
    resourceLinearDepth->AddShader(ShaderResource::eVertexShader,
      "resources/TacShaders/ShaderFullscreenTexture.v.glsl");
    resourceLinearDepth->AddShader(ShaderResource::eFragmentShader,
      "resources/TacShaders/ShaderLinearDepth.f.glsl");
    resourceLinearDepth->Init();
    manager.AddResource(resourceLinearDepth);
    mShaderLinearDepth.BindToResource(resourceLinearDepth);

    ShaderResource * resourceOctreeVis = new ShaderResource();
    resourceOctreeVis->AddShader(ShaderResource::eVertexShader,
      "resources/TacShaders/ShaderOctreeVis.v.glsl");
    resourceOctreeVis->AddShader(ShaderResource::eFragmentShader,
      "resources/TacShaders/ShaderOctreeVis.f.glsl");
    resourceOctreeVis->Init();
    manager.AddResource(resourceOctreeVis);
    mOctreeVisualizer.BindToShader(resourceOctreeVis);

    CheckOpenGLErrors();
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glClearColor(
      mClearcolor.x, mClearcolor.z, mClearcolor.z, mClearcolor.w);
    glClearDepth(1.0f);
    glClear(
      GL_COLOR_BUFFER_BIT | 
      GL_DEPTH_BUFFER_BIT | 
      GL_STENCIL_BUFFER_BIT);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);


    // we want a gl debug callback
    glEnable(GL_DEBUG_OUTPUT);
    // we want the callback to execute immediately
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    //GLDEBUGPROC errorHandler = &GraphicsSystem::GLErrorCallback;
    glDebugMessageCallback( &GLErrorCallback, this ); 

    // for drawing the depth buffer to the screen
    std::shared_ptr<GeometryData> ndcquaddata(new GeometryData());
    ndcquaddata->LoadNDCQuad();

    mNDCQuad = SPGeometry(new Geometry(ndcquaddata));

    // voxelization
    glGenBuffers(1, &mFragmentBuffer);
    glBindBufferBase(
      GL_SHADER_STORAGE_BUFFER, 
      ShaderVoxelFragments::mFragBufferBinding, 
      mFragmentBuffer);
    glBufferData(
      GL_SHADER_STORAGE_BUFFER,
      mMaxNumFragments * sizeof(Frag),
      nullptr,
      GL_DYNAMIC_DRAW);

    // i think glBindBufferBase alone sets the buffer's binding point
    glGenBuffers(1, &mCounterBuffer);
    glBindBufferBase(
      GL_ATOMIC_COUNTER_BUFFER, 
      ShaderVoxelFragments::mFragCountBinding, 
      mCounterBuffer);

    glBufferData(
      GL_ATOMIC_COUNTER_BUFFER, 
      sizeof(GLuint), 
      nullptr,
      GL_DYNAMIC_DRAW);

    mDebugDrawer = new DebugDrawer(this);
    GetCore()->PrintToStatus("GraphicsSystem::GraphicsSystem()");
  
  }

} // Tac

/*!
\author Nathan Park
\brief
*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#ifndef __TAC_SYSTEM_GRAPHICS_H_
#define __TAC_SYSTEM_GRAPHICS_H_

#include "TacSystem.h"
#include "TacCallback.h"

#include "TacShaderPos.h"
#include "TacShaderShadow.h"

#include "TacVector3.h"
#include "TacVector4.h"
#include "TacBoundingSphere.h"
#include "TacShaderFullscreenTexture.h"
#include "TacGeometry.h"
#include "TacShaderVoxelFragments.h"
#include "TacShaderPixels.h"
#include "TacShaderNonlinearDepth.h"

#include "TacOctree.h"
#include "TacOctreeVisualizer.h"

// temp
#include "TacCS260AsteroidsCommon.h"
#include "TacShaderAsteroids.h"
#include "TacShaderAssBackwards.h"
#include "TacDebugDraw.h"

#include "TacGraphics/TacFramebufferStage.h"
#include <TacGraphics/TacRenderer.h>

#define TEMPVoxelPointx


namespace Tac
{
  class CameraComponent;
  class LightComponent;
  class ModelComponent;
  class Glew
  {
  public:
    Glew();
  };

  namespace Tacchic
  {
    class Renderer;
    class AccelerationStructure;
  }
  class GraphicsSystem : public System
  {
  public:
    Tacchic::Renderer * mTacchicRenderer;
    std::map<std::string, Tacchic::AccelerationStructure*> mGroups;








  public:
    TacDLLAPI GraphicsSystem();
    TacDLLAPI virtual ~GraphicsSystem();
    TacDLLAPI virtual void Init()override;
    TacDLLAPI virtual void Update(float dt);
    TacDLLAPI virtual void UpdateFakeTime(float dt);
    TacDLLAPI void Render();

    // you can use System::GetRandomComponent(type) to get these
    TacDLLAPI void SetCurrentCamera(CameraComponent * cameraComp);
    TacDLLAPI CameraComponent * GetCurrentCamera() const;
    TacDLLAPI void SetCurrentLight(LightComponent * lightComp);

    // there must be objects in the scene
    TacDLLAPI BoundingSphere GetSceneBoundingSphere() const;
    TacDLLAPI BoundingSphere GetBoundingSphere(ModelComponent * modl) const;
    TacDLLAPI DebugDrawer & GetDebugDrawer();

    void revoxelize();
    void AddFrameBufferStage(FramebufferStage* stage);

#ifdef TACTWEAK
    SPTweakBar mRenderBarIsWunderBar;
#endif // TACTWEAK

    static GraphicsSystem * gInstance;

    Renderer * GetRenderer();
  private:

    std::unique_ptr<Tac::Renderer> mRenderer;

    Vector4 mClearcolor;
    Glew mGlew;

    static void APIENTRY GLErrorCallback(
      GLenum source, 
      GLenum type, 
      GLuint id, 
      GLenum severity, 
      GLsizei length, 
      const GLchar* message, 
      GLvoid* userParam);
    std::string mGLVersion;
    std::string mGLSLVersion;

    // maybe these should be weakpointers
    CameraComponent * mCurrentCamera;
    LightComponent * mCurrentLight;
    
    void RenderDepthBufferToScreen() const;
    void RenderObjectsShadowed() const;
    void TestOctreeDebugDraw() const;
    SPGeometry mNDCQuad;
    BoundingSphere mSceneBounds;

    GLuint mFragmentBuffer;
    GLuint mCounterBuffer;

    ShaderFullscreenTexture mShaderFSTex;
    ShaderPixels mShaderPixels;
    ShaderPos mShaderPos;
    ShaderVoxelFragments mShaderVoxelFrags;
    ShaderNonlinearDepth mShaderNonlinearDepth;
    ShaderLinearDepth mShaderLinearDepth;
    ShaderResource * TEMPresourceshadow;
    static const unsigned mMaxNumFragments = 1000000;


    Octree mOctree;
    OctreeVis mOctreeVisualizer;
    int mLayerToRender; // -1 means draw none
#ifdef TACTWEAK
    SPTweakBar mOctreeVisBar;
#endif
    
    DebugDrawer * mDebugDrawer;

#ifdef TEMPVoxelPoint
    ////////////////////////////
    // HERE DOWNWARDS IS TEMP // 
    ////////////////////////////
  public:
    SPGeometry mTEMPDELETEME;
    Vector3 mTEMPpos[3];
    Vector3 mTEMPnor;
#ifdef TACTWEAK
    SPTweakBar mTEMPTWEAKBAR;
#endif

#endif

#ifdef TACTWEAK
    SPTweakBar mGraphicsFrontendBar;
#endif // TACTWEAK
    bool mRenderObjects;
    bool mRenderDepthBuffer;
  };

  // base this from ShaderVoxelFragments.f.glsl
  struct Frag
  {
    Frag() : fragPos(0,0,0), padding(0), fragCol(0,0,0,0){}
    Vector3 fragPos;
    unsigned padding;
    // fragCol, a vec4 must be aligned on sizeof(vec4) boundaries
    Vector4 fragCol;
  }; // total size: 32 bytes (a multiple of vec4)

} // Tac

#endif

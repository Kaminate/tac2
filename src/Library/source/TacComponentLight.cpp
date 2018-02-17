/*!
\author Nathan Park
*/
#include "TacComponentLight.h"
#include "TacMathUtil.h"


// pass
#include "TacFramebuffer.h"
#include "TacSystemGraphics.h"
#include "TacResourceManager.h"
#include "TacGraphics\TacFramebufferStage.h"
#include "TacGraphics\TacOperations\TacBindUniform.h"
#include "TacGraphics\TacOperations\TacBindAttribute.h"
#include "TacGraphics\TacOperations\TacBindCameraUniform.h"
#include "TacGraphics\TacOperations\TacBindModelMatrix.h"
#include "TacGraphics\TacOperations\TacDrawOperation.h"
#include "TacGraphics\TacOperations\TacViewportOperation.h"
#include "TacGraphics\TacOperations\TacDepthEnableOp.h"
#include "TacGraphics\TacOperations\TacDepthFuncOp.h"
#include "TacGraphics\TacOperations\TacColorMaskOp.h"
#include "TacGraphics\TacOperations\TacDepthMaskOp.h"
#include "TacGraphics\TacOperations\TacClearDepthOp.h"
#include "TacGraphics\TacOperations\TacClearOp.h"
namespace Tac
{
  LightComponent::LightComponent()
    : mDirection(0,0,-1)
    , mColor(1,1,1,1)
    , lightFBOstage(nullptr)
    , mFBO(nullptr)
    , lightNearData(GL_FLOAT, 1, "light camera near", &mCamera.mNear)
    , lightFarData(GL_FLOAT, 1, "light camera far", &mCamera.mFar)
    , mProjData(GL_FLOAT_MAT4, 1, "light proj matrix", &mProj)
    , mViewData(GL_FLOAT_MAT4, 1, "light view matrix", &mView)
  {
    mCamera.mAspectRatio = 1;
    mCamera.mNear = 0.01f;
    mCamera.mFar = 69.0f;
    mCamera.mFieldOfViewYRad = ToRadians(90);
    mCamera.mPosition; // set during grpahics::fakeupdate
    mCamera.mViewDirection; // set during graphics::fakeupdate
    mCamera.mWorldSpaceUp = Vector3(0,0,1);
    mCamera.mWidth = 50.0f;
    mCamera.mHeight = 50.0f;
  }

  LightComponent::~LightComponent()
  {
  }

  void LightComponent::Load( Loader & serializer )
  {
    serializer.Load(mDirection);
    serializer.Load(mColor);
    serializer.Load(mCastsShadows);
    if(mCastsShadows)
    {
      AddShadowMapCreationPass();
    }
  }

  void LightComponent::Save( Saver & serializer )
  {
    serializer.SaveFormatted("direction", mDirection);
    serializer.SaveFormatted("color", mColor);
    serializer.SaveFormatted("castsShadows", mCastsShadows);
  }

  bool LightComponent::Initialize()
  {
    return true;
  }

  void LightComponent::AddShadowMapCreationPass()
  {
    GraphicsSystem * graphicsSys = (GraphicsSystem*)Comp::GetSystem();
    // v
    lightFBOstage = new FramebufferStage();
    lightFBOstage->SetName("create shadow map");
    lightFBOstage->GetActiveRef() = true;
    ResourceManager::Instance().AddResource(lightFBOstage);

    mFBO = new Framebuffer("shadow map fbo");
    ResourceManager::Instance().AddResource(mFBO);
    Texture::Properties props;
    props.mBorder = 0;
    props.mDepth = 0;
    props.mFormat = GL_DEPTH_COMPONENT;
    props.mHeight = 1024; // width must equal height for 90 fov
    props.mWidth =  1024; // width must equal height for 90 fov
    props.mInternalFormat = GL_DEPTH_COMPONENT32;
    props.mPixelType = GL_FLOAT;
    props.mTarget = GL_TEXTURE_2D;
    // todo: replace with a TextureResource
    Texture * depthTex = new Texture(props, nullptr);
    glBindTexture(GL_TEXTURE_2D, depthTex->GetHandle());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    mFBO->AttachTexture(depthTex, GL_DEPTH_ATTACHMENT);

    props.mFormat = GL_RED;
    props.mInternalFormat = GL_R32F;
    Texture * depthColorTex = new Texture(props, nullptr);
    glBindTexture(GL_TEXTURE_2D, depthColorTex->GetHandle());
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    mFBO->AttachTexture(depthColorTex, GL_COLOR_ATTACHMENT0);

    mFBO->CheckCompleteness();

    std::vector<GLenum> drawBufs;
    drawBufs.push_back(GL_COLOR_ATTACHMENT0);
    lightFBOstage->SetFramebuffer(mFBO, drawBufs);
    graphicsSys->GetRenderer()->AddFramebufferStage(lightFBOstage);

    ShaderPass *lightPass = new ShaderPass();
    ResourceManager::Instance().AddResource(lightPass);
    lightFBOstage->AddShaderPass(lightPass);

    ShaderResource * resourceShadow = new ShaderResource();
    resourceShadow->AddShader(ShaderResource::eVertexShader,
      "resources/TacShaders/ShaderShadow.v.glsl");
    resourceShadow->AddShader(ShaderResource::eFragmentShader,
      "resources/TacShaders/ShaderShadow.f.glsl");
    resourceShadow->Init();
    ResourceManager::Instance().AddResource(resourceShadow);
    lightPass->SetShader(resourceShadow);

    lightPass->AddPreRenderOp(new BindViewportOperation(0,0));
    lightPass->AddPreRenderOp(new DepthEnableOp(true));
    lightPass->AddPreRenderOp(new DepthFuncOp(GL_LESS));
    lightPass->AddPreRenderOp(new DepthMaskOp(true));
    lightPass->AddPreRenderOp(new ClearDepthDistOp(1));
    lightPass->AddPreRenderOp(new ClearColorOp(1,1,1,1));
    lightPass->AddPreRenderOp(new ClearOp(ClearOp::eDepth|ClearOp::eColor));
    lightPass->AddPreRenderOp(new BindUniform(resourceShadow->GetUniform("view"), &mViewData));
    lightPass->AddPreRenderOp(new BindUniform(resourceShadow->GetUniform("proj"), &mProjData));
    lightPass->AddPreRenderOp(new BindUniform(
      resourceShadow->GetUniform("lightNear"), &lightNearData));
    lightPass->AddPreRenderOp(new BindUniform(
      resourceShadow->GetUniform("lightFar"), &lightFarData));
    lightPass->AddModelRenderOperation(new BindModelMatrix(
      resourceShadow->GetUniform("world"),
      BindModelMatrix::eWorld));
    lightPass->AddModelRenderOperation(new BindAttribute(
      resourceShadow->GetAttribute("position"),
      Attribute::ePos,
      nullptr));
    lightPass->AddModelRenderOperation(new DrawOperation(GL_TRIANGLES));

#ifdef TACTWEAK
    std::string tweakGroup(" group=shadowmap ");
    graphicsSys->mRenderBarIsWunderBar->AddVar(
      "ShadowmapActive",
      "label='Active'" + tweakGroup,
      TW_TYPE_BOOLCPP, &lightFBOstage->GetActiveRef());
#endif
  }

  void LightComponent::RemShadowMapCreationPass()
  {
    GraphicsSystem * gSys = (GraphicsSystem*)Comp::GetSystem();
    Renderer * renderMan = gSys->GetRenderer();
    renderMan->RemoveFramebufferStage(lightFBOstage);
    lightFBOstage = nullptr;
    mFBO = nullptr;
  }	

  bool LightComponent::CastsShadows() const 
  { 
    return mCastsShadows;
  }

  void LightComponent::SetShadowCasting(bool castsShadows)
  {
    if(castsShadows != mCastsShadows)
    {
      if(mCastsShadows)
      {
        RemShadowMapCreationPass();
      }
      else
      {
        AddShadowMapCreationPass();
      }

      mCastsShadows = castsShadows;
    }
  }

#ifdef TACTWEAK

  void LightComponent::AddToTweakBar( TweakBar & bar )
  {
    bar.AddVar("Light Direction", 
      "group=Light "
      "label='wsDirection '",
      TW_TYPE_DIR3F, &mDirection);
    bar.AddVar("Light Color", 
      "group=Light "
      "label='Color '",
      TW_TYPE_COLOR4F, &mColor);

    bar.AddVar("Light Distance", "group=Light "
      "label='Light dist (camera far)'", TW_TYPE_FLOAT, 
      &mCamera.mFar);
    
    typedef std::function<void(void*)> GetterFn;
    typedef std::function<void(const void*)> SetterFn;

    bar.AddVarCB("castshadows", "label='cast shadows'", TW_TYPE_BOOLCPP,
      [this](void*val){
      *((bool*)val) = mCastsShadows;
    },
      [this](const void* val){
        bool castsShadows = *((bool*)val);
        SetShadowCasting(castsShadows);
    });

  }

  void LightComponent::RemoveFromTweakBar( TweakBar & bar )
  {
    bar.RemoveVar("Light Direction");
    bar.RemoveVar("Light Color");
    bar.RemoveVar("Light Distance");
  }

#endif

  Framebuffer * LightComponent::GetFBO()
  {
    return mFBO;
  }

} // Tac

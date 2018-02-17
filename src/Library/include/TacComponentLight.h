/*!
\author Nathan Park
*/
#ifdef TacDLL_Export
#define TacDLLAPI __declspec(dllexport)
#else
#define TacDLLAPI __declspec(dllimport)
#endif

#pragma once

#include "TacComponent.h"
#include "TacVector3.h"
#include "TacVector4.h"
#include "TacGLIncludes.h"
#include "TacCamera.h"
#include "TacGraphics\TacShaderData.h"

namespace Tac
{
  class Framebuffer;
  class FramebufferStage;
  class LightComponent : public Comp
  {
  public:
    LightComponent();
    ~LightComponent();
    virtual void Load(Loader & serializer)override;
    virtual void Save(Saver & serializer)override;
    virtual bool Initialize()override;

#ifdef TACTWEAK
    virtual void AddToTweakBar(TweakBar & bar)override;
    virtual void RemoveFromTweakBar(TweakBar & bar)override;
#endif

    Vector3 mDirection;
    Vector4 mColor;
    Camera mCamera;

    void AddShadowMapCreationPass();
    void RemShadowMapCreationPass();

    bool CastsShadows() const;
    void SetShadowCasting(bool castsShadows);
    TacDLLAPI Framebuffer * GetFBO();
    ShaderData lightFarData;
    ShaderData lightNearData;

    Matrix4 mView;
    Matrix4 mProj;
    ShaderData mViewData;
    ShaderData mProjData;
  private:
    bool mCastsShadows;

    // owned by resource manager
    FramebufferStage * lightFBOstage;
    Framebuffer * mFBO;
  };
  DefineSharedPtr(LightComponent);

} // Tac

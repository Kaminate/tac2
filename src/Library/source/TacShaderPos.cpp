// Nathan Park
#include "TacShaderPos.h"
namespace Tac
{
  void ShaderPos::OnReload()
  {
    Shader & s = ShaderView::mResource->GetShader();
    mPos = s.GetAttrib("position");
    mNor = s.GetAttrib("normal");

    mCol = s.GetUnifor( "color" );
    mWorl = s.GetUnifor( "world" );
    mView = s.GetUnifor( "view" );
    mProj = s.GetUnifor( "proj" );
    mLightDir = s.GetUnifor( "wsLightDir" );
    mLightCol = s.GetUnifor( "lightCol" );
    mInvTransWorld = s.GetUnifor( "invTransWorld" );
    mShadowMap = s.GetUnifor( "shadowMap" );
    mLightView = s.GetUnifor( "lightView" );
    mLightProj = s.GetUnifor( "lightProj" );
    mLightNear = s.GetUnifor( "lightNear" );
    mLightFar = s.GetUnifor( "lightFar" );
  }
} // Tac
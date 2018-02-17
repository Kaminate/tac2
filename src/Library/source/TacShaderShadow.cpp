// Nathan Park
#include "TacShaderShadow.h"
namespace Tac
{
  void ShaderShadow::OnReload()
  {
    Shader & s = ShaderView::mResource->GetShader();
    mPos       = s.GetAttrib("position");

    mWorl      = s.GetUnifor("world");
    mView      = s.GetUnifor("view");
    mProj      = s.GetUnifor("proj");
    mLightNear = s.GetUnifor("lightNear");
    mLightFar  = s.GetUnifor("lightFar");
  }
}
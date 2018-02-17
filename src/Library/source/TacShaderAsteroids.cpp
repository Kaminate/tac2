// Nathan Park
#include "TacShaderAsteroids.h"
namespace Tac
{
  void ShaderAsteroids::OnReload()
  {
    Shader & s = ShaderView::mResource->GetShader();

    mPos  = s.GetAttrib("position");
    mNor  = s.GetAttrib("normal");

    mCol  = s.GetUnifor("color");
    mWorl = s.GetUnifor("world");
    mView = s.GetUnifor("view");
    mProj = s.GetUnifor("proj");
  }
}

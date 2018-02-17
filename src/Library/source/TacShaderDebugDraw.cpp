// Nathan Park
#include "TacShaderDebugDraw.h"
namespace Tac
{
  void ShaderDebugDraw::OnReload()
  {
    Shader & s = ShaderView::mResource->GetShader();
    mPos = s.GetAttrib("position");
    mCol = s.GetAttrib("color");

    mView = s.GetUnifor("view");
    mProj = s.GetUnifor("proj");
  }
} // Tac
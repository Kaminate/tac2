// Nathan Park
#include "TacShaderPixels.h"
namespace Tac
{
  void ShaderPixels::OnReload()
  {
    Shader & s = mResource->GetShader();
    mPos = s.GetAttrib("position");
    mCol = s.GetAttrib("color");

    mView = s.GetUnifor( "view");
    mProj = s.GetUnifor( "proj");
  }
}
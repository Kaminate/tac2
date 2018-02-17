// Nathan Park
#include "TacShaderViewNormals.h"
namespace Tac
{
  void ShaderViewNor::OnReload()
  {
    Shader & s = ShaderView::mResource->GetShader();
    mPos = s.GetAttrib("position");
    mNor = s.GetAttrib("normal");

    mWorl = s.GetUnifor( "world" );
    mView = s.GetUnifor( "view" );
    mProj = s.GetUnifor( "proj" );
    mInvTransWorld = s.GetUnifor( "invTransWorld" );
    mArrowLen = s.GetUnifor( "arrowLen" );
    mArrowDistFromSurface = s.GetUnifor( "arrowDistFromSurface" );
  }
} // Tac
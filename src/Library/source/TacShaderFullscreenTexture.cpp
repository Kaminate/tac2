// Nathan Park
#include "TacShaderFullscreenTexture.h"
namespace Tac
{
  void ShaderFullscreenTexture::OnReload()
  {
    Shader & s = mResource->GetShader();
    mPos       = s.GetAttrib( "position" );
    mTexCoords = s.GetAttrib( "texcoord" );

    mTexture   = s.GetUnifor( "textureMap" );
  }
}
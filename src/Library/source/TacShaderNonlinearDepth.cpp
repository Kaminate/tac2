// Nathan Park
#include "TacShaderNonlinearDepth.h"
namespace Tac
{
  void ShaderNonlinearDepth::OnReload()
  {
    Shader & s = mResource->GetShader();
    mPos       = s.GetAttrib("position");
    mTexCoords = s.GetAttrib("texcoord");

    mTexture   = s.GetUnifor("textureMap");
    mLightNear = s.GetUnifor("lightNear");
    mLightFar  = s.GetUnifor("lightFar");
  }
  void ShaderLinearDepth::OnReload()
  {
    Shader & s = mResource->GetShader();
    mPos       = s.GetAttrib("position");
    mTexCoords = s.GetAttrib("texcoord");

    mTexture   = s.GetUnifor("textureMap");
  }
}

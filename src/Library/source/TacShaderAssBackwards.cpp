// Nathan Park
#include "TacShaderAssBackwards.h"
namespace Tac
{
  void ShaderAssBackwards::OnReload()
  {
    Shader & s = mResource->GetShader();

    mPos = s.GetAttrib("position");
    mTexcoord = s.GetAttrib("texcoord");

    mTime = s.GetUnifor("time");
    mResolution = s.GetUnifor("resolution");
    mNoise = s.GetUnifor("noise");
  }
}
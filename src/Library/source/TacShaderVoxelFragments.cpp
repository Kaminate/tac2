// Nathan Park
#include "TacShaderVoxelFragments.h"
namespace Tac
{
  void ShaderVoxelFragments::OnReload()
  {
    Shader & s = mResource->GetShader();
    mPos = s.GetAttrib("position");
    mNor = s.GetAttrib("normal");

    mCol = s.GetUnifor("color");
    mWorl = s.GetUnifor("world");
    mWorlNormal = s.GetUnifor("worldNormal");
    mView = s.GetUnifor("view");
    mProj= s.GetUnifor("proj");
    mMaxFrags= s.GetUnifor("maxFrags");
    mViewProjs = s.GetUnifor("viewProjs[0]");
  }
}

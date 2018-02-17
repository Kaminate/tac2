/*!
\author Nathan Park
*/
#pragma once

#include "TacShaderView.h"

namespace Tac
{
  class ShaderAssBackwards: public ShaderView
  {
  public:
    virtual void OnReload() override;

    // attributes
    GLuint mPos;
    GLuint mTexcoord;

    // uniforms
    GLuint mTime;
    GLuint mResolution;
    GLuint mNoise;
  };

} // Tac

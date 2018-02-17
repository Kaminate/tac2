/*!
\author Nathan Park
*/
#pragma once

#include "TacShaderView.h"

namespace Tac
{
  class ShaderShadow : public ShaderView
  {
  public:
    virtual void OnReload() override;

    // attributes
    GLuint mPos;

    // uniforms
    GLuint mWorl;
    GLuint mView;
    GLuint mProj;
    GLuint mLightNear;
    GLuint mLightFar;
  };

} // Tac

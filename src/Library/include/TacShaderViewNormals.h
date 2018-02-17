/*!
\author Nathan Park
*/
#pragma once

#include "TacShaderView.h"

namespace Tac
{
  class ShaderViewNor : public ShaderView
  {
  public:
    virtual void OnReload() override;

    // attributes
    GLuint mPos;
    GLuint mNor;

    // uniforms
    GLuint mWorl;
    GLuint mView;
    GLuint mProj;
    GLuint mInvTransWorld;
    GLuint mArrowLen;
    GLuint mArrowDistFromSurface;
  };

} // Tac

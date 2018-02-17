/*!
\author Nathan Park
*/
#pragma once

#include "TacShaderView.h"

namespace Tac
{
  class ShaderAsteroids: public ShaderView
  {
  public:
    virtual void OnReload() override;

    // attributes
    GLuint mPos;
    GLuint mNor;

    // uniforms
    GLuint mCol;
    GLuint mWorl;
    //GLuint mInvTransWorld;
    GLuint mView;
    GLuint mProj;
  };

} // Tac

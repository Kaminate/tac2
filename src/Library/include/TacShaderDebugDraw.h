/*!
\author Nathan Park
*/
#pragma once

#include "TacShaderView.h"

namespace Tac
{
  class ShaderDebugDraw : public ShaderView
  {
  public:
    virtual void OnReload() override;

    // attributes
    GLuint mPos;
    GLuint mCol;

    // uniforms
    GLuint mView;
    GLuint mProj;
  };

} // Tac

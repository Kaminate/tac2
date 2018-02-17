/*!
\author Nathan Park
*/
#pragma once

#include "TacShaderView.h"

namespace Tac
{
  class ShaderFullscreenTexture : public ShaderView
  {
  public:
    virtual void OnReload() override;

    // attributes
    GLuint mPos;
    GLuint mTexCoords;

    // uniforms
    GLuint mTexture;
  };

} // Tac

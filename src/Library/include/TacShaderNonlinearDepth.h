/*!
\author Nathan Park
*/
#pragma once

#include "TacShaderView.h"

namespace Tac
{
  class ShaderNonlinearDepth : public ShaderView
  {
  public:
    virtual void OnReload() override;

    // attributes
    GLuint mPos;
    GLuint mTexCoords;

    // uniforms
    GLuint mTexture;
    GLuint mLightNear;
    GLuint mLightFar;
  };

  class ShaderLinearDepth : public ShaderView
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

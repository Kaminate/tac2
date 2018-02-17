/*!
\author Nathan Park
*/
#pragma once

#include "TacShaderView.h"

namespace Tac
{
  // most useful shader na
  class ShaderPixels: public ShaderView
  {
  public:
    virtual void OnReload() override;

    GLuint mPos; // screenspace. rly. <-- ADDENUDM: NOW WORLDSPACE
    GLuint mCol;
    GLuint mView;
    GLuint mProj;
  };

} // Tac

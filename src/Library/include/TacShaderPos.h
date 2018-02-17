/*!
\author Nathan Park
*/
#pragma once

#include "TacShaderView.h"

namespace Tac
{
  class ShaderPos : public ShaderView
  {
  public:
    virtual void OnReload() override;

    // attributes
    GLuint mPos;
    GLuint mNor;

    // uniforms
    GLuint mCol;
    GLuint mWorl;
    GLuint mView;
    GLuint mProj;
    GLuint mInvTransWorld;
    GLuint mShadowMap;
    GLuint mLightCol;
    GLuint mLightDir;
    GLuint mLightView;
    GLuint mLightProj;
    GLuint mLightNear;
    GLuint mLightFar;
  };

} // Tac

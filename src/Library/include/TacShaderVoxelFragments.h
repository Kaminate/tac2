/*!
\author Nathan Park
*/
#pragma once

#include "TacShaderView.h"

namespace Tac
{
  class ShaderVoxelFragments: public ShaderView
  {
  public:
    virtual void OnReload() override;

    // attributes
    GLuint mPos;
    GLuint mNor;
    //GLuint mNor; // todo

    // uniforms
    GLuint mCol;
    GLuint mWorl; // I assume all we want is world matrix?
    GLuint mWorlNormal; // inverse transpose of world
    GLuint mView; // temp temp to draw scene from the buffer
    GLuint mProj; // temp temp to draw scene from the buffer
    GLuint mViewProjs;

    //GLuint mInvTransWorld; // todo
    
    // NOTE:
    // this shader uses a shader storage buffer
    // for the fragments and another for the fragCount
    GLuint mMaxFrags;
    static const unsigned mFragBufferBinding = 0;
    static const unsigned mFragCountBinding = 1;
  };

} // Tac

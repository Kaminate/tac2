#include "TacBindBuffer.h"
#include "TacGraphics\TacRendererState.h"

namespace Tac
{


  BindBuffer::BindBuffer( const GLenum target, const GLuint location )
    : mTarget(target)
    , mLocation(location)
  {
    

  }

  void BindBuffer::Execute( RendererState * state )
  {
    state;
    glBindBuffer(mTarget, mLocation);
  }


  /*
  AttribPointer::AttribPointer(ShaderInput * input)
    : mInput(input)
  {

  }

  void AttribPointer::Execute( RendererState * state )
  {

    glVertexAttribPointer(mShader.mPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
  }
  */

}
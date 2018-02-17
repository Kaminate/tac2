#include "TacDepthMaskOp.h"
#include "TacGraphics\TacRendererState.h"
#include "TacGLIncludes.h"

namespace Tac
{


  DepthMaskOp::DepthMaskOp( bool enableWritingToDepthBuffer )
    : mEnableWritingToDepthBuffer(enableWritingToDepthBuffer)
  {

  }

  void DepthMaskOp::Execute( RendererState * state )
  {
    state;
    //if(mEnableWritingToDepthBuffer)
    //  glDepthMask(GL_TRUE);
    //else
    //  glDepthMask(GL_FALSE);
    glDepthMask(mEnableWritingToDepthBuffer);
  }

}
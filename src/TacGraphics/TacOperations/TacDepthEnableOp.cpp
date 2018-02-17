#include "TacDepthEnableOp.h"
#include "TacGraphics\TacRendererState.h"
#include "TacGLIncludes.h"

namespace Tac
{
  DepthEnableOp::DepthEnableOp( bool enable )
    : mEnable(enable)
  {

  }

  void DepthEnableOp::Execute( RendererState * state )
  {
    state;
    glEnable(GL_DEPTH_TEST);  // We want depth test !
  }

}

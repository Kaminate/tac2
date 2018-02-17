#include "TacClearDepthOp.h"
#include "TacGraphics\TacRendererState.h"
#include "TacGLIncludes.h"
namespace Tac
{
  ClearDepthDistOp::ClearDepthDistOp( float clearDepthDist )
    : mClearDepthDist(clearDepthDist)
  {

  }

  void ClearDepthDistOp::Execute( RendererState * state )
  {
    state;
    glClearDepth(mClearDepthDist);
  }
}

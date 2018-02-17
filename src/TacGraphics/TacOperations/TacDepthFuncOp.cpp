#include "TacDepthFuncOp.h"
#include "TacGraphics\TacRendererState.h"

namespace Tac
{


  DepthFuncOp::DepthFuncOp( GLenum depthComparisonFn )
    : mDepthCompareFunction(depthComparisonFn)
  {


  }

  void DepthFuncOp::Execute( RendererState * state )
  {
    state;
    glDepthFunc(mDepthCompareFunction);
  }

}
#ifndef TAC_DEPTH_FUNC_OP_H
#define TAC_DEPTH_FUNC_OP_H

#include "TacGraphics\TacRenderOperation.h"
#include "TacGLIncludes.h"

namespace Tac
{
  class RendererState;
  class DepthFuncOp : public RenderOperation
  {
  public:
    DepthFuncOp(GLenum depthComparisonFn);
    virtual void Execute( RendererState * state) override;
  private:
    GLenum mDepthCompareFunction;
  };
}
#endif

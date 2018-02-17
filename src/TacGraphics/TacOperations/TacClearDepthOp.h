#ifndef TAC_CLEAR_DEPTH_OP_H
#define TAC_CLEAR_DEPTH_OP_H

#include "TacGraphics\TacRenderOperation.h"

namespace Tac
{
  class RendererState;
  class ClearDepthDistOp : public RenderOperation
  {
  public:
    ClearDepthDistOp(float clearDepthDist);
    virtual void Execute( RendererState * state) override;
  private:
    float mClearDepthDist;
  };
}
#endif

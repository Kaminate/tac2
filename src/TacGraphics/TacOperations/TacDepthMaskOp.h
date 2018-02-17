#ifndef TAC_DEPTH_MASK_OP_H
#define TAC_DEPTH_MASK_OP_H

#include "TacGraphics\TacRenderOperation.h"

namespace Tac
{
  class RendererState;

  class DepthMaskOp: public RenderOperation
  {
  public:
    DepthMaskOp(bool enableWritingToDepthBuffer);
    virtual void Execute( RendererState * state) override;

  private:
    bool mEnableWritingToDepthBuffer;
  };
}
#endif
#ifndef TAC_DEPTH_ENABLE_OP_H
#define TAC_DEPTH_ENABLE_OP_H

#include "TacGraphics\TacRenderOperation.h"

namespace Tac
{

  class RendererState;
  class DepthEnableOp : public RenderOperation
  {
  public:
    DepthEnableOp(bool enalbe);
    virtual void Execute( RendererState * state) override;
  private:
    bool mEnable;
  };

}
#endif

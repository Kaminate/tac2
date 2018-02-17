#ifndef TAC_COLOR_MASK_OP_H
#define TAC_COLOR_MASK_OP_H

#include "TacGraphics\TacRenderOperation.h"

namespace Tac
{
  class RendererState;
  class ColorMaskOp : public RenderOperation
  {
  public:
    ColorMaskOp(bool r, bool g, bool b, bool a);
    virtual void Execute( RendererState * state) override;
  private:
    bool mRMask;
    bool mGMask;
    bool mBMask;
    bool mAMask;
  };


}
#endif

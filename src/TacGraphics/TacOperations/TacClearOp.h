#ifndef TAC_CLEAR_OPERATION_H
#define TAC_CLEAR_OPERATION_H

#include "TacGraphics\TacRenderOperation.h"

namespace Tac
{
  class RendererState;
  class ClearOp : public RenderOperation
  {
  public:
    enum Type
    {
      eDepth   = 1 << 0,
      eStencil = 1 << 1,
      eAccum   = 1 << 2,
      eColor   = 1 << 3,
    };
    ClearOp(unsigned bitfield);
    virtual void Execute( RendererState * state) override;
  private:
    unsigned mType;
  };

  class ClearColorOp : public RenderOperation
  {
  public:
    ClearColorOp(float r, float g, float b, float a);
    virtual void Execute( RendererState * state) override;
  private:
    float mR, mG, mB, mA;
  };

}
#endif

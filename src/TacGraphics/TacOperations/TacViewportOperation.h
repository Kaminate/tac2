// structure based off KoRE rendering engine

#ifndef TAC_VIEWPORT_OPERATION_H
#define TAC_VIEWPORT_OPERATION_H

#include "TacGraphics\TacRenderOperation.h"

namespace Tac
{
  class ShaderData;
  class ShaderInput;
  class RendererState;

  class BindViewportOperation : public RenderOperation
  {
    bool mUseFramebufferDimensions;
    int mX, mY, mWidth, mHeight;
  public:
    // uses framebuffer width and height
    BindViewportOperation(int x,int y);
    BindViewportOperation(int x,int y,int width,int height);
    virtual void Execute(RendererState*state) override;
  };

}
#endif
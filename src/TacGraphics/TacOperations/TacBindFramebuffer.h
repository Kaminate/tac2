#ifndef TAC_BIND_FRAMEBUFFER_OPERATION_H_
#define TAC_BIND_FRAMEBUFFER_OPERATION_H_
#include "TacGraphics\TacRenderOperation.h"
#include "TacGLIncludes.h"
#include <vector>

namespace Tac
{
  class Framebuffer;
  class RendererState;

  class BindFramebuffer : public RenderOperation
  {
  public:
    BindFramebuffer(
      const Framebuffer * fbo,
      const std::vector<GLenum>& drawBufs );
    virtual void Execute(RendererState*state) override;
  private:
    const Framebuffer * mFbo;
    std::vector<GLenum> mDrawBuffers;

  };

}
#endif
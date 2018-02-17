#include "TacBindFramebuffer.h"
#include "TacFramebuffer.h"
#include "TacErrorIf.h"
#include "TacGraphics\TacRendererState.h"

namespace Tac
{
  BindFramebuffer::BindFramebuffer( const Framebuffer * fbo ,
      const std::vector<GLenum>& drawBufs )
      : mFbo(fbo)
      , mDrawBuffers(drawBufs)
  {
    ErrorIf(drawBufs.empty(), "Must have a drawbuffer to output to");
    ErrorIf(
      fbo == Framebuffer::GetBackbuffer()
      && mDrawBuffers.size() > 1 
      && mDrawBuffers[0]  != GL_BACK_LEFT, 
      "When drawing to the backbuffer, you must specify GL_BACK_LEFT");
  }

  void BindFramebuffer::Execute(RendererState*state)
  {
    state->SetFramebuffer(mFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, mFbo->GetHandle());
    glDrawBuffers(mDrawBuffers.size(), &mDrawBuffers.front());
  }
}
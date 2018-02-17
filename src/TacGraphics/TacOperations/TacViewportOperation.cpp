#include "TacViewportOperation.h"
#include "TacGLIncludes.h"
#include "TacGraphics\TacRendererState.h"
#include "TacFramebuffer.h"
#include "TacGLFW.h"
#include "TacTexture.h"
namespace Tac
{


  BindViewportOperation::BindViewportOperation( int x,int y )
    : mX(x)
    , mY(y)
    , mHeight(-1)
    , mWidth(-1)
    , mUseFramebufferDimensions(true)
  {

  }

  BindViewportOperation::BindViewportOperation( int x,int y,int width,int height )
    : mX(x)
    , mY(y)
    , mWidth(width)
    , mHeight(height)
    , mUseFramebufferDimensions(false)
  {

  }

  void BindViewportOperation::Execute(RendererState*state)
  {
    if(mUseFramebufferDimensions)
    {
      const Framebuffer * buffer = state->GetFramebuffer();
      if(buffer == Framebuffer::GetBackbuffer())
      {
        Window * wind = Window::GetInstance();
        mWidth = wind->GetWidth();
        mHeight = wind->GetHeight();
      }
      else
      {
        Texture * tex = nullptr;
        if(     buffer->HasTexture(GL_COLOR_ATTACHMENT0))
          tex = buffer->GetTexture(GL_COLOR_ATTACHMENT0);
        else if(buffer->HasTexture(GL_DEPTH_ATTACHMENT))
          tex = buffer->GetTexture(GL_DEPTH_ATTACHMENT);
        const Texture::Properties& props = tex->GetProperties();
        mWidth = props.mWidth;
        mHeight = props.mHeight;
      }
    }
    glViewport(mX, mY, mWidth, mHeight);
  }
}
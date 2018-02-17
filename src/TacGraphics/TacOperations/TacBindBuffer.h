#ifndef TAC_BIND_BUFFER_H
#define TAC_BIND_BUFFER_H
#include "TacGraphics\TacRenderOperation.h"
#include "TacGLIncludes.h"

namespace Tac
{
  class ShaderInput;

  class BindBuffer : public RenderOperation 
  {
  public:
    BindBuffer(const GLenum target, const GLuint location);

    virtual void Execute(RendererState * state)override;

  private:
    GLuint mTarget;
    GLenum mLocation;
  };

  /*
  class AttribPointer:public RenderOperation
  {
  public:
    AttribPointer(ShaderInput * input);
    virtual void Execute(RendererState * state) override;
  private:
    ShaderInput * mInput;
  }
  */
}
#endif

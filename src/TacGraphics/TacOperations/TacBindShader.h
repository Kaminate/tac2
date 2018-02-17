
#ifndef TAC_BIND_SHADER_OPERATION_H_
#define TAC_BIND_SHADER_OPERATION_H_

#include "TacGraphics\TacRenderOperation.h"

namespace Tac
{
  class ShaderResource;

  class BindShader : public RenderOperation
  {
  public:
    BindShader(ShaderResource * shaderRsc);
    virtual void Execute(RendererState*state) override;
  private:
    ShaderResource * mShaderResource;
  };

  class UnbindShader : public RenderOperation
  {
  public:
    UnbindShader(ShaderResource * shaderRsc);
    virtual void Execute(RendererState*state) override;
  private:
    ShaderResource * mShaderResource;
  };

}
#endif
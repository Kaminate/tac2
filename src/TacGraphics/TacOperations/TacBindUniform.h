// structure based off KoRE rendering engine

#ifndef TAC_BIND_UNIFORM_OPERATION_H_
#define TAC_BIND_UNIFORM_OPERATION_H_

#include "TacGraphics\TacRenderOperation.h"

#include <string>
namespace Tac
{
  class ShaderData;
  class ShaderInput;

  class BindUniform : public RenderOperation
  {
  public:
    BindUniform(ShaderInput * input, ShaderData * data);
    virtual void Execute(RendererState*state) override;
    // owned by a shader somewhere
    ShaderInput * mInput;
    // owned by someone somewhere
    ShaderData * mData;
  };

}
#endif
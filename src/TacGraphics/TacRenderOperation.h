// structure based off KoRE rendering engine

#ifndef TAC_RENDER_OPERATION_H_
#define TAC_RENDER_OPERATION_H_

#include <string>
namespace Tac
{
  class ShaderPass;
  class RendererState;

  class RenderOperation
  {
    ShaderPass * mOwner;
    std::string mName;
  public:
    RenderOperation();
    virtual void Execute(RendererState*state) = 0;
    void SetOwner(ShaderPass*owner);
  };

}
#endif
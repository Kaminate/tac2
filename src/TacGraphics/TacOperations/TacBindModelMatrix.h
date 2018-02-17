// structure based off KoRE rendering engine

#ifndef TAC_BIND_MODEL_MATRIX_OPERATION_H_
#define TAC_BIND_MODEL_MATRIX_OPERATION_H_
#include "TacGraphics\TacRenderOperation.h"
#include "TacGeometryData.h"

namespace Tac
{

  class ShaderInput;
  class RendererState;
  class BindModelMatrix : public RenderOperation
  {
  public:
    enum Type{eWorld, eInverseWorld};
    BindModelMatrix(ShaderInput * input, Type matrixType);
    virtual void Execute(RendererState * state) override;
    // owned by a shader somewhere
    ShaderInput * mInput;
    Type mType;
  };

}
#endif
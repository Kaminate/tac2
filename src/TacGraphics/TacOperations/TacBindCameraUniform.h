// structure based off KoRE rendering engine

#ifndef TAC_BIND_CAMERA_UNIFORM_OPERATION_H_
#define TAC_BIND_CAMERA_UNIFORM_OPERATION_H_

#include "TacGraphics\TacRenderOperation.h"

namespace Tac
{
  class ShaderData;
  class ShaderInput;

  class BindCameraUniform : public RenderOperation
  {
  public:
    enum Type
    {
      eMatrixProjPerspective, 
      eMatrixProjPerspectiveInverse,
      eMatrixView,
      eMatrixViewInverse,
      eMatrixOrthoProj,
      // todo: near, far, pos, dir, etc
    };
    BindCameraUniform(ShaderInput * input, Type uniformType);
    virtual void Execute(RendererState*state) override;
    // owned by a shader somewhere
    ShaderInput * mInput;
    Type mType;
  };

}
#endif
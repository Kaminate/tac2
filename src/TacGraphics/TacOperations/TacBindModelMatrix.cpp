#include "TacBindModelMatrix.h"
#include "TacComponentTransform.h"
#include "TacComponentModel.h"
#include "TacMatrix4.h"
#include "TacEntity.h"
#include "TacErrorIf.h"
#include "TacGraphics\TacShaderInput.h"
#include "TacGraphics\TacRendererState.h"

namespace Tac
{




  BindModelMatrix::BindModelMatrix( ShaderInput * input, Type matrixType )
    : mInput(input)
    , mType(matrixType)
  {

  }

  void BindModelMatrix::Execute(RendererState * state)
  {
    const ModelComponent * modelComp = state->GetModel();
    Matrix4 mat;

    const TransformComp* trans 
      =(const TransformComp*) modelComp->GetSibling(Comp::eCompTransform);
    switch (mType)
    {
    case Tac::BindModelMatrix::eWorld:
      mat = trans->mFakeTransform;
      break;
    case Tac::BindModelMatrix::eInverseWorld:
      //mat = trans->mFakeInverseTransform;
      //mat.Transpose();
      mat = trans->mFakeTransform;
      mat.Invert();
      mat.Transpose();
      break;
    default:
      DebugError("...");
      break;
    }

    glUniformMatrix4fv(mInput->mLocation, 1, GL_TRUE, mat.v);
  }

}
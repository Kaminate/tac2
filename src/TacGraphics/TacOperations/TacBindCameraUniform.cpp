#include "TacBindCameraUniform.h"
#include "TacSystemGraphics.h"
#include "TacGraphics\TacShaderInput.h"
#include "tacGraphics\TacShaderData.h"
#include "tacGraphics\TacRendererState.h"
#include "TacGraphics\TacOperations\TacBindUniform.h"
#include "TacErrorIf.h"
#include <TacComponentCamera.h>

namespace Tac
{
  BindCameraUniform::BindCameraUniform(
    ShaderInput * input, Type uniformType )
    : mInput(input)
    , mType(uniformType)
  {

  }

  void BindCameraUniform::Execute(RendererState*state)
  {
    state;
    GraphicsSystem * sys = GraphicsSystem::gInstance;
    CameraComponent* cameraComp = sys->GetCurrentCamera();
    ErrorIf(!cameraComp, "idk");
    Camera & cam = cameraComp->mCamera;

    ShaderData data;
    Matrix4 mat;

    switch (mType)
    {
    case Tac::BindCameraUniform::eMatrixProjPerspective:
      mat = cam.GetPerspectiveProjMatrix();
      data.mdata = &mat;
      data.mtype = GL_FLOAT_MAT4;
      data.msize = 1;
      data.mname = "perspective projection matrix";
      break;
    case Tac::BindCameraUniform::eMatrixProjPerspectiveInverse:
      mat = cam.GetInversePerspectiveProjMatrix();
      data.mdata = &mat;
      data.mtype = GL_FLOAT_MAT4;
      data.msize = 1;
      data.mname = "inverse perspective projection matrix";
      break;
    case Tac::BindCameraUniform::eMatrixView:
      mat = cam.GetViewMatrix();
      data.mdata = &mat;
      data.mtype = GL_FLOAT_MAT4;
      data.msize = 1;
      data.mname = "View matrix";
      break;
    case Tac::BindCameraUniform::eMatrixViewInverse:
      mat = cam.GetInverseViewMatrix();
      data.mdata = &mat;
      data.mtype = GL_FLOAT_MAT4;
      data.msize = 1;
      data.mname = "inverse view matrix";
      break;
    case Tac::BindCameraUniform::eMatrixOrthoProj:
      mat = cam.GetOrthographicProjMatrix();
      data.mdata = &mat;
      data.mtype = GL_FLOAT_MAT4;
      data.msize = 1;
      data.mname = "orthographic projection matrix";
      break;
    default:
      DebugError("...");
      break;
    }
    BindUniform bindCall(mInput, &data);
    bindCall.Execute(state);

  }

}
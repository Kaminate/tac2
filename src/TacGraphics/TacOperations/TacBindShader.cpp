#include "TacBindShader.h"
#include "TacShaderResource.h"
#include "TacShader.h"

namespace Tac
{
  BindShader::BindShader( ShaderResource * shaderRsc )
    : mShaderResource(shaderRsc)
  {

  }

  void BindShader::Execute(RendererState*state)
  {
    state;
    mShaderResource->GetShader().Activate();
  }

  UnbindShader::UnbindShader( ShaderResource * shaderRsc )
    : mShaderResource(shaderRsc)
  {

  }

  void UnbindShader::Execute(RendererState*state)
  {
    state;
    mShaderResource->GetShader().Deactivate();
  }

}
/*
*/
#pragma once
#include "TacShaderResource.h"
#include "TacShader.h"
namespace Tac
{
  class ShaderView
  {
  public:
    ShaderView();
    void BindToResource(ShaderResource * resource);
    virtual void OnReload() = 0;
    const Shader & GetShader()const;
    ShaderResource * mResource;
  };

} // Tac

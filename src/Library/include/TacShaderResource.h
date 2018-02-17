/*!
Nathan
*/
#pragma once

#include "TacResource.h"
#include <map>
#include <string>
#include <memory>
#include <iostream>

#include "TacGraphics\TacShaderInput.h"

namespace Tac
{
  class Shader;
  class ShaderResource : public Resource
  {
  public:
    ShaderResource();
    ~ShaderResource();
    enum ShaderType{eVertexShader, eGeometryShader, eFragmentShader};
    void AddShader(ShaderType type, const std::string & shaderPath);
    void Init();
    Shader & GetShader();

    ShaderInput * GetAttribute(const std::string & attributeName);
    ShaderInput * GetUniform(const std::string & uniformName);

  protected:
    virtual std::string ReloadAux() override;

  private:
    void ShaderResource::FillShaderInput(
      GLenum activeVarEnum,
      std::vector<ShaderInput>&datas);
    std::map<ShaderType, std::string> mInfoNeededForReload;
    Shader * mShader;
    std::vector<ShaderInput> mAttibuteInput;
    std::vector<ShaderInput> mUniformInput;
  };
} // Tac

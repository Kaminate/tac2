/*!
*/
#include "TacShaderResource.h"
#include "TacShader.h"
#include "TacErrorIf.h"
#include <windows.h>
namespace Tac
{
  static GLenum GetActualGLEnum(ShaderResource::ShaderType type)
  {
    switch (type)
    {
    case ShaderResource::eVertexShader:   return GL_VERTEX_SHADER;
    case ShaderResource::eFragmentShader: return GL_FRAGMENT_SHADER;
    case ShaderResource::eGeometryShader: return GL_GEOMETRY_SHADER;
    default:
      DebugError( "We don't support that kind of shader (yet)" );
      return 0;
      break;
    }
  }

  ShaderResource::ShaderResource() : mShader(new Shader)
  {
  }

  std::string ShaderResource::ReloadAux()
  {
    std::string errors;

    try
    {
      Shader * reloaded = new Shader();
      for(auto pair : mInfoNeededForReload)
        reloaded->LoadShader(GetActualGLEnum(pair.first), pair.second);
      reloaded->CompileAndLinkProgram();
      FillShaderInput(GL_ACTIVE_UNIFORMS, mUniformInput);
      FillShaderInput(GL_ACTIVE_ATTRIBUTES, mAttibuteInput);

      delete mShader;
      mShader = reloaded;
    }
    catch (Exception & e)
    {
      errors = e.what();
    }

    return errors;
  }

  Shader & ShaderResource::GetShader()
  {
    return *mShader;
  }

  void ShaderResource::AddShader(
    ShaderType type,
    const std::string & shaderPath )
  {
    mShader->LoadShader(GetActualGLEnum(type), shaderPath);
    Resource::AddPathToWatch(shaderPath);
    mInfoNeededForReload[type] = shaderPath;
  }

  void ShaderResource::Init()
  {
    mShader->CompileAndLinkProgram();
    FillShaderInput(GL_ACTIVE_UNIFORMS, mUniformInput);
    FillShaderInput(GL_ACTIVE_ATTRIBUTES, mAttibuteInput);
  }

  ShaderResource::~ShaderResource()
  {
    delete mShader;
  }

  void ShaderResource::FillShaderInput(
    GLenum activeVarEnum,
    std::vector<ShaderInput>&datas)
  {
    datas.clear();
    GLuint program = mShader->GetProgram();

    char nameBuffer[100];
    int varCount;
    glGetProgramiv(program, activeVarEnum, &varCount);
    ErrorIf(varCount > sizeof(nameBuffer), "Var too long!");

    for (int i = 0; i < varCount; i++)
    {
      GLsizei varNumChars; // no null
      ShaderInput input;

      if(activeVarEnum == GL_ACTIVE_ATTRIBUTES)
      {
        glGetActiveAttrib(
          program,
          i, 
          sizeof(nameBuffer), 
          &varNumChars, 
          &input.mSize,
          &input.mType, 
          nameBuffer ); // becomes null-terminated
      }
      else
      {
        glGetActiveUniform(
          program,
          i, 
          sizeof(nameBuffer),
          &varNumChars,
          &input.mSize,
          &input.mType,
          nameBuffer);
      }

      input.mName = std::string(nameBuffer);

      if(activeVarEnum == GL_ACTIVE_ATTRIBUTES)
        input.mLocation = glGetAttribLocation(program, nameBuffer);
      else if(activeVarEnum == GL_ACTIVE_UNIFORMS)
        input.mLocation = glGetUniformLocation(program, nameBuffer);

      if(input.mSize == 1)
      {
        datas.push_back(input);
      }
      else if (input.mSize > 1)
      {
        while(input.mName.back() != '[')
          input.mName.pop_back();

        for (int i = 0; i < input.mSize; ++i)
        {
          ShaderInput perElementInput = input;
          perElementInput.mSize = 1;
          perElementInput.mName += std::to_string(i) + ']';
          perElementInput.mLocation += i;
        }
      }
      else
      {
        __debugbreak(); // idk
      }
    }
  }

  ShaderInput * ShaderResource::GetAttribute( const std::string & attributeName )
  {
    for(unsigned i = 0; i < mAttibuteInput.size(); ++i)
    {
      ShaderInput * input = &mAttibuteInput[i];
      if(input->mName == attributeName)
        return input;
    }
    DebugError("Attribute not found");
    return nullptr;
  }

  ShaderInput * ShaderResource::GetUniform( const std::string & uniformName )
  {
    for(unsigned i = 0; i < mUniformInput.size(); ++i)
    {
      ShaderInput * input = &mUniformInput[i];
      if(input->mName == uniformName)
        return input;
    }
    DebugError("Uniform not found");
    return nullptr;
  }

} // Tac

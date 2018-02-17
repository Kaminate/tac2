/*!
\author Nathan Park
*/
#include "TacShader.h"
#include "TacFileReader.h"
#include "TacErrorIf.h"
#include "TacGraphicsErrors.h"
#include "TacException.h"

namespace Tac
{
  /*
  void Shader::Init() throw ()
  {
    CompileAndLinkProgram(); // may throw
    LoadShaderVars(
      GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, 
      GL_ACTIVE_ATTRIBUTES, 
      glGetActiveAttrib,
      glGetAttribLocation,
      mAttribs);
    LoadShaderVars(
      GL_ACTIVE_UNIFORM_MAX_LENGTH, 
      GL_ACTIVE_UNIFORMS, 
      glGetActiveUniform,
      glGetUniformLocation,
      mUniforms);
  }
  */

  Shader::~Shader()
  {
    // todo: for each shader
    // glDeleteShader(shader)
    glDeleteProgram(mProgram);
  }

  void Shader::LoadShaderVars(
    GLenum varMaxLenEnum, 
    GLenum activeVarEnum, 
    decltype(glGetActiveAttrib) getActiveVar,
    decltype(glGetAttribLocation) getVarLocation,
    std::map<std::string, GLuint> & varMap)
  {
    const int MAX_SHADER_VAR_LENGTH_INCLUDING_NULL = 50;
    static char buffer[MAX_SHADER_VAR_LENGTH_INCLUDING_NULL];

    int maxLength; // includes null
    glGetProgramiv(mProgram, varMaxLenEnum, &maxLength);
    ErrorIf(maxLength > MAX_SHADER_VAR_LENGTH_INCLUDING_NULL, 
      "variable name too long");

    int varCount;
    glGetProgramiv(mProgram, activeVarEnum, &varCount);

    for (int i = 0; i < varCount; i++)
    {
      int size;
      int charsWritten; // excluding null
      GLenum type;
      getActiveVar(
        mProgram, i, maxLength, &charsWritten, &size, &type, buffer);

      GLuint location =  getVarLocation(mProgram, buffer);
      std::string bufferName(buffer, charsWritten);

      if (location == -1)
      {
        // do nothing, because it doesn't work for atomic_uint
        // throw Exception("Failed to get attribute " + bufferName);
        // todo: something about this
      }
      else
      {
        varMap[bufferName] = location;
      }
    }
  }

  void Shader::LoadFromData( GLenum shaderType,const char * data )
  {
    GLuint shader = glCreateShader(shaderType);

    glShaderSource(shader, 1, &data, nullptr);

    glCompileShader(shader);
    GLint result = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

    bool success = result != GL_FALSE;
    if (success)
    {
      mShaders[shaderType] = shader; 
    }
    else
    {
      GLint errorLogLength;
      glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &errorLogLength);
      std::unique_ptr<GLchar> errorLog(new GLchar[errorLogLength]);
      glGetShaderInfoLog (shader, errorLogLength, NULL, errorLog.get());
      std::string errorString(errorLog.get());
      std::vector<std::string> errors;
      std::string curr;
      for (char c : errorString)
      {
        if (c == '\n')
        {
          errors.push_back(curr);
          curr.clear();
        }
        else
        {
          curr += c;
        }
      }
      throw Exception(errorString);
    }
  }

  void Shader::LoadShader(
    GLenum shaderType, 
    const std::string & shaderPath) 
  {
    ErrorIf(mShaders.find(shaderType) != mShaders.end(), 
      "There already exists a shader of this type");

    FileReader reader(shaderPath);
    LoadFromData(shaderType, reader.GetSource());
  }

  void Shader::CompileAndLinkProgram() 
  {
    mProgram = glCreateProgram();
    for (auto it = mShaders.begin(); it != mShaders.end(); ++it)
    {
      auto pair = *it; //GLenum, GLuint
      GLuint shader = pair.second;
      glAttachShader(mProgram, shader);
    }
    glLinkProgram(mProgram);
    GLint link_ok;
    glGetProgramiv(mProgram, GL_LINK_STATUS, &link_ok);

    bool success = link_ok != GL_FALSE;
    if (success)
    {
      // Done linking program. Don't need shaders anymore, so delete them
      for (auto it = mShaders.begin(); it != mShaders.end(); ++it)
      {
        auto pair = *it; //GLenum, GLuint
        GLuint shader = pair.second;
        glDeleteShader(shader);
      }
      mShaders.clear();
    }
    else
    {
      GLint errorLogLength;
      glGetProgramiv (mProgram, GL_INFO_LOG_LENGTH, &errorLogLength);
      std::unique_ptr<GLchar> errorLog(new GLchar[errorLogLength]);
      glGetProgramInfoLog (mProgram, errorLogLength, NULL, errorLog.get());
      throw Exception(errorLog.get());
    }


  // TEMP, MOVING THIS TO SHADERRESOURCE class instead
    LoadShaderVars(
      GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, 
      GL_ACTIVE_ATTRIBUTES, 
      glGetActiveAttrib,
      glGetAttribLocation,
      mAttribs);
    LoadShaderVars(
      GL_ACTIVE_UNIFORM_MAX_LENGTH, 
      GL_ACTIVE_UNIFORMS, 
      glGetActiveUniform,
      glGetUniformLocation,
      mUniforms);

  }

  void Shader::Activate() const
  {
    glUseProgram(mProgram);
    for (const std::pair<std::string, GLuint> & pair : mAttribs)
    {
      GLuint attrib = pair.second;
      glEnableVertexAttribArray(attrib);
    }
  }

  void Shader::Deactivate() const
  {
    for (const std::pair<std::string, GLuint> & pair : mAttribs)
    {
      GLuint attrib = pair.second;
      glDisableVertexAttribArray(attrib);
    }
    glUseProgram(0);
  }

  GLuint Shader::GetAttrib( const std::string& attribName ) const
  {
    return mAttribs.at(attribName);
  }

  GLuint Shader::GetUnifor( const std::string& uniforName ) const
  {
    return mUniforms.at(uniforName);
  }

  const std::map<GLenum, std::string> & Shader::GetPaths() const
  {
    return mShaderPaths;
  }


} // Tac



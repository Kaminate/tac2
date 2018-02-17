/*!
\author Nathan Park
How to use:
1. Call Shader::LoadShader once for GL_VERTEX_SHADER 
and once for GL_FRAGMENT_SHADER
2. Call Shader::CompileAndLinkProgram()
3. Bind a bunch of attributes and uniforms
4. When you're going to render a thing, call 
Shader::Activate()
5. Send your uniforms to your shader via
gluniform(... shader("uniform name"))

You should derive the shader to make your own shader classes
and keep the GLuint attributes and uniforms as member variabes
for quick lookup
*/
#pragma once

#include "TacGLIncludes.h"
#include "TacSharedPtr.h"
#include "TacResource.h"
#include "TacException.h"

#include <map>
#include <string>
#include <vector>

namespace Tac
{
  class Shader
  {
  public:
    ~Shader();
    
    // example: Load(GL_VERTEX_SHADER, "resources/shaders/shader.vert");
    void LoadShader(GLenum shaderType, const std::string & filePath); 
    //void Init() throw (...);
    void CompileAndLinkProgram() throw (...);

    void Activate() const;
    void Deactivate() const;

    const std::map<GLenum, std::string> & GetPaths() const;

    GLuint GetAttrib( const std::string& attribName )const;
    GLuint GetUnifor( const std::string& uniforName )const;

    GLuint GetProgram() const {return mProgram;}
  private:
    std::map<std::string, GLuint> mAttribs;
    std::map<std::string, GLuint> mUniforms;

    // data should be null terminated
    void LoadFromData(GLenum shaderType, const char * data);


    std::map<GLenum, GLuint> mShaders;
    std::map<GLenum, std::string> mShaderPaths;
    GLuint mProgram;
    void LoadAttributes();
    void LoadShaderVars(
      GLenum, 
      GLenum, 
      decltype(glGetActiveAttrib), 
      decltype(glGetAttribLocation),
      std::map<std::string, GLuint> &);
    void LoadUniforms();
    void ReleaseProgram();
  };
} // Tac

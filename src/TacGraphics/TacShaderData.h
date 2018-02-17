// Based off the KoRE Project
// Shader data is stuff you can send to a shader.
// They have void pointers for a reason

#ifndef TAC_SHADERDATA_H
#define TAC_SHADERDATA_H

#include "TacGLIncludes.h"
#include <string>
namespace Tac
{
  class ShaderData
  {
  public:
    ShaderData(
      const GLenum type,
      const GLuint size,
      const std::string & name,
      void * data);
    ShaderData();
    //virtual ~ShaderData(void) {};

    GLenum mtype;      // e.g. GL_FLOAT_VEC3

    // number of components in units of type (always 1 for non-array uniforms)
    GLuint msize;      
    std::string mname;
    void* mdata;
  };
}

#endif  

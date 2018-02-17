// Based off the KoRE Project

#ifndef TAC_SHADERINPUT_H
#define TAC_SHADERINPUT_H

#include "TacGLIncludes.h"
#include <string>
namespace Tac
{
  class ShaderInput
  {
  public:
    //enum Type{eUnknown, eUniform, eAttribute};
    ShaderInput();

    // ex: GL_FLOAT_VEC3
    GLenum mType;
    GLint mSize;               // in units of mType
    GLint mLocation;          // location of attribute/uniform in the shader
    std::string mName;        // input name
    /*
    union {
      GLuint texUnit;          // texUnit to bind a texture to.
      GLuint imgUnit;          // imgUnit to bind an image to.
      GLuint atomicCounterBindingPoint;       // bindingpoint for atomic counters
    };
    */

  };

}

#endif  

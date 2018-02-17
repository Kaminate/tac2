// Based off the KoRE Project
#include "TacShaderData.h"

namespace Tac
{

  ShaderData::ShaderData(void)
    :mtype(GL_NONE)
    ,msize(1)
    ,mname("Undefined ShaderData")
    ,mdata(NULL)
  {
  }

  ShaderData::ShaderData(
    const GLenum type,
    const GLuint size,
    const std::string& name,
    void* data)
    : mtype(type),
    msize(size),
    mname(name),
    mdata(data)
  {
  }

}
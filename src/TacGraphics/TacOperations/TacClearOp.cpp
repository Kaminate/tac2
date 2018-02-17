#include "TacClearOp.h"
#include "TacGraphics\TacRendererState.h"
#include "TacGLIncludes.h"

namespace Tac
{
  ClearOp::ClearOp( unsigned bitfield )
    : mType(bitfield)
  {

  }

  void ClearOp::Execute( RendererState * state )
  {
    state;
    GLbitfield mask = 0;
    if(mType & Type::eDepth) mask |= GL_DEPTH_BUFFER_BIT;
    if(mType & Type::eStencil)mask |= GL_STENCIL_BUFFER_BIT;
    if(mType & Type::eColor)mask |= GL_COLOR_BUFFER_BIT;
    if(mType & Type::eAccum)mask |= GL_ACCUM_BUFFER_BIT;
    glClear(mask);
  }

  ClearColorOp::ClearColorOp( float r, float g, float b, float a )
    : mR(r)
    , mG(g)
    , mB(b)
    , mA(a)
  {
  }

  void ClearColorOp::Execute( RendererState * state )
  {
    state;
    glClearColor(mR, mG, mB, mA);
  }

}
